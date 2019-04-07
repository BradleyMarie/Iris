/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    kd_tree_scene.c

Abstract:

    Implements a kd-tree scene.

--*/

#include "common/pointer_list.h"
#include "common/safe_math.h"
#include "iris_physx_toolkit/kd_tree_scene.h"

//
// Uncompressed Tree Types
//

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
    PUNCOMPRESSED_NODE left_child;
    PUNCOMPRESSED_NODE right_child;
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
ISTATUS
ComputeShapeBounds(
    _In_reads_(num_shapes) const PSHAPE shapes[],
    _In_reads_(num_shapes) const PMATRIX transforms[],
    _In_reads_(num_shapes) const bool premultiplied[],
    _In_ size_t num_shapes,
    _Out_ PBOUNDING_BOX bounds
    )
{
    PCMATRIX transform = premultiplied[0] ? NULL : transforms[0];
    ISTATUS status = ShapeComputeBounds(shapes[0],
                                        transform,
                                        bounds);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    for (size_t i = 1; i < num_shapes; i++)
    {
        BOUNDING_BOX shape_bounds;
        transform = premultiplied[i] ? NULL : transforms[i];
        status = ShapeComputeBounds(shapes[i],
                                    transform,
                                    &shape_bounds);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }

        *bounds = BoundingBoxUnion(*bounds, shape_bounds);
    }

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
        UncompressedKdTreeFree(node->data.interior.left_child);
        UncompressedKdTreeFree(node->data.interior.right_child);
    }

    free(node);
}

static
ISTATUS
UncompressedKdTreeBuildImpl(
    _In_reads_(num_shapes) const PSHAPE shapes[],
    _In_reads_(num_shapes) const PMATRIX transforms[],
    _In_reads_(num_shapes) const bool premultiplied[],
    _In_ BOUNDING_BOX bounds,
    _In_reads_(num_indices) size_t *shape_indices,
    _In_ size_t num_indices,
    _In_ size_t depth_remaining,
    _Out_ PUNCOMPRESSED_NODE *node,
    _Out_ size_t *num_nodes,
    _Out_ size_t *index_slots
    )
{
    // TODO
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
    _Out_ PBOUNDING_BOX bounds,
    _Out_ PUNCOMPRESSED_NODE *uncompressed_tree,
    _Out_ size_t *num_nodes,
    _Out_ size_t *num_indices
    )
{
    ISTATUS status = ComputeShapeBounds(shapes,
                                        transforms,
                                        premultiplied,
                                        num_shapes,
                                        bounds);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    size_t *indices = (size_t*)calloc(num_shapes, sizeof(size_t));

    if (indices == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    for (size_t i = 0; i < num_shapes; i++)
    {
        indices[i] = i;
    }

    status = UncompressedKdTreeBuildImpl(shapes,
                                         transforms,
                                         premultiplied,
                                         *bounds,
                                         indices,
                                         num_shapes,
                                         max_depth,
                                         uncompressed_tree,
                                         num_nodes,
                                         num_indices);

    free(indices);

    return ISTATUS_SUCCESS;
}

//
// Compressed Tree Defines
//

#define MAX_CHILD_OFFSET (UINT32_MAX >> 2)
#define MAX_LEAF_SIZE    (UINT32_MAX >> 2)
#define INTERIOR_X_SPLIT 0u
#define INTERIOR_Y_SPLIT 1u
#define INTERIOR_Z_SPLIT 2u
#define LEAF             3u

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
    _In_ const KD_TREE_NODE nodes[],
    _In_ const uint32_t indices[],
    _Inout_ PKD_TREE_NODE *next_node,
    _Inout_ uint32_t **next_index
    )
{
    size_t current_index = *next_node - nodes;
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
        size_t offset = *next_index - indices;
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
        KdTreeBuildImpl(uncompressed_node->data.interior.left_child,
                        nodes,
                        indices,
                        next_node,
                        next_index);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    status =
        KdTreeInitializeInterior(current,
                                 uncompressed_node->data.interior.split_axis,
                                 (*next_node - nodes) - current_index,
                                 uncompressed_node->data.interior.split);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    status =
        KdTreeBuildImpl(uncompressed_node->data.interior.right_child,
                        nodes,
                        indices,
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
                                     nodes,
                                     indices,
                                     &nodes,
                                     &indices);

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

struct _KD_TREE_SCENE {
    KD_TREE_NODE *nodes;
    uint32_t *indices;
    PSHAPE_AND_DATA shapes;
    uint32_t num_shapes;
    BOUNDING_BOX scene_bounds;
};

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
VectorGetElement(
    _In_ VECTOR3 vector,
    _In_ uint32_t axis
    )
{
    float_t *elements = &vector.x;
    return elements[axis];
}

static
inline
float_t
PointGetElement(
    _In_ POINT3 point,
    _In_ uint32_t axis
    )
{
    float_t *elements = &point.x;
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

static
inline
ISTATUS
KdTreeTraceShape(
    _Inout_ PSHAPE_HIT_TESTER hit_tester,
    _In_ PCSHAPE_AND_DATA shape_and_data
    )
{
    ISTATUS status = ShapeHitTesterTestShape(hit_tester,
                                             shape_and_data->shape,
                                             shape_and_data->model_to_world,
                                             shape_and_data->premultiplied);

    return status;
}

static
inline
ISTATUS
KdTreeTraceTree(
    _In_ const KD_TREE_NODE node[],
    _In_ const uint32_t indices[],
    _In_ const SHAPE_AND_DATA shapes[],
    _Inout_ PSHAPE_HIT_TESTER hit_tester,
    _In_ POINT3 ray_origin,
    _In_ VECTOR3 inverse_direction
    )
{
    WORK_ITEM work_queue[MAX_TREE_DEPTH];
    size_t queue_size = 0;

    float_t min = (float_t)0.0;
    float_t max = (float_t)INFINITY;

    for (;;)
    {
        float_t closest_hit;
        ShapeHitTesterClosestHit(hit_tester, &closest_hit);

        if (closest_hit < min)
        {
            break;
        }

        if (KdTreeNodeIsLeaf(node))
        {
            uint32_t num_shapes = KdTreeLeafSize(node);

            if (num_shapes == 0)
            {
                return ISTATUS_SUCCESS;
            }

            uint32_t index = node->split_or_index.index;

            if (num_shapes == 1)
            {
                ISTATUS status = KdTreeTraceShape(hit_tester,
                                                  shapes + index);
                return status;
            }

            uint32_t *indices = indices + index;
            for (uint32_t i = 0; i < num_shapes; i++)
            {
                ISTATUS status = KdTreeTraceShape(hit_tester,
                                                  shapes + indices[i]);

                if (status != ISTATUS_SUCCESS)
                {
                    return status;
                }
            }

            if (queue_size == 0)
            {
                break;
            }

            queue_size -= 1;
            node = work_queue[queue_size].node;
            min = work_queue[queue_size].min;
            max = work_queue[queue_size].max;
            continue;
        }

        uint32_t split_axis = KdTreeNodeType(node);
        float_t origin = PointGetElement(ray_origin, split_axis);
        float_t direction = VectorGetElement(inverse_direction, split_axis);

        float_t split = KdTreeSplit(node);
        float_t plane_distance = (split - origin) * direction;

        uint32_t child_offset = KdTreeChildIndex(node);

        PCKD_TREE_NODE close_child, far_child;
        if (origin < split || (origin == split && direction <= (float_t)0.0))
        {
            close_child = node + 1;
            far_child = node + child_offset;
        }
        else
        {
            close_child = node + child_offset;
            far_child = node + 1;
        }

        if (max < plane_distance || plane_distance <= (float_t)0.0)
        {
            node = close_child;
        }
        else if (plane_distance < min)
        {
            node = far_child;
        }
        else
        {
            work_queue[queue_size].node = far_child;
            work_queue[queue_size].min = plane_distance;
            work_queue[queue_size].max = max;
            queue_size += 1;

            node = close_child;
            max = plane_distance;
        }
    }

    return ISTATUS_SUCCESS;
}

//
// Scene Functions
//

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

ISTATUS
KdTreeSceneAllocate(
    _In_reads_(num_shapes) const PSHAPE shapes[],
    _In_reads_(num_shapes) const PMATRIX transforms[],
    _In_reads_(num_shapes) const bool premultiplied[],
    _In_ size_t num_shapes,
    _Out_ PKD_TREE_SCENE *kd_tree_scene
    )
{
    if (shapes == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (transforms == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (premultiplied == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (kd_tree_scene == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    float_t max_depth =
        round((float_t)8.0 + (float_t)1.3 * (float_t)Log2(num_shapes));
    max_depth = fmin((float_t)MAX_TREE_DEPTH, max_depth);

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

    PSHAPE_AND_DATA data = calloc(num_shapes, sizeof(SHAPE_AND_DATA));

    if (data == NULL)
    {
        UncompressedKdTreeFree(uncompressed_node);
        free(nodes);
        free(indices);
        return ISTATUS_ALLOCATION_FAILED;
    }

    PKD_TREE_SCENE result = malloc(sizeof(KD_TREE_SCENE));

    if (result == NULL)
    {
        UncompressedKdTreeFree(uncompressed_node);
        free(nodes);
        free(indices);
        free(data);
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
        free(data);
        free(result);
        return status;
    }

    for (size_t i = 0; i < num_shapes; i++)
    {
        ShapeRetain(data[i].shape);
        MatrixRetain(data[i].model_to_world);
    }

    result->nodes = nodes;
    result->indices = indices;
    result->shapes = data;
    result->num_shapes = num_shapes;
    result->scene_bounds = scene_bounds;

    *kd_tree_scene = result;

    return ISTATUS_SUCCESS;
}

void
KdTreeSceneFree(
    _In_opt_ _Post_invalid_ PKD_TREE_SCENE kd_tree_scene
    )
{
    if (kd_tree_scene == NULL)
    {
        return;
    }

    for (size_t i = 0; i < kd_tree_scene->num_shapes; i++)
    {
        ShapeRelease(kd_tree_scene->shapes[i].shape);
        MatrixRelease(kd_tree_scene->shapes[i].model_to_world);
    }

    free(kd_tree_scene->nodes);
    free(kd_tree_scene->indices);
    free(kd_tree_scene->shapes);
    free(kd_tree_scene);
}

ISTATUS 
KdTreeSceneTraceCallback(
    _In_opt_ const void *context, 
    _Inout_ PSHAPE_HIT_TESTER hit_tester,
    _In_ RAY ray
    )
{
    assert(context != NULL);
    assert(hit_tester != NULL);
    assert(RayValidate(ray));

    PCKD_TREE_SCENE kd_tree = (PCKD_TREE_SCENE)context;

    VECTOR3 inverse_direction = VectorCreate((float_t)1.0 / ray.direction.x,
                                             (float_t)1.0 / ray.direction.y,
                                             (float_t)1.0 / ray.direction.z);

    ISTATUS status = KdTreeTraceTree(kd_tree->nodes,
                                     kd_tree->indices,
                                     kd_tree->shapes,
                                     hit_tester,
                                     ray.origin,
                                     inverse_direction);

    return status;
}