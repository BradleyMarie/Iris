/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    progress_reporter_vtable.h

Abstract:

    The vtable for a progress reporter.

--*/

#ifndef _IRIS_CAMERA_PROGRESS_REPORTER_VTABLE_
#define _IRIS_CAMERA_PROGRESS_REPORTER_VTABLE_

#include "common/free_routine.h"
#include "iris_advanced/iris_advanced.h"

//
// Types
//

typedef struct _PROGRESS_REPORTER PROGRESS_REPORTER, *PPROGRESS_REPORTER;
typedef const PROGRESS_REPORTER *PCPROGRESS_REPORTER;

typedef
ISTATUS
(*PPROGRESS_REPORTER_REPORT)(
    _In_opt_ const void *context,
    _In_ size_t num_pixels,
    _In_ size_t pixels_rendered
    );

typedef struct _PROGRESS_REPORTER_VTABLE {
    PPROGRESS_REPORTER_REPORT report_routine;
    PFREE_ROUTINE free_routine;
} PROGRESS_REPORTER_VTABLE, *PPROGRESS_REPORTER_VTABLE;

typedef const PROGRESS_REPORTER_VTABLE *PCPROGRESS_REPORTER_VTABLE;

#endif // _IRIS_CAMERA_PROGRESS_REPORTER_VTABLE_