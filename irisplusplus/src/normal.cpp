/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    normal.cpp

Abstract:

    This file contains the definitions for the 
    Iris++ normal type.

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
NormalComputeNormal(
    _In_ PCVOID Context, 
    _In_ POINT3 ModelHitPoint,
    _In_ PCVOID AdditionalData,
    _Out_ PVECTOR3 SurfaceNormal
    )
{
    assert(Context != NULL);
    assert(SurfaceNormal != NULL);

    const Normal *NormalPointer = static_cast<const Normal*>(Context);

    Point IrisPlusPlusPoint(ModelHitPoint);

    *SurfaceNormal = NormalPointer->ComputeNormal(IrisPlusPlusPoint, 
                                                  AdditionalData);

    return ISTATUS_SUCCESS;
}

static
VOID
NormalFree(
    _In_ _Post_invalid_ PVOID Context
    )
{
    assert(Context != NULL);

    const Normal *NormalPointer = static_cast<const Normal*>(Context);
    delete NormalPointer;
}

//
// Static Variables
//

const static NORMAL_VTABLE InteropVTable = {
    Iris::NormalComputeNormal,
    NormalFree,
    FALSE
};

const static NORMAL_VTABLE PrenormalizedInteropVTable = {
    Iris::NormalComputeNormal,
    NormalFree,
    TRUE
};

//
// Functions
//

Normal::Normal(
    bool Prenormalized
    )
{
    PCNORMAL_VTABLE VTable;

    if (Prenormalized)
    {
        VTable = &PrenormalizedInteropVTable;
    }
    else
    {
        VTable = &InteropVTable;
    }

    Normal *NormalPointer = this;

    PNORMAL IrisNormal = NormalAllocate(VTable,
                                        &NormalPointer,
                                        sizeof(Normal*),
                                        sizeof(Normal*));

    if (IrisNormal == NULL)
    {
        throw std::bad_alloc();
    }

    Data = IrisNormal;
}

Normal::Normal(
    _In_ PNORMAL IrisNormal
    )
: Data(IrisNormal)
{ }

void
Normal::Reference(
    void
    )
{
    NormalReference(Data);
}

void 
Normal::Dereference(
    void
    )
{
    NormalDereference(Data);
}

CNormal::CNormal(
    _In_ PNORMAL IrisNormal
    )
: Normal(IrisNormal)
{ }

IrisPointer<Normal>
CNormal::Create(
    _In_ PNORMAL IrisNormal
    )
{
    CNormal* Allocated = new CNormal(IrisNormal);
    return IrisPointer<Normal>(Allocated);
}

VECTOR3
CNormal::ComputeNormal(
    _In_ const Point & ModelHitPoint,
    _In_opt_ const void *AdditionalData
    ) const
{
    POINT3 IrisModelHitPoint = ModelHitPoint.AsPOINT3();

    VECTOR3 SurfaceNormal;

    ISTATUS Status = NormalComputeNormal(Data,
                                         IrisModelHitPoint,
                                         AdditionalData,
                                         &SurfaceNormal);

    if (Status != ISTATUS_SUCCESS)
    {
        std::ostringstream error;
        error << "Iris Error: " << Status;
        throw std::runtime_error(error.str());
    }

    return SurfaceNormal;
}

BOOL
CNormal::Prenormalized(
    void
    ) const
{
    BOOL IrisPrenormalized;

    NormalPrenormalized(Data, &IrisPrenormalized);

    return IrisPrenormalized;
}

void
CNormal::Reference(
    void
    )
{
    Normal::Reference();
    References += 1;
}

void 
CNormal::Dereference(
    void
    )
{
    Normal::Dereference();
    References -= 1;

    if (References == 0)
    {
        delete this;
    }
}

} // namespace Iris