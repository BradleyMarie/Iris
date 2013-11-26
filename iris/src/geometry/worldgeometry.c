/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    worldgeometry.c

Abstract:

    This module declares the world model geometry callback.

--*/

#include <irisp.h>

ISTATUS
GeometryTraceShapeCallback(
    _In_ PVOID Context, 
    _In_ PRAY Ray,
    _In_ PVOID GeometryHitList,
    _Inout_ PSHAPE_HIT_LIST ShapeHitList
    )
{
    return ISTATUS_SUCCESS;
}