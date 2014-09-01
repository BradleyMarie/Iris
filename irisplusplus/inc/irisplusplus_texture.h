/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisplusplus_texture.h

Abstract:

    This file contains the definitions for the 
    Iris++ texture type.

--*/

#include <irisplusplus.h>

#ifndef _TEXTURE_IRIS_PLUS_PLUS_
#define _TEXTURE_IRIS_PLUS_PLUS_

namespace Iris {

//
// Types
//

class TextureShader final {
public:
    PTEXTURE_SHADER
    AsPTEXTURE_SHADER(
        void
        )
    {
        return Data;
    }

    IRISPLUSPLUSAPI
    void
    Shade(
        _In_opt_ PCEMISSIVE_SHADER EmissiveShader,
        _In_opt_ PCDIRECT_SHADER DirectShader,
        _In_opt_ PCINDIRECT_SHADER IndirectShader,
        _In_opt_ PCTRANSLUCENT_SHADER TranslucentShader
        );

private:
    const PTEXTURE_SHADER Data;

    IRISPLUSPLUSAPI
    TextureShader(
        _In_ PTEXTURE_SHADER Shader
        );

    _Check_return_
    _Success_(return == ISTATUS_SUCCESS)
    friend
    ISTATUS
    TextureShade(
        _In_ PCVOID Context,
        _In_ POINT3 WorldHitPoint,
        _In_ POINT3 ModelHitPoint,
        _In_opt_ PCVOID AdditionalData,
        _Inout_ PTEXTURE_SHADER TextureShader
        );
};

class Texture {
protected:
    IRISPLUSPLUSAPI
    Texture(
        void
        );

public:
    _Ret_
    PTEXTURE
    AsPTEXTURE(
        void
        ) const
    {
        return Data;
    }

    virtual
    ~Texture(
        void
        )
    { }

    IRISPLUSPLUSAPI
    virtual
    void
    Shade(
        _In_ const Point & WorldHitPoint,
        _In_ const Point & ModelHitPoint,
        _In_ const void * AdditionalData,
        _Inout_ TextureShader & Shader
        ) const = 0;

private:
    PTEXTURE Data;

    IRISPLUSPLUSAPI
    Texture(
        _In_ PTEXTURE IrisTexture
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

    friend class IrisPointer<Texture>;
    friend class CTexture;
};

class CTexture final : public Texture {
public:
    IRISPLUSPLUSAPI
    static
    IrisPointer<Texture>
    Create(
        _In_ PTEXTURE IrisTexture
        );

private:
    IRISPLUSPLUSAPI
    CTexture(
        _In_ PTEXTURE IrisTexture
        );

    IRISPLUSPLUSAPI
    virtual
    void
    Shade(
        _In_ const Point & WorldHitPoint,
        _In_ const Point & ModelHitPoint,
        _In_ const void * AdditionalData,
        _Inout_ TextureShader & Shader
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

    std::atomic_size_t References;
};

} // namespace Iris

#endif // _TEXTURE_IRIS_PLUS_PLUS_