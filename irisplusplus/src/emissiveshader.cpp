/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    emissiveshader.cpp

Abstract:

    This file contains the definitions for the 
    Iris++ emissive shader type.

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
EmissiveShaderShade(
    _In_ PCVOID Context,
    _In_ POINT3 WorldHitPoint,
    _In_ POINT3 ModelHitPoint,
    _In_opt_ PCVOID AdditionalData,
    _Out_ PCOLOR3 Emissive
    )
{
    assert(Context != NULL);
    assert(Emissive != NULL);

    Point WorldHit(WorldHitPoint);
    Point ModelHit(ModelHitPoint);

    const EmissiveShader *ShaderPointer = static_cast<const EmissiveShader*>(Context);
    *Emissive = ShaderPointer->Shade(WorldHit, ModelHit, AdditionalData);
    return ISTATUS_SUCCESS;
}

static
VOID
EmissiveShaderFree(
    _In_ _Post_invalid_ PVOID Context
    )
{
    assert(Context != NULL);

    EmissiveShader *ShaderPointer = static_cast<EmissiveShader*>(Context);
    delete ShaderPointer;
}

//
// Static Variables
//

const static EMISSIVE_SHADER_VTABLE InteropVTable = {
    EmissiveShaderShade,
    EmissiveShaderFree
};

//
// Functions
//

EmissiveShader::EmissiveShader(
    void
    )
{
    EmissiveShader *ShaderPointer = this;

    PEMISSIVE_SHADER Shader = EmissiveShaderAllocate(&InteropVTable,
                                                     &ShaderPointer,
                                                     sizeof(EmissiveShader*),
                                                     sizeof(EmissiveShader*));

    if (Shader == NULL)
    {
        throw std::bad_alloc();
    }

    Data = Shader;
}

EmissiveShader::EmissiveShader(
    _In_ PEMISSIVE_SHADER Shader
    )
: Data(Shader)
{ }

void
EmissiveShader::Reference(
    void
    )
{
    EmissiveShaderReference(Data);
}

void 
EmissiveShader::Dereference(
    void
    )
{
    EmissiveShaderDereference(Data);
}

CEmissiveShader::CEmissiveShader(
    _In_ PEMISSIVE_SHADER Shader
    )
: EmissiveShader(Shader), References(1)
{ }

_Ret_
COLOR3
CEmissiveShader::Shade(
    _In_ const Point & WorldHitPoint,
    _In_ const Point & ModelHitPoint,
    _In_opt_ const void * AdditionalData
    ) const
{
    POINT3 IrisWorldHit = WorldHitPoint.AsPOINT3();
    POINT3 IrisModelHit = ModelHitPoint.AsPOINT3();

    COLOR3 Emissive;

    ISTATUS Status = EmissiveShaderShade(Data,
                                         IrisWorldHit,
                                         IrisModelHit,
                                         AdditionalData,
                                         &Emissive);

    if (Status != ISTATUS_SUCCESS)
    {
        std::ostringstream error;
        error << "Iris Error: " << Status;
        throw std::runtime_error(error.str());
    }

    return Emissive;
}

IrisPointer<EmissiveShader>
CEmissiveShader::Create(
    _In_ PEMISSIVE_SHADER Shader
    )
{
    if (Shader == NULL)
    {
        throw std::invalid_argument("Shader");
    }

    CEmissiveShader* Allocated = new CEmissiveShader(Shader);
    return IrisPointer<EmissiveShader>(Allocated);
}

void
CEmissiveShader::Reference(
    void
    )
{
    EmissiveShader::Reference();
    References += 1;
}

void 
CEmissiveShader::Dereference(
    void
    )
{
    EmissiveShader::Dereference();
    References -= 1;

    if (References == 0)
    {
        delete this;
    }
}

} // namespace Iris