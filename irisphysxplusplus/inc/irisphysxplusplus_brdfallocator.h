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
private:
    _Check_return_
    _Success_(return == ISTATUS_SUCCESS)
    template<typename T>
    static
    ISTATUS
    BRDFSampleAdapter(
        _In_ PCVOID Context,
        _In_ VECTOR3 Incoming,
        _Inout_ PRANDOM_REFERENCE Rng,
        _Inout_ PREFLECTOR_COMPOSITOR_REFERENCE Compositor,
        _Out_ PCREFLECTOR *Reflector,
        _Out_ PVECTOR3 Outgoing,
        _Out_ PFLOAT Pdf
        )
    {
        assert(Context != nullptr);
        assert(VectorValidate(Incoming) != FALSE);
        assert(Rng != nullptr);
        assert(Compositor != nullptr);
        assert(Reflector != nullptr);
        assert(Outgoing != nullptr);
        assert(Pdf != nullptr);
        
        const T * Brdf = static_cast<const T*>(Context);
        
        std::tuple<IrisSpectrum::ReflectorReference, Iris::Vector, FLOAT> Result = 
            Brdf->Sample(Iris::Vector(Incoming),
                         IrisAdvanced::RandomReference(Rng),
                         IrisSpecrum::ReflectorCompositorReference(Compositor));
    
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
        _Inout_ PRANDOM_REFERENCE Rng,
        _Inout_ PREFLECTOR_COMPOSITOR_REFERENCE Compositor,
        _Out_ PCREFLECTOR *Reflector,
        _Out_ PVECTOR3 Outgoing,
        _Out_ PFLOAT Pdf
        )
    {
        assert(Context != nullptr);
        assert(VectorValidate(Incoming) != FALSE);
        assert(Rng != nullptr);
        assert(Compositor != nullptr);
        assert(Reflector != nullptr);
        assert(Outgoing != nullptr);
        assert(Pdf != nullptr);
        
        const T * Brdf = static_cast<const T*>(Context);
        
        std::tuple<IrisSpectrum::ReflectorReference, Iris::Vector, FLOAT> Result = 
            Brdf->SampleWithLambertianFalloff(Iris::Vector(Incoming),
                                              IrisAdvanced::RandomReference(Rng),
                                              IrisSpecrum::ReflectorCompositorReference(Compositor));
    
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
        _In_ VECTOR3 Outgoing,
        _Inout_ PREFLECTOR_COMPOSITOR_REFERENCE Compositor,
        _Out_ PCREFLECTOR *Reflector
        )
    {
        assert(Context != nullptr);
        assert(VectorValidate(Incoming) != FALSE);
        assert(VectorValidate(Outgoing) != FALSE);
        assert(Compositor != nullptr);
        assert(Reflector != nullptr);
        
        const T * Brdf = static_cast<const T*>(Context);
        
        IrisSpectrum::ReflectorReference Result =
            Brdf->ComputeReflectance(Iris::Vector(Incoming),
                                     Iris::Vector(Outgoing),
                                     IrisSpecrum::ReflectorCompositorReference(Compositor));
    
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
        _In_ VECTOR3 Outgoing,
        _Inout_ PREFLECTOR_COMPOSITOR_REFERENCE Compositor,
        _Out_ PCREFLECTOR *Reflector
        )
    {
        assert(Context != nullptr);
        assert(VectorValidate(Incoming) != FALSE);
        assert(VectorValidate(Outgoing) != FALSE);
        assert(Compositor != nullptr);
        assert(Reflector != nullptr);
        
        const T * Brdf = static_cast<const T*>(Context);
        
        IrisSpectrum::ReflectorReference Result =
            Brdf->ComputeReflectanceWithLambertianFalloff(Iris::Vector(Incoming),
                                                          Iris::Vector(Outgoing),
                                                          IrisSpecrum::ReflectorCompositorReference(Compositor));
    
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
        _In_ VECTOR3 Outgoing,
        _Inout_ PREFLECTOR_COMPOSITOR_REFERENCE Compositor,
        _Out_ PCREFLECTOR *Reflector,
        _Out_ PFLOAT Pdf
        )
    {
        assert(Context != nullptr);
        assert(VectorValidate(Incoming) != FALSE);
        assert(VectorValidate(Outgoing) != FALSE);
        assert(Compositor != nullptr);
        assert(Reflector != nullptr);
        assert(Pdf != nullptr);
        
        const T * Brdf = static_cast<const T*>(Context);
        
        
        std::tuple<IrisSpectrum::ReflectorReference, FLOAT> Result =
            Brdf->ComputeReflectanceWithPdf(Iris::Vector(Incoming),
                                            Iris::Vector(Outgoing),
                                            IrisSpecrum::ReflectorCompositorReference(Compositor));
    
        *Reflector = Result.AsPCREFLECTOR();
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
        _In_ VECTOR3 Outgoing,
        _Inout_ PREFLECTOR_COMPOSITOR_REFERENCE Compositor,
        _Out_ PCREFLECTOR *Reflector,
        _Out_ PFLOAT Pdf
        )
    {
        assert(Context != nullptr);
        assert(VectorValidate(Incoming) != FALSE);
        assert(VectorValidate(Outgoing) != FALSE);
        assert(Compositor != nullptr);
        assert(Reflector != nullptr);
        assert(Pdf != nullptr);
        
        const T * Brdf = static_cast<const T*>(Context);
        
        std::tuple<IrisSpectrum::ReflectorReference, FLOAT> Result =
            Brdf->ComputeReflectanceWithPdfWithLambertianFalloff(Iris::Vector(Incoming),
                                                                 Iris::Vector(Outgoing),
                                                                 IrisSpecrum::ReflectorCompositorReference(Compositor));
    
        *Reflector = std::get<0>(Result).AsPCREFLECTOR();
        *Pdf = std::get<1>(Result);
        
        return ISTATUS_SUCCESS;
    }
    
public:
    IRISPHYSXPLUSPLUSAPI
    BRDFAllocator(
        _In_ PPBR_BRDF_ALLOCATOR BrdfAllocator
        );
    
    _Ret_
    PPBR_BRDF_ALLOCATOR
    AsPPBR_BRDF_ALLOCATOR(
        void
        )
    {
        return Data;
    }

    _Ret_
    template<typename T>
    BRDF
    Allocate(
        _In_ const T & BRDFData
        )
    {
        static_assert(std::is_trivially_copyable<T>::value,
                      "T must be trivially copyable");
    
        static const PBR_BRDF_VTABLE VTable {
            BRDFSampleAdapter<T>,
            BRDFSampleWithLambertianFalloffAdapter<T>,
            BRDFComputeReflectanceAdapter<T>,
            BRDFComputeReflectanceWithLambertianFalloffAdapter<T>,
            BRDFComputeReflectanceWithPdfAdapter<T>,
            BRDFComputeReflectanceWithPdfWithLambertianFalloffAdapter<T>
        };
        
        PCPBR_BRDF AllocatedBrdf;
        
        ISTATUS Status = PbrBrdfAllocatorAllocate(Data,
                                                  &VTable,
                                                  &BRDFData,
                                                  sizeof(T),
                                                  alignof(T),
                                                  &AllocatedBrdf);
    
        switch (Status)
        {
            case ISTATUS_SUCCESS:
                break;
            case ISTATUS_ALLOCATION_FAILED:
                throw std::bac_alloc();
                break;
            default:
                assert(false);
        }
        
        return BRDF(AllocatedBrdf);
    }
    
private:
    PPBR_BRDF_ALLOCATOR Data;
};

} // namespace Iris

#endif // _BRDF_ALLOCATOR_IRIS_PHYSX_PLUS_PLUS_