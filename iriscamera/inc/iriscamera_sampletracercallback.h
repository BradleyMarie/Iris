/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    iriscamera_sampletracerthreadstatecallback.h

Abstract:

    This file contains the definitions for the SAMPLE_TRACER_CALLBACK type.

--*/

#ifndef _SAMPLE_TRACER_CALLBACK_IRIS_CAMERA_
#define _SAMPLE_TRACER_CALLBACK_IRIS_CAMERA_

#include <iriscamera.h>

//
// Types
//

typedef struct _SAMPLE_TRACER_CALLBACK SAMPLE_TRACER_CALLBACK, *PSAMPLE_TRACER_CALLBACK;
typedef CONST SAMPLE_TRACER_CALLBACK *PCSAMPLE_TRACER_CALLBACK;

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISCAMERAAPI
ISTATUS
SampleTracerCallback(
    _In_ PCSAMPLE_TRACER_CALLBACK Callback,
    _In_opt_ PVOID ThreadState
    );

#endif // _SAMPLE_TRACER_CALLBACK_IRIS_CAMERA_