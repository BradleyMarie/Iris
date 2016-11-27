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
    _Field_size_(GeometryCapacity) PPHYSX_GEOMETRY *Geometry;
    SIZE_T NumberOfGeometry;
    SIZE_T GeometryCapacity;
    _Field_size_(LightsCapacity) PPHYSX_LIGHT *Lights;
    SIZE_T NumberOfLights;
    SIZE_T LightsCapacity;
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

    Allocation = calloc(NumberOfGeometry, sizeof(PPHYSX_GEOMETRY));

    if (Allocation == NULL)
    {
        free(AllocatedReferenceCount);
        return ISTATUS_ALLOCATION_FAILED;
    }
    
    AllocatedReferenceCount->Geometry = (PPHYSX_GEOMETRY*) Allocation;
    AllocatedReferenceCount->NumberOfGeometry = 0;
    AllocatedReferenceCount->GeometryCapacity = NumberOfGeometry;

    Allocation = calloc(NumberOfLights, sizeof(PPHYSX_LIGHT));

    if (Allocation == NULL)
    {
        free(AllocatedReferenceCount->Geometry);
        free(AllocatedReferenceCount);
        return ISTATUS_ALLOCATION_FAILED;
    }

    AllocatedReferenceCount->Lights = (PPHYSX_LIGHT*) Allocation;
    AllocatedReferenceCount->NumberOfLights = 0;
    AllocatedReferenceCount->LightsCapacity = NumberOfLights;
    
    AllocatedReferenceCount->ReferenceCount = 0;

    *ReferenceCount = AllocatedReferenceCount;

    return ISTATUS_SUCCESS;
}

VOID
AreaLightReferenceCountAddGeometry(
    _Inout_ PPHYSX_AREA_LIGHT_REFERENCE_COUNT ReferenceCount,
    _In_ PPHYSX_GEOMETRY Geometry
    )
{
    ASSERT(ReferenceCount != NULL);
    ASSERT(Geometry != NULL);
    ASSERT(ReferenceCount->NumberOfGeometry < ReferenceCount->GeometryCapacity);

    ReferenceCount->Geometry[ReferenceCount->NumberOfGeometry] = Geometry;
    ReferenceCount->NumberOfGeometry += 1;
}

VOID
AreaLightReferenceCountAddLight(
    _Inout_ PPHYSX_AREA_LIGHT_REFERENCE_COUNT ReferenceCount,
    _In_ PPHYSX_LIGHT Light
    )
{
    ASSERT(ReferenceCount != NULL);
    ASSERT(Light != NULL);
    ASSERT(ReferenceCount->NumberOfLights < ReferenceCount->LightsCapacity);

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
        PhysxGeometryFree(AreaLightReferenceCount->Geometry[Index]);
    }

    for (Index = 0; Index < AreaLightReferenceCount->NumberOfLights; Index++)
    {
        PhysxLightFree(AreaLightReferenceCount->Lights[Index]);
    }

    free(AreaLightReferenceCount->Geometry);
    free(AreaLightReferenceCount->Lights);
    free(AreaLightReferenceCount);
}
