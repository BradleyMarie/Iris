/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    status_bar_progress_reporter.c

Abstract:

    Implements the status bar progress reporter.

--*/

#include <stdalign.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "iris_camera_toolkit/status_bar_progress_reporter.h"

//
// Defines
//

#define DEFAULT_CONSOLE_WIDTH   80
#define BAR_RESERVED_CHARACTERS 28
#define BAR_MINIMUM_WIDTH        2

//
// Types
//

typedef struct _STATUS_BAR_PROGRESS_REPORTER {
    _Field_size_(label_size) char *label;
    size_t label_size;
    time_t start_time;
    time_t last_update_time;
    unsigned long int current_bar_size;
    unsigned long int bar_width;
} STATUS_BAR_PROGRESS_REPORTER, *PSTATUS_BAR_PROGRESS_REPORTER;

//
// Static Functions
//

ISTATUS
StatusBarProgressReporterReport(
    _Inout_ void *context,
    _In_ size_t num_pixels,
    _In_ size_t pixels_rendered
    )
{
    PSTATUS_BAR_PROGRESS_REPORTER status_bar =
        (PSTATUS_BAR_PROGRESS_REPORTER)context;

    time_t current_time = time(NULL);

    double time_elapsed, pixels_per_second;
    bool first_draw, time_advanced;
    if (pixels_rendered == 0)
    {
        time_advanced = false;
        status_bar->start_time = current_time;
        status_bar->last_update_time = current_time;
        time_elapsed = 0.0;
        pixels_per_second = 0.0;
        first_draw = true;
    }
    else
    {
        time_advanced = status_bar->last_update_time != current_time;
        status_bar->last_update_time = current_time;
        time_elapsed =
            difftime(current_time, status_bar->start_time);
        pixels_per_second =
            (double)pixels_rendered / time_elapsed;
        first_draw = false;
    }

    double progress = (double)pixels_rendered / (double)num_pixels;
    unsigned long int bar_progress = (double)status_bar->bar_width * progress;

    if (!first_draw && !time_advanced &&
        bar_progress == status_bar->current_bar_size)
    {
        return ISTATUS_SUCCESS;
    }

    status_bar->current_bar_size = bar_progress;

    printf("\r%s: [", status_bar->label);

    for (unsigned long int i = 0; i < bar_progress; i++)
    {
        if (i + 1 != bar_progress || pixels_rendered == num_pixels)
        {
            printf("=");
        }
        else
        {
            printf(">");
        }
    }

    for (unsigned long int i = bar_progress; i < status_bar->bar_width; i++)
    {
        printf(" ");
    }

    printf("]");

    if (pixels_per_second == 0.0)
    {
        printf(" (%.0fs|?s)    ", time_elapsed);
    }
    else if (pixels_rendered == num_pixels)
    {
        printf(" (%.0fs)       \n", time_elapsed);
    }
    else
    {
        double estimated_render_time = (double)num_pixels / pixels_per_second;
        double estimated_time_remaining =
            IMax(0.0, estimated_render_time - time_elapsed);
        printf(" (%.0fs|%.0fs) ", time_elapsed, estimated_time_remaining);
    }

    fflush(stdout);

    return ISTATUS_SUCCESS;
}

static
void
StatusBarProgressReporterFree(
    _In_opt_ _Post_invalid_ void *context
    )
{
    PSTATUS_BAR_PROGRESS_REPORTER status_bar =
        (PSTATUS_BAR_PROGRESS_REPORTER)context;

    free(status_bar->label);
}

//
// Static Variables
//

static const PROGRESS_REPORTER_VTABLE status_bar_progress_reporter_vtable = {
    StatusBarProgressReporterReport,
    StatusBarProgressReporterFree
};

//
// Functions
//

ISTATUS
StatusBarProgressReporterAllocate(
    _In_opt_ const char* label,
    _Out_ PPROGRESS_REPORTER *progress_reporter
    )
{
    if (progress_reporter == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    STATUS_BAR_PROGRESS_REPORTER status_bar;
    
    if (label != NULL)
    {
        status_bar.label_size = strlen(label);
    }
    else
    {
        status_bar.label_size = 0;
    }

    size_t allocation_size = status_bar.label_size + 1;
    status_bar.label = malloc(allocation_size);

    if (status_bar.label == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    char *columns = getenv("COLUMNS");

    unsigned long int num_columns;
    if (columns != NULL)
    {
        num_columns = strtoul(columns, NULL, 10);
    }
    else
    {
        num_columns = DEFAULT_CONSOLE_WIDTH;
    }

    if (num_columns < BAR_RESERVED_CHARACTERS)
    {
        status_bar.bar_width = 0;
    }
    else
    {
        status_bar.bar_width = num_columns - BAR_RESERVED_CHARACTERS;
    }

    if (status_bar.bar_width < status_bar.label_size)
    {
        status_bar.bar_width = 0;
    }
    else
    {
        status_bar.bar_width -= status_bar.label_size;
    }

    if (status_bar.bar_width < BAR_MINIMUM_WIDTH)
    {
        status_bar.bar_width = BAR_MINIMUM_WIDTH;
    }

    ISTATUS status =
        ProgressReporterAllocate(&status_bar_progress_reporter_vtable,
                                 &status_bar,
                                 sizeof(STATUS_BAR_PROGRESS_REPORTER),
                                 alignof(STATUS_BAR_PROGRESS_REPORTER),
                                 progress_reporter);

    if (status != ISTATUS_SUCCESS)
    {
        free(status_bar.label);
        return status;
    }

    strncpy(status_bar.label, label, allocation_size);

    return ISTATUS_SUCCESS;
}