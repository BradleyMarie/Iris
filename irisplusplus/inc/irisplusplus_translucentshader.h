/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisplusplus_translucentshader.h

Abstract:

    This file contains the definitions for the 
    Iris++ translucent shader type.

--*/

#include <irisplusplus.h>

#ifndef _TRANSLUCENT_SHADER_IRIS_PLUS_PLUS_
#define _TRANSLUCENT_SHADER_IRIS_PLUS_PLUS_

namespace Iris {

//
// Types
//

class TranslucentShader {
protected:
    IRISPLUSPLUSAPI
    TranslucentShader(
        void
        );

public:
    _Ret_
    PTRANSLUCENT_SHADER
    AsPTRANSLUCENT_SHADER(
        void
        ) const
    {
        return Data;
    }

    _Ret_
    virtual
    FLOAT
    Shade(
        _In_ const Point & WorldHitPoint,
        _In_ const Point & ModelHitPoint,
        _In_opt_ const void * AdditionalData
        ) const = 0;

    virtual
    ~TranslucentShader(
        void
        )
    { }

private:
    PTRANSLUCENT_SHADER Data;

    IRISPLUSPLUSAPI
    TranslucentShader(
        _In_ PTRANSLUCENT_SHADER Shader
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

    friend class IrisPointer<TranslucentShader>;
    friend class CTranslucentShader;
};

class CTranslucentShader final : public TranslucentShader {
public:
    IRISPLUSPLUSAPI
    static
    IrisPointer<TranslucentShader>
    Create(
        _In_ PTRANSLUCENT_SHADER Shader
        );

    _Ret_
    virtual
    FLOAT
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
    CTranslucentShader(
        _In_ PTRANSLUCENT_SHADER Shader
        );

    std::atomic_size_t References;
};

} // namespace Iris

#endif // _TRANSLUCENT_SHADER_IRIS_PLUS_PLUS_