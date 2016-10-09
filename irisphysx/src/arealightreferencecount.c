/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    arealightreferencecount.c

Abstract:

    This file contains the definitions for the AREA_LIGHT_BUILDER_REFERENCE_COUNT type.

--*/

#include <irisphysxp.h>

#define PHYSX_AREA_LIGHT_BUILDER_GEOMETRY_DATA_INITIAL_CAPACITY 2
#define PHYSX_AREA_LIGHT_BUILDER_LIGHT_DATA_INITIAL_CAPACITY 2

//
// Types
//

struct _PHYSX_AREA_LIGHT_REFERENCE_COUNT {
    _Field_size_(NumberOfGeometry) PPBR_GEOMETRY *Geometry;
    SIZE_T NumberOfGeometry;
    _Field_size_(NumberOfLights) PPBR_LIGHT *Lights;
    SIZE_T NumberOfLights;
    SIZE_T ReferenceCount;
};

//
// Internal Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
AreaLightReferenceCountAllocate(
    _In_ SIZE_T NumberOfGeometry,
    _In_ SIZE_T NumberOfLights,
    _Out_ PPHYSX_AREA_LIGHT_REFERENCE_COUNT *ReferenceCount
    )
{
    PVOID Allocation;
    PPHYSX_AREA_LIGHT_REFERENCE_COUNT AllocatedReferenceCount;

    ASSERT(NumberOfGeometry != 0);
    ASSERT(NumberOfLights != 0);
    ASSERT(ReferenceCount != NULL);

    Allocation = malloc(sizeof(PHYSX_AREA_LIGHT_REFERENCE_COUNT));

    if (Allocation == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    AllocatedReferenceCount = (PPHYSX_AREA_LIGHT_REFERENCE_COUNT) Allocation;

    Allocation = malloc(sizeof(PPBR_GEOMETRY) * NumberOfGeometry);

    if (Allocation == NULL)
    {
        free(AllocatedReferenceCount);
        return ISTATUS_ALLOCATION_FAILED;
    }
    
    AllocatedReferenceCount->Geometry = (PPBR_GEOMETRY*) Allocation;
    AllocatedReferenceCount->NumberOfGeometry = 0;

    Allocation = malloc(sizeof(PPBR_LIGHT) * NumberOfLights);

    if (Allocation == NULL)
    {
        free(AllocatedReferenceCount->Geometry);
        free(AllocatedReferenceCount);
        return ISTATUS_ALLOCATION_FAILED;
    }

    AllocatedReferenceCount->Lights = (PPBR_LIGHT*) Allocation;
    AllocatedReferenceCount->NumberOfLights = 0;

    *ReferenceCount = AllocatedReferenceCount;

    return ISTATUS_SUCCESS;
}

VOID
AreaLightReferenceCountAddGeometry(
    _Inout_ PPHYSX_AREA_LIGHT_REFERENCE_COUNT ReferenceCount,
    _In_ PPBR_GEOMETRY Geometry
    )
{
    ASSERT(ReferenceCount != NULL);
    ASSERT(Geometry != NULL);

    ReferenceCount->Geometry[ReferenceCount->NumberOfGeometry] = Geometry;
    ReferenceCount->NumberOfGeometry += 1;
}

VOID
AreaLightReferenceCountAddLight(
    _Inout_ PPHYSX_AREA_LIGHT_REFERENCE_COUNT ReferenceCount,
    _In_ PPBR_LIGHT Light
    )
{
    ASSERT(ReferenceCount != NULL);
    ASSERT(Light != NULL);

    ReferenceCount->Lights[ReferenceCount->NumberOfLights] = Light;
    ReferenceCount->NumberOfLights += 1;
}

VOID
AreaLightReferenceCountRetain(
    _In_ PPHYSX_AREA_LIGHT_REFERENCE_COUNT AreaLightReferenceCount
    )
{
    ASSERT(AreaLightReferenceCount != NULL);

    AreaLightReferenceCount->ReferenceCount += 1;
}

VOID
AreaLightReferenceCountRelease(
    _In_ _Post_invalid_ PPHYSX_AREA_LIGHT_REFERENCE_COUNT AreaLightReferenceCount
    )
{
    ASSERT(AreaLightReferenceCount != NULL);

    AreaLightReferenceCount->ReferenceCount -= 1;

    if (AreaLightReferenceCount->ReferenceCount == 0)
    {
        AreaLightReferenceCountFree(AreaLightReferenceCount);
    }
}

VOID
AreaLightReferenceCountFree(
    _In_ _Post_invalid_ PPHYSX_AREA_LIGHT_REFERENCE_COUNT AreaLightReferenceCount
    )
{
    SIZE_T Index;

    ASSERT(AreaLightReferenceCount != NULL);

    for (Index = 0; Index < AreaLightReferenceCount->NumberOfGeometry; Index++)
    {
        PBRGeometryFree(AreaLightReferenceCount->Geometry[Index]);
    }

    for (Index = 0; Index < AreaLightReferenceCount->NumberOfLights; Index++)
    {
        PBRLightFree(AreaLightReferenceCount->Lights[Index]);
    }

    free(AreaLightReferenceCount);
}
