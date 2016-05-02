/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    pinhole.c

Abstract:

    This file contains the definitions for pinhole render.

--*/

#include <irisphysxtoolkitp.h>
#include <omp.h>

//
// Types
//

typedef struct _PINHOLE_INTEGRATE_CONTEXT {
    PPBR_TONE_MAPPING_ROUTINE ToneMappingRoutine;
    PPBR_RAYTRACER_PROCESS_HIT_ROUTINE ProcessHitRoutine;
    PVOID ProcessHitContext;
    PCVOID ToneMappingContext;
    COLOR3 ColorSum;
} PINHOLE_INTEGRATE_CONTEXT, *PPINHOLE_INTEGRATE_CONTEXT;

//
// Static Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS 
PinholeIntegrateRoutine(
    _Inout_opt_ PVOID Context, 
    _Inout_ PPBR_RAYTRACER PBRRayTracer,
    _In_ RAY Ray
    )
{
    PPINHOLE_INTEGRATE_CONTEXT IntegrateContext;
    COLOR3 ResultColor;
    PCSPECTRUM ResultSpectrum;
    ISTATUS Status;

    ASSERT(Context != NULL);
    ASSERT(PBRRayTracer != NULL);
    ASSERT(RayValidate(Ray) != FALSE);

    IntegrateContext = (PPINHOLE_INTEGRATE_CONTEXT) Context;

    Status = PBRRayTracerTraceSceneProcessClosestHit(PBRRayTracer,
                                                     Ray,
                                                     IntegrateContext->ProcessHitRoutine,
                                                     IntegrateContext->ProcessHitContext,
                                                     &ResultSpectrum);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    Status = IntegrateContext->ToneMappingRoutine(IntegrateContext->ToneMappingContext,
                                                  ResultSpectrum,
                                                  &ResultColor);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    IntegrateContext->ColorSum = Color3Add(IntegrateContext->ColorSum, 
                                           ResultColor);

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
StaticPinholeCameraRender(
    _In_ POINT3 PinholeLocation,
    _In_ POINT3 ImagePlaneStartingLocation,
    _In_ VECTOR3 PixelXDimensions,
    _In_ VECTOR3 PixelYDimensions,
    _In_ SIZE_T PixelColumn,
    _In_ SIZE_T PixelRow,
    _In_ SIZE_T AdditionalXSamplesPerPixel,
    _In_ SIZE_T AdditionalYSamplesPerPixel,
    _In_ FLOAT Epsilon,
    _In_ BOOL Jitter,
    _In_ PRANDOM_REFERENCE Rng,
    _In_ PPBR_INTEGRATOR Integrator,
    _In_ PPBR_INTEGRATOR_TEST_GEOMETRY_ROUTINE TestGeometryRoutine,
    _In_opt_ PCVOID TestGeometryRoutineContext,
    _In_reads_(NumberOfLights) PCPBR_LIGHT *Lights,
    _In_ SIZE_T NumberOfLights,
    _Inout_ PPINHOLE_INTEGRATE_CONTEXT IntegrateContext,
    _Inout_ PFRAMEBUFFER Framebuffer
    )
{
    VECTOR3 Direction;
    SIZE_T FramebufferColumnSubpixelIndex;
    SIZE_T FramebufferColumns;
    SIZE_T FramebufferRowSubpixelIndex;
    SIZE_T FramebufferRows;
    POINT3 Origin;
    COLOR3 PixelColor;
    FLOAT RandomNumber0;
    FLOAT RandomNumber1;
    POINT3 RayOrigin;
    POINT3 RowStart;
    ISTATUS Status;
    POINT3 SubpixelOrigin;
    POINT3 SubpixelRowStart;
    FLOAT SubpixelWeight;
    VECTOR3 SubpixelXDimensions;
    VECTOR3 SubpixelYDimensions;
    RAY WorldRay;

    ASSERT(PointValidate(PinholeLocation) != FALSE);
    ASSERT(PointValidate(ImagePlaneStartingLocation) != FALSE);
    ASSERT(VectorValidate(PixelXDimensions) != FALSE);
    ASSERT(VectorValidate(PixelYDimensions) != FALSE);
    ASSERT(IsFiniteFloat(Epsilon) != FALSE);
    ASSERT(Rng != NULL);
    ASSERT(IntegrateContext != NULL);
    ASSERT(Framebuffer != NULL);

    FramebufferGetDimensions(Framebuffer, 
                             &FramebufferRows,
                             &FramebufferColumns);

    ASSERT(PixelColumn < FramebufferColumns);
    ASSERT(PixelRow < FramebufferRows);

    if (AdditionalXSamplesPerPixel != 0)
    {
        SubpixelXDimensions = VectorScale(PixelXDimensions,
                                          (FLOAT) 1.0 / (FLOAT) AdditionalXSamplesPerPixel);
    }
    else
    {
        SubpixelXDimensions = PixelXDimensions;
    }

    if (AdditionalYSamplesPerPixel != 0)
    {
        SubpixelYDimensions = VectorScale(PixelYDimensions,
                                          (FLOAT) 1.0 / (FLOAT) AdditionalYSamplesPerPixel);
    }
    else
    {
        SubpixelYDimensions = PixelYDimensions;
    }

    SubpixelWeight = (FLOAT) 1.0 /
                      (((FLOAT) 1.0 + (FLOAT)AdditionalXSamplesPerPixel) *
                      ((FLOAT)AdditionalYSamplesPerPixel + (FLOAT) 1.0));

    IntegrateContext->ColorSum = Color3InitializeBlack();

    RowStart = ImagePlaneStartingLocation;

    Origin = RowStart;

    SubpixelRowStart = Origin;

    FramebufferRowSubpixelIndex = 0;

    do
    {
        SubpixelOrigin = SubpixelRowStart;

        FramebufferColumnSubpixelIndex = 0;

        do
        {
            RayOrigin = SubpixelOrigin;

            if (Jitter != FALSE)
            {
                Status = RandomReferenceGenerateFloat(Rng,
                                                      (FLOAT) 0.0,
                                                      (FLOAT) 1.0,
                                                      &RandomNumber0);

                if (Status != ISTATUS_SUCCESS)
                {
                    return Status;
                }

                Status = RandomReferenceGenerateFloat(Rng,
                                                      (FLOAT) 0.0,
                                                      (FLOAT) 1.0,
                                                      &RandomNumber1);

                if (Status != ISTATUS_SUCCESS)
                {
                    return Status;
                }

                RayOrigin = PointVectorAddScaled(RayOrigin,
                                                 SubpixelXDimensions,
                                                 RandomNumber0);

                RayOrigin = PointVectorAddScaled(RayOrigin,
                                                 SubpixelYDimensions,
                                                 RandomNumber1);
            }
            else
            {
                RayOrigin = PointVectorAddScaled(RayOrigin,
                                                 SubpixelXDimensions,
                                                 (FLOAT) 0.5);

                RayOrigin = PointVectorAddScaled(RayOrigin,
                                                 SubpixelYDimensions,
                                                 (FLOAT) 0.5);
            }

            Direction = PointSubtract(RayOrigin, PinholeLocation);

            Direction = VectorNormalize(Direction, NULL, NULL);

            WorldRay = RayCreate(RayOrigin, Direction);

            Status = PBRIntegratorIntegrate(Integrator,
                                            TestGeometryRoutine,
                                            TestGeometryRoutineContext,
                                            PinholeIntegrateRoutine,
                                            IntegrateContext,
                                            Lights,
                                            NumberOfLights,
                                            Epsilon,
                                            WorldRay,
                                            Rng);

            if (Status != ISTATUS_SUCCESS)
            {
                return Status;
            }

            SubpixelOrigin = PointVectorAdd(SubpixelOrigin, SubpixelXDimensions);

        } while (++FramebufferColumnSubpixelIndex < AdditionalXSamplesPerPixel);

        SubpixelRowStart = PointVectorAdd(SubpixelRowStart, SubpixelYDimensions);

    } while (++FramebufferRowSubpixelIndex < AdditionalYSamplesPerPixel);

    PixelColor = Color3ScaleByScalar(IntegrateContext->ColorSum,
                                     SubpixelWeight);

    FramebufferSetPixel(Framebuffer,
                        PixelColor,
                        PixelRow,
                        PixelColumn);

    return ISTATUS_SUCCESS;
}

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PinholeRender(
    _In_ POINT3 PinholeLocation,
    _In_ FLOAT ImagePlaneDistance,
    _In_ FLOAT ImagePlaneHeight,
    _In_ FLOAT ImagePlaneWidth,
    _In_ VECTOR3 CameraDirection,
    _In_ VECTOR3 Up,
    _In_ SIZE_T AdditionalXSamplesPerPixel,
    _In_ SIZE_T AdditionalYSamplesPerPixel,
    _In_ FLOAT Epsilon,
    _In_ SIZE_T MaxDepth,
    _In_ BOOL Jitter,
    _In_ BOOL Parallelize,
    _In_ PPBR_INTEGRATOR_TEST_GEOMETRY_ROUTINE TestGeometryRoutine,
    _In_opt_ PCVOID TestGeometryRoutineContext,
    _In_reads_(NumberOfLights) PCPBR_LIGHT *Lights,
    _In_ SIZE_T NumberOfLights,
    _In_ PPBR_TOOLKIT_CREATE_CAMERA_STATE_ROUTINE CreateStateRoutine,
    _In_opt_ PPBR_TOOLKIT_FREE_CAMERA_STATE_ROUTINE FreeCameraStateRoutine,
    _Inout_opt_ PVOID CreateStateContext,
    _Inout_ PFRAMEBUFFER Framebuffer
    )
{
    SIZE_T FramebufferColumns;
    SIZE_T FramebufferRows;
    POINT3 ImagePlaneCorner;
    VECTOR3 ImagePlaneHeightVector;
    VECTOR3 ImagePlaneWidthVector;
    PINHOLE_INTEGRATE_CONTEXT IntegrateContext;
    PPBR_INTEGRATOR *Integrators;
    VECTOR3 NormalizedCameraDirection;
    VECTOR3 NormalizedUpVector;
    SIZE_T NumberOfPixels;
    SIZE_T NumberOfThreads;
    SIZE_T PixelIndex;
    PVOID *ProcessHitContexts;
    PPBR_RAYTRACER_PROCESS_HIT_ROUTINE *ProcessHitRoutines;
    PRANDOM *Rngs;
    ISTATUS Status;
    SIZE_T ThreadIndex;
    PVOID *ToneMappingContexts;
    PPBR_TONE_MAPPING_ROUTINE *ToneMappingRoutines;

    if (PointValidate(PinholeLocation) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (IsFiniteFloat(ImagePlaneDistance) == FALSE ||
        IsZeroFloat(ImagePlaneDistance) != FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (IsFiniteFloat(ImagePlaneHeight) == FALSE ||
        IsZeroFloat(ImagePlaneHeight) != FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }
    
    if (IsFiniteFloat(ImagePlaneWidth) == FALSE ||
        IsZeroFloat(ImagePlaneWidth) != FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (VectorValidate(CameraDirection) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }
    
    if (VectorValidate(Up) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    if (IsFiniteFloat(Epsilon) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_08;
    }

    if (TestGeometryRoutine == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_12;
    }

    if (Lights == NULL && NumberOfLights != 0)
    {
        return ISTATUS_INVALID_ARGUMENT_COMBINATION_00;
    }
    
    if (Lights != NULL && NumberOfLights == 0)
    {
        return ISTATUS_INVALID_ARGUMENT_COMBINATION_01;
    }

    if (CreateStateRoutine == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_16;
    }

    if (Framebuffer == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_19;
    }

    FramebufferGetDimensions(Framebuffer, 
                             &FramebufferRows,
                             &FramebufferColumns);

    NormalizedCameraDirection = VectorNormalize(CameraDirection, NULL, NULL);

    NormalizedUpVector = VectorNormalize(Up, NULL, NULL);

    ImagePlaneWidthVector = VectorCrossProduct(NormalizedCameraDirection, Up);

    ImagePlaneWidthVector = VectorNormalize(ImagePlaneWidthVector, NULL, NULL);

    ImagePlaneHeightVector = VectorCrossProduct(CameraDirection,
                                                ImagePlaneWidthVector);

    ImagePlaneHeightVector = VectorNormalize(ImagePlaneHeightVector, NULL, NULL);

    ImagePlaneWidthVector = VectorScale(ImagePlaneWidthVector,
                                        ImagePlaneWidth);

    ImagePlaneHeightVector = VectorScale(ImagePlaneHeightVector,
                                         ImagePlaneHeight);

    ImagePlaneCorner = PointVectorAddScaled(PinholeLocation,
                                            NormalizedCameraDirection,
                                            ImagePlaneDistance);

    ImagePlaneCorner = PointVectorSubtractScaled(ImagePlaneCorner,
                                                 ImagePlaneWidthVector,
                                                 (FLOAT) 0.5);

    ImagePlaneCorner = PointVectorSubtractScaled(ImagePlaneCorner,
                                                 ImagePlaneHeightVector,
                                                 (FLOAT) 0.5);

    ImagePlaneWidthVector = VectorScale(ImagePlaneWidthVector,
                                        (FLOAT) 1.0 / (FLOAT) FramebufferColumns);

    ImagePlaneHeightVector = VectorScale(ImagePlaneHeightVector,
                                         (FLOAT) 1.0 / (FLOAT) FramebufferRows);

    if (Parallelize != FALSE)
    {
        NumberOfThreads = (SIZE_T) omp_get_num_procs();
        omp_set_num_threads((int) NumberOfThreads);
    }
    else
    {
        omp_set_num_threads(1);
        NumberOfThreads = 1;
    }

    Rngs = calloc(NumberOfThreads, sizeof(PRANDOM));

    if (Rngs == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    ProcessHitRoutines = calloc(NumberOfThreads, sizeof(PPBR_RAYTRACER_PROCESS_HIT_ROUTINE));

    if (ProcessHitRoutines == NULL)
    {
        free(Rngs);
        return ISTATUS_ALLOCATION_FAILED;
    }

    ProcessHitContexts = calloc(NumberOfThreads, sizeof(PVOID));

    if (ProcessHitContexts == NULL)
    {
        free(ProcessHitRoutines);
        free(Rngs);
        return ISTATUS_ALLOCATION_FAILED;
    }

    ToneMappingRoutines = calloc(NumberOfThreads, sizeof(PPBR_TONE_MAPPING_ROUTINE));

    if (ToneMappingRoutines == NULL)
    {
        free(ProcessHitContexts);
        free(ProcessHitRoutines);
        free(Rngs);
        return ISTATUS_ALLOCATION_FAILED;
    }

    ToneMappingContexts = calloc(NumberOfThreads, sizeof(PVOID));

    if (ToneMappingContexts == NULL)
    {
        free(ToneMappingRoutines);
        free(ProcessHitContexts);
        free(ProcessHitRoutines);
        free(Rngs);
        return ISTATUS_ALLOCATION_FAILED;
    }

    Integrators = calloc(NumberOfThreads, sizeof(PPBR_INTEGRATOR));

    if (Integrators == NULL)
    {
        free(ToneMappingContexts);
        free(ToneMappingRoutines);
        free(ProcessHitContexts);
        free(ProcessHitRoutines);
        free(Rngs);
        return ISTATUS_ALLOCATION_FAILED;
    }

    Status = CreateStateRoutine(CreateStateContext,
                                Rngs,
                                ProcessHitRoutines,
                                ProcessHitContexts,
                                ToneMappingRoutines,
                                ToneMappingContexts,
                                NumberOfThreads);

    if (Status == ISTATUS_SUCCESS)
    {
        NumberOfPixels = FramebufferRows * FramebufferColumns;

        #pragma omp parallel default(shared) private(IntegrateContext, ThreadIndex) reduction(max, Status)
        {
            ThreadIndex = (SIZE_T) omp_get_thread_num();

            IntegrateContext.ToneMappingRoutine = ToneMappingRoutines[ThreadIndex];
            IntegrateContext.ProcessHitRoutine = ProcessHitRoutines[ThreadIndex];
            IntegrateContext.ProcessHitContext = ProcessHitContexts[ThreadIndex];
            IntegrateContext.ToneMappingContext = ToneMappingContexts[ThreadIndex];

            #pragma omp for schedule(dynamic, 16)
            for (PixelIndex = 0; PixelIndex < NumberOfPixels && Status == ISTATUS_SUCCESS; PixelIndex++)
            {
                Status = StaticPinholeCameraRender(PinholeLocation,
                                                   ImagePlaneCorner,
                                                   ImagePlaneWidthVector,
                                                   ImagePlaneHeightVector,
                                                   PixelIndex % FramebufferColumns,
                                                   PixelIndex / FramebufferColumns,
                                                   AdditionalXSamplesPerPixel,
                                                   AdditionalYSamplesPerPixel,
                                                   Epsilon,
                                                   Jitter,
                                                   RandomGetRandomReference(Rngs[ThreadIndex]),
                                                   Integrators[ThreadIndex],
                                                   TestGeometryRoutine,
                                                   TestGeometryRoutineContext,
                                                   Lights,
                                                   NumberOfLights,
                                                   &IntegrateContext,
                                                   Framebuffer);
            }
        }
    }

    FreeCameraStateRoutine(CreateStateContext, ProcessHitContexts, ToneMappingContexts, NumberOfThreads);

    for (ThreadIndex = 0;
         ThreadIndex < NumberOfThreads;
         ThreadIndex++)
    {
        RandomRelease(Rngs[ThreadIndex]);
        PBRIntegratorFree(Integrators[ThreadIndex]);
    }

    free(Integrators);
    free(ToneMappingContexts);
    free(ToneMappingRoutines);
    free(ProcessHitContexts);
    free(ProcessHitRoutines);
    free(Rngs);

    return Status;
}