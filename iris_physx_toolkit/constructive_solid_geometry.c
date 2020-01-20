/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    constructive_solid_geometry.c

Abstract:

    Implements a constructive solid geometry.

--*/

#include <stdalign.h>

#include "iris_physx_toolkit/constructive_solid_geometry.h"

//
// Types
//

typedef struct _CSG_SHAPE {
    PSHAPE shapes[2];
} CSG_SHAPE, *PCSG_SHAPE;

typedef const CSG_SHAPE *PCCSG_SHAPE;

//
// Static Functions
//

static
ISTATUS
DifferenceShapeTrace(
    _In_ const void *context,
    _In_ PCRAY ray,
    _In_ PSHAPE_HIT_ALLOCATOR allocator,
    _Out_ PHIT *hit
    )
{
    PCCSG_SHAPE csg_shape = (PCCSG_SHAPE)context;

    PHIT hits0;
    ISTATUS status = ShapeHitTesterTestNestedShape(allocator,
                                                   csg_shape->shapes[0],
                                                   &hits0);

    if (status == ISTATUS_NO_INTERSECTION)
    {
        return ISTATUS_NO_INTERSECTION;
    }

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    PHIT hits1;
    status = ShapeHitTesterTestNestedShape(allocator,
                                           csg_shape->shapes[1],
                                           &hits1);

    if (status == ISTATUS_NO_INTERSECTION)
    {
        hits1 = NULL;
    }
    else if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    PHIT last_hit = NULL;
    bool inside0 = false;
    bool inside1 = false;
    status = ISTATUS_NO_INTERSECTION;
    *hit = NULL;
    while (hits0 != NULL && hits1 != NULL)
    {
        assert(!hits0->next || hits0->distance <= hits0->distance);
        assert(!hits1->next || hits1->distance <= hits1->distance);
        if (hits0->distance < hits1->distance)
        {
            if (!inside1)
            {
                if (*hit == NULL)
                {
                    *hit = hits0;
                    status = ISTATUS_SUCCESS;
                }
                else
                {
                    last_hit->next = hits0;
                }

                PHIT current_hit = hits0;
                hits0 = hits0->next;
                current_hit->next = NULL;
                last_hit = current_hit;
            }
            else
            {
                hits0 = hits0->next;
            }

            inside0 = !inside0;
        }
        else if (hits0->distance > hits1->distance)
        {
            if (inside0)
            {
                if (*hit == NULL)
                {
                    *hit = hits1;
                    status = ISTATUS_SUCCESS;
                }
                else
                {
                    last_hit->next = hits1;
                }

                PHIT current_hit = hits1;
                hits1 = hits1->next;
                current_hit->next = NULL;
                last_hit = current_hit;
            }
            else
            {
                hits1 = hits1->next;
            }

            inside1 = !inside1;
        }
        else
        {
            if (!inside1)
            {
                if (*hit == NULL)
                {
                    *hit = hits0;
                    status = ISTATUS_SUCCESS;
                }
                else
                {
                    last_hit->next = hits0;
                }

                PHIT current_hit = hits0;
                hits0 = hits0->next;
                hits1 = hits1->next;
                current_hit->next = NULL;
                last_hit = current_hit;
            }
            else if (inside0)
            {
                if (*hit == NULL)
                {
                    *hit = hits1;
                    status = ISTATUS_SUCCESS;
                }
                else
                {
                    last_hit->next = hits1;
                }

                PHIT current_hit = hits1;
                hits0 = hits0->next;
                hits1 = hits1->next;
                current_hit->next = NULL;
                last_hit = current_hit;
            }
            else
            {
                hits0 = hits0->next;
                hits1 = hits1->next;
            }

            inside0 = !inside0;
            inside1 = !inside1;
        }
    }

    while (hits0 != NULL)
    {
        assert(!hits0->next || hits0->distance <= hits0->distance);
        if (*hit == NULL)
        {
            *hit = hits0;
            status = ISTATUS_SUCCESS;
        }
        else
        {
            last_hit->next = hits0;
        }

        PHIT current_hit = hits0;
        hits0 = hits0->next;
        current_hit->next = NULL;
        last_hit = current_hit;
    }

    return status;
}

static
ISTATUS
DifferenceShapeComputeBounds(
    _In_ const void *context,
    _In_opt_ PCMATRIX model_to_world,
    _Out_ PBOUNDING_BOX world_bounds
    )
{
    PCSG_SHAPE csg_shape = (PCSG_SHAPE)context;

    ISTATUS status = ShapeComputeBounds(csg_shape->shapes[0],
                                        model_to_world,
                                        world_bounds);

    return status;
}

static
ISTATUS
IntersectionShapeTrace(
    _In_ const void *context,
    _In_ PCRAY ray,
    _In_ PSHAPE_HIT_ALLOCATOR allocator,
    _Out_ PHIT *hit
    )
{
    PCCSG_SHAPE csg_shape = (PCCSG_SHAPE)context;

    PHIT hits0;
    ISTATUS status = ShapeHitTesterTestNestedShape(allocator,
                                                   csg_shape->shapes[0],
                                                   &hits0);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    PHIT hits1;
    status = ShapeHitTesterTestNestedShape(allocator,
                                           csg_shape->shapes[1],
                                           &hits1);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    PHIT last_hit = NULL;
    bool inside0 = false;
    bool inside1 = false;
    status = ISTATUS_NO_INTERSECTION;
    *hit = NULL;
    while (hits0 != NULL && hits1 != NULL)
    {
        assert(!hits0->next || hits0->distance <= hits0->distance);
        assert(!hits1->next || hits1->distance <= hits1->distance);
        if (hits0->distance < hits1->distance)
        {
            if (inside1)
            {
                if (*hit == NULL)
                {
                    *hit = hits0;
                    status = ISTATUS_SUCCESS;
                }
                else
                {
                    last_hit->next = hits0;
                }

                PHIT current_hit = hits0;
                hits0 = hits0->next;
                current_hit->next = NULL;
                last_hit = current_hit;
            }
            else
            {
                hits0 = hits0->next;
            }

            inside0 = !inside0;
        }
        else if(hits0->distance > hits1->distance)
        {
            if (inside0)
            {
                if (*hit == NULL)
                {
                    *hit = hits1;
                    status = ISTATUS_SUCCESS;
                }
                else
                {
                    last_hit->next = hits1;
                }

                PHIT current_hit = hits1;
                hits1 = hits1->next;
                current_hit->next = NULL;
                last_hit = current_hit;
            }
            else
            {
                hits1 = hits1->next;
            }

            inside1 = !inside1;
        }
        else
        {
            if (inside1)
            {
                if (*hit == NULL)
                {
                    *hit = hits0;
                    status = ISTATUS_SUCCESS;
                }
                else
                {
                    last_hit->next = hits0;
                }

                PHIT current_hit = hits0;
                hits0 = hits0->next;
                hits1 = hits1->next;
                current_hit->next = NULL;
                last_hit = current_hit;
            }
            else if (inside0)
            {
                if (*hit == NULL)
                {
                    *hit = hits1;
                    status = ISTATUS_SUCCESS;
                }
                else
                {
                    last_hit->next = hits1;
                }

                PHIT current_hit = hits1;
                hits0 = hits0->next;
                hits1 = hits1->next;
                current_hit->next = NULL;
                last_hit = current_hit;
            }
            else
            {
                hits0 = hits0->next;
                hits1 = hits1->next;
            }

            inside0 = !inside0;
            inside1 = !inside1;
        }
    }

    return status;
}

static
ISTATUS
IntersectionShapeComputeBounds(
    _In_ const void *context,
    _In_opt_ PCMATRIX model_to_world,
    _Out_ PBOUNDING_BOX world_bounds
    )
{
    PCSG_SHAPE csg_shape = (PCSG_SHAPE)context;

    BOUNDING_BOX bounds0;
    ISTATUS status = ShapeComputeBounds(csg_shape->shapes[0],
                                        model_to_world,
                                        &bounds0);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    BOUNDING_BOX bounds1;
    status = ShapeComputeBounds(csg_shape->shapes[1],
                                model_to_world,
                                &bounds1);

    *world_bounds = BoundingBoxIntersection(bounds0, bounds1);

    return ISTATUS_SUCCESS;
}

static
ISTATUS
UnionShapeTrace(
    _In_ const void *context,
    _In_ PCRAY ray,
    _In_ PSHAPE_HIT_ALLOCATOR allocator,
    _Out_ PHIT *hit
    )
{
    PCCSG_SHAPE csg_shape = (PCCSG_SHAPE)context;

    PHIT hits0;
    ISTATUS status = ShapeHitTesterTestNestedShape(allocator,
                                                   csg_shape->shapes[0],
                                                   &hits0);

    if (status == ISTATUS_NO_INTERSECTION)
    {
        hits0 = NULL;
    }
    else if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    PHIT hits1;
    status = ShapeHitTesterTestNestedShape(allocator,
                                           csg_shape->shapes[1],
                                           &hits1);

    if (status == ISTATUS_NO_INTERSECTION)
    {
        hits1 = NULL;
    }
    else if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    PHIT last_hit = NULL;
    bool inside0 = false;
    bool inside1 = false;
    status = ISTATUS_NO_INTERSECTION;
    *hit = NULL;
    while (hits0 != NULL && hits1 != NULL)
    {
        assert(!hits0->next || hits0->distance <= hits0->distance);
        assert(!hits1->next || hits1->distance <= hits1->distance);
        if (hits0->distance < hits1->distance)
        {
            if (!inside1)
            {
                if (*hit == NULL)
                {
                    *hit = hits0;
                    status = ISTATUS_SUCCESS;
                }
                else
                {
                    last_hit->next = hits0;
                }

                PHIT current_hit = hits0;
                hits0 = hits0->next;
                current_hit->next = NULL;
                last_hit = current_hit;
            }
            else
            {
                hits0 = hits0->next;
            }

            inside0 = !inside0;
        }
        else if (hits0->distance > hits1->distance)
        {
            if (!inside0)
            {
                if (*hit == NULL)
                {
                    *hit = hits1;
                    status = ISTATUS_SUCCESS;
                }
                else
                {
                    last_hit->next = hits1;
                }

                PHIT current_hit = hits1;
                hits1 = hits1->next;
                current_hit->next = NULL;
                last_hit = current_hit;
            }
            else
            {
                hits1 = hits1->next;
            }

            inside1 = !inside1;
        }
        else
        {
            if (!inside1)
            {
                if (*hit == NULL)
                {
                    *hit = hits0;
                    status = ISTATUS_SUCCESS;
                }
                else
                {
                    last_hit->next = hits0;
                }

                PHIT current_hit = hits0;
                hits0 = hits0->next;
                hits1 = hits1->next;
                current_hit->next = NULL;
                last_hit = current_hit;
            }
            else if (!inside0)
            {
                if (*hit == NULL)
                {
                    *hit = hits1;
                    status = ISTATUS_SUCCESS;
                }
                else
                {
                    last_hit->next = hits1;
                }

                PHIT current_hit = hits1;
                hits0 = hits0->next;
                hits1 = hits1->next;
                current_hit->next = NULL;
                last_hit = current_hit;
            }
            else
            {
                hits0 = hits0->next;
                hits1 = hits1->next;
            }

            inside0 = !inside0;
            inside1 = !inside1;
        }
    }

    while (hits0 != NULL)
    {
        assert(!hits0->next || hits0->distance <= hits0->distance);
        if (*hit == NULL)
        {
            *hit = hits0;
            status = ISTATUS_SUCCESS;
        }
        else
        {
            last_hit->next = hits0;
        }

        PHIT current_hit = hits0;
        hits0 = hits0->next;
        current_hit->next = NULL;
        last_hit = current_hit;
    }

    while (hits1 != NULL)
    {
        assert(!hits1->next || hits1->distance <= hits1->distance);
        if (*hit == NULL)
        {
            *hit = hits1;
            status = ISTATUS_SUCCESS;
        }
        else
        {
            last_hit->next = hits1;
        }

        PHIT current_hit = hits1;
        hits1 = hits1->next;
        current_hit->next = NULL;
        last_hit = current_hit;
    }

    return status;
}

static
ISTATUS
UnionShapeComputeBounds(
    _In_ const void *context,
    _In_opt_ PCMATRIX model_to_world,
    _Out_ PBOUNDING_BOX world_bounds
    )
{
    PCSG_SHAPE csg_shape = (PCSG_SHAPE)context;

    BOUNDING_BOX bounds0;
    ISTATUS status = ShapeComputeBounds(csg_shape->shapes[0],
                                        model_to_world,
                                        &bounds0);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    BOUNDING_BOX bounds1;
    status = ShapeComputeBounds(csg_shape->shapes[1],
                                model_to_world,
                                &bounds1);

    *world_bounds = BoundingBoxUnion(bounds0, bounds1);

    return status;
}

static
ISTATUS
ConstructiveSolidShapeCacheColors(
    _In_ const void *context,
    _Inout_ PCOLOR_CACHE color_cache
    )
{
    PCSG_SHAPE csg_shape = (PCSG_SHAPE)context;

    ISTATUS status = ShapeCacheColors(csg_shape->shapes[0], color_cache);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    status = ShapeCacheColors(csg_shape->shapes[1], color_cache);

    return status;
}

static
void
ConstructiveSolidShapeFree(
    _In_opt_ _Post_invalid_ void *context
    )
{
    PCSG_SHAPE csg_shape = (PCSG_SHAPE)context;

    ShapeRelease(csg_shape->shapes[0]);
    ShapeRelease(csg_shape->shapes[1]);
}

//
// Static Variables
//

static const SHAPE_VTABLE difference_vtable = {
    DifferenceShapeTrace,
    DifferenceShapeComputeBounds,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    ConstructiveSolidShapeCacheColors,
    ConstructiveSolidShapeFree
};

static const SHAPE_VTABLE intersection_vtable = {
    IntersectionShapeTrace,
    IntersectionShapeComputeBounds,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    ConstructiveSolidShapeCacheColors,
    ConstructiveSolidShapeFree
};

static const SHAPE_VTABLE union_vtable = {
    UnionShapeTrace,
    UnionShapeComputeBounds,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    ConstructiveSolidShapeCacheColors,
    ConstructiveSolidShapeFree
};

//
// Static Functions
//

static
ISTATUS
CsgAllocate(
    _In_ PCSHAPE_VTABLE vtable,
    _In_ PSHAPE shape0,
    _In_ PSHAPE shape1,
    _Out_ PSHAPE *result
    )
{
    if (shape0 == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (shape1 == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (result == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (shape0 == shape1)
    {
        *result = shape0;
        ShapeRetain(shape0);
        return ISTATUS_SUCCESS;
    }

    CSG_SHAPE csg_shape;
    csg_shape.shapes[0] = shape0;
    csg_shape.shapes[1] = shape1;

    ISTATUS status = ShapeAllocate(vtable,
                                   &csg_shape,
                                   sizeof(CSG_SHAPE),
                                   alignof(CSG_SHAPE),
                                   result);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    ShapeRetain(shape0);
    ShapeRetain(shape1);

    return ISTATUS_SUCCESS;
}

//
// Functions
//

ISTATUS
DifferenceAllocate(
    _In_opt_ PSHAPE minuend,
    _In_opt_ PSHAPE subtrahend,
    _Out_ PSHAPE *result
    )
{
    if (minuend == NULL)
    {
        *result = NULL;
        return ISTATUS_SUCCESS;
    }

    if (subtrahend == NULL)
    {
        ShapeRetain(minuend);
        *result = minuend;
        return ISTATUS_SUCCESS;
    }

    ISTATUS status = CsgAllocate(&difference_vtable,
                                 minuend,
                                 subtrahend,
                                 result);

    return status;
}

ISTATUS
IntersectionAllocate(
    _In_opt_ PSHAPE shape0,
    _In_opt_ PSHAPE shape1,
    _Out_ PSHAPE *result
    )
{
    if (shape0 == NULL || shape1 == NULL)
    {
        *result = NULL;
        return ISTATUS_SUCCESS;
    }

    ISTATUS status = CsgAllocate(&intersection_vtable,
                                 shape0,
                                 shape1,
                                 result);

    return status;
}

ISTATUS
UnionAllocate(
    _In_opt_ PSHAPE shape0,
    _In_opt_ PSHAPE shape1,
    _Out_ PSHAPE *result
    )
{
    if (shape0 == NULL)
    {
        ShapeRetain(shape1);
        *result = shape1;
        return ISTATUS_SUCCESS;
    }

    if (shape1 == NULL)
    {
        ShapeRetain(shape0);
        *result = shape0;
        return ISTATUS_SUCCESS;
    }

    ISTATUS status = CsgAllocate(&union_vtable,
                                 shape0,
                                 shape1,
                                 result);

    return status;
}