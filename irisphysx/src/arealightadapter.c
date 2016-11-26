/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    arealightadapter.c

Abstract:

    This file contains the definitions for the PHYSX_AREA_LIGHT_ADAPTER type.

--*/

#include <irisphysxp.h>

//
// Macros
//

#define PHYSX_AREA_LIGHT_MAX_ATTACHED_LIGHTS \
    (SIZE_MAX / sizeof(PHYSX_AREA_LIGHT_ADAPTER))

//
// Types
//

typedef struct _PHYSX_ATTACHED_GEOMETRY {
    PCPHYSX_LIGHTED_GEOMETRY LightedGeometry;
    UINT32 Face;
    FLOAT StartArea;
    FLOAT EndArea;
} PHYSX_ATTACHED_GEOMETRY, *PPHYSX_ATTACHED_GEOMETRY;

typedef CONST PHYSX_ATTACHED_GEOMETRY *PCPHYSX_ATTACHED_GEOMETRY;

struct _PHYSX_AREA_LIGHT_ADAPTER {
    PPHYSX_AREA_LIGHT AreaLight;
    _Field_size_(NumberOfGeometry) PPHYSX_ATTACHED_GEOMETRY Geometry;
    SIZE_T NumberOfGeometry;
    PCPHYSX_LIGHT SelfReference;
    FLOAT SamplePdf;
};

//
// Static Functions
//

STATIC
int
PhysxAreaLightAdapterCompareGeometry(
    _In_ const void * Ptr0,
    _In_ const void * Ptr1
    )
{
    PCPHYSX_ATTACHED_GEOMETRY AttachedGeometry0;
    PCPHYSX_ATTACHED_GEOMETRY AttachedGeometry1;

    AttachedGeometry0 = (PCPHYSX_ATTACHED_GEOMETRY) Ptr0;
    AttachedGeometry1 = (PCPHYSX_ATTACHED_GEOMETRY) Ptr1;

    if (AttachedGeometry0->EndArea < AttachedGeometry1->StartArea)
    {
        return -1;
    }

    if (AttachedGeometry0->StartArea > AttachedGeometry1->EndArea)
    {
        return 1;
    }

    return 0;
}

//
// Static Light Function
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
PhysxAreaLightAdapterComputeEmissive(
    _In_ PCVOID Context,
    _In_ RAY ToLight,
    _Inout_ PPHYSX_VISIBILITY_TESTER VisibilityTester,
    _Inout_ PSPECTRUM_COMPOSITOR_REFERENCE Compositor,
    _Out_ PCSPECTRUM *Spectrum
    )
{
    PCPHYSX_AREA_LIGHT_ADAPTER AreaLightAdapter;
    FLOAT DistanceToLight;
    POINT3 PointOnLight;
    ISTATUS Status;
    BOOL Visible;

    ASSERT(Context != NULL);
    ASSERT(RayValidate(ToLight) != FALSE);
    ASSERT(VisibilityTester != NULL);
    ASSERT(Spectrum != NULL);

    AreaLightAdapter = (PCPHYSX_AREA_LIGHT_ADAPTER) Context;
    
    Status = PhysxVisibilityTesterFindDistanceToLight(VisibilityTester,
                                                      ToLight,
                                                      AreaLightAdapter->SelfReference,
                                                      &Visible,
                                                      &DistanceToLight);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    if (Visible == FALSE)
    {
        *Spectrum = NULL;
        return ISTATUS_SUCCESS;
    }

    PointOnLight = RayEndpoint(ToLight, DistanceToLight);

    Status = PhysxAreaLightComputeEmissive(AreaLightAdapter->AreaLight,
                                           PointOnLight,
                                           Compositor,
                                           Spectrum);

    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
PhysxAreaLightAdapterComputeEmissiveWithPdf(
    _In_ PCVOID Context,
    _In_ RAY ToLight,
    _Inout_ PPHYSX_VISIBILITY_TESTER VisibilityTester,
    _Inout_ PSPECTRUM_COMPOSITOR_REFERENCE Compositor,
    _Out_ PCSPECTRUM *Spectrum,
    _Out_ PFLOAT Pdf
    )
{
    PCPHYSX_AREA_LIGHT_ADAPTER AreaLightAdapter;
    POINT3 ClosestPointOnLight;
    FLOAT LocalPdf;
    ISTATUS Status;

    ASSERT(Context != NULL);
    ASSERT(RayValidate(ToLight) != FALSE);
    ASSERT(VisibilityTester != NULL);
    ASSERT(Spectrum != NULL);
    ASSERT(Pdf != NULL);

    AreaLightAdapter = (PCPHYSX_AREA_LIGHT_ADAPTER) Context;
    
    //
    // Verify Light Visibility and Compute PDF
    //

    Status = PhysxVisibilityTesterComputePdf(VisibilityTester,
                                             ToLight,
                                             AreaLightAdapter->SelfReference,
                                             AreaLightAdapter->SamplePdf,
                                             &ClosestPointOnLight,
                                             &LocalPdf);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    if (IsZeroFloat(LocalPdf))
    {
        *Spectrum = NULL;
        *Pdf = (FLOAT) 0.0f;
        return ISTATUS_SUCCESS;
    }

    //
    // Compute Lighting
    //

    Status = PhysxAreaLightComputeEmissive(AreaLightAdapter->AreaLight,
                                           ClosestPointOnLight,
                                           Compositor,
                                           Spectrum);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    if (*Spectrum == NULL)
    {
        *Pdf = (FLOAT) 0.0f;
    }
    else
    {
        *Pdf = LocalPdf;
    }

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
PhysxAreaLightAdapterSample(
    _In_ PCVOID Context,
    _In_ POINT3 HitPoint,
    _Inout_ PPHYSX_VISIBILITY_TESTER VisibilityTester,
    _Inout_ PRANDOM_REFERENCE Rng,
    _Inout_ PSPECTRUM_COMPOSITOR_REFERENCE Compositor,
    _Out_ PCSPECTRUM *Spectrum,
    _Out_ PVECTOR3 ToLight,
    _Out_ PFLOAT Pdf
    )
{
    PCPHYSX_AREA_LIGHT_ADAPTER AreaLightAdapter;
    VECTOR3 DirectionToLight;
    PHYSX_ATTACHED_GEOMETRY Key;
    VECTOR3 NormalizedDirectionToLight;
    RAY RayToLight;
    PCVOID Result;
    PCPHYSX_ATTACHED_GEOMETRY SampledGeometry;
    POINT3 SampledLocation;
    ISTATUS Status;
    FLOAT TotalArea;

    ASSERT(Context != NULL);
    ASSERT(PointValidate(HitPoint) != FALSE);
    ASSERT(VisibilityTester != NULL);
    ASSERT(Rng != NULL);
    ASSERT(Compositor != NULL);
    ASSERT(Spectrum != NULL);
    ASSERT(ToLight != NULL);
    ASSERT(Pdf != NULL);

    AreaLightAdapter = (PCPHYSX_AREA_LIGHT_ADAPTER) Context;

    //
    // Sample Surface
    //
    
    TotalArea = AreaLightAdapter->Geometry[AreaLightAdapter->NumberOfGeometry - 1].EndArea;

    Status = RandomReferenceGenerateFloat(Rng,
                                          (FLOAT) 0.0f,
                                          TotalArea,
                                          &Key.StartArea);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    Key.LightedGeometry = NULL;
    Key.Face = 0;
    Key.EndArea = Key.StartArea;

    Result = bsearch(&Key,
                     AreaLightAdapter->Geometry,
                     AreaLightAdapter->NumberOfGeometry,
                     sizeof(PHYSX_ATTACHED_GEOMETRY),
                     PhysxAreaLightAdapterCompareGeometry);

    SampledGeometry = (PCPHYSX_ATTACHED_GEOMETRY) Result;

    Status = PhysxLightedGeometrySampleSurface(SampledGeometry->LightedGeometry,
                                               SampledGeometry->Face,
                                               Rng,
                                               &SampledLocation);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    //
    // Compute Spectrum and PDF
    //
    
    DirectionToLight = PointSubtract(SampledLocation, HitPoint);
    NormalizedDirectionToLight = VectorNormalize(DirectionToLight, NULL, NULL);
    RayToLight = RayCreate(HitPoint, NormalizedDirectionToLight);

    Status = PhysxAreaLightAdapterComputeEmissiveWithPdf(AreaLightAdapter,
                                                         RayToLight,
                                                         VisibilityTester,
                                                         Compositor,
                                                         Spectrum,
                                                         Pdf);

    if (Status == ISTATUS_SUCCESS)
    {
        *ToLight = NormalizedDirectionToLight;
    }                     

    return Status;
}

STATIC
VOID
PhysxAreaLightAdapterFree(
    _In_ _Post_invalid_ PVOID Context
    )
{
    PPHYSX_AREA_LIGHT_ADAPTER AreaLightAdapter;

    ASSERT(Context != NULL);

    AreaLightAdapter = (PPHYSX_AREA_LIGHT_ADAPTER) Context;
    
    PhysxAreaLightFree(AreaLightAdapter->AreaLight);
    free(AreaLightAdapter->Geometry);
}

//
// Public Variables
//

CONST PHYSX_LIGHT_VTABLE AreaLightAdapterVTable = {
    PhysxAreaLightAdapterSample,
    PhysxAreaLightAdapterComputeEmissive,
    PhysxAreaLightAdapterComputeEmissiveWithPdf,
    PhysxAreaLightAdapterFree
};

//
// Public Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PhysxAreaLightAdapterAllocate(
    _In_ PPHYSX_AREA_LIGHT_REFERENCE_COUNT ReferenceCount,
    _In_ SIZE_T NumberOfAttachedGeometry,
    _In_ PCPHYSX_AREA_LIGHT_VTABLE AreaLightVTable,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PPHYSX_AREA_LIGHT_ADAPTER *AreaLightAdapter,
    _Out_ PPHYSX_LIGHT *Light
    )
{
    PPHYSX_LIGHT AllocatedAreaLightAdapter;
    PHYSX_AREA_LIGHT_ADAPTER AreaLightAdapterData;
    PPHYSX_AREA_LIGHT_ADAPTER AllocatedAreaLightAdapterData;
    PVOID DataAllocation;
    ISTATUS Status;

    ASSERT(ReferenceCount != NULL);
    ASSERT(NumberOfAttachedGeometry != 0);
    ASSERT(AreaLightVTable != NULL);
    ASSERT(DataSizeInBytes == 0 || 
           (Data != NULL && DataAlignment != 0 && 
           (DataAlignment & DataAlignment - 1) && 
           DataSizeInBytes % DataAlignment == 0));
    ASSERT(AreaLightAdapter != NULL);
    ASSERT(Light != NULL);

    if (PHYSX_AREA_LIGHT_MAX_ATTACHED_LIGHTS < NumberOfAttachedGeometry)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    //
    // Initialize Adapter
    //

    AreaLightAdapterData.NumberOfGeometry = 0;
    AreaLightAdapterData.AreaLight = NULL;

    AreaLightAdapterData.Geometry = malloc(sizeof(PHYSX_ATTACHED_GEOMETRY) * NumberOfAttachedGeometry);

    if (AreaLightAdapterData.Geometry == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    //
    // Allocate Adapter
    //

    Status = PhysxLightAllocateInternal(&AreaLightAdapterVTable,
                                        ReferenceCount,
                                        &AreaLightAdapterData,
                                        sizeof(PHYSX_AREA_LIGHT_ADAPTER),
                                        _Alignof(PHYSX_AREA_LIGHT_ADAPTER),
                                        &AllocatedAreaLightAdapter);

    if (Status != ISTATUS_SUCCESS)
    {
        ASSERT(Status == ISTATUS_ALLOCATION_FAILED);
        free(AreaLightAdapterData.Geometry);
        return Status;
    }

    DataAllocation = PhysxLightGetData(AllocatedAreaLightAdapter);
    AllocatedAreaLightAdapterData = (PPHYSX_AREA_LIGHT_ADAPTER) DataAllocation;

    //
    // The area light must be allocated last. We should never call the
    // destructor of the area light from this routine as it may have 
    // side affects.
    //

    Status = PhysxAreaLightAllocate(AreaLightVTable,
                                    Data,
                                    DataSizeInBytes,
                                    DataAlignment,
                                    &AllocatedAreaLightAdapterData->AreaLight);

    if (Status != ISTATUS_SUCCESS)
    {
        ASSERT(Status = ISTATUS_ALLOCATION_FAILED);
        PhysxLightFree(AllocatedAreaLightAdapter);
        return Status;
    }

    AllocatedAreaLightAdapterData->SelfReference = AllocatedAreaLightAdapter;

    *AreaLightAdapter = AllocatedAreaLightAdapterData;
    *Light = AllocatedAreaLightAdapter;

    return Status;
}

VOID
PhysxAreaLightAdapterAttachGeometry(
    _Inout_ PPHYSX_AREA_LIGHT_ADAPTER AreaLightAdapter,
    _In_ PCPHYSX_LIGHTED_GEOMETRY LightedGeometry,
    _In_ UINT32 Face,
    _In_ FLOAT SurfaceArea
    )
{
    PPHYSX_ATTACHED_GEOMETRY AttachedGeometry;

    ASSERT(AreaLightAdapter != NULL);
    ASSERT(LightedGeometry != NULL);
    ASSERT(IsGreaterThanOrEqualToZeroFloat(SurfaceArea) != FALSE);

    AttachedGeometry = &AreaLightAdapter->Geometry[AreaLightAdapter->NumberOfGeometry];

    AttachedGeometry->LightedGeometry = LightedGeometry;
    AttachedGeometry->Face = Face;
    AttachedGeometry->StartArea = SurfaceArea;
    AttachedGeometry->EndArea = SurfaceArea;

    AreaLightAdapter->NumberOfGeometry += 1;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PhysxAreaLightAdapterFinalize(
    _Inout_ PPHYSX_AREA_LIGHT_ADAPTER AreaLightAdapter
    )
{
    PPHYSX_ATTACHED_GEOMETRY AttachedGeometryInstance;
    SIZE_T Index;
    FLOAT TotalArea;

    ASSERT(AreaLightAdapter != NULL);

    //
    // TODO: Fails if light area becomes infinite.
    // TODO: Determine best order to add numbers together.
    //

    TotalArea = (FLOAT) 0.0f;

    for (Index = 0; Index < AreaLightAdapter->NumberOfGeometry; Index += 1)
    {
        AttachedGeometryInstance = &AreaLightAdapter->Geometry[Index];

        AttachedGeometryInstance->StartArea = TotalArea;
        AttachedGeometryInstance->EndArea += TotalArea;

        TotalArea = AttachedGeometryInstance->EndArea;
    }

    AreaLightAdapter->SamplePdf = (FLOAT) 1.0f / TotalArea;

    return ISTATUS_SUCCESS;
}
