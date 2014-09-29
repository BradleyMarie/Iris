/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    visibilitytester.cpp

Abstract:

    This file contains the definitions for the 
    Iris++ visibility tester type.

--*/

#include <irisplusplus.h>
#include <sstream>

namespace Iris {

//
// Functions
//

VisibiltyTester::VisibiltyTester(
    _In_ IrisPointer<Scene> Scene,
    _In_ FLOAT Epsilon
    )
: FreeData(false)
{
    PSCENE IrisScene = Scene->AsPSCENE();

    ISTATUS Status = VisibilityTesterAllocate(IrisScene,
                                              Epsilon,
                                              &Data);

    switch (Status)
    {
        case ISTATUS_SUCCESS:
            break;
        case ISTATUS_ALLOCATION_FAILED:
            throw std::bad_alloc();
            break;
        case ISTATUS_INVALID_ARGUMENT_00:
            throw std::invalid_argument("Scene");
            break;
        case ISTATUS_INVALID_ARGUMENT_01:
            throw std::invalid_argument("Epsilon");
            break;
    }
}

VisibiltyTester::VisibiltyTester(
    _In_ PVISIBILITY_TESTER Tester,
    _In_ bool FreeTesterOnDestruction
    )
: FreeData(FreeTesterOnDestruction)
{ 
    if (Tester == NULL)
    {
        throw std::invalid_argument("Tester");
    }
}

_Ret_
bool
VisibiltyTester::Test(
    _In_ const Ray & WorldRay,
    _In_ FLOAT DistanceToObject
    ) const
{
    RAY IrisWorldRay;
    BOOL Visible;

    IrisWorldRay = WorldRay.AsRAY();

    ISTATUS Status = VisibilityTesterTestVisibility(Data,
                                                    IrisWorldRay,
                                                    DistanceToObject,
                                                    &Visible);

    switch (Status)
    {
        case ISTATUS_SUCCESS:
            break;
        case ISTATUS_ALLOCATION_FAILED:
            throw std::bad_alloc();
            break;
        case ISTATUS_INVALID_ARGUMENT_01:
            throw std::invalid_argument("WorldRay");
            break;
        case ISTATUS_INVALID_ARGUMENT_02:
            throw std::invalid_argument("Distance");
            break;
        default:
            ASSERT(FALSE);
    }

    return Visible != FALSE;
}

_Ret_
bool
VisibiltyTester::TestAnyDistance(
    _In_ const Ray & WorldRay
    ) const
{
    RAY IrisWorldRay;
    BOOL Visible;

    IrisWorldRay = WorldRay.AsRAY();

    ISTATUS Status = VisibilityTesterTestVisibilityAnyDistance(Data,
                                                               IrisWorldRay,
                                                               &Visible);

    switch (Status)
    {
        case ISTATUS_SUCCESS:
            break;
        case ISTATUS_ALLOCATION_FAILED:
            throw std::bad_alloc();
            break;
        case ISTATUS_INVALID_ARGUMENT_01:
            throw std::invalid_argument("WorldRay");
            break;
        default:
            ASSERT(FALSE);
    }

    return Visible != FALSE;
}

VisibiltyTester::~VisibiltyTester(
    VOID
    )
{
    if (FreeData)
    {
        VisibilityTesterFree(Data);
    }
}

IRISPLUSPLUSAPI
std::shared_ptr<VisibiltyTester>
VisibiltyTester::Create(
    _In_ IrisPointer<Scene> Scene,
    _In_ FLOAT Epsilon
    )
{
    VisibiltyTester * Tester = new VisibiltyTester(Scene, Epsilon);
    return std::shared_ptr<VisibiltyTester>(Tester);
}

} // namespace Iris