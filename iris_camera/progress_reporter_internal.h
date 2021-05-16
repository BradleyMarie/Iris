/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    progress_reporter_internal.h

Abstract:

    The internal routines for a progress reporter.

--*/

#ifndef _IRIS_CAMERA_PROGRESS_REPORTER_INTERNAL_
#define _IRIS_CAMERA_PROGRESS_REPORTER_INTERNAL_

#include "iris_camera/progress_reporter_vtable.h"

//
// Types
//

struct _PROGRESS_REPORTER {
    PCPROGRESS_REPORTER_VTABLE vtable;
    void *data;
};

//
// Functions
//

static
inline
ISTATUS
ProgressReporterReport(
    _Inout_ struct _PROGRESS_REPORTER *progress_reporter,
    _In_ size_t num_pixels,
    _In_ size_t pixels_rendered
    )
{
    assert(progress_reporter != NULL);
    assert(num_pixels != 0);
    assert(pixels_rendered <= num_pixels);

    ISTATUS status =
        progress_reporter->vtable->report_routine(progress_reporter->data,
                                                  num_pixels,
                                                  pixels_rendered);

    return status;
}

#endif // _IRIS_CAMERA_PROGRESS_REPORTER_INTERNAL_