/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    kd_tree_scene.c

Abstract:

    Implements a kd-tree scene.

--*/

#include <stdalign.h>
#include <string.h>

#include "common/pointer_list.h"
#include "common/safe_math.h"
#include "iris_physx_toolkit/kd_tree_scene.h"

//
// Uncompressed Tree Defines
//

#define TARGET_LEAF_SIZE ((size_t)1)
#define INTERSECTION_COST ((float_t)80.0)
#define TRAVERSAL_COST ((float_t)1.0)
#define EMPTY_BONUS ((float_t)0.5)

//
// Uncompressed Tree Types
//

typedef struct _EDGE {
    size_t primitive;
    float_t value;
    bool is_start;
} EDGE, *PEDGE;

typedef const EDGE *PCEDGE;

typedef struct _UNCOMPRESSED_NODE UNCOMPRESSED_NODE, *PUNCOMPRESSED_NODE;
typedef const UNCOMPRESSED_NODE *PCUNCOMPRESSED_NODE;

typedef struct _UNCOMPRESSED_LEAF {
    _Field_size_(num_indices) size_t *indices;
    size_t num_indices;
} UNCOMPRESSED_LEAF, *PUNCOMPRESSED_LEAF;

typedef const UNCOMPRESSED_LEAF *PCUNCOMPRESSED_LEAF;

typedef struct _UNCOMPRESSED_INTERIOR {
    VECTOR_AXIS split_axis;
    float_t split;
    PUNCOMPRESSED_NODE below_child;
    PUNCOMPRESSED_NODE above_child;
} UNCOMPRESSED_INTERIOR, *PUNCOMPRESSED_INTERIOR;

typedef const UNCOMPRESSED_INTERIOR *PCUNCOMPRESSED_INTERIOR;

typedef union _UNCOMPRESSED_INTERIOR_OR_LEAF {
    UNCOMPRESSED_INTERIOR interior;
    UNCOMPRESSED_LEAF leaf;
} UNCOMPRESSED_INTERIOR_OR_LEAF, *PUNCOMPRESSED_INTERIOR_OR_LEAF;

struct _UNCOMPRESSED_NODE {
    bool is_leaf;
    UNCOMPRESSED_INTERIOR_OR_LEAF data;
};

//
// Uncompressed Tree Static Functions
//

static
int
EdgeCompare(
    _In_ const void *edge0,
    _In_ const void *edge1
    )
{
    PCEDGE left = (PCEDGE)edge0;
    PCEDGE right = (PCEDGE)edge1;

    if (left->value < right->value)
    {
        return -1;
    }

    if (right->value < left->value)
    {
        return 1;
    }

    if (left->is_start)
    {
        return -1;
    }

    return 1;
}

static
VECTOR_AXIS
NextAxis(
    _In_ VECTOR_AXIS axis
    )
{
    if (axis == VECTOR_X_AXIS)
    {
        return VECTOR_Y_AXIS;
    }

    if (axis == VECTOR_Y_AXIS)
    {
        return VECTOR_Z_AXIS;
    }

    return VECTOR_X_AXIS;
}

static
float_t
VectorGetElementByAxis(
    _In_ VECTOR3 vector,
    _In_ VECTOR_AXIS axis
    )
{
    if (axis == VECTOR_X_AXIS)
    {
        return vector.x;
    }

    if (axis == VECTOR_Y_AXIS)
    {
        return vector.y;
    }

    return vector.z;
}

static
float_t
PointGetElementByAxis(
    _In_ POINT3 point,
    _In_ VECTOR_AXIS axis
    )
{
    if (axis == VECTOR_X_AXIS)
    {
        return point.x;
    }

    if (axis == VECTOR_Y_AXIS)
    {
        return point.y;
    }

    return point.z;
}

static
ISTATUS
ComputeBounds(
    _In_reads_(num_shapes) const PSHAPE shapes[],
    _In_reads_(num_shapes) const PMATRIX transforms[],
    _In_reads_(num_shapes) const bool premultiplied[],
    _In_ size_t num_shapes,
    _Out_writes_(num_shapes) BOUNDING_BOX bounds[],
    _Out_ PBOUNDING_BOX total_bounds
    )
{
    if (num_shapes == 0)
    {
        POINT3 point = PointCreate((float_t)0.0, (float_t)0.0, (float_t)0.0);
        *total_bounds = BoundingBoxCreate(point, point);
        return ISTATUS_SUCCESS;
    }

    PCMATRIX transform = premultiplied[0] ? NULL : transforms[0];
    ISTATUS status = ShapeComputeBounds(shapes[0],
                                        transform,
                                        total_bounds);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    bounds[0] = *total_bounds;

    for (size_t i = 1; i < num_shapes; i++)
    {
        transform = premultiplied[i] ? NULL : transforms[i];
        status = ShapeComputeBounds(shapes[i],
                                    transform,
                                    bounds + i);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }

        *total_bounds = BoundingBoxUnion(*total_bounds, bounds[i]);
    }

    return ISTATUS_SUCCESS;
}

static
ISTATUS
AllocateEdges(
    _In_ const BOUNDING_BOX all_bounds[],
    _In_reads_(num_indices) const size_t shape_indices[],
    _In_ size_t num_indices,
    _In_ VECTOR_AXIS axis,
    _Outptr_result_buffer_(2 * num_indices) PEDGE *edges
    )
{
    size_t num_entries;
    bool success = CheckedMultiplySizeT(num_indices, 2, &num_entries);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    *edges = calloc(num_entries, sizeof(EDGE));

    if (*edges == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    for (size_t i = 0; i < num_indices; i++)
    {
        float_t min, max;
        switch (axis)
        {
            case VECTOR_X_AXIS:
                min = all_bounds[shape_indices[i]].corners[0].x;
                max = all_bounds[shape_indices[i]].corners[1].x;
                break;
            case VECTOR_Y_AXIS:
                min = all_bounds[shape_indices[i]].corners[0].y;
                max = all_bounds[shape_indices[i]].corners[1].y;
                break;
            default: // VECTOR_Z_AXIS
                min = all_bounds[shape_indices[i]].corners[0].z;
                max = all_bounds[shape_indices[i]].corners[1].z;
                break;
        }

        (*edges)[2 * i].is_start = true;
        (*edges)[2 * i].primitive = shape_indices[i];
        (*edges)[2 * i].value = min;

        (*edges)[2 * i + 1].is_start = false;
        (*edges)[2 * i + 1].primitive = shape_indices[i];
        (*edges)[2 * i + 1].value = max;
    }

    qsort(*edges, num_entries, sizeof(EDGE), EdgeCompare);

    return ISTATUS_SUCCESS;
}

static
bool
EvaluateSplitsOnAxis(
    _In_reads_(num_edges) const EDGE edges[],
    _In_ size_t num_edges,
    _In_ BOUNDING_BOX node_bound,
    _In_ VECTOR_AXIS axis,
    _Out_ float_t *best_cost,
    _Out_ size_t *best_split
    )
{
    *best_cost = INFINITY;
    *best_split = 0;
    bool result = false;

    float_t box_surface_area = BoundingBoxSurfaceArea(node_bound);
    float_t inv_surface_area = (float_t)1.0 / box_surface_area;

    float_t lower_bound = PointGetElementByAxis(node_bound.corners[0], axis);
    float_t upper_bound = PointGetElementByAxis(node_bound.corners[1], axis);

    VECTOR3 diagonal = PointSubtract(node_bound.corners[1],
                                     node_bound.corners[0]);

    VECTOR_AXIS next_axis = NextAxis(axis);
    VECTOR_AXIS next_next_axis = NextAxis(next_axis);

    float_t other_axis0 = VectorGetElementByAxis(diagonal, next_axis);
    float_t other_axis1 = VectorGetElementByAxis(diagonal, next_next_axis);
    float_t other_sum = other_axis0 + other_axis1;
    float_t side_face_area = other_axis0 * other_axis1;

    size_t num_above = num_edges / 2;
    size_t num_below = 0;

    for (size_t i = 0; i < num_edges; i++)
    {
        if (!edges[i].is_start)
        {
            num_above -= 1;
        }

        if (lower_bound < edges[i].value && edges[i].value < upper_bound)
        {
            float_t below_area =(float_t)2.0 *
                (side_face_area + (edges[i].value - lower_bound) * other_sum);
            float_t above_area = (float_t)2.0 *
                (side_face_area + (upper_bound - edges[i].value) * other_sum);

            float_t percent_below = below_area * inv_surface_area;
            float_t percent_above = above_area * inv_surface_area;

            float_t empty_bonus;
            if (num_above == 0 || num_below == 0)
            {
                empty_bonus = EMPTY_BONUS;
            }
            else
            {
                empty_bonus = (float_t)0.0;
            }

            float_t empty_modifier = (float_t)1.0 - empty_bonus;
            float_t cost = TRAVERSAL_COST +
                INTERSECTION_COST * empty_modifier *
                (percent_below * num_below + percent_above * num_above);

            if (cost < *best_cost)
            {
                result = true;
                *best_cost = cost;
                *best_split = i;
            }
        }

        if (edges[i].is_start)
        {
            num_below += 1;
        }
    }

    return result;
}

static
ISTATUS
UncompressedKdTreeLeafAllocate(
    _Out_ PUNCOMPRESSED_NODE *node,
    _In_reads_(num_indices) const size_t shape_indices[],
    _In_ size_t num_indices
    )
{
    size_t *indices = calloc(num_indices, sizeof(size_t));

    if (indices == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    *node = malloc(sizeof(UNCOMPRESSED_NODE));

    if (*node == NULL)
    {
        free(indices);
        return ISTATUS_ALLOCATION_FAILED;
    }

    memcpy(indices, shape_indices, sizeof(size_t) * num_indices);

    (*node)->is_leaf = true;
    (*node)->data.leaf.indices = indices;
    (*node)->data.leaf.num_indices = num_indices;

    return ISTATUS_SUCCESS;
}

static
void
UncompressedKdTreeFree(
    _Inout_ _Post_invalid_ PUNCOMPRESSED_NODE node
    )
{
    if (node->is_leaf)
    {
        free(node->data.leaf.indices);
    }
    else
    {
        UncompressedKdTreeFree(node->data.interior.below_child);
        UncompressedKdTreeFree(node->data.interior.above_child);
    }

    free(node);
}

static
ISTATUS
UncompressedKdTreeBuildImpl(
    _In_ const BOUNDING_BOX all_bounds[],
    _In_ BOUNDING_BOX node_bounds,
    _In_reads_(num_indices) const size_t shape_indices[],
    _In_ size_t num_indices,
    _In_ size_t depth_remaining,
    _Out_ PUNCOMPRESSED_NODE *node,
    _Out_ size_t *num_nodes,
    _Out_ size_t *index_slots
    )
{
    if (num_indices <= TARGET_LEAF_SIZE || depth_remaining == 0)
    {
        ISTATUS status = UncompressedKdTreeLeafAllocate(node,
                                                        shape_indices,
                                                        num_indices);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }

        *num_nodes += 1;

        if (1 < num_indices)
        {
            *index_slots += num_indices;
        }

        return ISTATUS_SUCCESS;
    }

    float_t best_cost = (float_t)num_indices * INTERSECTION_COST;
    VECTOR_AXIS best_axis = VECTOR_X_AXIS;
    size_t best_split = 0;
    PEDGE best_edges = NULL;
    bool should_split = false;

    VECTOR_AXIS axis = BoundingBoxDominantAxis(node_bounds);

    for (size_t i = 0; i < 3; i++)
    {
        PEDGE edges = NULL;
        ISTATUS status = AllocateEdges(all_bounds,
                                       shape_indices,
                                       num_indices,
                                       axis,
                                       &edges);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }

        float_t cost;
        size_t split;
        bool success = EvaluateSplitsOnAxis(edges,
                                            num_indices * 2,
                                            node_bounds,
                                            axis,
                                            &cost,
                                            &split);

        if (success && cost < best_cost)
        {
            should_split = true;
            free(best_edges);

            best_cost = cost;
            best_axis = axis;
            best_split = split;
            best_edges = edges;
        }
        else
        {
            free(edges);
        }

        axis = NextAxis(axis);
    }

    if (!should_split)
    {
        ISTATUS status = UncompressedKdTreeLeafAllocate(node,
                                                        shape_indices,
                                                        num_indices);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }

        *num_nodes += 1;

        if (1 < num_indices)
        {
            *index_slots += num_indices;
        }

        return ISTATUS_SUCCESS;
    }

    size_t below_shapes = 0;
    for (size_t i = 0; i < best_split; i++)
    {
        if (best_edges[i].is_start)
        {
            below_shapes += 1;
        }
    }

    size_t above_shapes = 0;
    for (size_t i = best_split; i < num_indices * 2; i++)
    {
        if (!best_edges[i].is_start)
        {
            above_shapes += 1;
        }
    }

    size_t *below_indices = calloc(below_shapes, sizeof(size_t));
    if (below_indices == NULL)
    {
        free(best_edges);
        return ISTATUS_ALLOCATION_FAILED;
    }

    size_t *above_indices = calloc(above_shapes, sizeof(size_t));
    if (above_indices == NULL)
    {
        free(best_edges);
        free(below_indices);
        return ISTATUS_ALLOCATION_FAILED;
    }

    size_t below_index = 0;
    for (size_t i = 0; i < best_split; i++)
    {
        if (best_edges[i].is_start)
        {
            below_indices[below_index++] = best_edges[i].primitive;
        }
    }

    size_t above_index = 0;
    for (size_t i = best_split; i < num_indices * 2; i++)
    {
        if (!best_edges[i].is_start)
        {
            above_indices[above_index++] = best_edges[i].primitive;
        }
    }

    float_t split = best_edges[best_split].value;

    free(best_edges);

    BOUNDING_BOX below_bounds = node_bounds;
    BOUNDING_BOX above_bounds = node_bounds;

    switch (best_axis)
    {
        case VECTOR_X_AXIS:
            below_bounds.corners[1].x = split;
            above_bounds.corners[0].x = split;
            break;
        case VECTOR_Y_AXIS:
            below_bounds.corners[1].y = split;
            above_bounds.corners[0].y = split;
            break;
        default:
            below_bounds.corners[1].z = split;
            above_bounds.corners[0].z = split;
            break;
    }

    PUNCOMPRESSED_NODE below_node;
    ISTATUS status = UncompressedKdTreeBuildImpl(all_bounds,
                                                 below_bounds,
                                                 below_indices,
                                                 below_shapes,
                                                 depth_remaining - 1,
                                                 &below_node,
                                                 num_nodes,
                                                 index_slots);

    free(below_indices);

    if (status != ISTATUS_SUCCESS)
    {
        free(above_indices);
        return status;
    }

    PUNCOMPRESSED_NODE above_node;
    status = UncompressedKdTreeBuildImpl(all_bounds,
                                         above_bounds,
                                         above_indices,
                                         above_shapes,
                                         depth_remaining - 1,
                                         &above_node,
                                         num_nodes,
                                         index_slots);

    free(above_indices);

    if (status != ISTATUS_SUCCESS)
    {
        UncompressedKdTreeFree(below_node);
        return status;
    }

    *node = malloc(sizeof(UNCOMPRESSED_NODE));

    if (*node == NULL)
    {
        UncompressedKdTreeFree(below_node);
        UncompressedKdTreeFree(above_node);
        return ISTATUS_ALLOCATION_FAILED;
    }

    *num_nodes += 1;

    (*node)->is_leaf = false;
    (*node)->data.interior.split = split;
    (*node)->data.interior.split_axis = best_axis;
    (*node)->data.interior.below_child = below_node;
    (*node)->data.interior.above_child = above_node;

    return ISTATUS_SUCCESS;
}

static
ISTATUS
UncompressedKdTreeBuild(
    _In_reads_(num_shapes) const PSHAPE shapes[],
    _In_reads_(num_shapes) const PMATRIX transforms[],
    _In_reads_(num_shapes) const bool premultiplied[],
    _In_ size_t num_shapes,
    _In_ size_t max_depth,
    _Out_ PBOUNDING_BOX scene_bounds,
    _Out_ PUNCOMPRESSED_NODE *uncompressed_tree,
    _Out_ size_t *num_nodes,
    _Out_ size_t *num_indices
    )
{
    PBOUNDING_BOX all_bounds = calloc(num_shapes, sizeof(BOUNDING_BOX));

    if (all_bounds == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    ISTATUS status = ComputeBounds(shapes,
                                   transforms,
                                   premultiplied,
                                   num_shapes,
                                   all_bounds,
                                   scene_bounds);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    size_t *indices = calloc(num_shapes, sizeof(size_t));

    if (indices == NULL)
    {
        free(all_bounds);
        return ISTATUS_ALLOCATION_FAILED;
    }

    for (size_t i = 0; i < num_shapes; i++)
    {
        indices[i] = i;
    }

    *num_nodes = 0;
    *num_indices = 0;
    status = UncompressedKdTreeBuildImpl(all_bounds,
                                         *scene_bounds,
                                         indices,
                                         num_shapes,
                                         max_depth,
                                         uncompressed_tree,
                                         num_nodes,
                                         num_indices);

    free(all_bounds);
    free(indices);

    return status;
}

//
// Compressed Tree Defines
//

#define MAX_CHILD_OFFSET (UINT32_MAX >> 2)
#define MAX_LEAF_SIZE    (UINT32_MAX >> 2)
#define INTERIOR_X_SPLIT 0U
#define INTERIOR_Y_SPLIT 1U
#define INTERIOR_Z_SPLIT 2U
#define LEAF             3U

//
// Compressed Tree Types
//

typedef union _SPLIT_OR_INDEX {
    float split;
    uint32_t index;
} SPLIT_OR_INDEX, *PSPLIT_OR_INDEX;

typedef struct _KD_TREE_NODE {
    uint32_t flags_and_num_shapes_or_child;
    SPLIT_OR_INDEX split_or_index;
} KD_TREE_NODE, *PKD_TREE_NODE;

typedef const KD_TREE_NODE *PCKD_TREE_NODE;

//
// Compressed Tree Static Functions
//

static
inline
ISTATUS
KdTreeInitializeLeaf(
    _Inout_ PKD_TREE_NODE node,
    _In_ size_t size,
    _In_ size_t index
    )
{
    if (MAX_LEAF_SIZE < size)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    if (UINT32_MAX < index)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    node->flags_and_num_shapes_or_child = size | (LEAF << 30);
    node->split_or_index.index = index;

    return ISTATUS_SUCCESS;
}

static
inline
ISTATUS
KdTreeInitializeInterior(
    _Inout_ PKD_TREE_NODE node,
    _In_ VECTOR_AXIS axis,
    _In_ size_t child,
    _In_ float_t split
    )
{
    if (MAX_CHILD_OFFSET < child)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    switch (axis)
    {
        case VECTOR_X_AXIS:
            child |= INTERIOR_X_SPLIT << 30;
            break;
        case VECTOR_Y_AXIS:
            child |= INTERIOR_Y_SPLIT << 30;
            break;
        case VECTOR_Z_AXIS:
            child |= INTERIOR_Z_SPLIT << 30;
            break;
        default:
            return ISTATUS_ALLOCATION_FAILED;
    }

    node->flags_and_num_shapes_or_child = (uint32_t)child;
    node->split_or_index.split = split;

    return ISTATUS_SUCCESS;
}

static
ISTATUS
KdTreeBuildImpl(
    _In_ PCUNCOMPRESSED_NODE uncompressed_node,
    _In_ const uint32_t *index_base,
    _Inout_ PKD_TREE_NODE *next_node,
    _Inout_ uint32_t **next_index
    )
{
    PKD_TREE_NODE current = *next_node;
    *next_node += 1;

    if (uncompressed_node->is_leaf)
    {
        if (uncompressed_node->data.leaf.num_indices == 1)
        {
            size_t offset = uncompressed_node->data.leaf.indices[0];
            ISTATUS status = KdTreeInitializeLeaf(current,
                                                  1,
                                                  offset);

            return status;
        }

        size_t num_indices = uncompressed_node->data.leaf.num_indices;
        size_t offset = *next_index - index_base;
        for (size_t i = 0; i < num_indices; i++)
        {
            if (UINT32_MAX < uncompressed_node->data.leaf.indices[i])
            {
                return ISTATUS_ALLOCATION_FAILED;
            }

            **next_index = (uint32_t)uncompressed_node->data.leaf.indices[i];
            *next_index += 1;
        }

        ISTATUS status = KdTreeInitializeLeaf(current,
                                              num_indices,
                                              offset);

        return status;
    }

    ISTATUS status =
        KdTreeBuildImpl(uncompressed_node->data.interior.below_child,
                        index_base,
                        next_node,
                        next_index);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    status =
        KdTreeInitializeInterior(current,
                                 uncompressed_node->data.interior.split_axis,
                                 *next_node - current,
                                 uncompressed_node->data.interior.split);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    status =
        KdTreeBuildImpl(uncompressed_node->data.interior.above_child,
                        index_base,
                        next_node,
                        next_index);

    return status;
}

static
ISTATUS
KdTreeBuild(
    _In_ PCUNCOMPRESSED_NODE uncompressed_node,
    _Inout_ KD_TREE_NODE nodes[],
    _Inout_ uint32_t indices[]
    )
{
    ISTATUS status = KdTreeBuildImpl(uncompressed_node,
                                     indices,
                                     &nodes,
                                     &indices);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    return status;
}

//
// Scene Defines
//

#define DESIRED_ALIGNMENT 128
#define MAX_TREE_DEPTH    64

//
// Scene Types
//

typedef struct _SHAPE_AND_DATA {
    PSHAPE shape;
    PMATRIX model_to_world;
    bool premultiplied;
} SHAPE_AND_DATA, *PSHAPE_AND_DATA;

typedef const SHAPE_AND_DATA *PCSHAPE_AND_DATA;

typedef struct _SHAPE_AND_TRANSFORM {
    PSHAPE shape;
    PMATRIX model_to_world;
} SHAPE_AND_TRANSFORM, *PSHAPE_AND_TRANSFORM;

typedef const SHAPE_AND_TRANSFORM *PCSHAPE_AND_TRANSFORM;

typedef union _SHAPES_POINTER {
    PSHAPE_AND_DATA shape_and_data;
    PSHAPE_AND_TRANSFORM shape_and_transform;
    PSHAPE *shapes;
} SHAPES_POINTER, *PSHAPES_POINTER;

typedef const SHAPES_POINTER *PCSHAPES_POINTER;

typedef struct _KD_TREE_SCENE {
    KD_TREE_NODE *nodes;
    uint32_t *indices;
    SHAPES_POINTER shapes;
    uint32_t num_shapes;
    BOUNDING_BOX scene_bounds;
} KD_TREE_SCENE, *PKD_TREE_SCENE;

typedef const KD_TREE_SCENE *PCKD_TREE_SCENE;

typedef struct _WORK_ITEM {
    PCKD_TREE_NODE node;
    float_t min;
    float_t max;
} WORK_ITEM, *PWORK_ITEM;

//
// Scene Static Functions
//

static
inline
float_t
PointGetElement(
    _In_ POINT3 point,
    _In_ uint32_t axis
    )
{
    const float_t *elements = &point.x;
    return elements[axis];
}

static
inline
float_t
VectorGetElement(
    _In_ VECTOR3 vector,
    _In_ uint32_t axis
    )
{
    const float_t *elements = &vector.x;
    return elements[axis];
}

static
inline
uint32_t
KdTreeNodeType(
    _In_ PCKD_TREE_NODE node
    )
{
    return node->flags_and_num_shapes_or_child >> 30;
}

static
inline
bool
KdTreeNodeIsLeaf(
    _In_ PCKD_TREE_NODE node
    )
{
    return node->flags_and_num_shapes_or_child >= (LEAF << 30);
}

static
inline
uint32_t
KdTreeLeafSize(
    _In_ PCKD_TREE_NODE node
    )
{
    assert(KdTreeNodeIsLeaf(node));

    return node->flags_and_num_shapes_or_child & ~(LEAF << 30);
}

static
inline
float_t
KdTreeSplit(
    _In_ PCKD_TREE_NODE node
    )
{
    assert(!KdTreeNodeIsLeaf(node));

    return node->split_or_index.split;
}

static
inline
uint32_t
KdTreeChildIndex(
    _In_ PCKD_TREE_NODE node
    )
{
    assert(!KdTreeNodeIsLeaf(node));

    return node->flags_and_num_shapes_or_child & ~(LEAF << 30);
}

//
// Scene Functions
//

static
ISTATUS
KdTreeSceneTrace(
    _In_opt_ const void *context,
    _Inout_ PSHAPE_HIT_TESTER hit_tester,
    _In_ RAY ray
    )
{
    PCKD_TREE_SCENE kd_tree = (PCKD_TREE_SCENE)context;
    PCKD_TREE_NODE node = kd_tree->nodes;
    const uint32_t *all_indices = kd_tree->indices;
    PCSHAPE_AND_DATA shapes = kd_tree->shapes.shape_and_data;

    VECTOR3 inverse_direction;
    float_t node_min, node_max;
    bool intersects = BoundingBoxIntersect(kd_tree->scene_bounds,
                                           ray,
                                           &inverse_direction,
                                           &node_min,
                                           &node_max);

    if (!intersects)
    {
        return ISTATUS_SUCCESS;
    }

    float_t farthest_object_allowed;
    ShapeHitTesterFarthestHitAllowed(hit_tester, &farthest_object_allowed);

    WORK_ITEM work_queue[MAX_TREE_DEPTH];
    size_t queue_size = 0;
    for (;;)
    {
        if (farthest_object_allowed < node_min)
        {
            break;
        }

        if (KdTreeNodeIsLeaf(node))
        {
            uint32_t num_shapes = KdTreeLeafSize(node);

            if (num_shapes == 1)
            {
                uint32_t index = node->split_or_index.index;
                PCSHAPE_AND_DATA shape = shapes + index;
                ISTATUS status =
                    ShapeHitTesterTestShapeWithLimit(hit_tester,
                                                     shape->shape,
                                                     shape->model_to_world,
                                                     shape->premultiplied,
                                                     &farthest_object_allowed);

                if (status != ISTATUS_SUCCESS)
                {
                    return status;
                }
            }
            else if (num_shapes != 0)
            {
                uint32_t index = node->split_or_index.index;
                const uint32_t *node_indices = all_indices + index;
                for (uint32_t i = 0; i < num_shapes; i++)
                {
                    PCSHAPE_AND_DATA shape = shapes + node_indices[i];
                    ISTATUS status =
                        ShapeHitTesterTestShapeWithLimit(hit_tester,
                                                         shape->shape,
                                                         shape->model_to_world,
                                                         shape->premultiplied,
                                                         &farthest_object_allowed);

                    if (status != ISTATUS_SUCCESS)
                    {
                        return status;
                    }
                }
            }

            if (queue_size == 0)
            {
                break;
            }

            queue_size -= 1;
            node = work_queue[queue_size].node;
            node_min = work_queue[queue_size].min;
            node_max = work_queue[queue_size].max;
            continue;
        }

        uint32_t split_axis = KdTreeNodeType(node);
        float_t origin = PointGetElement(ray.origin, split_axis);
        float_t direction = VectorGetElement(inverse_direction, split_axis);

        float_t split = KdTreeSplit(node);
        float_t plane_distance = (split - origin) * direction;

        PCKD_TREE_NODE below_child = node + 1;
        PCKD_TREE_NODE above_child = node + KdTreeChildIndex(node);

        PCKD_TREE_NODE close_child, far_child;
        if (origin < split || (origin == split && direction <= (float_t)0.0))
        {
            close_child = below_child;
            far_child = above_child;
        }
        else
        {
            close_child = above_child;
            far_child = below_child;
        }

        if (node_max < plane_distance || plane_distance <= (float_t)0.0)
        {
            node = close_child;
        }
        else if (plane_distance < node_min)
        {
            node = far_child;
        }
        else
        {
            work_queue[queue_size].node = far_child;
            work_queue[queue_size].min = plane_distance;
            work_queue[queue_size].max = node_max;
            queue_size += 1;

            node = close_child;
            node_max = plane_distance;
        }
    }

    return ISTATUS_SUCCESS;
}

static
void
KdTreeSceneFree(
    _In_opt_ _Post_invalid_ void *context
    )
{
    PKD_TREE_SCENE kd_tree_scene = (PKD_TREE_SCENE)context;

    for (size_t i = 0; i < kd_tree_scene->num_shapes; i++)
    {
        ShapeRelease(kd_tree_scene->shapes.shape_and_data[i].shape);
        MatrixRelease(kd_tree_scene->shapes.shape_and_data[i].model_to_world);
    }

    free(kd_tree_scene->nodes);
    free(kd_tree_scene->indices);
    free(kd_tree_scene->shapes.shape_and_data);
}

static
ISTATUS
KdTreeTransformedSceneTrace(
    _In_opt_ const void *context,
    _Inout_ PSHAPE_HIT_TESTER hit_tester,
    _In_ RAY ray
    )
{
    PCKD_TREE_SCENE kd_tree = (PCKD_TREE_SCENE)context;
    PCKD_TREE_NODE node = kd_tree->nodes;
    const uint32_t *all_indices = kd_tree->indices;
    PCSHAPE_AND_TRANSFORM shapes = kd_tree->shapes.shape_and_transform;

    VECTOR3 inverse_direction;
    float_t node_min, node_max;
    bool intersects = BoundingBoxIntersect(kd_tree->scene_bounds,
                                           ray,
                                           &inverse_direction,
                                           &node_min,
                                           &node_max);

    if (!intersects)
    {
        return ISTATUS_SUCCESS;
    }

    float_t farthest_object_allowed;
    HitTesterFarthestHitAllowed(hit_tester, &farthest_object_allowed);

    WORK_ITEM work_queue[MAX_TREE_DEPTH];
    size_t queue_size = 0;
    for (;;)
    {
        if (farthest_object_allowed < node_min)
        {
            break;
        }

        if (KdTreeNodeIsLeaf(node))
        {
            uint32_t num_shapes = KdTreeLeafSize(node);

            if (num_shapes == 1)
            {
                uint32_t index = node->split_or_index.index;
                PCSHAPE_AND_TRANSFORM shape = shapes + index;
                ISTATUS status =
                    ShapeHitTesterTestTransformedShapeWithLimit(hit_tester,
                                                                shape->shape,
                                                                shape->model_to_world,
                                                                &farthest_object_allowed);

                if (status != ISTATUS_SUCCESS)
                {
                    return status;
                }
            }
            else if (num_shapes != 0)
            {
                uint32_t index = node->split_or_index.index;
                const uint32_t *node_indices = all_indices + index;
                for (uint32_t i = 0; i < num_shapes; i++)
                {
                    PCSHAPE_AND_TRANSFORM shape = shapes + node_indices[i];
                    ISTATUS status =
                        ShapeHitTesterTestTransformedShapeWithLimit(hit_tester,
                                                                    shape->shape,
                                                                    shape->model_to_world,
                                                                    &farthest_object_allowed);

                    if (status != ISTATUS_SUCCESS)
                    {
                        return status;
                    }
                }
            }

            if (queue_size == 0)
            {
                break;
            }

            queue_size -= 1;
            node = work_queue[queue_size].node;
            node_min = work_queue[queue_size].min;
            node_max = work_queue[queue_size].max;
            continue;
        }

        uint32_t split_axis = KdTreeNodeType(node);
        float_t origin = PointGetElement(ray.origin, split_axis);
        float_t direction = VectorGetElement(inverse_direction, split_axis);

        float_t split = KdTreeSplit(node);
        float_t plane_distance = (split - origin) * direction;

        PCKD_TREE_NODE below_child = node + 1;
        PCKD_TREE_NODE above_child = node + KdTreeChildIndex(node);

        PCKD_TREE_NODE close_child, far_child;
        if (origin < split || (origin == split && direction <= (float_t)0.0))
        {
            close_child = below_child;
            far_child = above_child;
        }
        else
        {
            close_child = above_child;
            far_child = below_child;
        }

        if (node_max < plane_distance || plane_distance <= (float_t)0.0)
        {
            node = close_child;
        }
        else if (plane_distance < node_min)
        {
            node = far_child;
        }
        else
        {
            work_queue[queue_size].node = far_child;
            work_queue[queue_size].min = plane_distance;
            work_queue[queue_size].max = node_max;
            queue_size += 1;

            node = close_child;
            node_max = plane_distance;
        }
    }

    return ISTATUS_SUCCESS;
}

static
void
KdTreeTransformedSceneFree(
    _In_opt_ _Post_invalid_ void *context
    )
{
    PKD_TREE_SCENE kd_tree_scene = (PKD_TREE_SCENE)context;

    for (size_t i = 0; i < kd_tree_scene->num_shapes; i++)
    {
        ShapeRelease(kd_tree_scene->shapes.shape_and_transform[i].shape);
        MatrixRelease(kd_tree_scene->shapes.shape_and_transform[i].model_to_world);
    }

    free(kd_tree_scene->nodes);
    free(kd_tree_scene->indices);
    free(kd_tree_scene->shapes.shape_and_transform);
}

static
ISTATUS
KdTreeWorldSceneTrace(
    _In_opt_ const void *context,
    _Inout_ PSHAPE_HIT_TESTER hit_tester,
    _In_ RAY ray
    )
{
    PCKD_TREE_SCENE kd_tree = (PCKD_TREE_SCENE)context;
    PCKD_TREE_NODE node = kd_tree->nodes;
    const uint32_t *all_indices = kd_tree->indices;
    SHAPE **const shapes = kd_tree->shapes.shapes;\

    VECTOR3 inverse_direction;
    float_t node_min, node_max;
    bool intersects = BoundingBoxIntersect(kd_tree->scene_bounds,
                                           ray,
                                           &inverse_direction,
                                           &node_min,
                                           &node_max);

    if (!intersects)
    {
        return ISTATUS_SUCCESS;
    }

    float_t farthest_object_allowed;
    ShapeHitTesterFarthestHitAllowed(hit_tester, &farthest_object_allowed);

    WORK_ITEM work_queue[MAX_TREE_DEPTH];
    size_t queue_size = 0;
    for (;;)
    {
        if (farthest_object_allowed < node_min)
        {
            break;
        }

        if (KdTreeNodeIsLeaf(node))
        {
            uint32_t num_shapes = KdTreeLeafSize(node);

            if (num_shapes == 1)
            {
                uint32_t index = node->split_or_index.index;
                ISTATUS status =
                    ShapeHitTesterTestWorldShapeWithLimit(hit_tester,
                                                          shapes[index],
                                                          &farthest_object_allowed);

                if (status != ISTATUS_SUCCESS)
                {
                    return status;
                }
            }
            else if (num_shapes != 0)
            {
                uint32_t index = node->split_or_index.index;
                const uint32_t *node_indices = all_indices + index;
                for (uint32_t i = 0; i < num_shapes; i++)
                {
                    ISTATUS status =
                        ShapeHitTesterTestWorldShapeWithLimit(hit_tester,
                                                              shapes[node_indices[i]],
                                                              &farthest_object_allowed);

                    if (status != ISTATUS_SUCCESS)
                    {
                        return status;
                    }
                }
            }

            if (queue_size == 0)
            {
                break;
            }

            queue_size -= 1;
            node = work_queue[queue_size].node;
            node_min = work_queue[queue_size].min;
            node_max = work_queue[queue_size].max;
            continue;
        }

        uint32_t split_axis = KdTreeNodeType(node);
        float_t origin = PointGetElement(ray.origin, split_axis);
        float_t direction = VectorGetElement(inverse_direction, split_axis);

        float_t split = KdTreeSplit(node);
        float_t plane_distance = (split - origin) * direction;

        PCKD_TREE_NODE below_child = node + 1;
        PCKD_TREE_NODE above_child = node + KdTreeChildIndex(node);

        PCKD_TREE_NODE close_child, far_child;
        if (origin < split || (origin == split && direction <= (float_t)0.0))
        {
            close_child = below_child;
            far_child = above_child;
        }
        else
        {
            close_child = above_child;
            far_child = below_child;
        }

        if (node_max < plane_distance || plane_distance <= (float_t)0.0)
        {
            node = close_child;
        }
        else if (plane_distance < node_min)
        {
            node = far_child;
        }
        else
        {
            work_queue[queue_size].node = far_child;
            work_queue[queue_size].min = plane_distance;
            work_queue[queue_size].max = node_max;
            queue_size += 1;

            node = close_child;
            node_max = plane_distance;
        }
    }

    return ISTATUS_SUCCESS;
}

static
void
KdTreeWorldSceneFree(
    _In_opt_ _Post_invalid_ void *context
    )
{
    PKD_TREE_SCENE kd_tree_scene = (PKD_TREE_SCENE)context;

    for (size_t i = 0; i < kd_tree_scene->num_shapes; i++)
    {
        ShapeRelease(kd_tree_scene->shapes.shapes[i]);
    }

    free(kd_tree_scene->nodes);
    free(kd_tree_scene->indices);
    free(kd_tree_scene->shapes.shapes);
}

//
// Static Data
//

static const SCENE_VTABLE kd_tree_scene_vtable = {
    KdTreeSceneTrace,
    KdTreeSceneFree
};

static const SCENE_VTABLE kd_tree_transformed_scene_vtable = {
    KdTreeTransformedSceneTrace,
    KdTreeTransformedSceneFree
};

static const SCENE_VTABLE kd_tree_world_scene_vtable = {
    KdTreeWorldSceneTrace,
    KdTreeWorldSceneFree
};

//
// Static Functions
//

static
size_t
Log2(
    _In_ size_t value
    )
{
    for (size_t i = (sizeof(size_t) * 8); i != 0; i--)
    {
        if (value & ((size_t)1 << ((sizeof(size_t) * 8) - 1)))
        {
            return i - 1;
        }

        value <<= 1;
    }

    return 0;
}

//
// Public Functions
//

ISTATUS
KdTreeSceneAllocate(
    _In_reads_(num_shapes) const PSHAPE shapes[],
    _In_reads_(num_shapes) const PMATRIX transforms[],
    _In_reads_(num_shapes) const bool premultiplied[],
    _In_ size_t num_shapes,
    _In_opt_ PENVIRONMENTAL_LIGHT environment,
    _Out_ PSCENE *scene
    )
{
    if (shapes == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    for (size_t i = 0; i < num_shapes; i++)
    {
        if (shapes[i] == NULL)
        {
            return ISTATUS_INVALID_ARGUMENT_00;
        }
    }

    if (transforms == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (premultiplied == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (scene == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    float_t max_depth =
        round((float_t)8.0 + (float_t)1.3 * (float_t)Log2(num_shapes));
    max_depth = IMin((float_t)MAX_TREE_DEPTH, max_depth);

    BOUNDING_BOX scene_bounds;
    PUNCOMPRESSED_NODE uncompressed_node;
    size_t num_nodes, num_indices;
    ISTATUS status = UncompressedKdTreeBuild(shapes,
                                             transforms,
                                             premultiplied,
                                             num_shapes,
                                             (size_t)max_depth,
                                             &scene_bounds,
                                             &uncompressed_node,
                                             &num_nodes,
                                             &num_indices);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    size_t num_node_bytes;
    bool success = CheckedMultiplySizeT(num_nodes,
                                        sizeof(KD_TREE_NODE),
                                        &num_node_bytes);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    PKD_TREE_NODE nodes = aligned_alloc(DESIRED_ALIGNMENT, num_node_bytes);

    if (nodes == NULL)
    {
        UncompressedKdTreeFree(uncompressed_node);
        return ISTATUS_ALLOCATION_FAILED;
    }

    size_t num_index_bytes;
    success = CheckedMultiplySizeT(num_indices,
                                   sizeof(uint32_t),
                                   &num_index_bytes);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    uint32_t *indices = aligned_alloc(DESIRED_ALIGNMENT, num_index_bytes);

    if (indices == NULL)
    {
        UncompressedKdTreeFree(uncompressed_node);
        free(nodes);
        return ISTATUS_ALLOCATION_FAILED;
    }

    status = KdTreeBuild(uncompressed_node,
                         nodes,
                         indices);

    UncompressedKdTreeFree(uncompressed_node);

    if (status != ISTATUS_SUCCESS)
    {
        free(nodes);
        free(indices);
        return status;
    }

    bool premultiply_needed = false;
    bool transform_needed = false;
    for (size_t i = 0; i < num_shapes; i++)
    {
        if (premultiplied[i])
        {
            premultiply_needed = true;
        }

        if (transforms[i] != NULL)
        {
            transform_needed = true;
        }

        if (premultiply_needed && transform_needed)
        {
            break;
        }
    }

    KD_TREE_SCENE result;
    result.nodes = nodes;
    result.indices = indices;
    result.num_shapes = num_shapes;
    result.scene_bounds = scene_bounds;

    PCSCENE_VTABLE vtable;
    void *data;
    if (!transform_needed)
    {
        data = calloc(num_shapes, sizeof(PSHAPE));

        if (data == NULL)
        {
            free(nodes);
            free(indices);
            return ISTATUS_ALLOCATION_FAILED;
        }

        vtable = &kd_tree_world_scene_vtable;
        result.shapes.shapes = data;
        memcpy(data, shapes, sizeof(PSHAPE) * num_shapes);
    }
    else if (!premultiply_needed)
    {
        data = calloc(num_shapes, sizeof(SHAPE_AND_TRANSFORM));

        if (data == NULL)
        {
            free(nodes);
            free(indices);
            return ISTATUS_ALLOCATION_FAILED;
        }

        vtable = &kd_tree_transformed_scene_vtable;
        result.shapes.shape_and_transform = data;

        for (size_t i = 0; i < num_shapes; i++)
        {
            result.shapes.shape_and_transform[i].shape = shapes[i];
            result.shapes.shape_and_transform[i].model_to_world = transforms[i];
        }
    }
    else
    {
        data = calloc(num_shapes, sizeof(SHAPE_AND_DATA));

        if (data == NULL)
        {
            free(nodes);
            free(indices);
            return ISTATUS_ALLOCATION_FAILED;
        }

        result.shapes.shape_and_data = data;
        vtable = &kd_tree_scene_vtable;

        for (size_t i = 0; i < num_shapes; i++)
        {
            result.shapes.shape_and_data[i].shape = shapes[i];
            result.shapes.shape_and_data[i].model_to_world = transforms[i];
            result.shapes.shape_and_data[i].premultiplied = premultiplied[i];
        }
    }

    status = SceneAllocate(vtable,
                           &result,
                           sizeof(KD_TREE_SCENE),
                           alignof(KD_TREE_SCENE),
                           environment,
                           scene);

    if (status != ISTATUS_SUCCESS)
    {
        free(nodes);
        free(indices);
        free(data);
        return status;
    }

    for (size_t i = 0; i < num_shapes; i++)
    {
        ShapeRetain(shapes[i]);
        MatrixRetain(transforms[i]);
    }

    return ISTATUS_SUCCESS;
}