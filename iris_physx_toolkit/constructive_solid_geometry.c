/*++

Copyright (c) 2019 Brad Weinberger

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
        else
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
    }

    return status;
}

static
ISTATUS
IntersectionShapeCheckBounds(
    _In_ const void *context,
    _In_opt_ PCMATRIX model_to_world,
    _In_ BOUNDING_BOX world_bounds,
    _Out_ bool *contains
    )
{
    PCSG_SHAPE csg_shape = (PCSG_SHAPE)context;

    bool contains0;
    ISTATUS status = ShapeCheckBounds(csg_shape->shapes[0],
                                      model_to_world,
                                      world_bounds,
                                      &contains0);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    bool contains1;
    status = ShapeCheckBounds(csg_shape->shapes[1],
                              model_to_world,
                              world_bounds,
                              &contains1);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    *contains = contains0 && contains1;

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
    status = ISTATUS_NO_INTERSECTION;
    *hit = NULL;
    while (hits0 != NULL && hits1 != NULL)
    {
        assert(!hits0->next || hits0->distance <= hits0->distance);
        assert(!hits1->next || hits1->distance <= hits1->distance);
        if (hits0->distance < hits1->distance)
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
UnionShapeCheckBounds(
    _In_ const void *context,
    _In_opt_ PCMATRIX model_to_world,
    _In_ BOUNDING_BOX world_bounds,
    _Out_ bool *contains
    )
{
    PCSG_SHAPE csg_shape = (PCSG_SHAPE)context;

    ISTATUS status = ShapeCheckBounds(csg_shape->shapes[0],
                                      model_to_world,
                                      world_bounds,
                                      contains);

    if (status != ISTATUS_SUCCESS || *contains)
    {
        return status;
    }

    status = ShapeCheckBounds(csg_shape->shapes[1],
                              model_to_world,
                              world_bounds,
                              contains);

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

static const SHAPE_VTABLE intersection_vtable = {
    IntersectionShapeTrace,
    IntersectionShapeCheckBounds,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    ConstructiveSolidShapeFree
};

static const SHAPE_VTABLE union_vtable = {
    UnionShapeTrace,
    UnionShapeCheckBounds,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
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
IntersectionAllocate(
    _In_ PSHAPE shape0,
    _In_ PSHAPE shape1,
    _Out_ PSHAPE *result
    )
{
    ISTATUS status = CsgAllocate(&intersection_vtable,
                                 shape0,
                                 shape1,
                                 result);

    return status;
}

ISTATUS
UnionAllocate(
    _In_ PSHAPE shape0,
    _In_ PSHAPE shape1,
    _Out_ PSHAPE *result
    )
{
    ISTATUS status = CsgAllocate(&union_vtable,
                                 shape0,
                                 shape1,
                                 result);

    return status;
}