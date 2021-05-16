/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    progress_reporter.h

Abstract:

    Reports the progress of rendering as it occurs.

--*/

#ifndef _IRIS_CAMERA_PROGRESS_REPORTER_
#define _IRIS_CAMERA_PROGRESS_REPORTER_

#include "iris_camera/progress_reporter_vtable.h"

//
// Functions
//

ISTATUS
ProgressReporterAllocate(
    _In_ PCPROGRESS_REPORTER_VTABLE vtable,
    _In_reads_bytes_opt_(data_size) const void *data,
    _In_ size_t data_size,
    _In_ size_t data_alignment,
    _Out_ PPROGRESS_REPORTER *progress_reporter
    );

void
ProgressReporterFree(
    _In_opt_ _Post_invalid_ PPROGRESS_REPORTER progress_reporter
    );

#endif // _IRIS_CAMERA_PROGRESS_REPORTER_