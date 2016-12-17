/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    raygenerator.c

Abstract:

    This file contains the definitions for the RAY_GENERATOR type.

--*/

#include <iriscamerap.h>

//
// Public Functions
//

_Success_(return == ISTATUS_SUCCESS)
ISTATUS
RayGeneratorGenerateRay(
    _In_ PCRAY_GENERATOR RayGenerator,
    _In_ FLOAT PixelU,
    _In_ FLOAT PixelV,
    _In_ FLOAT LensU,
    _In_ FLOAT LensV,
    _Out_ PRAY WorldRay
    )
{
    ISTATUS Status;

    if (RayGenerator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (WorldRay == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    Status = CameraComputeRay(RayGenerator->Camera,
                              RayGenerator->Row,
                              RayGenerator->NumberOfRows,
                              RayGenerator->Column,
                              RayGenerator->NumberOfColumns,
                              PixelU,
                              PixelV,
                              LensU,
                              LensV,
                              WorldRay);

    return Status;
}