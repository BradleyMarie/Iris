/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    spectrum.c

Abstract:

    Interface representing a spectrum of light.

--*/

#include "common/alloc.h"
#include "iris_spectrum/spectrum.h"
#include "iris_spectrum/spectrum_internal.h"

#include <stdalign.h>
#include <stdatomic.h>

//
// Functions
//

ISTATUS
SpectrumAllocate(
    _In_ PCSPECTRUM_VTABLE vtable,
    _When_(data_size != 0, _In_reads_bytes_opt_(data_size)) const void *data,
    _In_ size_t data_size,
    _When_(data_size != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && data_size % _Curr_ == 0)) size_t data_alignment,
    _Out_ PSPECTRUM *spectrum
    )
{
    if (vtable == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (data_size != 0)
    {
        if (data == NULL)
        {
            return ISTATUS_INVALID_ARGUMENT_COMBINATION_00;
        }
        
        if (data_alignment == 0 ||
            (data_alignment & (data_alignment - 1)) != 0)
        {
            return ISTATUS_INVALID_ARGUMENT_COMBINATION_01;    
        }
        
        if (data_size % data_alignment != 0)
        {
            return ISTATUS_INVALID_ARGUMENT_COMBINATION_02;
        }
    }

    if (spectrum == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    void *data_allocation;
    bool success = AlignedAllocWithHeader(sizeof(SPECTRUM),
                                          alignof(SPECTRUM),
                                          (void **)spectrum,
                                          data_size,
                                          data_alignment,
                                          &data_allocation,
                                          NULL);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    SpectrumInitialize(vtable,
                       data_allocation,
                       *spectrum);

    if (data_size != 0)
    {
        memcpy(data_allocation, data, data_size);
    }

    return ISTATUS_SUCCESS;
}

ISTATUS
SpectrumSample(
    _In_opt_ PCSPECTRUM spectrum,
    _In_ float_t wavelength,
    _Out_ float_t *intensity
    )
{
    if (spectrum == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (!isfinite(wavelength) || 
        wavelength <= (float_t)0.0)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (intensity == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    ISTATUS status = SpectrumSampleInline(spectrum,
                                          wavelength,
                                          intensity);

    return status;
}

void
SpectrumRetain(
    _In_opt_ PSPECTRUM spectrum
    )
{
    if (spectrum == NULL)
    {
        return;
    }

    atomic_fetch_add(&spectrum->reference_count, 1);
}

void
SpectrumRelease(
    _In_opt_ _Post_invalid_ PSPECTRUM Spectrum
    )
{
    PFREE_ROUTINE FreeRoutine;
    
    if (Spectrum == NULL)
    {
        return;
    }

    if (atomic_fetch_sub(&spectrum->reference_count, 1) == 1)
    {
        if (spectrum->vtable->free_routine != NULL)
        {
            spectrum->vtable->free_routine(spectrum->data);
        }
    
        free(spectrum);
    }
}