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
// Hash Set Type
//

typedef struct _HASH_SET {
    _Field_size_(num_entries) uint32_t *entries;
    size_t num_entries;
} HASH_SET, *PHASH_SET;

typedef const HASH_SET *PCHASH_SET;

//
// Hash Set Static Functions
//

static
inline
bool
HashSetInitialize(
    _In_ PHASH_SET hash_set,
    _In_ size_t max_num_entries
    )
{
    bool success = CheckedMultiplySizeT(max_num_entries, 4, &max_num_entries);

    if (!success)
    {
        return false;
    }

    max_num_entries /= 3;

    success = CheckedAddSizeT(max_num_entries, 1, &max_num_entries);

    if (!success)
    {
        return false;
    }

    if (UINT32_MAX < max_num_entries)
    {
        return false;
    }

    hash_set->entries = (uint32_t*)calloc(max_num_entries, sizeof(uint32_t));

    if (hash_set->entries == NULL)
    {
        return false;
    }

    hash_set->num_entries = max_num_entries;

    return true;
}

static
inline
void
HashSetClear(
    _Inout_ PHASH_SET hash_set
    )
{
    memset(hash_set->entries, 0, sizeof(uint32_t) * hash_set->num_entries);
}

static
inline
void
HashSetDestroy(
    _Inout_ _Post_invalid_ PHASH_SET hash_set
    )
{
    free(hash_set->entries);
    hash_set->entries = NULL;
}

static
inline
size_t
HashSetFindEntryIndex(
    _In_ PCHASH_SET hash_set,
    _In_ size_t key
    )
{
    assert(key < UINT32_MAX - 1);
    assert(key != 0);

    size_t hashed_key = key;
    hashed_key ^= hashed_key >> 16;
    hashed_key *= 0x85ebca6b;
    hashed_key ^= hashed_key >> 13;
    hashed_key *= 0xc2b2ae35;
    hashed_key ^= hashed_key >> 16;

    size_t index = hashed_key % hash_set->num_entries;
    for (;;)
    {
        if (hash_set->entries[index] == 0 ||
            hash_set->entries[index] == (uint32_t)key)
        {
            break;
        }

        index += 1;

        if (index == hash_set->num_entries)
        {
            index = 0;
        }
    }

    return index;
}

static
inline
void
HashSetInsert(
    _In_ PHASH_SET hash_set,
    _In_ size_t key
    )
{
    key += 1;
    size_t index = HashSetFindEntryIndex(hash_set, key);
    assert(hash_set->entries[index] == 0 || hash_set->entries[index] == key);
    hash_set->entries[index] = key;
}

static
inline
bool
HashSetContains(
    _In_ PCHASH_SET hash_set,
    _In_ size_t key
    )
{
    key += 1;
    size_t index = HashSetFindEntryIndex(hash_set, key);
    assert(hash_set->entries[index] == 0 || hash_set->entries[index] == key);
    return hash_set->entries[index] == (uint32_t)key;
}

//
// Edge Type
//

typedef struct _EDGE {
    uint32_t primitive;
    float_t value;
    bool is_start;
} EDGE, *PEDGE;

typedef const EDGE *PCEDGE;

//
// Edge Static Functions
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

//
// 1D Edges Type
//

typedef struct _EDGES_1D {
    _Field_size_(num_edges) PEDGE edges;
    size_t num_edges;
} EDGES_1D, *PEDGES_1D;

typedef const EDGES_1D *PCEDGES_1D;

//
// 1D Edges Static Functions
//

static
bool
Edges1DInitialize(
    _Out_ PEDGES_1D edges,
    _In_ size_t num_edges
    )
{
    edges->edges = (PEDGE)malloc(sizeof(EDGE) * num_edges);
    if (edges->edges == NULL)
    {
        return false;
    }

    edges->num_edges = num_edges;

    return true;
}

static
void
Edges1DDownsize(
    _Inout_ PEDGES_1D edges,
    _In_ size_t num_edges
    )
{
    assert(num_edges <= edges->num_edges);

    void* resized = realloc(edges->edges, sizeof(EDGE) * num_edges);

    if (resized != NULL)
    {
        edges->edges = (PEDGE)resized;
    }

    edges->num_edges = num_edges;
}

static
void
Edges1DDestroy(
    _Inout_ _Post_invalid_ PEDGES_1D edges
    )
{
    free(edges->edges);
}

//
// 3D Edges Type
//

typedef struct _EDGES_3D {
    EDGES_1D edges[3];
} EDGES_3D, *PEDGES_3D;

typedef const EDGES_3D *PCEDGES_3D;

//
// 3D Edges Functions
//

static
bool
EdgesInitialize(
    _Out_ PEDGES_3D edges,
    _In_ size_t num_edges
    )
{
    if (!Edges1DInitialize(edges->edges + 0, num_edges))
    {
        return false;
    }

    if (!Edges1DInitialize(edges->edges + 1, num_edges))
    {
        Edges1DDestroy(edges->edges + 0);
        return false;
    }

    if (!Edges1DInitialize(edges->edges + 2, num_edges))
    {
        Edges1DDestroy(edges->edges + 1);
        Edges1DDestroy(edges->edges + 0);
        return false;
    }

    return true;
}

static
void
EdgesDownsize(
    _Inout_ PEDGES_3D edges,
    _In_ size_t num_edges
    )
{
    Edges1DDownsize(edges->edges + 0, num_edges);
    Edges1DDownsize(edges->edges + 1, num_edges);
    Edges1DDownsize(edges->edges + 2, num_edges);
}

static
void
EdgesDestroy(
    _Inout_ _Post_invalid_ PEDGES_3D edges
    )
{
    Edges1DDestroy(edges->edges + 0);
    Edges1DDestroy(edges->edges + 1);
    Edges1DDestroy(edges->edges + 2);
}

//
// KD-Tree Node Defines
//

#define MAX_CHILD_OFFSET (UINT32_MAX >> 2)
#define MAX_LEAF_SIZE    (UINT32_MAX >> 2)
#define INTERIOR_X_SPLIT 0U
#define INTERIOR_Y_SPLIT 1U
#define INTERIOR_Z_SPLIT 2U
#define LEAF             3U

//
// Node Types
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
// Node Static Functions
//

static
inline
void
KdTreeNodeInitializeLeaf(
    _Inout_ PKD_TREE_NODE node,
    _In_ uint32_t size,
    _In_ uint32_t index
    )
{
    assert(size < MAX_LEAF_SIZE);

    node->flags_and_num_shapes_or_child = size | (LEAF << 30);
    node->split_or_index.index = index;
}

static
inline
void
KdTreeNodeInitializeInterior(
    _Inout_ PKD_TREE_NODE node,
    _In_ VECTOR_AXIS axis,
    _In_ uint32_t offset_to_child,
    _In_ float_t split
    )
{
    assert(offset_to_child < MAX_CHILD_OFFSET);

    switch (axis)
    {
        case VECTOR_X_AXIS:
            offset_to_child |= INTERIOR_X_SPLIT << 30;
            break;
        case VECTOR_Y_AXIS:
            offset_to_child |= INTERIOR_Y_SPLIT << 30;
            break;
        case VECTOR_Z_AXIS:
            offset_to_child |= INTERIOR_Z_SPLIT << 30;
            break;
        default:
            assert(false);
    }

    node->flags_and_num_shapes_or_child = (uint32_t)offset_to_child;
    node->split_or_index.split = split;
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
KdTreeNodeLeafSize(
    _In_ PCKD_TREE_NODE node
    )
{
    assert(KdTreeNodeIsLeaf(node));

    return node->flags_and_num_shapes_or_child & ~(LEAF << 30);
}

static
inline
float_t
KdTreeNodeSplit(
    _In_ PCKD_TREE_NODE node
    )
{
    assert(!KdTreeNodeIsLeaf(node));

    return node->split_or_index.split;
}

static
inline
uint32_t
KdTreeNodeChildIndex(
    _In_ PCKD_TREE_NODE node
    )
{
    assert(!KdTreeNodeIsLeaf(node));

    return node->flags_and_num_shapes_or_child & ~(LEAF << 30);
}

//
// Node Builder Defines
//

#define DESIRED_ALIGNMENT 4096
#define INITIAL_NODES 512
#define INITIAL_INDICIES 1024

//
// Node Builder Type
//

typedef struct _NODE_BUILDER {
    _Field_size_(nodes_capacity) PKD_TREE_NODE nodes;
    size_t nodes_capacity;
    size_t nodes_size;
    _Field_size_(indices_capacity) uint32_t *indices;
    size_t indices_capacity;
    size_t indices_size;
} NODE_BUILDER, *PNODE_BUILDER;

//
// Node Builder Static Functions
//

static
bool
NodeBuilderInitialize(
    _Out_ PNODE_BUILDER node_builder
    )
{
    node_builder->nodes = aligned_alloc(DESIRED_ALIGNMENT,
                                        INITIAL_NODES * sizeof(KD_TREE_NODE));

    if (node_builder->nodes == NULL)
    {
        return false;
    }

    node_builder->nodes_capacity = INITIAL_NODES;
    node_builder->nodes_size = 0;

    node_builder->indices = malloc(INITIAL_INDICIES * sizeof(uint32_t));

    if (node_builder->indices == NULL)
    {
        free(node_builder->nodes);
        return false;
    }

    node_builder->indices_capacity = INITIAL_INDICIES;
    node_builder->indices_size = 0;

    return true;
}

static
void
NodeBuilderResizeToFit(
    _Inout_ PNODE_BUILDER node_builder
    )
{
    size_t nodes_size = node_builder->nodes_size * sizeof(KD_TREE_NODE);
    void *nodes_allocation = aligned_alloc(DESIRED_ALIGNMENT, nodes_size);
    if (nodes_allocation != NULL)
    {
        memcpy(nodes_allocation, node_builder->nodes, nodes_size);
        free(node_builder->nodes);
        node_builder->nodes = (PKD_TREE_NODE)nodes_allocation;
        node_builder->nodes_capacity = node_builder->nodes_size;
    }

    if (node_builder->indices_size == 0)
    {
        free(node_builder->indices);
        node_builder->indices = NULL;
        node_builder->indices_capacity = 0;
    }
    else
    {
        size_t indices_size = node_builder->indices_size * sizeof(uint32_t);
        void *indices_allocation = realloc(node_builder->indices, indices_size);
        if (indices_allocation != NULL)
        {
            node_builder->indices = (uint32_t*)indices_allocation;
            node_builder->indices_capacity = node_builder->indices_size;
        }
    }
}

static
void
NodeBuilderDestroy(
    _Inout_ _Post_invalid_ PNODE_BUILDER node_builder
    )
{
    free(node_builder->indices);
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
    success = CheckedMultiplySizeT(new_capacity, sizeof(KD_TREE_NODE), &bytes);

    if (!success)
    {
        return false;
    }

    PKD_TREE_NODE new_nodes =
        (PKD_TREE_NODE)aligned_alloc(DESIRED_ALIGNMENT, bytes);

    if (new_nodes == NULL)
    {
        return false;
    }

    memcpy(new_nodes,
           node_builder->nodes,
           node_builder->nodes_capacity * sizeof(KD_TREE_NODE));

    free(node_builder->nodes);
    node_builder->nodes = new_nodes;
    node_builder->nodes_capacity = new_capacity;

    return true;
}

static
bool
NodeBuilderGrowIndices(
    _Inout_ PNODE_BUILDER node_builder
    )
{
    size_t new_capacity;
    bool success = CheckedMultiplySizeT(node_builder->indices_capacity,
                                        2,
                                        &new_capacity);

    if (!success)
    {
        return false;
    }

    size_t bytes;
    success = CheckedMultiplySizeT(new_capacity, sizeof(uint32_t), &bytes);

    if (!success)
    {
        return false;
    }

    uint32_t *new_indices =
        (uint32_t*)realloc(node_builder->indices, bytes);

    if (new_indices == NULL)
    {
        return false;
    }

    node_builder->indices = new_indices;
    node_builder->indices_capacity = new_capacity;

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
    _Inout_ PNODE_BUILDER node_builder,
    _In_ PCEDGES_3D edges,
    _In_ bool is_below,
    _Out_ size_t *index
    )
{
    uint32_t num_shapes = (uint32_t)(edges->edges[0].num_edges >> 1);

    if (MAX_LEAF_SIZE < num_shapes)
    {
        return false;
    }

    bool success = NodeBuilderAllocateNode(node_builder, index);

    if (!success)
    {
        return false;
    }

    if (num_shapes == 1)
    {
        KdTreeNodeInitializeLeaf(node_builder->nodes + *index,
                                 1,
                                 edges->edges[0].edges[0].primitive);

        return true;
    }

    size_t required_size;
    success = CheckedAddSizeT(node_builder->indices_size,
                              (size_t)num_shapes,
                              &required_size);

    if (!success)
    {
        return false;
    }

    while (node_builder->indices_capacity < required_size)
    {
        if (!NodeBuilderGrowIndices(node_builder))
        {
            return false;
        }
    }

    uint32_t start_index = (uint32_t)node_builder->indices_size;
    for (size_t i = 0; i < edges->edges[0].num_edges; i++)
    {
        if (edges->edges[0].edges[i].is_start == is_below)
        {
            uint32_t primitive = edges->edges[0].edges[i].primitive;
            node_builder->indices[node_builder->indices_size++] = primitive;
        }
    }

    KdTreeNodeInitializeLeaf(node_builder->nodes + *index,
                             num_shapes,
                             start_index);

    return true;
}

static
bool
NodeBuilderInitializeInteriorNode(
    _Inout_ PNODE_BUILDER node_builder,
    _In_ size_t node_index,
    _In_ VECTOR_AXIS axis,
    _In_ size_t child_index,
    _In_ float_t split
    )
{
    size_t offset_to_child = child_index - node_index;

    if (MAX_CHILD_OFFSET < offset_to_child)
    {
        return false;
    }

    KdTreeNodeInitializeInterior(node_builder->nodes + node_index,
                                 axis,
                                 offset_to_child,
                                 split);

    return true;
}

//
// Build Tree Defines
//

#define TARGET_LEAF_SIZE ((size_t)1)
#define INTERSECTION_COST ((float_t)80.0)
#define TRAVERSAL_COST ((float_t)1.0)
#define EMPTY_BONUS ((float_t)0.5)

//
// Build Tree Static Functions
//

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
bool
KdTreeEvaluateSplitsOnAxis(
    _In_ PCEDGES_3D edges,
    _In_ BOUNDING_BOX node_bound,
    _In_ VECTOR_AXIS axis,
    _Out_ float_t *best_cost,
    _Out_ size_t *best_split,
    _Out_ uint32_t *best_above_shapes,
    _Out_ uint32_t *best_below_shapes
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

    uint32_t num_above = (uint32_t)(edges->edges[axis].num_edges >> 1);
    uint32_t num_below = 0;

    for (size_t i = 0; i < edges->edges[axis].num_edges; i++)
    {
        bool is_start = edges->edges[axis].edges[i].is_start;
        if (!is_start)
        {
            num_above -= 1;
        }

        float_t value = edges->edges[axis].edges[i].value;
        if (lower_bound < value && value < upper_bound)
        {
            float_t below_area = (float_t)2.0 *
                (side_face_area + (value - lower_bound) * other_sum);
            float_t above_area = (float_t)2.0 *
                (side_face_area + (upper_bound - value) * other_sum);

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
                assert(num_above != 0 || num_below != 0);

                result = true;
                *best_cost = cost;
                *best_split = i;
                *best_above_shapes = num_above;
                *best_below_shapes = num_below;
            }
        }

        if (is_start)
        {
            num_below += 1;
        }
    }

    return result;
}

static
bool
KdTreePartitionEdges(
    _Inout_ _Post_invalid_ PEDGES_3D edges,
    _In_ VECTOR_AXIS split_axis,
    _In_ size_t split_index,
    _In_ uint32_t above_shapes,
    _In_ uint32_t below_shapes,
    _Out_ PEDGES_3D above_edges,
    _Out_ PEDGES_3D below_edges
    )
{
    uint32_t hash_map_size;
    if (below_shapes < above_shapes)
    {
        hash_map_size = above_shapes;
    }
    else
    {
        hash_map_size = below_shapes;
    }

    HASH_SET hash_set;
    bool success = HashSetInitialize(&hash_set, (size_t)hash_map_size);

    if (!success)
    {
        EdgesDestroy(edges);
        return false;
    }

    if (below_shapes < above_shapes)
    {
        for (size_t i = 0; i < split_index; i++)
        {
            if (edges->edges[split_axis].edges[i].is_start)
            {
                HashSetInsert(&hash_set,
                              edges->edges[split_axis].edges[i].primitive);
            }
        }

        success = EdgesInitialize(below_edges, 2 * (size_t)below_shapes);
        if (!success)
        {
            HashSetDestroy(&hash_set);
            EdgesDestroy(edges);
            return false;
        }

        for (size_t i = 0; i < 3; i++)
        {
            PCEDGES_1D edges_1d = &edges->edges[i];
            PEDGES_1D insert = &below_edges->edges[i];
            size_t index = 0;
            for (size_t j = 0; j < edges->edges[i].num_edges; j++)
            {
                if (HashSetContains(&hash_set, edges_1d->edges[j].primitive))
                {
                    insert->edges[index].primitive = edges_1d->edges[j].primitive;
                    insert->edges[index].value = edges_1d->edges[j].value;
                    insert->edges[index].is_start = edges_1d->edges[j].is_start;
                    index += 1;
                }
            }
        }

        HashSetClear(&hash_set);

        for (size_t i = split_index;
             i < edges->edges[split_axis].num_edges;
             i++)
        {
            if (!edges->edges[split_axis].edges[i].is_start)
            {
                if (i == split_index)
                {
                    continue;
                }

                HashSetInsert(&hash_set,
                              edges->edges[split_axis].edges[i].primitive);
            }
        }

        for (size_t i = 0; i < 3; i++)
        {
            PCEDGES_1D edges_1d = &edges->edges[i];
            size_t index = 0;
            for (size_t j = 0; j < edges->edges[i].num_edges; j++)
            {
                if (HashSetContains(&hash_set, edges_1d->edges[j].primitive))
                {
                    edges_1d->edges[index].primitive = edges_1d->edges[j].primitive;
                    edges_1d->edges[index].value = edges_1d->edges[j].value;
                    edges_1d->edges[index].is_start = edges_1d->edges[j].is_start;
                    index += 1;
                }
            }
        }

        EdgesDownsize(edges, 2 * (size_t)above_shapes);

        *above_edges = *edges;
    }
    else
    {
        for (size_t i = split_index;
             i < edges->edges[split_axis].num_edges;
             i++)
        {
            if (!edges->edges[split_axis].edges[i].is_start)
            {
                if (i == split_index)
                {
                    continue;
                }

                HashSetInsert(&hash_set,
                              edges->edges[split_axis].edges[i].primitive);
            }
        }

        success = EdgesInitialize(above_edges, 2 * (size_t)above_shapes);
        if (!success)
        {
            HashSetDestroy(&hash_set);
            EdgesDestroy(edges);
            return false;
        }

        for (size_t i = 0; i < 3; i++)
        {
            PCEDGES_1D edges_1d = &edges->edges[i];
            PEDGES_1D insert = &above_edges->edges[i];
            size_t index = 0;
            for (size_t j = 0; j < edges->edges[i].num_edges; j++)
            {
                if (HashSetContains(&hash_set, edges_1d->edges[j].primitive))
                {
                    insert->edges[index].primitive = edges_1d->edges[j].primitive;
                    insert->edges[index].value = edges_1d->edges[j].value;
                    insert->edges[index].is_start = edges_1d->edges[j].is_start;
                    index += 1;
                }
            }
        }

        HashSetClear(&hash_set);

        for (size_t i = 0; i < split_index; i++)
        {
            if (edges->edges[split_axis].edges[i].is_start)
            {
                HashSetInsert(&hash_set,
                              edges->edges[split_axis].edges[i].primitive);
            }
        }

        for (size_t i = 0; i < 3; i++)
        {
            PCEDGES_1D edges_1d = &edges->edges[i];
            size_t index = 0;
            for (size_t j = 0; j < edges->edges[i].num_edges; j++)
            {
                if (HashSetContains(&hash_set, edges_1d->edges[j].primitive))
                {
                    edges_1d->edges[index].primitive = edges_1d->edges[j].primitive;
                    edges_1d->edges[index].value = edges_1d->edges[j].value;
                    edges_1d->edges[index].is_start = edges_1d->edges[j].is_start;
                    index += 1;
                }
            }
        }

        EdgesDownsize(edges, 2 * (size_t)below_shapes);

        *below_edges = *edges;
    }

    HashSetDestroy(&hash_set);

    return true;
}

static
bool
KdTreeBuildImpl(
    _Inout_ PNODE_BUILDER node_builder,
    _Inout_ _Post_invalid_ PEDGES_3D edges,
    _In_ BOUNDING_BOX node_bounds,
    _In_ size_t depth_remaining,
    _In_ bool is_below,
    _Out_ size_t *index
    )
{
    uint32_t num_shapes = (uint32_t)(edges->edges[0].num_edges >> 1);

    if (num_shapes <= TARGET_LEAF_SIZE || depth_remaining == 0)
    {
        bool success = NodeBuilderAllocateLeafNode(node_builder,
                                                   edges,
                                                   is_below,
                                                   index);

        EdgesDestroy(edges);

        return success;
    }

    float_t best_cost = INFINITY;
    VECTOR_AXIS best_axis = VECTOR_X_AXIS;
    size_t best_split = 0;
    uint32_t best_above_shapes = num_shapes;
    uint32_t best_below_shapes = num_shapes;
    bool should_split = false;

    VECTOR_AXIS axis = BoundingBoxDominantAxis(node_bounds);

    for (size_t i = 0; i < 3; i++)
    {
        float_t cost;
        size_t split;
        uint32_t num_above, num_below;
        bool success = KdTreeEvaluateSplitsOnAxis(edges,
                                                  node_bounds,
                                                  axis,
                                                  &cost,
                                                  &split,
                                                  &num_above,
                                                  &num_below);

        if (success && cost < best_cost)
        {
            should_split = true;

            best_cost = cost;
            best_axis = axis;
            best_split = split;
            best_above_shapes = num_above;
            best_below_shapes = num_below;
        }

        axis = NextAxis(axis);
    }

    if (!should_split)
    {
        bool success = NodeBuilderAllocateLeafNode(node_builder,
                                                   edges,
                                                   is_below,
                                                   index);

        EdgesDestroy(edges);

        return success;
    }

    bool success = NodeBuilderAllocateNode(node_builder, index);

    if (!success)
    {
        EdgesDestroy(edges);
        return false;
    }

    float_t split = edges->edges[best_axis].edges[best_split].value;

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

    EDGES_3D above_edges;
    EDGES_3D below_edges;
    if (best_below_shapes == 0)
    {
        above_edges = *edges;
        memset(&below_edges, 0, sizeof(EDGES_3D));
    }
    else if (best_above_shapes == 0)
    {
        memset(&above_edges, 0, sizeof(EDGES_3D));
        below_edges = *edges;
    }
    else
    {
        success = KdTreePartitionEdges(edges,
                                       best_axis,
                                       best_split,
                                       best_above_shapes,
                                       best_below_shapes,
                                       &above_edges,
                                       &below_edges);

        if (!success)
        {
            return false;
        }
    }

    size_t unused_below_index;
    success = KdTreeBuildImpl(node_builder,
                              &below_edges,
                              below_bounds,
                              depth_remaining - 1,
                              true,
                              &unused_below_index);

    if (!success)
    {
        EdgesDestroy(&above_edges);
        return false;
    }

    size_t above_index;
    success = KdTreeBuildImpl(node_builder,
                              &above_edges,
                              above_bounds,
                              depth_remaining - 1,
                              false,
                              &above_index);

    if (!success)
    {
        return false;
    }

    success = NodeBuilderInitializeInteriorNode(node_builder,
                                                *index,
                                                best_axis,
                                                above_index,
                                                split);

    return success;
}

static
ISTATUS
KdTreeSortEdges(
    _In_reads_(num_shapes) const PSHAPE shapes[],
    _In_reads_opt_(num_shapes) const PMATRIX transforms[],
    _In_reads_opt_(num_shapes) const bool premultiplied[],
    _In_ size_t num_shapes,
    _Out_ PEDGES_3D edges,
    _Out_ PBOUNDING_BOX total_bounds
    )
{
    if (num_shapes > UINT32_MAX)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    if (num_shapes == 0)
    {
        POINT3 point = PointCreate((float_t)0.0, (float_t)0.0, (float_t)0.0);
        *total_bounds = BoundingBoxCreate(point, point);
        return ISTATUS_SUCCESS;
    }

    PCMATRIX transform = NULL;
    if (transforms != NULL && transforms[0] != NULL)
    {
        if (premultiplied == NULL || !premultiplied[0])
        {
            transform = transforms[0];
        }
    }

    ISTATUS status = ShapeComputeBounds(shapes[0],
                                        transform,
                                        total_bounds);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    for (size_t j = 0; j < 3; j++)
    {
        edges->edges[j].edges[0].is_start = true;
        edges->edges[j].edges[0].primitive = 0;
        edges->edges[j].edges[0].value =
            PointGetElement(total_bounds->corners[0], j);

        edges->edges[j].edges[1].is_start = false;
        edges->edges[j].edges[1].primitive = 0;
        edges->edges[j].edges[1].value =
            PointGetElement(total_bounds->corners[1], j);
    }

    for (uint32_t i = 1; i < (uint32_t)num_shapes; i++)
    {
        transform = NULL;
        if (transforms != NULL && transforms[i] != NULL)
        {
            if (premultiplied == NULL || !premultiplied[i])
            {
                transform = transforms[i];
            }
        }

        BOUNDING_BOX shape_bounds;
        status = ShapeComputeBounds(shapes[i],
                                    transform,
                                    &shape_bounds);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }

        *total_bounds = BoundingBoxUnion(*total_bounds, shape_bounds);

        for (size_t j = 0; j < 3; j++)
        {
            edges->edges[j].edges[2 * i].is_start = true;
            edges->edges[j].edges[2 * i].primitive = i;
            edges->edges[j].edges[2 * i].value =
                PointGetElement(shape_bounds.corners[0], j);

            edges->edges[j].edges[2 * i + 1].is_start = false;
            edges->edges[j].edges[2 * i + 1].primitive = i;
            edges->edges[j].edges[2 * i + 1].value =
                PointGetElement(shape_bounds.corners[1], j);
        }
    }

    qsort(edges->edges[0].edges, num_shapes * 2, sizeof(EDGE), EdgeCompare);
    qsort(edges->edges[1].edges, num_shapes * 2, sizeof(EDGE), EdgeCompare);
    qsort(edges->edges[2].edges, num_shapes * 2, sizeof(EDGE), EdgeCompare);

    return ISTATUS_SUCCESS;
}

static
ISTATUS
KdTreeBuild(
    _Inout_ PNODE_BUILDER node_builder,
    _In_reads_opt_(num_shapes) const PSHAPE shapes[],
    _In_reads_opt_(num_shapes) const PMATRIX transforms[],
    _In_reads_(num_shapes) const bool premultiplied[],
    _In_ size_t num_shapes,
    _In_ size_t max_depth,
    _Out_ PBOUNDING_BOX scene_bounds
    )
{
    size_t num_edges;
    bool success = CheckedMultiplySizeT(num_shapes, 2, &num_edges);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    EDGES_3D edges;
    success = EdgesInitialize(&edges, num_edges);
    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    ISTATUS status = KdTreeSortEdges(shapes,
                                     transforms,
                                     premultiplied,
                                     num_shapes,
                                     &edges,
                                     scene_bounds);

    if (status != ISTATUS_SUCCESS)
    {
        EdgesDestroy(&edges);
        return status;
    }

    size_t unused_index;
    success = KdTreeBuildImpl(node_builder,
                              &edges,
                              *scene_bounds,
                              max_depth,
                              true,
                              &unused_index);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    return ISTATUS_SUCCESS;
}

//
// Scene Defines
//

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
            uint32_t num_shapes = KdTreeNodeLeafSize(node);

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

        float_t split = KdTreeNodeSplit(node);
        float_t plane_distance = (split - origin) * direction;

        PCKD_TREE_NODE below_child = node + 1;
        PCKD_TREE_NODE above_child = node + KdTreeNodeChildIndex(node);

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
            uint32_t num_shapes = KdTreeNodeLeafSize(node);

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

        float_t split = KdTreeNodeSplit(node);
        float_t plane_distance = (split - origin) * direction;

        PCKD_TREE_NODE below_child = node + 1;
        PCKD_TREE_NODE above_child = node + KdTreeNodeChildIndex(node);

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
    SHAPE **const shapes = kd_tree->shapes.shapes;

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
            uint32_t num_shapes = KdTreeNodeLeafSize(node);

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

        float_t split = KdTreeNodeSplit(node);
        float_t plane_distance = (split - origin) * direction;

        PCKD_TREE_NODE below_child = node + 1;
        PCKD_TREE_NODE above_child = node + KdTreeNodeChildIndex(node);

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

    float_t max_depth =
        round((float_t)8.0 + (float_t)1.3 * (float_t)Log2(num_shapes));
    max_depth = IMin((float_t)MAX_TREE_DEPTH, max_depth);

    NODE_BUILDER node_builder;
    bool success = NodeBuilderInitialize(&node_builder);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    BOUNDING_BOX scene_bounds;
    ISTATUS status = KdTreeBuild(&node_builder,
                                 shapes,
                                 transforms,
                                 premultiplied,
                                 num_shapes,
                                 (size_t)max_depth,
                                 &scene_bounds);

    if (status != ISTATUS_SUCCESS)
    {
        NodeBuilderDestroy(&node_builder);
        return status;
    }

    NodeBuilderResizeToFit(&node_builder);

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

    KD_TREE_SCENE result;
    result.nodes = node_builder.nodes;
    result.indices = node_builder.indices;
    result.num_shapes = num_shapes;
    result.scene_bounds = scene_bounds;

    PCSCENE_VTABLE vtable;
    void *data;
    if (!transform_needed)
    {
        data = calloc(num_shapes, sizeof(PSHAPE));

        if (data == NULL)
        {
            NodeBuilderDestroy(&node_builder);
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
            NodeBuilderDestroy(&node_builder);
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
            NodeBuilderDestroy(&node_builder);
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
        free(data);
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
// KD Tree Aggregate Type
//

typedef struct _KD_TREE_AGGREGATE {
    BOUNDING_BOX bounds;
    KD_TREE_NODE *nodes;
    uint32_t *indices;
    _Field_size_(num_shapes) PSHAPE *shapes;
    size_t num_shapes;
} KD_TREE_AGGREGATE, *PKD_TREE_AGGREGATE;

typedef const KD_TREE_AGGREGATE *PCKD_TREE_AGGREGATE;

//
// KD Tree Aggregate Static Functions
//

static
inline
ISTATUS
KdTreeAggregateTraceShape(
    _In_ PCSHAPE shape,
    _In_ PCRAY ray,
    _In_ PSHAPE_HIT_ALLOCATOR allocator,
    _Out_ PHIT *hit_list,
    _Out_ ISTATUS *return_status
    )
{
    PHIT hit;
    ISTATUS status = ShapeHitTesterTestNestedShape(allocator,
                                                   shape,
                                                   &hit);

    if (status == ISTATUS_NO_INTERSECTION)
    {
        return ISTATUS_SUCCESS;
    }

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    PHIT last_hit = hit;
    while (last_hit->next != NULL) {
        last_hit = last_hit->next;
    }

    last_hit->next = *hit_list;
    *hit_list = hit;
    *return_status = ISTATUS_SUCCESS;

    return ISTATUS_SUCCESS;
}

static
ISTATUS
KdTreeAggregateTrace(
    _In_ const void *context,
    _In_ PCRAY ray,
    _In_ PSHAPE_HIT_ALLOCATOR allocator,
    _Out_ PHIT *hit
    )
{
    PCKD_TREE_AGGREGATE aggregate = (PCKD_TREE_AGGREGATE)context;
    PCKD_TREE_NODE node = aggregate->nodes;
    const uint32_t *all_indices = aggregate->indices;

    VECTOR3 inverse_direction;
    float_t node_min, node_max;
    bool intersects = BoundingBoxIntersect(aggregate->bounds,
                                           *ray,
                                           &inverse_direction,
                                           &node_min,
                                           &node_max);

    if (!intersects)
    {
        return ISTATUS_NO_INTERSECTION;
    }

    ISTATUS return_status = ISTATUS_NO_INTERSECTION;
    *hit = NULL;

    WORK_ITEM work_queue[MAX_TREE_DEPTH];
    size_t queue_size = 0;
    for (;;)
    {
        // TODO: Consider breaking if closest_hit_distance < node_min
        //       Will improve performance; however, requires epsilon

        if (KdTreeNodeIsLeaf(node))
        {
            uint32_t num_shapes = KdTreeNodeLeafSize(node);

            if (num_shapes == 1)
            {
                uint32_t index = node->split_or_index.index;
                ISTATUS status =
                    KdTreeAggregateTraceShape(aggregate->shapes[index],
                                              ray,
                                              allocator,
                                              hit,
                                              &return_status);

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
                    PCSHAPE shape = aggregate->shapes[node_indices[i]];
                    ISTATUS status =
                        KdTreeAggregateTraceShape(shape,
                                                  ray,
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
            node = work_queue[queue_size].node;
            node_min = work_queue[queue_size].min;
            node_max = work_queue[queue_size].max;
            continue;
        }

        uint32_t split_axis = KdTreeNodeType(node);
        float_t origin = PointGetElement(ray->origin, split_axis);
        float_t direction = VectorGetElement(inverse_direction, split_axis);

        float_t split = KdTreeNodeSplit(node);
        float_t plane_distance = (split - origin) * direction;

        PCKD_TREE_NODE below_child = node + 1;
        PCKD_TREE_NODE above_child = node + KdTreeNodeChildIndex(node);

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

    return return_status;
}

static
ISTATUS
KdTreeAggregateComputeBounds(
    _In_ const void *context,
    _In_opt_ PCMATRIX model_to_world,
    _Out_ PBOUNDING_BOX world_bounds
    )
{
    PCKD_TREE_AGGREGATE aggregate = (PCKD_TREE_AGGREGATE)context;

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
KdTreeAggregateFree(
    _In_opt_ _Post_invalid_ void *context
    )
{
    PKD_TREE_AGGREGATE aggregate = (PKD_TREE_AGGREGATE)context;

    for (size_t i = 0; i < aggregate->num_shapes; i++)
    {
        ShapeRelease(aggregate->shapes[i]);
    }

    free(aggregate->shapes);
    free(aggregate->nodes);
    free(aggregate->indices);
}

//
// KD Tree Aggregate Static Data
//

static const SHAPE_VTABLE kd_tree_aggregate_vtable = {
    KdTreeAggregateTrace,
    KdTreeAggregateComputeBounds,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    KdTreeAggregateFree
};

//
// KD Tree Aggregate Functions
//

ISTATUS
KdTreeAggregateAllocate(
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

    float_t max_depth =
        round((float_t)8.0 + (float_t)1.3 * (float_t)Log2(num_shapes));
    max_depth = IMin((float_t)MAX_TREE_DEPTH, max_depth);

    NODE_BUILDER node_builder;
    bool success = NodeBuilderInitialize(&node_builder);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    KD_TREE_AGGREGATE kd_aggregate;

    ISTATUS status = KdTreeBuild(&node_builder,
                                 shapes,
                                 NULL,
                                 NULL,
                                 num_shapes,
                                 (size_t)max_depth,
                                 &kd_aggregate.bounds);

    if (status != ISTATUS_SUCCESS)
    {
        NodeBuilderDestroy(&node_builder);
    }

    NodeBuilderResizeToFit(&node_builder);

    kd_aggregate.shapes = (PSHAPE*)calloc(num_shapes, sizeof(PSHAPE));

    if (kd_aggregate.shapes == NULL)
    {
        NodeBuilderDestroy(&node_builder);
        return ISTATUS_ALLOCATION_FAILED;
    }

    memcpy(kd_aggregate.shapes, shapes, num_shapes * sizeof(PSHAPE));

    kd_aggregate.num_shapes = num_shapes;

    kd_aggregate.nodes = node_builder.nodes;
    kd_aggregate.indices = node_builder.indices;

    status = ShapeAllocate(&kd_tree_aggregate_vtable,
                           &kd_aggregate,
                           sizeof(KD_TREE_AGGREGATE),
                           alignof(KD_TREE_AGGREGATE),
                           aggregate);

    if (status != ISTATUS_SUCCESS)
    {
        free(kd_aggregate.shapes);
        NodeBuilderDestroy(&node_builder);
    }

    for (size_t i = 0; i < num_shapes; i++)
    {
        ShapeRetain(shapes[i]);
    }

    return ISTATUS_SUCCESS;
}