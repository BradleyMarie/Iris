/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    grid_pixel_sampler_generator.c

Abstract:

    Implements a grid pixel sampler generator.

--*/

#include <stdalign.h>

#include "iris_camera_toolkit/grid_pixel_sampler_generator.h"
#include "iris_camera_toolkit/grid_pixel_sampler.h"

//
// Types
//

typedef struct _GRID_PIXEL_SAMPLER_GENERATOR {
    uint16_t pixel_samples_u;
    uint16_t pixel_samples_v;
    bool jitter_pixel_samples;
    uint16_t lens_samples_u;
    uint16_t lens_samples_v;
    bool jitter_lens_samples;
} GRID_PIXEL_SAMPLER_GENERATOR, *PGRID_PIXEL_SAMPLER_GENERATOR;

typedef const GRID_PIXEL_SAMPLER_GENERATOR *PCGRID_PIXEL_SAMPLER_GENERATOR;

//
// Static Functions
//

ISTATUS
GridPixelSamplerGeneratorGenerate(
    _In_ const void *context,
    _Out_ PPIXEL_SAMPLER *pixel_sampler
    )
{
    PCGRID_PIXEL_SAMPLER_GENERATOR generator =
        (PCGRID_PIXEL_SAMPLER_GENERATOR)context;

    ISTATUS status = GridPixelSamplerAllocate(generator->pixel_samples_u,
                                              generator->pixel_samples_v,
                                              generator->jitter_pixel_samples,
                                              generator->lens_samples_u,
                                              generator->lens_samples_v,
                                              generator->jitter_lens_samples,
                                              pixel_sampler);

    return status;
}

//
// Static Variables
//

static const PIXEL_SAMPLER_GENERATOR_VTABLE grid_pixel_sampler_generator_vtable = {
    GridPixelSamplerGeneratorGenerate,
    NULL
};

//
// Functions
//

ISTATUS
GridPixelSamplerGeneratorAllocate(
    _In_ uint16_t pixel_samples_u,
    _In_ uint16_t pixel_samples_v,
    _In_ bool jitter_pixel_samples,
    _In_ uint16_t lens_samples_u,
    _In_ uint16_t lens_samples_v,
    _In_ bool jitter_lens_samples,
    _Out_ PPIXEL_SAMPLER_GENERATOR *pixel_sampler_generator
    )
{
    if (pixel_samples_u == 0)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }
    
    if (pixel_samples_v == 0)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }
    
    if (lens_samples_u == 0)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }
    
    if (lens_samples_v == 0)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (pixel_sampler_generator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_06;
    }

    GRID_PIXEL_SAMPLER_GENERATOR grid_pixel_sampler_generator;
    grid_pixel_sampler_generator.pixel_samples_u = pixel_samples_u;
    grid_pixel_sampler_generator.pixel_samples_v = pixel_samples_v;
    grid_pixel_sampler_generator.jitter_pixel_samples = jitter_pixel_samples;
    grid_pixel_sampler_generator.lens_samples_u = lens_samples_u;
    grid_pixel_sampler_generator.lens_samples_v = lens_samples_v;
    grid_pixel_sampler_generator.jitter_lens_samples = jitter_lens_samples;

    ISTATUS status =
        PixelSamplerGeneratorAllocate(&grid_pixel_sampler_generator_vtable,
                                      &grid_pixel_sampler_generator,
                                      sizeof(GRID_PIXEL_SAMPLER_GENERATOR),
                                      alignof(GRID_PIXEL_SAMPLER_GENERATOR),
                                      pixel_sampler_generator);

    return status;
}