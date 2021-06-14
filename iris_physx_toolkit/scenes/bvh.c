/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    bvh.c

Abstract:

    Implements a BVH scene.

--*/

#include <stdalign.h>
#include <stdlib.h>
#include <string.h>

#include "common/safe_math.h"
#include "iris_physx_toolkit/scenes/bvh.h"

//
// Shape Bounds Type
//

typedef struct _SHAPE_BOUNDS {
    BOUNDING_BOX bounds;
    POINT3 bounds_centroid;
    PSHAPE shape;
    PMATRIX model_to_world;
    bool premultiplied;
} SHAPE_BOUNDS, *PSHAPE_BOUNDS;

typedef const SHAPE_BOUNDS *PCSHAPE_BOUNDS;

//
// Shape Bounds Static Functions
//

static
ISTATUS
ShapeBoundsInitialize(
    _Inout_ PSHAPE_BOUNDS bounds,
    _In_ PSHAPE shape,
    _In_ PMATRIX model_to_world,
    _In_ bool premultiplied
    )
{
    ISTATUS status;
    if (premultiplied)
    {
        status = ShapeComputeBounds(shape,
                                    model_to_world,
                                    &bounds->bounds);
    }
    else
    {
        status = ShapeComputeBounds(shape,
                                    NULL,
                                    &bounds->bounds);
    }

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    VECTOR3 diagonal = PointSubtract(bounds->bounds.corners[1],
                                     bounds->bounds.corners[0]);
    bounds->bounds_centroid = PointVectorAddScaled(bounds->bounds.corners[0],
                                                   diagonal,
                                                   (float_t)0.5);
    bounds->shape = shape;
    bounds->model_to_world = model_to_world;
    bounds->premultiplied = premultiplied;

    return ISTATUS_SUCCESS;
}

//
// Node Defines
//

#define MAX_LEAF_SIZE UINT16_MAX
#define MAX_CHILD_OFFSET UINT32_MAX
#define MAX_PRIMITIVE_OFFSET UINT32_MAX

//
// Node Type
//

typedef struct _BVH_NODE {
    BOUNDING_BOX bounds;
    uint32_t offset;
    uint16_t num_shapes;
    uint16_t axis;
} BVH_NODE, *PBVH_NODE;

typedef const BVH_NODE *PCBVH_NODE;

//
// Node Static Functions
//

static
inline
void
BvhNodeInitializeLeaf(
    _Inout_ PBVH_NODE node,
    _In_ BOUNDING_BOX bounds,
    _In_ size_t offset,
    _In_ size_t num_shapes
    )
{
    assert(offset < MAX_PRIMITIVE_OFFSET);
    assert(num_shapes < MAX_LEAF_SIZE);

    node->bounds = bounds;
    node->offset = offset;
    node->num_shapes = num_shapes;
    node->axis = UINT16_MAX;
}

static
inline
void
BvhNodeInitializeInterior(
    _Inout_ PBVH_NODE node,
    _In_ BOUNDING_BOX bounds,
    _In_ size_t offset,
    _In_ VECTOR_AXIS axis
    )
{
    assert(offset < MAX_CHILD_OFFSET);

    node->bounds = bounds;
    node->offset = offset;
    node->num_shapes = 0;
    node->axis = axis;
}

//
// Node Builder Defines
//

#define DESIRED_ALIGNMENT 4096
#define INITIAL_NODES 512

//
// Node Builder Type
//

typedef struct _NODE_BUILDER {
    _Field_size_(nodes_capacity) PBVH_NODE nodes;
    size_t nodes_capacity;
    size_t nodes_size;
} NODE_BUILDER, *PNODE_BUILDER;

typedef const NODE_BUILDER *PCNODE_BUILDER;

//
// Node Builder Static Functions
//

static
bool
NodeBuilderInitialize(
    _Out_ PNODE_BUILDER node_builder,
    _In_ size_t num_shapes
    )
{
    node_builder->nodes = aligned_alloc(DESIRED_ALIGNMENT,
                                        INITIAL_NODES * sizeof(BVH_NODE));

    if (node_builder->nodes == NULL)
    {
        return false;
    }

    node_builder->nodes_capacity = INITIAL_NODES;
    node_builder->nodes_size = 0;

    return true;
}

static
void
NodeBuilderResizeToFit(
    _Inout_ PNODE_BUILDER node_builder
    )
{
    size_t nodes_size = node_builder->nodes_size * sizeof(BVH_NODE);
    void *nodes_allocation = aligned_alloc(DESIRED_ALIGNMENT, nodes_size);
    if (nodes_allocation != NULL)
    {
        memcpy(nodes_allocation, node_builder->nodes, nodes_size);
        free(node_builder->nodes);
        node_builder->nodes = (PBVH_NODE)nodes_allocation;
        node_builder->nodes_capacity = node_builder->nodes_size;
    }
}

static
void
NodeBuilderDestroy(
    _Inout_ _Post_invalid_ PNODE_BUILDER node_builder
    )
{
    free(node_builder->nodes);
}

static
bool
NodeBuilderGrowNodes(
    _Inout_ PNODE_BUILDER node_builder
    )
{
    size_t new_capacity;
    bool success = CheckedMultiplySizeT(node_builder->nodes_capacity,
                                        2,
                                        &new_capacity);

    if (!success)
    {
        return false;
    }

    size_t bytes;
    success = CheckedMultiplySizeT(new_capacity, sizeof(BVH_NODE), &bytes);

    if (!success)
    {
        return false;
    }

    PBVH_NODE new_nodes =
        (PBVH_NODE)aligned_alloc(DESIRED_ALIGNMENT, bytes);

    if (new_nodes == NULL)
    {
        return false;
    }

    memcpy(new_nodes,
           node_builder->nodes,
           node_builder->nodes_capacity * sizeof(BVH_NODE));

    free(node_builder->nodes);
    node_builder->nodes = new_nodes;
    node_builder->nodes_capacity = new_capacity;

    return true;
}

static
bool
NodeBuilderAllocateNode(
    _Inout_ PNODE_BUILDER node_builder,
    _Out_ size_t *index
    )
{
    if (node_builder->nodes_size == node_builder->nodes_capacity)
    {
        if (!NodeBuilderGrowNodes(node_builder))
        {
            return false;
        }
    }

    *index = node_builder->nodes_size;

    node_builder->nodes_size += 1;

    return true;
}

static
bool
NodeBuilderAllocateLeafNode(
    _Inout_ PNODE_BUILDER builder,
    _In_ BOUNDING_BOX node_bounds,
    _In_ size_t shape_offset,
    _In_ size_t num_shapes,
    _Out_ size_t *index
    )
{
    if (MAX_LEAF_SIZE < num_shapes)
    {
        return false;
    }

    if (MAX_PRIMITIVE_OFFSET < shape_offset)
    {
        return false;
    }

    bool success = NodeBuilderAllocateNode(builder, index);

    if (!success)
    {
        return false;
    }

    BvhNodeInitializeLeaf(builder->nodes + *index,
                          node_bounds,
                          shape_offset,
                          num_shapes);

    return true;
}

static
bool
NodeBuilderInitializeInteriorNode(
    _Inout_ PNODE_BUILDER node_builder,
    _In_ BOUNDING_BOX bounds,
    _In_ size_t node_index,
    _In_ size_t child_index,
    _In_ VECTOR_AXIS axis
    )
{
    assert(node_index < child_index);

    size_t offset_to_child = child_index - node_index;

    if (MAX_CHILD_OFFSET < offset_to_child)
    {
        return false;
    }

    BvhNodeInitializeInterior(node_builder->nodes + node_index,
                              bounds,
                              offset_to_child,
                              axis);

    return true;
}

//
// BVH Build Defines
//

#define SPLITS_TO_EVALUATE 12

//
// BVH Build Types
//

typedef struct _BVH_SPLIT {
    BOUNDING_BOX bounds;
    size_t num_shapes;
} BVH_SPLIT, *PBVH_SPLIT;

//
// BVH Build Static Functions
//

static
BOUNDING_BOX
BvhComputeNodeBounds(
    _In_reads_(num_shapes) PCSHAPE_BOUNDS shape_bounds,
    _In_ size_t num_shapes
    )
{
    assert(num_shapes != 0);

    BOUNDING_BOX bounds = shape_bounds[0].bounds;
    for (size_t i = 1; i < num_shapes; i++)
    {
        bounds = BoundingBoxUnion(shape_bounds[i].bounds, bounds);
    }

    return bounds;
}

static
float_t
BvhComputeNodeCost(
    _In_ BOUNDING_BOX bounding_box,
    _In_ size_t num_shapes
    )
{
    float_t surface_area = BoundingBoxSurfaceArea(bounding_box);
    return (float_t)num_shapes * surface_area;
}

static
float_t
BvhEvaluateSplitsOnAxis(
    _In_ BOUNDING_BOX node_bounds,
    _In_reads_(num_shapes) PCSHAPE_BOUNDS shape_bounds,
    _In_ size_t num_shapes,
    _In_ VECTOR_AXIS axis
    )
{
    assert(num_shapes != 0);

    BVH_SPLIT splits[SPLITS_TO_EVALUATE];
    for (size_t i = 0; i < SPLITS_TO_EVALUATE; i++)
    {
        splits[i].num_shapes = 0;
    }

    float_t min = PointGetElement(node_bounds.corners[0], axis);
    float_t max = PointGetElement(node_bounds.corners[1], axis);
    float_t range = max - min;

    for (size_t i = 0; i < num_shapes; i++)
    {
        float_t value = PointGetElement(shape_bounds[i].bounds_centroid, axis);
        float_t offset = value - min;
        float_t scaled_offset = offset / range;

        size_t split_index = (float_t)SPLITS_TO_EVALUATE * scaled_offset;
        if (split_index == SPLITS_TO_EVALUATE)
        {
            split_index = SPLITS_TO_EVALUATE - 1;
        }

        if (splits[split_index].num_shapes == 0)
        {
            splits[split_index].bounds = shape_bounds[i].bounds;
        }
        else
        {
            splits[split_index].bounds =
                BoundingBoxUnion(splits[split_index].bounds,
                                 shape_bounds[i].bounds);
        }

        splits[split_index].num_shapes += 1;
    }

    float_t below_cost[SPLITS_TO_EVALUATE - 1];
    BOUNDING_BOX cumulative_bounds;
    size_t cumulative_shapes = splits[0].num_shapes;
    if (cumulative_shapes != 0)
    {
        cumulative_bounds = splits[0].bounds;
        below_cost[0] = BvhComputeNodeCost(cumulative_bounds,
                                           cumulative_shapes);
    }
    else
    {
        below_cost[0] = (float_t)0.0;
    }

    for (size_t i = 1; i < SPLITS_TO_EVALUATE - 1; i++)
    {
        if (splits[i].num_shapes == 0)
        {
            below_cost[i] = below_cost[i - 1];
            continue;
        }

        if (cumulative_shapes != 0)
        {
            cumulative_bounds = BoundingBoxUnion(cumulative_bounds,
                                                 splits[i].bounds);
        }
        else
        {
            cumulative_bounds = splits[i].bounds;
        }

        cumulative_shapes += splits[i].num_shapes;
        below_cost[i] = BvhComputeNodeCost(cumulative_bounds,
                                           cumulative_shapes);
    }

    float_t above_cost[SPLITS_TO_EVALUATE - 1];
    cumulative_shapes = splits[SPLITS_TO_EVALUATE - 1].num_shapes;
    if (cumulative_shapes != 0)
    {
        cumulative_bounds = splits[SPLITS_TO_EVALUATE - 1].bounds;
        above_cost[SPLITS_TO_EVALUATE - 2] =
            BvhComputeNodeCost(cumulative_bounds, cumulative_shapes);
    }
    else
    {
        above_cost[SPLITS_TO_EVALUATE - 2] = (float_t)0.0;
    }

    for (size_t i = 1; i < SPLITS_TO_EVALUATE - 1; i++)
    {
        size_t index = SPLITS_TO_EVALUATE - 2 - i;
        if (splits[index].num_shapes == 0)
        {
            above_cost[index] = above_cost[index + 1];
            continue;
        }

        if (cumulative_shapes != 0)
        {
            cumulative_bounds = BoundingBoxUnion(cumulative_bounds,
                                                 splits[index].bounds);
        }
        else
        {
            cumulative_bounds = splits[index].bounds;
        }

        cumulative_shapes += splits[index].num_shapes;
        above_cost[index] = BvhComputeNodeCost(cumulative_bounds,
                                               cumulative_shapes);
    }

    float_t node_surface_area = BoundingBoxSurfaceArea(node_bounds);
    float_t best_cost =
        ((float_t)1.0 + above_cost[0] + below_cost[0]) / node_surface_area;
    size_t best_split = 0;
    for (size_t i = 1; i < SPLITS_TO_EVALUATE - 2; i++)
    {
        float_t cost =
            ((float_t)1.0 + above_cost[i] + below_cost[i]) / node_surface_area;
        if (cost < best_cost)
        {
            best_cost = cost;
            best_split = i;
        }
    }

    float_t relative_split =
        (float_t)(1 + best_split) / (float_t)SPLITS_TO_EVALUATE;
    return min + range * relative_split;
}

static
void
BvhPartitionShapes(
    _Inout_updates_(num_shapes) PSHAPE_BOUNDS shape_bounds,
    _In_ size_t num_shapes,
    _In_ VECTOR_AXIS split_axis,
    _In_ float_t split,
    _Outptr_result_buffer_(num_above_bounds) PSHAPE_BOUNDS *above_bounds,
    _Out_ size_t *num_above_bounds,
    _Outptr_result_buffer_(num_below_bounds) PSHAPE_BOUNDS *below_bounds,
    _Out_ size_t *num_below_bounds
    )
{
    size_t insert_index = 0;
    for (size_t i = 0; i < num_shapes; i++)
    {
        float_t value =
            PointGetElement(shape_bounds[i].bounds_centroid, split_axis);
        if (value < split)
        {
            SHAPE_BOUNDS tmp = shape_bounds[insert_index];
            shape_bounds[insert_index++] = shape_bounds[i];
            shape_bounds[i] = tmp;
        }
    }

    *above_bounds = shape_bounds + insert_index;
    *num_above_bounds = num_shapes - insert_index;
    *below_bounds = shape_bounds;
    *num_below_bounds = insert_index;
}

static
bool
BvhBuildImpl(
    _Inout_ PNODE_BUILDER node_builder,
    _In_ BOUNDING_BOX node_bounds,
    _Inout_updates_(num_shapes) PSHAPE_BOUNDS shape_bounds,
    _In_ size_t num_shapes,
    _In_ size_t shape_offset,
    _In_ size_t depth_remaining,
    _Out_ size_t *index
    )
{
    if (num_shapes == 1 || depth_remaining == 0)
    {
        bool success = NodeBuilderAllocateLeafNode(node_builder,
                                                   node_bounds,
                                                   shape_offset,
                                                   num_shapes,
                                                   index);

        return success;
    }

    BOUNDING_BOX centroid_bounds =
        BoundingBoxCreate(shape_bounds[0].bounds_centroid,
                          shape_bounds[0].bounds_centroid);

    for (size_t i = 1; i < num_shapes; i++)
    {
        centroid_bounds = BoundingBoxEnvelop(centroid_bounds,
                                             shape_bounds[i].bounds_centroid);
    }

    VECTOR3 centroid_diagonal = PointSubtract(centroid_bounds.corners[1],
                                              centroid_bounds.corners[0]);
    VECTOR_AXIS axis = VectorDominantAxis(centroid_diagonal);

    float_t min = PointGetElement(centroid_bounds.corners[0], axis);
    float_t max = PointGetElement(centroid_bounds.corners[1], axis);

    if (min == max)
    {
        bool success = NodeBuilderAllocateLeafNode(node_builder,
                                                   node_bounds,
                                                   shape_offset,
                                                   num_shapes,
                                                   index);

        return success;
    }

    PSHAPE_BOUNDS above_bounds, below_bounds;
    size_t above_bounds_size, below_bounds_size;
    if (num_shapes == 2)
    {
        float_t shape0 = PointGetElement(shape_bounds[0].bounds_centroid, axis);
        float_t shape1 = PointGetElement(shape_bounds[1].bounds_centroid, axis);
        if (shape1 < shape0)
        {
            SHAPE_BOUNDS tmp = shape_bounds[0];
            shape_bounds[0] = shape_bounds[1];
            shape_bounds[1] = tmp;
        }

        below_bounds = &shape_bounds[0];
        above_bounds = &shape_bounds[1];

        below_bounds_size = 1;
        above_bounds_size = 1;
    }
    else
    {
        float_t split = BvhEvaluateSplitsOnAxis(node_bounds,
                                                shape_bounds,
                                                num_shapes,
                                                axis);

        BvhPartitionShapes(shape_bounds,
                           num_shapes,
                           axis,
                           split,
                           &above_bounds,
                           &above_bounds_size,
                           &below_bounds,
                           &below_bounds_size);
    }

    if (above_bounds_size == 0 || below_bounds_size == 0)
    {
        bool success = NodeBuilderAllocateLeafNode(node_builder,
                                                   node_bounds,
                                                   shape_offset,
                                                   num_shapes,
                                                   index);

        return success;
    }

    bool success = NodeBuilderAllocateNode(node_builder, index);

    if (!success)
    {
        return false;
    }

    size_t unused_below_index;
    BOUNDING_BOX below_node_bounds =
        BvhComputeNodeBounds(below_bounds, below_bounds_size);
    success = BvhBuildImpl(node_builder,
                           below_node_bounds,
                           below_bounds,
                           below_bounds_size,
                           shape_offset,
                           depth_remaining - 1,
                           &unused_below_index);

    if (!success)
    {
        return false;
    }

    size_t above_index;
    BOUNDING_BOX above_node_bounds =
        BvhComputeNodeBounds(above_bounds, above_bounds_size);
    success = BvhBuildImpl(node_builder,
                           above_node_bounds,
                           above_bounds,
                           above_bounds_size,
                           shape_offset + below_bounds_size,
                           depth_remaining - 1,
                           &above_index);

    if (!success)
    {
        return false;
    }

    success = NodeBuilderInitializeInteriorNode(node_builder,
                                                node_bounds,
                                                *index,
                                                above_index,
                                                axis);

    return success;          
}

static
ISTATUS
BvhBuild(
    _Inout_ PNODE_BUILDER node_builder,
    _In_reads_opt_(num_shapes) const PSHAPE shapes[],
    _In_reads_opt_(num_shapes) const PMATRIX transforms[],
    _In_reads_opt_(num_shapes) const bool premultiplied[],
    _In_ size_t num_shapes,
    _In_ size_t max_depth,
    _Outptr_result_buffer_(num_shapes) PSHAPE_BOUNDS *shape_bounds
    )
{
    *shape_bounds = (PSHAPE_BOUNDS)calloc(num_shapes, sizeof(SHAPE_BOUNDS));

    if (*shape_bounds == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    for (size_t i = 0; i < num_shapes; i++)
    {
        bool is_premultiplied;
        if (premultiplied != NULL)
        {
            is_premultiplied = premultiplied[i];
        }
        else
        {
            is_premultiplied = false;
        }

        PMATRIX matrix;
        if (transforms != NULL)
        {
            matrix = transforms[i];
        }
        else
        {
            matrix = NULL;
        }

        ISTATUS status = ShapeBoundsInitialize(*shape_bounds + i,
                                               shapes[i],
                                               matrix,
                                               is_premultiplied);

        if (status != ISTATUS_SUCCESS)
        {
            free(*shape_bounds);
            return status;
        }
    }

    BOUNDING_BOX node_bounds = BvhComputeNodeBounds(*shape_bounds, num_shapes);

    size_t unused_index;
    bool success = BvhBuildImpl(node_builder,
                                node_bounds,
                                *shape_bounds,
                                num_shapes,
                                0,
                                max_depth - 1,
                                &unused_index);

    if (!success)
    {
        free(*shape_bounds);
        return ISTATUS_ALLOCATION_FAILED;
    }

    return ISTATUS_SUCCESS;
}

//
// Scene Defines
//

#define MAX_TREE_DEPTH 64

//
// Scene Types
//

typedef struct _BVH_SCENE {
    PBVH_NODE nodes;
    _Field_size_(num_shapes) PSHAPE *shapes;
    _Field_size_opt_(num_shapes) PMATRIX *transforms;
    _Field_size_opt_(num_shapes) bool *premultiplied;
    size_t num_shapes;
} BVH_SCENE, *PBVH_SCENE;

typedef const BVH_SCENE *PCBVH_SCENE;

//
// Scene Static Functions
//

static
ISTATUS
BvhSceneTrace(
    _In_opt_ const void *context,
    _Inout_ PSHAPE_HIT_TESTER hit_tester,
    _In_ RAY ray
    )
{
    PCBVH_SCENE bvh_scene = (PCBVH_SCENE)context;

    float_t closest_hit;
    ShapeHitTesterFarthestHitAllowed(hit_tester, &closest_hit);

    PCBVH_NODE work_list[MAX_TREE_DEPTH];
    PCBVH_NODE current = bvh_scene->nodes;
    size_t queue_size = 0;
    for (;;)
    {
        float_t min;
        if (BoundingBoxIntersect(current->bounds, ray, NULL, &min, NULL) &&
            min <= closest_hit)
        {
            if (current->num_shapes == 0)
            {
                float_t direction =
                    VectorGetElement(ray.direction, current->axis);
                if (direction < (float_t)0.0)
                {
                    work_list[queue_size++] = current + 1;
                    current = current + current->offset;
                }
                else
                {
                    work_list[queue_size++] = current + current->offset;
                    current = current + 1;
                }
                continue;
            }

            size_t offset = current->offset;
            for (size_t i = 0; i < current->num_shapes; i++)
            {
                PCSHAPE shape = bvh_scene->shapes[offset + i];
                PCMATRIX matrix = bvh_scene->transforms[offset + i];
                bool premultiplied = bvh_scene->premultiplied[offset + i];
                ISTATUS status =
                    ShapeHitTesterTestShapeWithLimit(hit_tester,
                                                     shape,
                                                     matrix,
                                                     premultiplied,
                                                     &closest_hit);

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
        current = work_list[queue_size];
    }

    return ISTATUS_SUCCESS;
}

static
ISTATUS
BvhTransformedSceneTrace(
    _In_opt_ const void *context,
    _Inout_ PSHAPE_HIT_TESTER hit_tester,
    _In_ RAY ray
    )
{
    PCBVH_SCENE bvh_scene = (PCBVH_SCENE)context;

    float_t closest_hit;
    ShapeHitTesterFarthestHitAllowed(hit_tester, &closest_hit);

    PCBVH_NODE work_list[MAX_TREE_DEPTH];
    PCBVH_NODE current = bvh_scene->nodes;
    size_t queue_size = 0;
    for (;;)
    {
        float_t min;
        if (BoundingBoxIntersect(current->bounds, ray, NULL, &min, NULL) &&
            min <= closest_hit)
        {
            if (current->num_shapes == 0)
            {
                float_t direction =
                    VectorGetElement(ray.direction, current->axis);
                if (direction < (float_t)0.0)
                {
                    work_list[queue_size++] = current + 1;
                    current = current + current->offset;
                }
                else
                {
                    work_list[queue_size++] = current + current->offset;
                    current = current + 1;
                }
                continue;
            }

            size_t offset = current->offset;
            for (size_t i = 0; i < current->num_shapes; i++)
            {
                PCSHAPE shape = bvh_scene->shapes[offset + i];
                PCMATRIX matrix = bvh_scene->transforms[offset + i];
                ISTATUS status =
                    ShapeHitTesterTestTransformedShapeWithLimit(hit_tester,
                                                                shape,
                                                                matrix,
                                                                &closest_hit);

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
        current = work_list[queue_size];
    }

    return ISTATUS_SUCCESS;
}

static
ISTATUS
BvhWorldSceneTrace(
    _In_opt_ const void *context,
    _Inout_ PSHAPE_HIT_TESTER hit_tester,
    _In_ RAY ray
    )
{
    PCBVH_SCENE bvh_scene = (PCBVH_SCENE)context;

    float_t closest_hit;
    ShapeHitTesterFarthestHitAllowed(hit_tester, &closest_hit);

    PCBVH_NODE work_list[MAX_TREE_DEPTH];
    PCBVH_NODE current = bvh_scene->nodes;
    size_t queue_size = 0;
    for (;;)
    {
        float_t min;
        if (BoundingBoxIntersect(current->bounds, ray, NULL, &min, NULL) &&
            min <= closest_hit)
        {
            if (current->num_shapes == 0)
            {
                float_t direction =
                    VectorGetElement(ray.direction, current->axis);
                if (direction < (float_t)0.0)
                {
                    work_list[queue_size++] = current + 1;
                    current = current + current->offset;
                }
                else
                {
                    work_list[queue_size++] = current + current->offset;
                    current = current + 1;
                }
                continue;
            }

            size_t offset = current->offset;
            for (size_t i = 0; i < current->num_shapes; i++)
            {
                PCSHAPE shape = bvh_scene->shapes[offset + i];
                ISTATUS status =
                    ShapeHitTesterTestWorldShapeWithLimit(hit_tester,
                                                          shape,
                                                          &closest_hit);

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
        current = work_list[queue_size];
    }

    return ISTATUS_SUCCESS;
}

static
void
BvhSceneFree(
    _In_opt_ _Post_invalid_ void *context
    )
{
    PBVH_SCENE bvh_scene = (PBVH_SCENE)context;

    for (size_t i = 0; i < bvh_scene->num_shapes; i++)
    {
        ShapeRelease(bvh_scene->shapes[i]);
    }

    free(bvh_scene->shapes);

    if (bvh_scene->transforms != NULL)
    {
        for (size_t i = 0; i < bvh_scene->num_shapes; i++)
        {
            MatrixRelease(bvh_scene->transforms[i]);
        }

        free(bvh_scene->transforms);
    }

    if (bvh_scene->premultiplied != NULL)
    {
        free(bvh_scene->premultiplied);
    }

    free(bvh_scene->nodes);
}

//
// Scene Static Data
//

static const SCENE_VTABLE bvh_scene_vtable = {
    BvhSceneTrace,
    BvhSceneFree
};

static const SCENE_VTABLE bvh_transformed_scene_vtable = {
    BvhTransformedSceneTrace,
    BvhSceneFree
};

static const SCENE_VTABLE bvh_world_scene_vtable = {
    BvhWorldSceneTrace,
    BvhSceneFree
};

//
// Scene Functions
//

ISTATUS
BvhSceneAllocate(
    _In_reads_(num_shapes) const PSHAPE shapes[],
    _In_reads_opt_(num_shapes) const PMATRIX transforms[],
    _In_reads_opt_(num_shapes) const bool premultiplied[],
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

    if (scene == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    NODE_BUILDER node_builder;
    bool success = NodeBuilderInitialize(&node_builder, num_shapes);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    PSHAPE_BOUNDS shape_bounds;
    ISTATUS status = BvhBuild(&node_builder,
                              shapes,
                              transforms,
                              premultiplied,
                              num_shapes,
                              MAX_TREE_DEPTH,
                              &shape_bounds);

    if (status != ISTATUS_SUCCESS)
    {
        NodeBuilderDestroy(&node_builder);
        return status;
    }

    NodeBuilderResizeToFit(&node_builder);

    BVH_SCENE result;
    result.nodes = node_builder.nodes;
    result.shapes = calloc(num_shapes, sizeof(PSHAPE));
    result.num_shapes = num_shapes;

    if (result.shapes == NULL)
    {
        free(shape_bounds);
        NodeBuilderDestroy(&node_builder);
        return ISTATUS_ALLOCATION_FAILED;
    }

    for (size_t i = 0; i < num_shapes; i++)
    {
        result.shapes[i] = shape_bounds[i].shape;
    }

    bool premultiply_needed = false;
    if (premultiplied != NULL)
    {
        for (size_t i = 0; i < num_shapes; i++)
        {
            if (premultiplied[i])
            {
                premultiply_needed = true;
                break;
            }
        }
    }

    bool transform_needed = false;
    if (transforms != NULL)
    {
        for (size_t i = 0; i < num_shapes; i++)
        {
            if (transforms[i] != NULL)
            {
                transform_needed = true;
                break;
            }
        }
    }

    PCSCENE_VTABLE vtable;
    if (!transform_needed)
    {
        vtable = &bvh_world_scene_vtable;
        result.transforms = NULL;
        result.premultiplied = NULL;
    }
    else if (!premultiply_needed)
    {
        vtable = &bvh_transformed_scene_vtable;
        result.transforms = calloc(num_shapes, sizeof(PMATRIX));
        result.premultiplied = NULL;

        if (result.transforms == NULL)
        {
            free(result.shapes);
            free(shape_bounds);
            NodeBuilderDestroy(&node_builder);
            return ISTATUS_ALLOCATION_FAILED;
        }

        for (size_t i = 0; i < num_shapes; i++)
        {
            result.transforms[i] = shape_bounds[i].model_to_world;
        }
    }
    else
    {
        vtable = &bvh_scene_vtable;
        result.transforms = calloc(num_shapes, sizeof(PMATRIX));
        result.premultiplied = calloc(num_shapes, sizeof(bool));

        if (result.transforms == NULL || result.premultiplied == NULL)
        {
            free(result.shapes);
            free(result.transforms);
            free(result.premultiplied);
            free(shape_bounds);
            NodeBuilderDestroy(&node_builder);
            return ISTATUS_ALLOCATION_FAILED;
        }

        for (size_t i = 0; i < num_shapes; i++)
        {
            result.transforms[i] = shape_bounds[i].model_to_world;
            result.premultiplied[i] = shape_bounds[i].premultiplied;
        }
    }

    free(shape_bounds);

    status = SceneAllocate(vtable,
                           &result,
                           sizeof(BVH_SCENE),
                           alignof(BVH_SCENE),
                           environment,
                           scene);

    if (status != ISTATUS_SUCCESS)
    {
        free(result.shapes);
        free(result.transforms);
        free(result.premultiplied);
        NodeBuilderDestroy(&node_builder);
        return status;
    }

    for (size_t i = 0; i < num_shapes; i++)
    {
        ShapeRetain(shapes[i]);
    }

    if (transform_needed)
    {
        for (size_t i = 0; i < num_shapes; i++)
        {
            MatrixRetain(transforms[i]);
        }
    }

    return ISTATUS_SUCCESS;
}

//
// BVH Aggregate Type
//

typedef struct _BVH_AGGREGATE {
    PBVH_NODE nodes;
    _Field_size_(num_shapes) PSHAPE *shapes;
    size_t num_shapes;
} BVH_AGGREGATE, *PBVH_AGGREGATE;

typedef const BVH_AGGREGATE *PCBVH_AGGREGATE;

//
// BVH Aggregate Static Functions
//

static
inline
ISTATUS
BvhAggregateTraceShape(
    _In_ PCSHAPE shape,
    _In_ PCRAY ray,
    _In_ float_t minimum_distance,
    _Inout_ float_t *maximum_distance,
    _In_ PSHAPE_HIT_ALLOCATOR allocator,
    _Out_ PHIT *hit_list,
    _Out_ ISTATUS *return_status
    )
{
    PHIT hit;
    ISTATUS status = ShapeHitTesterTestNestedShape(allocator,
                                                   shape,
                                                   minimum_distance,
                                                   *maximum_distance,
                                                   &hit);

    if (status == ISTATUS_NO_INTERSECTION)
    {
        return ISTATUS_SUCCESS;
    }

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    while (hit != NULL)
    {
        if (minimum_distance < hit->distance &&
            hit->distance < *maximum_distance)
        {
            *maximum_distance = hit->distance;
            *hit_list = hit;
            *return_status = ISTATUS_SUCCESS;
        }

        hit = hit->next;
    }

    return ISTATUS_SUCCESS;
}

static
ISTATUS
BvhAggregateTrace(
    _In_ const void *context,
    _In_ PCRAY ray,
    _In_ float_t minimum_distance,
    _In_ float_t maximum_distance,
    _In_ PSHAPE_HIT_ALLOCATOR allocator,
    _Out_ PHIT *hit
    )
{
    PCBVH_SCENE bvh_scene = (PCBVH_SCENE)context;

    ISTATUS return_status = ISTATUS_NO_INTERSECTION;
    PCBVH_NODE work_list[MAX_TREE_DEPTH];
    PCBVH_NODE current = bvh_scene->nodes;
    size_t queue_size = 0;
    for (;;)
    {
        float_t near;
        if (BoundingBoxIntersect(current->bounds, *ray, NULL, &near, NULL) &&
            near <= maximum_distance)
        {
            if (current->num_shapes == 0)
            {
                float_t direction =
                    VectorGetElement(ray->direction, current->axis);
                if (direction < (float_t)0.0)
                {
                    work_list[queue_size++] = current + 1;
                    current = current + current->offset;
                }
                else
                {
                    work_list[queue_size++] = current + current->offset;
                    current = current + 1;
                }
                continue;
            }

            size_t offset = current->offset;
            for (size_t i = 0; i < current->num_shapes; i++)
            {
                PCSHAPE shape = bvh_scene->shapes[offset + i];
                ISTATUS status =
                    BvhAggregateTraceShape(shape,
                                           ray,
                                           minimum_distance,
                                           &maximum_distance,
                                           allocator,
                                           hit,
                                           &return_status);

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
        current = work_list[queue_size];
    }

    return return_status;
}

static
ISTATUS
BvhAggregateComputeBounds(
    _In_ const void *context,
    _In_opt_ PCMATRIX model_to_world,
    _Out_ PBOUNDING_BOX world_bounds
    )
{
    PCBVH_AGGREGATE aggregate = (PCBVH_AGGREGATE)context;

    ISTATUS status = ShapeComputeBounds(aggregate->shapes[0],
                                        model_to_world,
                                        world_bounds);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    for (size_t i = 1; i < aggregate->num_shapes; i++)
    {
        BOUNDING_BOX bounds;
        status = ShapeComputeBounds(aggregate->shapes[i],
                                    model_to_world,
                                    &bounds);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }

        *world_bounds = BoundingBoxUnion(*world_bounds, bounds);
    }

    return status;
}

static
void
BvhAggregateFree(
    _In_opt_ _Post_invalid_ void *context
    )
{
    PCBVH_AGGREGATE aggregate = (PCBVH_AGGREGATE)context;

    for (size_t i = 0; i < aggregate->num_shapes; i++)
    {
        ShapeRelease(aggregate->shapes[i]);
    }

    free(aggregate->shapes);
    free(aggregate->nodes);
}

//
// BVH Aggregate Static Data
//

static const SHAPE_VTABLE bvh_aggregate_vtable = {
    BvhAggregateTrace,
    BvhAggregateComputeBounds,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    BvhAggregateFree
};

//
// BVH Aggregate Functions
//

ISTATUS
BvhAggregateAllocate(
    _In_reads_(num_shapes) const PSHAPE shapes[],
    _In_ size_t num_shapes,
    _Out_ PSHAPE *aggregate
    )
{
    if (shapes == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (num_shapes == 0)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (aggregate == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    NODE_BUILDER node_builder;
    bool success = NodeBuilderInitialize(&node_builder, num_shapes);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    PSHAPE_BOUNDS shape_bounds;
    ISTATUS status = BvhBuild(&node_builder,
                              shapes,
                              NULL,
                              NULL,
                              num_shapes,
                              MAX_TREE_DEPTH,
                              &shape_bounds);

    if (status != ISTATUS_SUCCESS)
    {
        NodeBuilderDestroy(&node_builder);
        return status;
    }

    NodeBuilderResizeToFit(&node_builder);

    BVH_AGGREGATE result;
    result.nodes = node_builder.nodes;
    result.shapes = calloc(num_shapes, sizeof(PSHAPE));
    result.num_shapes = num_shapes;

    if (result.shapes == NULL)
    {
        free(shape_bounds);
        NodeBuilderDestroy(&node_builder);
        return ISTATUS_ALLOCATION_FAILED;
    }

    for (size_t i = 0; i < num_shapes; i++)
    {
        result.shapes[i] = shape_bounds[i].shape;
    }

    free(shape_bounds);

    status = ShapeAllocate(&bvh_aggregate_vtable,
                           &result,
                           sizeof(BVH_AGGREGATE),
                           alignof(BVH_AGGREGATE),
                           aggregate);

    if (status != ISTATUS_SUCCESS)
    {
        free(result.shapes);
        NodeBuilderDestroy(&node_builder);
        return status;
    }

    for (size_t i = 0; i < num_shapes; i++)
    {
        ShapeRetain(shapes[i]);
    }

    return ISTATUS_SUCCESS;
}