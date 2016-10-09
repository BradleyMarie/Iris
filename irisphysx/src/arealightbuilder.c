/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    arealightbuilder.c

Abstract:

    This file contains the definitions for the AREA_LIGHT_BUILDER type.

--*/

#include <irisphysxp.h>

#define PHYSX_AREA_LIGHT_BUILDER_GEOMETRY_DATA_INITIAL_CAPACITY 2
#define PHYSX_AREA_LIGHT_BUILDER_LIGHT_DATA_INITIAL_CAPACITY 2

//
// Types
//

typedef struct _PHYSX_AREA_LIGHT_GEOMETRY_DATA {
    PCPHYSX_LIGHTED_GEOMETRY_VTABLE VTable;
    _Field_size_bytes_(DataSizeInBytes) PVOID Data;
    SIZE_T DataSizeInBytes;
    SIZE_T DataAlignment;
    UINT32_TO_INDEX_MAP AttachedLights;
} PHYSX_AREA_LIGHT_GEOMETRY_DATA, *PPHYSX_AREA_LIGHT_GEOMETRY_DATA;

typedef struct _PHYSX_AREA_LIGHT_LIGHT_DATA {
    PCPHYSX_AREA_LIGHT_VTABLE VTable;
    _Field_size_bytes_(DataSizeInBytes) PVOID Data;
    SIZE_T DataSizeInBytes;
    SIZE_T DataAlignment;
    SIZE_T AttachCount;
} PHYSX_AREA_LIGHT_LIGHT_DATA, *PPHYSX_AREA_LIGHT_LIGHT_DATA;

struct _PHYSX_AREA_LIGHT_BUILDER {
    POINTER_LIST GeometryDataList;
    POINTER_LIST LightDataList;
    BOOL CallDestructorsOnClear;
};

//
// Static Functions
//

STATIC
VOID
PhysxAreaLightBuilderClear(
    _Inout_ PPHYSX_AREA_LIGHT_BUILDER Builder
    )
{
    PPHYSX_AREA_LIGHT_GEOMETRY_DATA GeometryData;
    SIZE_T Index;
    PPHYSX_AREA_LIGHT_LIGHT_DATA LightData;
    SIZE_T NumberOfElements;
    PVOID Pointer;
    
    ASSERT(Builder);
    
    NumberOfElements = PointerListGetSize(&Builder->GeometryDataList);
    
    for (Index = 0; Index < NumberOfElements; Index++)
    {
        Pointer = PointerListRetrieveAtIndex(&Builder->GeometryDataList,
                                             Index);
        
        GeometryData = (PPHYSX_AREA_LIGHT_GEOMETRY_DATA) Pointer;
        
        UInt32ToIndexMapDestroy(&GeometryData->AttachedLights);
        
        if (Builder->CallDestructorsOnClear != FALSE)
        {
            GeometryData->VTable->Header.FreeRoutine(GeometryData->Data);
        }
        
        free(GeometryData->Data);
        free(GeometryData);
    }
    
    PointerListClear(&Builder->GeometryDataList);
    
    NumberOfElements = PointerListGetSize(&Builder->LightDataList);
    
    for (Index = 0; Index < NumberOfElements; Index++)
    {
        Pointer = PointerListRetrieveAtIndex(&Builder->LightDataList,
                                             Index);
        
        LightData = (PPHYSX_AREA_LIGHT_LIGHT_DATA) Pointer;
        
        if (Builder->CallDestructorsOnClear != FALSE)
        {
            LightData->VTable->FreeRoutine(LightData->Data);
        }
        
        free(LightData->Data);
        free(LightData);
    }
    
    PointerListClear(&Builder->LightDataList);
}

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PhysxAreaLightBuilderAllocate(
    _Out_ PPHYSX_AREA_LIGHT_BUILDER *Builder
    )
{
    PPHYSX_AREA_LIGHT_BUILDER Result;
    ISTATUS Status;

    if (Builder == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    Result = (PPHYSX_AREA_LIGHT_BUILDER) malloc(sizeof(PHYSX_AREA_LIGHT_BUILDER));

    if (Result == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    Status = PointerListInitialize(&Result->GeometryDataList);

    if (Status != ISTATUS_SUCCESS)
    {
        ASSERT(Status == ISTATUS_ALLOCATION_FAILED);
        free(Result);
        return Status;
    }

    Status = PointerListInitialize(&Result->LightDataList);

    if (Status != ISTATUS_SUCCESS)
    {
        ASSERT(Status == ISTATUS_ALLOCATION_FAILED);
        PointerListDestroy(&Result->GeometryDataList);
        free(Result);
        return Status;
    }

    Result->CallDestructorsOnClear = FALSE;
    *Builder = Result;

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PhysxAreaLightBuilderAddGeometry(
    _Inout_ PPHYSX_AREA_LIGHT_BUILDER Builder,
    _In_ PCPHYSX_LIGHTED_GEOMETRY_VTABLE LightedGeometryVTable,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PSIZE_T GeometryIndex
    )
{
    SIZE_T Index;
    PPHYSX_AREA_LIGHT_GEOMETRY_DATA GeometryData;
    ISTATUS Status;

    if (Builder == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (LightedGeometryVTable == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (DataSizeInBytes != 0)
    {
        if (Data == NULL)
        {
            return ISTATUS_INVALID_ARGUMENT_COMBINATION_00;
        }

        if (DataAlignment == 0 ||
            DataAlignment & DataAlignment - 1)
        {
            return ISTATUS_INVALID_ARGUMENT_COMBINATION_01;
        }

        if (DataSizeInBytes % DataAlignment != 0)
        {
            return ISTATUS_INVALID_ARGUMENT_COMBINATION_02;
        }
    }

    if (GeometryIndex == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    GeometryData = (PPHYSX_AREA_LIGHT_GEOMETRY_DATA) malloc(sizeof(PHYSX_AREA_LIGHT_GEOMETRY_DATA));

    if (GeometryData == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    GeometryData->Data = IrisAlignedAlloc(DataSizeInBytes, DataAlignment);
    
    if (GeometryData->Data == NULL)
    {
        free(GeometryData);
        return ISTATUS_ALLOCATION_FAILED;
    }
    
    Status = UInt32ToIndexMapInitialize(&GeometryData->AttachedLights);
    
    if (Status != ISTATUS_SUCCESS)
    {
        ASSERT(Status == ISTATUS_ALLOCATION_FAILED);
        IrisAlignedFree(GeometryData->Data);
        free(GeometryData);
        return Status;
    }
    
    Index = PointerListGetSize(&Builder->GeometryDataList);

    Status = PointerListAddPointer(&Builder->GeometryDataList,
                                   GeometryData);

    if (Status != ISTATUS_SUCCESS)
    {
        ASSERT(Status == ISTATUS_ALLOCATION_FAILED);
        UInt32ToIndexMapDestroy(&GeometryData->AttachedLights);
        IrisAlignedFree(GeometryData->Data);
        free(GeometryData);
        return Status;
    }

    memcpy(GeometryData->Data, Data, DataSizeInBytes);

    GeometryData->VTable = LightedGeometryVTable;
    GeometryData->DataSizeInBytes = DataSizeInBytes;
    GeometryData->DataAlignment = DataAlignment;
    *GeometryIndex = Index;
    
    Builder->CallDestructorsOnClear = TRUE;

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PhysxAreaLightBuilderAddLight(
    _Inout_ PPHYSX_AREA_LIGHT_BUILDER Builder,
    _In_ PCPHYSX_AREA_LIGHT_VTABLE AreaLightVTable,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PSIZE_T LightIndex
    )
{
    SIZE_T Index;
    PPHYSX_AREA_LIGHT_LIGHT_DATA LightData;
    ISTATUS Status;

    if (Builder == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (AreaLightVTable == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (DataSizeInBytes != 0)
    {
        if (Data == NULL)
        {
            return ISTATUS_INVALID_ARGUMENT_COMBINATION_00;
        }

        if (DataAlignment == 0 ||
            DataAlignment & DataAlignment - 1)
        {
            return ISTATUS_INVALID_ARGUMENT_COMBINATION_01;
        }

        if (DataSizeInBytes % DataAlignment != 0)
        {
            return ISTATUS_INVALID_ARGUMENT_COMBINATION_02;
        }
    }

    if (LightIndex == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    LightData = (PPHYSX_AREA_LIGHT_LIGHT_DATA) malloc(sizeof(PPHYSX_AREA_LIGHT_LIGHT_DATA));

    if (LightData == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    LightData->Data = IrisAlignedAlloc(DataSizeInBytes, DataAlignment);

    if (LightData->Data == NULL)
    {
        free(LightData);
        return ISTATUS_ALLOCATION_FAILED;
    }

    Index = PointerListGetSize(&Builder->LightDataList);

    Status = PointerListAddPointer(&Builder->LightDataList,
                                   LightData);

    if (Status != ISTATUS_SUCCESS)
    {
        ASSERT(Status == ISTATUS_ALLOCATION_FAILED);
        IrisAlignedFree(LightData->Data);
        free(LightData);
        return Status;
    }

    memcpy(LightData->Data, Data, DataSizeInBytes);

    LightData->VTable = AreaLightVTable;
    LightData->DataSizeInBytes = DataSizeInBytes;
    LightData->DataAlignment = DataAlignment;
    LightData->AttachCount = 0;
    *LightIndex = Index;

    Builder->CallDestructorsOnClear = TRUE;
    
    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PhysxAreaLightBuilderAttachLightToGeometry(
    _Inout_ PPHYSX_AREA_LIGHT_BUILDER Builder,
    _In_ SIZE_T GeometryIndex,
    _In_ UINT32 FaceIndex,
    _In_ SIZE_T LightIndex
    )
{
    FLOAT Area;
    PVOID Data;
    PPHYSX_AREA_LIGHT_GEOMETRY_DATA GeometryData;
    PPHYSX_AREA_LIGHT_LIGHT_DATA LightData;
    SIZE_T NumberOfGeometry;
    SIZE_T NumberOfLights;
    ISTATUS Status;
    BOOL UpdateSucceeded;

    if (Builder == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    NumberOfGeometry = PointerListGetSize(&Builder->GeometryDataList);

    if (NumberOfGeometry <= GeometryIndex)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    NumberOfLights = PointerListGetSize(&Builder->LightDataList);

    if (NumberOfLights <= LightIndex)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    Data = PointerListRetrieveAtIndex(&Builder->GeometryDataList,
                                      GeometryIndex);

    GeometryData = (PPHYSX_AREA_LIGHT_GEOMETRY_DATA) Data;

    Status = GeometryData->VTable->ComputeSurfaceAreaRoutine(GeometryData->Data,
                                                             FaceIndex,
                                                             &Area);
    if (Status != ISTATUS_SUCCESS ||
        IsLessThanOrEqualToZeroFloat(Area) != FALSE ||
        IsFiniteFloat(Area) != FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_COMBINATION_00;
    }

    Status = UInt32ToIndexMapAddMapping(&GeometryData->AttachedLights,
                                        FaceIndex,
                                        LightIndex,
                                        FALSE,
                                        &UpdateSucceeded);

    if (Status != ISTATUS_SUCCESS)
    {
        ASSERT(Status == ISTATUS_ALLOCATION_FAILED);
        return Status;
    }

    if (UpdateSucceeded == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_COMBINATION_01;
    }

    Data = PointerListRetrieveAtIndex(&Builder->LightDataList,
                                      LightIndex);

    LightData = (PPHYSX_AREA_LIGHT_LIGHT_DATA) Data;
    
    LightData->AttachCount += 1;

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PhysxAreaLightBuilderBuildLightsAndGeometry(
    _Inout_ PPHYSX_AREA_LIGHT_BUILDER Builder,
    _Outptr_result_buffer_(*NumberOfGeometry) PPBR_GEOMETRY **Geometry,
    _Out_ PSIZE_T NumberOfGeometry,
    _Outptr_result_buffer_(*NumberOfLights) PPBR_LIGHT **Lights,
    _Out_ PSIZE_T NumberOfLights
    )
{
    SIZE_T AttachedLightIndex;
    FLOAT Area;
    SIZE_T LightIndex;
    PVOID Allocation;
    PVOID Data;
    UINT32 Face;
    SIZE_T Index;
    PPHYSX_AREA_LIGHT_GEOMETRY_DATA GeometryData;
    PPHYSX_AREA_LIGHT_LIGHT_DATA LightData;
    SIZE_T NumberOfAttachedLights;
    SIZE_T LocalNumberOfGeometry;
    SIZE_T LocalNumberOfLights;
    PPHYSX_AREA_LIGHT_REFERENCE_COUNT ReferenceCount;
    PPBR_GEOMETRY *LocalGeometry;
    PPBR_LIGHT *LocalLights;
    ISTATUS Status;
    PCPHYSX_LIGHTED_GEOMETRY LightedGeometry;
    PPHYSX_LIGHTED_GEOMETRY_ADAPTER LightedGeometryAdapter;
    PPHYSX_AREA_LIGHT_ADAPTER *LocalAreaLightAdapters;

    if (Builder == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (Geometry == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (NumberOfGeometry == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (Lights == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (NumberOfLights == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    //
    // At somepoint this code should be smart enough to
    // allow disjoint lights/geometry to not share a 
    // reference count. For now, everything will just
    // be managed together for simplicity.
    //

    LocalNumberOfGeometry = PointerListGetSize(&Builder->GeometryDataList);
    LocalNumberOfLights = PointerListGetSize(&Builder->LightDataList);

    Status = AreaLightReferenceCountAllocate(LocalNumberOfGeometry,
                                             LocalNumberOfLights,
                                             &ReferenceCount);

    if (Status != ISTATUS_SUCCESS)
    {
        ASSERT(Status == ISTATUS_ALLOCATION_FAILED);
        return Status;
    }

    //
    // Allocate Result Arrays
    //
    
    Allocation = malloc(sizeof(PPBR_GEOMETRY) * LocalNumberOfGeometry);
    
    if (Allocation == NULL)
    {
        AreaLightReferenceCountFree(ReferenceCount);
        return ISTATUS_ALLOCATION_FAILED;
    }
    
    LocalGeometry = (PPBR_GEOMETRY*) Allocation;
    
    Allocation = malloc(sizeof(PPBR_LIGHT) * LocalNumberOfLights);
    
    if (Allocation == NULL)
    {
        AreaLightReferenceCountFree(ReferenceCount);
        free(LocalGeometry);
        return ISTATUS_ALLOCATION_FAILED;
    }
    
    LocalLights = (PPBR_LIGHT*) Allocation;

    //
    // Allocate Local Light Adapter Array
    //

    Allocation = malloc(sizeof(PPHYSX_AREA_LIGHT_ADAPTER) * LocalNumberOfLights);
    
    if (Allocation == NULL)
    {
        AreaLightReferenceCountFree(ReferenceCount);
        free(LocalGeometry);
        free(LocalLights);
        return ISTATUS_ALLOCATION_FAILED;
    }

    LocalAreaLightAdapters = (PPHYSX_AREA_LIGHT_ADAPTER*) Allocation;
    
    //
    // Create Lights
    //

    for (Index = 0; Index < LocalNumberOfLights; Index++)
    {
        Data = PointerListRetrieveAtIndex(&Builder->LightDataList,
                                          Index);

        LightData = (PPHYSX_AREA_LIGHT_LIGHT_DATA) Data;

        Status = PhysxAreaLightAdapterAllocate(ReferenceCount,
                                               LightData->AttachCount,
                                               LightData->VTable,
                                               LightData->Data,
                                               LightData->DataSizeInBytes,
                                               LightData->DataAlignment,
                                               &LocalAreaLightAdapters[Index],
                                               &LocalLights[Index]);

        if (Status != ISTATUS_SUCCESS)
        {
            ASSERT(Status == ISTATUS_ALLOCATION_FAILED);
            AreaLightReferenceCountFree(ReferenceCount);
            free(LocalGeometry);
            free(LocalLights);
            free(LocalAreaLightAdapters);
            return Status;
        }
        
        AreaLightReferenceCountAddLight(ReferenceCount,
                                        LocalLights[Index]);
    }

    //
    // Create Geometry
    //

    for (Index = 0; Index < LocalNumberOfGeometry; Index++)
    {
        Data = PointerListRetrieveAtIndex(&Builder->GeometryDataList,
                                          Index);

        GeometryData = (PPHYSX_AREA_LIGHT_GEOMETRY_DATA) Data;

        NumberOfAttachedLights = UInt32ToIndexMapGetSize(&GeometryData->AttachedLights);

        Status = PhysxLightedGeometryAdapterAllocate(ReferenceCount,
                                                     NumberOfAttachedLights,
                                                     GeometryData->VTable,
                                                     GeometryData->Data,
                                                     GeometryData->DataSizeInBytes,
                                                     GeometryData->DataAlignment,
                                                     &LightedGeometry,
                                                     &LightedGeometryAdapter,
                                                     &LocalGeometry[Index]);

        if (Status != ISTATUS_SUCCESS)
        {
            ASSERT(Status == ISTATUS_ALLOCATION_FAILED);
            AreaLightReferenceCountFree(ReferenceCount);
            free(LocalGeometry);
            free(LocalLights);
            free(LocalAreaLightAdapters);
            return Status;
        }
        
        AreaLightReferenceCountAddGeometry(ReferenceCount,
                                           LocalGeometry[Index]);
        
        //
        // Attach Lights And Geometry
        //

        for (AttachedLightIndex = 0; 
             AttachedLightIndex < NumberOfAttachedLights; 
             AttachedLightIndex++)
        {
            Face = UInt32ToIndexMapGetKeyByIndex(&GeometryData->AttachedLights,
                                                 AttachedLightIndex);

            LightIndex = UInt32ToIndexMapGetValueByIndex(&GeometryData->AttachedLights,
                                                         AttachedLightIndex);

            Status = PhysxLightedGeometryComputeSurfaceArea(LightedGeometry,
                                                            Face,
                                                            &Area);

            if (Status != ISTATUS_SUCCESS)
            {
                ASSERT(Status == ISTATUS_ALLOCATION_FAILED);
                AreaLightReferenceCountFree(ReferenceCount);
                free(LocalGeometry);
                free(LocalLights);
                free(LocalAreaLightAdapters);
                return Status;
            }

            PhysxAreaLightAdapterAttachGeometry(LocalAreaLightAdapters[LightIndex],
                                                LightedGeometry,
                                                Face,
                                                Area);

            PhysxLightedGeometryAdapterAttachLight(LightedGeometryAdapter,
                                                   LocalLights[LightIndex],
                                                   Face);
        }

        PhysxLightedGeometryAdapterFinalize(LightedGeometryAdapter);
    }

    free(LocalAreaLightAdapters);

    //
    // TODO: Move NULL lights to end of array
    //

    for (Index = 0; Index < LocalNumberOfLights; Index++)
    {
        PhysxAreaLightAdapterFinalize(LocalAreaLightAdapters[Index]);
    }
    
    //
    // Reset the builder and copy out results
    //
    
    *Lights = LocalLights;
    *NumberOfLights = LocalNumberOfLights;
    *Geometry = LocalGeometry;
    *NumberOfGeometry = LocalGeometry;
    
    Builder->CallDestructorsOnClear = FALSE;
    PhysxAreaLightBuilderClear(Builder);
    
    return ISTATUS_SUCCESS;
}

VOID
PhysxAreaLightBuilderFree(
    _In_opt_ _Post_invalid_ PPHYSX_AREA_LIGHT_BUILDER Builder
    )
{
    if (Builder == NULL)
    {
        return;
    }

    PhysxAreaLightBuilderClear(Builder);
    PointerListDestroy(&Builder->GeometryDataList);
    PointerListDestroy(&Builder->LightDataList);
    free(Builder);
}
