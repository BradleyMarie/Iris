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
    _Inout_ PSHAPE_HIT_COLLECTION_INTERNAL ShapeHitCollection
    )
{
	ASSERT(Context != NULL);
	ASSERT(Ray != NULL);
	ASSERT(ShapeHitCollection != NULL);

	return ISTATUS_SUCCESS;
}