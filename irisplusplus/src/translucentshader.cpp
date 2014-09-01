/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    translucentshader.cpp

Abstract:

    This file contains the definitions for the 
    Iris++ translucent shader type.

--*/

#include <irisplusplus.h>
#include <sstream>

namespace Iris {

//
// Static Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
static
ISTATUS 
TranslucentShaderShade(
    _In_ PCVOID Context,
    _In_ POINT3 WorldHitPoint,
    _In_ POINT3 ModelHitPoint,
    _In_opt_ PCVOID AdditionalData,
    _Out_ PFLOAT Alpha
    )
{
    assert(Context != NULL);
    assert(Alpha != NULL);

    Point WorldHit(WorldHitPoint);
    Point ModelHit(ModelHitPoint);

    const TranslucentShader *ShaderPointer = static_cast<const TranslucentShader*>(Context);
    *Alpha = ShaderPointer->Shade(WorldHit, ModelHit, AdditionalData);
    return ISTATUS_SUCCESS;
}

static
VOID
TranslucentShaderFree(
    _In_ _Post_invalid_ PVOID Context
    )
{
    assert(Context != NULL);

    TranslucentShader *ShaderPointer = static_cast<TranslucentShader*>(Context);
    delete ShaderPointer;
}

//
// Static Variables
//

const static TRANSLUCENT_SHADER_VTABLE InteropVTable = {
    TranslucentShaderShade,
    TranslucentShaderFree
};

//
// Functions
//

TranslucentShader::TranslucentShader(
    void
    )
{
    TranslucentShader *ShaderPointer = this;

    PTRANSLUCENT_SHADER Shader = TranslucentShaderAllocate(&InteropVTable,
                                                           &ShaderPointer,
                                                           sizeof(TranslucentShader*),
                                                           sizeof(TranslucentShader*));

    if (Shader == NULL)
    {
        throw std::bad_alloc();
    }

    Data = Shader;
}

TranslucentShader::TranslucentShader(
    _In_ PTRANSLUCENT_SHADER Shader
    )
: Data(Shader)
{ }

void
TranslucentShader::Reference(
    void
    )
{
    TranslucentShaderReference(Data);
}

void 
TranslucentShader::Dereference(
    void
    )
{
    TranslucentShaderDereference(Data);
}

CTranslucentShader::CTranslucentShader(
    _In_ PTRANSLUCENT_SHADER Shader
    )
: TranslucentShader(Shader), References(1)
{ }

_Ret_
FLOAT
CTranslucentShader::Shade(
    _In_ const Point & WorldHitPoint,
    _In_ const Point & ModelHitPoint,
    _In_opt_ const void * AdditionalData
    ) const
{
    POINT3 IrisWorldHit = WorldHitPoint.AsPOINT3();
    POINT3 IrisModelHit = ModelHitPoint.AsPOINT3();

    FLOAT Alpha;

    ISTATUS Status = TranslucentShaderShade(Data,
                                            IrisWorldHit,
                                            IrisModelHit,
                                            AdditionalData,
                                            &Alpha);

    if (Status != ISTATUS_SUCCESS)
    {
        std::ostringstream error;
        error << "Iris Error: " << Status;
        throw std::runtime_error(error.str());
    }

    return Alpha;
}

IrisPointer<TranslucentShader>
CTranslucentShader::Create(
    _In_ PTRANSLUCENT_SHADER Shader
    )
{
    if (Shader == NULL)
    {
        throw std::invalid_argument("Shader");
    }

    CTranslucentShader* Allocated = new CTranslucentShader(Shader);
    return IrisPointer<TranslucentShader>(Allocated);
}

void
CTranslucentShader::Reference(
    void
    )
{
    TranslucentShader::Reference();
    References += 1;
}

void 
CTranslucentShader::Dereference(
    void
    )
{
    TranslucentShader::Dereference();
    References -= 1;

    if (References == 0)
    {
        delete this;
    }
}

} // namespace Iris