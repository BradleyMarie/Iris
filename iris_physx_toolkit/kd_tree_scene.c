/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    kd_tree_scene.c

Abstract:

    Implements a kd-tree scene.

--*/

#include "common/pointer_list.h"
#include "iris_physx_toolkit/kd_tree_scene.h"

//
// Defines
//

#define MAX_TREE_DEPTH   64
#define INTERIOR_X_SPLIT 0
#define INTERIOR_Y_SPLIT 1
#define INTERIOR_Z_SPLIT 2
#define LEAF             3

//
// Types
//

typedef union _SPLIT_OR_OFFSET {
    float split;
    uint32_t offset;
} SPLIT_OR_OFFSET, *PSPLIT_OR_OFFSET;

typedef struct _KD_TREE_NODE {
    uint32_t flags_and_num_shapes_or_offset;
    SPLIT_OR_OFFSET split_or_offset;
} KD_TREE_NODE, *PKD_TREE_NODE;

typedef const KD_TREE_NODE *PCKD_TREE_NODE;

typedef struct _SHAPE_AND_DATA {
    PSHAPE shape;
    PMATRIX model_to_world;
    bool premultiplied;
} SHAPE_AND_DATA, *PSHAPE_AND_DATA;

typedef const SHAPE_AND_DATA *PCSHAPE_AND_DATA;

struct _KD_TREE_SCENE {
    KD_TREE_NODE *nodes;
    uint32_t *shape_indices;
    PSHAPE_AND_DATA *shapes;
    uint32_t num_shapes;
    BOUNDING_BOX scene_bounds;
};

typedef struct _WORK_ITEM {
    PCKD_TREE_NODE node;
    float_t min;
    float_t max;
} WORK_ITEM, *PWORK_ITEM;

//
// Static Functions
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
    return node->flags_and_num_shapes_or_offset >> 30;
}

static
inline
bool
KdTreeNodeIsLeaf(
    _In_ PCKD_TREE_NODE node
    )
{
    return node->flags_and_num_shapes_or_offset >= 0xC0000000;
}

static
inline
uint32_t
KdTreeLeafSize(
    _In_ PCKD_TREE_NODE node
    )
{
    assert(KdTreeNodeIsLeaf(node));

    return node->flags_and_num_shapes_or_offset & 0x3FFFFFFF;
}

static
inline
float_t
KdTreeSplit(
    _In_ PCKD_TREE_NODE node
    )
{
    assert(!KdTreeNodeIsLeaf(node));

    return node->split_or_offset.split;
}

static
inline
uint32_t
KdTreeChildOffset(
    _In_ PCKD_TREE_NODE node
    )
{
    assert(!KdTreeNodeIsLeaf(node));

    return node->flags_and_num_shapes_or_offset & 0x3FFFFFFF;
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
KdTreeProcessLeaf(
    _In_ PCKD_TREE_SCENE kd_tree,
    _In_ PCKD_TREE_NODE node,
    _Inout_ PSHAPE_HIT_TESTER hit_tester
    )
{
    uint32_t num_shapes = KdTreeLeafSize(node);

    if (num_shapes == 0)
    {
        return ISTATUS_SUCCESS;
    }

    uint32_t offset = node->split_or_offset.offset;

    if (num_shapes == 1)
    {
        ISTATUS status = KdTreeTraceShape(hit_tester, kd_tree->shapes[offset]);
        return status;
    }

    uint32_t *indices = kd_tree->shape_indices + offset;
    for (uint32_t i = 0; i < num_shapes; i++)
    {
        ISTATUS status = KdTreeTraceShape(hit_tester,
                                          kd_tree->shapes[indices[i]]);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }
    }

    return ISTATUS_SUCCESS;
}

//
// Functions
//

ISTATUS
KdTreeSceneAllocate(
    _In_ BOUNDING_BOX scene_bounds,
    _In_reads_(num_shapes) PSHAPE const *shapes,
    _In_reads_(num_shapes) PMATRIX const *transforms,
    _In_reads_(num_shapes) const bool *premultiplied,
    _In_ size_t num_shapes,
    _Out_ PKD_TREE_SCENE *kd_tree_scene
    );

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
        ShapeRelease(kd_tree_scene->shapes[i]->shape);
        MatrixRelease(kd_tree_scene->shapes[i]->model_to_world);
    }

    free(kd_tree_scene->nodes);
    free(kd_tree_scene->shape_indices);
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

    WORK_ITEM work_queue[MAX_TREE_DEPTH];
    size_t queue_size = 0;

    PCKD_TREE_NODE current = &kd_tree->nodes[0];
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

        if (KdTreeNodeIsLeaf(current))
        {
            ISTATUS status = KdTreeProcessLeaf(kd_tree,
                                               current,
                                               hit_tester);

            if (status != ISTATUS_SUCCESS)
            {
                return status;
            }

            if (queue_size == 0)
            {
                break;
            }

            queue_size -= 1;
            current = work_queue[queue_size].node;
            min = work_queue[queue_size].min;
            max = work_queue[queue_size].max;
            continue;
        }

        uint32_t split_axis = KdTreeNodeType(current);
        float_t origin = PointGetElement(ray.origin, split_axis);
        float_t direction = VectorGetElement(inverse_direction, split_axis);

        float_t split = KdTreeSplit(current);
        float_t plane_distance = (split - origin) * direction;

        uint32_t child_offset = KdTreeChildOffset(current);

        PCKD_TREE_NODE close_child, far_child;
        if (origin < split || (origin == split && direction <= (float_t)0.0))
        {
            close_child = current + 1;
            far_child = current + child_offset;
        }
        else
        {
            close_child = current + child_offset;
            far_child = current + 1;
        }

        if (max < plane_distance || plane_distance <= (float_t)0.0)
        {
            current = close_child;
        }
        else if (plane_distance < min)
        {
            current = far_child;
        }
        else
        {
            work_queue[queue_size].node = far_child;
            work_queue[queue_size].min = plane_distance;
            work_queue[queue_size].max = max;
            queue_size += 1;

            current = close_child;
            max = plane_distance;
        }
    }

    return ISTATUS_SUCCESS;
}