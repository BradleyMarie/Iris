/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisphysxplusplus_brdfallocator.h

Abstract:

    This file contains the declarations for the 
    IrisPysx++ BRDFAllocator type.

--*/

#include <irisphysxplusplus.h>

#ifndef _BRDF_ALLOCATOR_IRIS_PHYSX_PLUS_PLUS_
#define _BRDF_ALLOCATOR_IRIS_PHYSX_PLUS_PLUS_

namespace IrisPhysx {

//
// Types
//

class BRDFAllocator final {    
public:
    BRDFAllocator(
        _In_ PPHYSX_BRDF_ALLOCATOR Allocator
        )
    : Data(Allocator)
    { 
        if (Allocator == nullptr)
        {
            throw std::invalid_argument("Allocator");
        }
    }
    
    _Ret_
    PPHYSX_BRDF_ALLOCATOR
    AsPPHYSX_BRDF_ALLOCATOR(
        void
        )
    {
        return Data;
    }

    template<typename T>
    BRDFReference
    Allocate(
        _In_ const T & BRDFData
        )
    {
        static_assert(std::is_trivially_copyable<T>::value,
                      "BRDFData must be trivially copyable");
    
        static_assert(sizeof(T) != 0, "Size of type must not be 0");
        static_assert(alignof(T) != 0, "Alignment of type must not be 0");
        static_assert((alignof(T) & (alignof(T) - 1)) == 0, "Alignment of type must not be a power of two");
        static_assert(sizeof(T) % alignof(T) == 0, "Size of type must be evenly divisible by the alignment");
    
        static const PHYSX_BRDF_VTABLE VTable {
            BRDFSampleAdapter<T>,
            BRDFSampleWithLambertianFalloffAdapter<T>,
            BRDFComputeReflectanceAdapter<T>,
            BRDFComputeReflectanceWithLambertianFalloffAdapter<T>,
            BRDFComputeReflectanceWithPdfAdapter<T>,
            BRDFComputeReflectanceWithPdfWithLambertianFalloffAdapter<T>,
            nullptr
        };
        
        PCPHYSX_BRDF AllocatedBrdf;
        
        ISTATUS Status = PhysxBrdfAllocatorAllocate(Data,
                                                    &VTable,
                                                    &BRDFData,
                                                    sizeof(T),
                                                    alignof(T),
                                                    &AllocatedBrdf);
    
        if (Status != ISTATUS_SUCCESS)
        {
            assert(Status == ISTATUS_ALLOCATION_FAILED);
            throw std::bad_alloc();
        }
        
        return BRDFReference(AllocatedBrdf);
    }
    
private:
    PPHYSX_BRDF_ALLOCATOR Data;

    _Check_return_
    _Success_(return == ISTATUS_SUCCESS)
    template<typename T>
    static
    ISTATUS
    BRDFSampleAdapter(
        _In_ PCVOID Context,
        _In_ VECTOR3 Incoming,
        _In_ VECTOR3 SurfaceNormal,
        _Inout_ PRANDOM Rng,
        _Inout_ PREFLECTOR_COMPOSITOR_REFERENCE Compositor,
        _Out_ PCREFLECTOR *Reflector,
        _Out_ PVECTOR3 Outgoing,
        _Out_ PFLOAT Pdf
        )
    {
        assert(Context != nullptr);
        assert(VectorValidate(Incoming) != FALSE);
        assert(VectorValidate(SurfaceNormal) != FALSE);
        assert(Rng != nullptr);
        assert(Compositor != nullptr);
        assert(Reflector != nullptr);
        assert(Outgoing != nullptr);
        assert(Pdf != nullptr);
        
        const T * Brdf = static_cast<const T*>(Context);
        
        std::tuple<IrisSpectrum::ReflectorReference, Iris::Vector, FLOAT> Result = 
            Brdf->Sample(Iris::Vector(Incoming),
                         Iris::Vector(SurfaceNormal),
                         IrisAdvanced::Random(Rng),
                         IrisSpectrum::ReflectorCompositorReference(Compositor));
    
        *Reflector = std::get<0>(Result).AsPCREFLECTOR();
        *Outgoing = std::get<1>(Result).AsVECTOR3();
        *Pdf = std::get<2>(Result);
        
        return ISTATUS_SUCCESS;
    }

    _Check_return_
    _Success_(return == ISTATUS_SUCCESS)
    template<typename T>
    static
    ISTATUS
    BRDFSampleWithLambertianFalloffAdapter(
        _In_ PCVOID Context,
        _In_ VECTOR3 Incoming,
        _In_ VECTOR3 SurfaceNormal,
        _Inout_ PRANDOM Rng,
        _Inout_ PREFLECTOR_COMPOSITOR_REFERENCE Compositor,
        _Out_ PCREFLECTOR *Reflector,
        _Out_ PVECTOR3 Outgoing,
        _Out_ PFLOAT Pdf
        )
    {
        assert(Context != nullptr);
        assert(VectorValidate(Incoming) != FALSE);
        assert(VectorValidate(SurfaceNormal) != FALSE);
        assert(Rng != nullptr);
        assert(Compositor != nullptr);
        assert(Reflector != nullptr);
        assert(Outgoing != nullptr);
        assert(Pdf != nullptr);
        
        const T * Brdf = static_cast<const T*>(Context);
        
        std::tuple<IrisSpectrum::ReflectorReference, Iris::Vector, FLOAT> Result = 
            Brdf->SampleWithLambertianFalloff(Iris::Vector(Incoming),
                                              Iris::Vector(SurfaceNormal),
                                              IrisAdvanced::Random(Rng),
                                              IrisSpectrum::ReflectorCompositorReference(Compositor));
    
        *Reflector = std::get<0>(Result).AsPCREFLECTOR();
        *Outgoing = std::get<1>(Result).AsVECTOR3();
        *Pdf = std::get<2>(Result);
        
        return ISTATUS_SUCCESS;
    }

    _Check_return_
    _Success_(return == ISTATUS_SUCCESS)
    template<typename T>
    static
    ISTATUS
    BRDFComputeReflectanceAdapter(
        _In_ PCVOID Context,
        _In_ VECTOR3 Incoming,
        _In_ VECTOR3 SurfaceNormal,
        _In_ VECTOR3 Outgoing,
        _Inout_ PREFLECTOR_COMPOSITOR_REFERENCE Compositor,
        _Out_ PCREFLECTOR *Reflector
        )
    {
        assert(Context != nullptr);
        assert(VectorValidate(Incoming) != FALSE);
        assert(VectorValidate(SurfaceNormal) != FALSE);
        assert(VectorValidate(Outgoing) != FALSE);
        assert(Compositor != nullptr);
        assert(Reflector != nullptr);
        
        const T * Brdf = static_cast<const T*>(Context);
        
        IrisSpectrum::ReflectorReference Result =
            Brdf->ComputeReflectance(Iris::Vector(Incoming),
                                     Iris::Vector(SurfaceNormal),
                                     Iris::Vector(Outgoing),
                                     IrisSpectrum::ReflectorCompositorReference(Compositor));
    
        *Reflector = Result.AsPCREFLECTOR();
        
        return ISTATUS_SUCCESS;
    }

    _Check_return_
    _Success_(return == ISTATUS_SUCCESS)
    template<typename T>
    static
    ISTATUS
    BRDFComputeReflectanceWithLambertianFalloffAdapter(
        _In_ PCVOID Context,
        _In_ VECTOR3 Incoming,
        _In_ VECTOR3 SurfaceNormal,
        _In_ VECTOR3 Outgoing,
        _Inout_ PREFLECTOR_COMPOSITOR_REFERENCE Compositor,
        _Out_ PCREFLECTOR *Reflector
        )
    {
        assert(Context != nullptr);
        assert(VectorValidate(Incoming) != FALSE);
        assert(VectorValidate(SurfaceNormal) != FALSE);
        assert(VectorValidate(Outgoing) != FALSE);
        assert(Compositor != nullptr);
        assert(Reflector != nullptr);
        
        const T * Brdf = static_cast<const T*>(Context);
        
        IrisSpectrum::ReflectorReference Result =
            Brdf->ComputeReflectanceWithLambertianFalloff(Iris::Vector(Incoming),
                                                          Iris::Vector(SurfaceNormal),
                                                          Iris::Vector(Outgoing),
                                                          IrisSpectrum::ReflectorCompositorReference(Compositor));
    
        *Reflector = Result.AsPCREFLECTOR();
        
        return ISTATUS_SUCCESS;
    }

    _Check_return_
    _Success_(return == ISTATUS_SUCCESS)
    template<typename T>
    static
    ISTATUS
    BRDFComputeReflectanceWithPdfAdapter(
        _In_ PCVOID Context,
        _In_ VECTOR3 Incoming,
        _In_ VECTOR3 SurfaceNormal,
        _In_ VECTOR3 Outgoing,
        _Inout_ PREFLECTOR_COMPOSITOR_REFERENCE Compositor,
        _Out_ PCREFLECTOR *Reflector,
        _Out_ PFLOAT Pdf
        )
    {
        assert(Context != nullptr);
        assert(VectorValidate(Incoming) != FALSE);
        assert(VectorValidate(SurfaceNormal) != FALSE);
        assert(VectorValidate(Outgoing) != FALSE);
        assert(Compositor != nullptr);
        assert(Reflector != nullptr);
        assert(Pdf != nullptr);
        
        const T * Brdf = static_cast<const T*>(Context);
        
        std::tuple<IrisSpectrum::ReflectorReference, FLOAT> Result =
            Brdf->ComputeReflectanceWithPdf(Iris::Vector(Incoming),
                                            Iris::Vector(SurfaceNormal),
                                            Iris::Vector(Outgoing),
                                            IrisSpectrum::ReflectorCompositorReference(Compositor));
    
        *Reflector = std::get<0>(Result).AsPCREFLECTOR();
        *Pdf = std::get<1>(Result);
        
        return ISTATUS_SUCCESS;
    }

    _Check_return_
    _Success_(return == ISTATUS_SUCCESS)
    template<typename T>
    static
    ISTATUS
    BRDFComputeReflectanceWithPdfWithLambertianFalloffAdapter(
        _In_ PCVOID Context,
        _In_ VECTOR3 Incoming,
        _In_ VECTOR3 SurfaceNormal,
        _In_ VECTOR3 Outgoing,
        _Inout_ PREFLECTOR_COMPOSITOR_REFERENCE Compositor,
        _Out_ PCREFLECTOR *Reflector,
        _Out_ PFLOAT Pdf
        )
    {
        assert(Context != nullptr);
        assert(VectorValidate(Incoming) != FALSE);
        assert(VectorValidate(SurfaceNormal) != FALSE);
        assert(VectorValidate(Outgoing) != FALSE);
        assert(Compositor != nullptr);
        assert(Reflector != nullptr);
        assert(Pdf != nullptr);
        
        const T * Brdf = static_cast<const T*>(Context);
        
        std::tuple<IrisSpectrum::ReflectorReference, FLOAT> Result =
            Brdf->ComputeReflectanceWithPdfWithLambertianFalloff(Iris::Vector(Incoming),
                                                                 Iris::Vector(SurfaceNormal),
                                                                 Iris::Vector(Outgoing),
                                                                 IrisSpectrum::ReflectorCompositorReference(Compositor));
    
        *Reflector = std::get<0>(Result).AsPCREFLECTOR();
        *Pdf = std::get<1>(Result);
        
        return ISTATUS_SUCCESS;
    }
};

} // namespace Iris

#endif // _BRDF_ALLOCATOR_IRIS_PHYSX_PLUS_PLUS_
