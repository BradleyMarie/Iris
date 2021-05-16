/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    status_bar_progress_reporter.h

Abstract:

    Creates a progress reporter that writes a status bar to standard out.

--*/

#ifndef _IRIS_CAMERA_TOOLKIT_STATUS_BAR_PROGRESS_REPORTER_
#define _IRIS_CAMERA_TOOLKIT_STATUS_BAR_PROGRESS_REPORTER_

#include "iris_camera/iris_camera.h"

#if __cplusplus 
extern "C" {
#endif // __cplusplus

//
// Functions
//

ISTATUS
StatusBarProgressReporterAllocate(
    _In_opt_ const char* label,
    _Out_ PPROGRESS_REPORTER *progress_reporter
    );

#if __cplusplus 
}
#endif // __cplusplus

#endif // _IRIS_CAMERA_TOOLKIT_STATUS_BAR_PROGRESS_REPORTER_