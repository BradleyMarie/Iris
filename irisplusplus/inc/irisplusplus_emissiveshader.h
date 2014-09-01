/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisplusplus_emissiveshader.h

Abstract:

    This file contains the definitions for the 
    Iris++ emiwssive shader type.

--*/

#include <irisplusplus.h>

#ifndef _EMISSIVE_SHADER_IRIS_PLUS_PLUS_
#define _EMISSIVE_SHADER_IRIS_PLUS_PLUS_

namespace Iris {

//
// Types
//

class EmissiveShader {
protected:
    IRISPLUSPLUSAPI
    EmissiveShader(
        void
        );

public:
    _Ret_
    PEMISSIVE_SHADER
    AsPEMISSIVE_SHADER(
        void
        ) const
    {
        return Data;
    }

    _Ret_
    virtual
    COLOR3
    Shade(
        _In_ const Point & WorldHitPoint,
        _In_ const Point & ModelHitPoint,
        _In_opt_ const void * AdditionalData
        ) const = 0;

    virtual
    ~EmissiveShader(
        void
        )
    { }

private:
    PEMISSIVE_SHADER Data;

    IRISPLUSPLUSAPI
    EmissiveShader(
        _In_ PEMISSIVE_SHADER Shader
        );

    IRISPLUSPLUSAPI
    virtual
    void
    Reference(
        void
        );

    IRISPLUSPLUSAPI
    virtual
    void 
    Dereference(
        void
        );

    friend class IrisPointer<EmissiveShader>;
    friend class CEmissiveShader;
};

class CEmissiveShader final : public EmissiveShader {
public:
    IRISPLUSPLUSAPI
    static
    IrisPointer<EmissiveShader>
    Create(
        _In_ PEMISSIVE_SHADER Shader
        );

    _Ret_
    virtual
    COLOR3
    Shade(
        _In_ const Point & WorldHitPoint,
        _In_ const Point & ModelHitPoint,
        _In_opt_ const void * AdditionalData
        ) const;

    IRISPLUSPLUSAPI
    virtual
    void
    Reference(
        void
        );

    IRISPLUSPLUSAPI
    virtual
    void 
    Dereference(
        void
        );

private:
    CEmissiveShader(
        _In_ PEMISSIVE_SHADER Shader
        );

    std::atomic_size_t References;
};

} // namespace Iris

#endif // _EMISSIVE_SHADER_IRIS_PLUS_PLUS_