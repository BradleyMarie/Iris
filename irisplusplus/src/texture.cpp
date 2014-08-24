/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    texture.cpp

Abstract:

    This file contains the definitions for the 
    Iris++ texture type.

--*/

#include <irisplusplus.h>
#include <sstream>

namespace Iris {

//
// TextureShader
//

IRISPLUSPLUSAPI
void
TextureShader::Shade(
    _In_opt_ PCEMISSIVE_SHADER EmissiveShader,
    _In_opt_ PCDIRECT_SHADER DirectShader,
    _In_opt_ PCINDIRECT_SHADER IndirectShader,
    _In_opt_ PCTRANSLUCENT_SHADER TranslucentShader
    )
{
    ISTATUS Status = TextureShaderShadeShader(Data,
                                              EmissiveShader,
                                              DirectShader,
                                              IndirectShader,
                                              TranslucentShader);

    if (Status != ISTATUS_SUCCESS)
    {
        std::ostringstream error;
        error << "Iris Error: " << Status;
        throw std::runtime_error(error.str());
    }
}

IRISPLUSPLUSAPI
TextureShader::TextureShader(
    _In_ PTEXTURE_SHADER Shader
    )
: Data(Shader)
{ }

//
// Static Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
TextureShade(
    _In_ PCVOID Context,
    _In_ POINT3 WorldHitPoint,
    _In_ POINT3 ModelHitPoint,
    _In_opt_ PCVOID AdditionalData,
    _Inout_ PTEXTURE_SHADER IrisTextureShader
    )
{
    assert(Context != NULL);
    assert(IrisTextureShader != NULL);

    const Texture *TexturePointer = static_cast<const Texture*>(Context);

    TextureShader Shader(IrisTextureShader);
    Point WorldHit(WorldHitPoint);
    Point ModelHit(ModelHitPoint);

    TexturePointer->Shade(WorldHit, ModelHit, AdditionalData, Shader);

    return ISTATUS_SUCCESS;
}

static
VOID
TextureFree(
    _In_ _Post_invalid_ PVOID Context
    )
{
    assert(Context != NULL);

    const Texture *TexturePointer = static_cast<const Texture*>(Context);
    delete TexturePointer;
}

//
// Static Variables
//

const static TEXTURE_VTABLE InteropVTable = {
    Iris::TextureShade,
    TextureFree
};

//
// Functions
//

Texture::Texture(
    void
    )
{
    Texture *TexturePointer = this;

    PTEXTURE IrisTexture = TextureAllocate(&InteropVTable,
                                           &TexturePointer,
                                           sizeof(Texture*),
                                           sizeof(Texture*));

    if (IrisTexture == NULL)
    {
        throw std::bad_alloc();
    }

    Data = IrisTexture;
}

Texture::Texture(
    _In_ PTEXTURE IrisTexture
    )
: Data(IrisTexture)
{ }

void
Texture::Reference(
    void
    )
{
    TextureReference(Data);
}

void 
Texture::Dereference(
    void
    )
{
    TextureDereference(Data);
}

IrisPointer<Texture>
CTexture::Create(
    _In_ PTEXTURE IrisTexture
    )
{
    CTexture* Allocated = new CTexture(IrisTexture);
    return IrisPointer<Texture>(Allocated);
}

CTexture::CTexture(
    _In_ PTEXTURE IrisTexture
    )
: Texture(IrisTexture)
{ }

void
CTexture::Shade(
    _In_ const Point & WorldHitPoint,
    _In_ const Point & ModelHitPoint,
    _In_ const void * AdditionalData,
    _Inout_ TextureShader & Shader
    ) const
{
    POINT3 WorldHit = WorldHitPoint.AsPOINT3();
    POINT3 ModelHit = ModelHitPoint.AsPOINT3();
    PTEXTURE_SHADER IrisShader = Shader.AsPTEXTURE_SHADER();

    ISTATUS Status = TextureShade(Data,
                                  WorldHit,
                                  ModelHit,
                                  AdditionalData,
                                  IrisShader);

    if (Status != ISTATUS_SUCCESS)
    {
        std::ostringstream error;
        error << "Iris Error: " << Status;
        throw std::runtime_error(error.str());
    }
}

void
CTexture::Reference(
    void
    )
{
    Texture::Reference();
    References += 1;
}

void 
CTexture::Dereference(
    void
    )
{
    Texture::Dereference();
    References -= 1;

    if (References == 0)
    {
        delete this;
    }
}

} // namespace Iris