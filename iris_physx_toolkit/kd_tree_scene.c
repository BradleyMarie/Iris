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

//
// Static Functions
//

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
ISTATUS
KdTreeTraceShape(
    _Inout_ PSHAPE_HIT_TESTER hit_tester,
    _In_ PCSHAPE_AND_DATA shape_and_data
    )
{
    if (shape_and_data->premultiplied)
    {
        ISTATUS status =
            ShapeHitTesterTestPremultipliedShape(hit_tester,
                                                 shape_and_data->shape,
                                                 shape_and_data->model_to_world);

        return status;
    }

    ISTATUS status =
        ShapeHitTesterTestTransformedShape(hit_tester,
                                           shape_and_data->shape,
                                           shape_and_data->model_to_world);

    return status;
}

static
ISTATUS
KdTreeProcessLeaf(
    _In_ PCKD_TREE_SCENE kd_tree,
    _In_ PCKD_TREE_NODE node,
    _Inout_ PSHAPE_HIT_TESTER hit_tester,
    _In_ PCRAY ray
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

static
ISTATUS
KdTreeProcessInteriorX(
    _In_ PCKD_TREE_SCENE kd_tree,
    _In_ PCKD_TREE_NODE node,
    _In_ BOUNDING_BOX node_bounds,
    _Inout_ PSHAPE_HIT_TESTER hit_tester,
    _In_ PCRAY ray
    )
{
    uint32_t num_shapes = KdTreeLeafSize(node);
}

static
ISTATUS
KdTreeProcessInteriorY(
    _In_ PCKD_TREE_SCENE kd_tree,
    _In_ PCKD_TREE_NODE node,
    _In_ BOUNDING_BOX node_bounds,
    _Inout_ PSHAPE_HIT_TESTER hit_tester,
    _In_ PCRAY ray
    )
{
    uint32_t num_shapes = KdTreeLeafSize(node);
}

static
ISTATUS
KdTreeProcessInteriorZ(
    _In_ PCKD_TREE_SCENE kd_tree,
    _In_ PCKD_TREE_NODE node,
    _In_ BOUNDING_BOX node_bounds,
    _Inout_ PSHAPE_HIT_TESTER hit_tester,
    _In_ PCRAY ray
    )
{
    uint32_t num_shapes = KdTreeLeafSize(node);
}

static
ISTATUS
KdTreeSceneTrace(
    _In_ PCKD_TREE_SCENE kd_tree,
    _In_ PCKD_TREE_NODE node,
    _In_ BOUNDING_BOX node_bounds,
    _Inout_ PSHAPE_HIT_TESTER hit_tester,
    _In_ PCRAY ray
    )
{
    if (KdTreeNodeIsLeaf(node))
    {
        ISTATUS status = KdTreeProcessLeaf(kd_tree,
                                           node,
                                           hit_tester,
                                           ray);
        
        return status;
    }

    uint32_t type = KdTreeNodeType(node);

    if (type == INTERIOR_X_SPLIT)
    {
        ISTATUS status = KdTreeProcessInteriorX(kd_tree,
                                                node,
                                                node_bounds,
                                                hit_tester,
                                                ray);
        
        return status;
    }

    if (type == INTERIOR_Y_SPLIT)
    {
        ISTATUS status = KdTreeProcessInteriorY(kd_tree,
                                                node,
                                                node_bounds,
                                                hit_tester,
                                                ray);
        
        return status;
    }

    ISTATUS status = KdTreeProcessInteriorZ(kd_tree,
                                            node,
                                            node_bounds,
                                            hit_tester,
                                            ray);
    
    return status;
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
        ShapeRelease(kd_tree_scene->shapes[i].shape);
        MatrixRelease(kd_tree_scene->shapes[i].model_to_world);
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

    ISTATUS status = KdTreeSceneTrace(kd_tree,
                                      kd_tree->nodes,
                                      kd_tree->scene_bounds,
                                      hit_tester,
                                      &ray);

    return status;
}