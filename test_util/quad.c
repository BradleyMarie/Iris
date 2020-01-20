/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    quad.c

Abstract:

    Allocates two triangles from the vertices of a quadrilateral.

--*/

#include "iris_physx_toolkit/triangle.h"

//
// Functions
//

ISTATUS
EmissiveQuadAllocate(
    _In_ POINT3 v0,
    _In_ POINT3 v1,
    _In_ POINT3 v2,
    _In_ POINT3 v3,
    _In_opt_ PMATERIAL front_material,
    _In_opt_ PMATERIAL back_material,
    _In_opt_ PEMISSIVE_MATERIAL front_emissive_material,
    _In_opt_ PEMISSIVE_MATERIAL back_emissive_material,
    _Out_ PSHAPE *shape0,
    _Out_ PSHAPE *shape1
    )
{
    if (!PointValidate(v0))
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (!PointValidate(v1))
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (!PointValidate(v2))
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (!PointValidate(v3))
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (shape0 == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_08;
    }

    if (shape1 == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_09;
    }

    ISTATUS status;
    if (front_emissive_material == NULL &&
        back_emissive_material == NULL)
    {
        status = TriangleAllocate(v0,
                                  v1,
                                  v2,
                                  front_material,
                                  back_material,
                                  shape0);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }

        status = TriangleAllocate(v0,
                                  v2,
                                  v3,
                                  front_material,
                                  back_material,
                                  shape1);
    }
    else
    {
        status = EmissiveTriangleAllocate(v0,
                                          v1,
                                          v2,
                                          front_material,
                                          back_material,
                                          front_emissive_material,
                                          back_emissive_material,
                                          shape0);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }

        status = EmissiveTriangleAllocate(v0,
                                          v2,
                                          v3,
                                          front_material,
                                          back_material,
                                          front_emissive_material,
                                          back_emissive_material,
                                          shape1);
    }

    return status;
}