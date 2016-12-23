/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    iriscamera_raygenerator.h

Abstract:

    This file contains the definitions for the RAY_GENERATOR type.

--*/

#ifndef _RAY_GENERATOR_IRIS_CAMERA_
#define _RAY_GENERATOR_IRIS_CAMERA_

#include <iriscamera.h>

//
// Types
//

typedef struct _RAY_GENERATOR RAY_GENERATOR, *PRAY_GENERATOR;
typedef CONST RAY_GENERATOR *PCRAY_GENERATOR;

//
// Functions
//

_Success_(return == ISTATUS_SUCCESS)
IRISCAMERAAPI
ISTATUS
RayGeneratorGenerateRay(
    _In_ PCRAY_GENERATOR RayGenerator,
    _In_ FLOAT PixelU,
    _In_ FLOAT PixelV,
    _In_ FLOAT LensU,
    _In_ FLOAT LensV,
    _Out_ PRAY WorldRay
    );

#endif // _RAY_GENERATOR_IRIS_CAMERA_