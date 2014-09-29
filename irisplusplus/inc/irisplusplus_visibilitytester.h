/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisplusplus_visibilitytester.h

Abstract:

    This file contains the definitions for the 
    Iris++ visibility tester type.

--*/

#include <irisplusplus.h>

#ifndef _VISIBILITY_TESTER_IRIS_PLUS_PLUS_
#define _VISIBILITY_TESTER_IRIS_PLUS_PLUS_

namespace Iris {

//
// Types
//

class VisibiltyTester final {
public:
    IRISPLUSPLUSAPI
    static
    std::shared_ptr<VisibiltyTester>
    Create(
        _In_ IrisPointer<Scene> Scene,
        _In_ FLOAT Epsilon
        );

    _Ret_
    IRISPLUSPLUSAPI
    bool
    Test(
        _In_ const Ray & WorldRay,
        _In_ FLOAT DistanceToObject
        ) const;

    _Ret_
    IRISPLUSPLUSAPI
    bool
    TestAnyDistance(
        _In_ const Ray & WorldRay
        ) const;

    IRISPLUSPLUSAPI
    ~VisibiltyTester(
        VOID
        );

    _Ret_
    PVISIBILITY_TESTER
    AsPVISIBILITY_TESTER(
        VOID
        ) const
    {
        return Data;
    }

private:
    IRISPLUSPLUSAPI
    VisibiltyTester(
        _In_ IrisPointer<Scene> Scene,
        _In_ FLOAT Epsilon
        );

    IRISPLUSPLUSAPI
    VisibiltyTester(
        _In_ PVISIBILITY_TESTER Tester,
        _In_ bool FreeTesterOnDestruction
        );

    //
    // Banned APIs
    //

    _Ret_
    VisibiltyTester &
    operator=(
        _In_ const VisibiltyTester & Tester
        );

    _Ret_
    VisibiltyTester &
    operator=(
        _In_ const VisibiltyTester && Tester
        );

    VisibiltyTester(
        _In_ const VisibiltyTester & Tester 
        );

    VisibiltyTester(
        _Inout_ VisibiltyTester && Tester
        );

    //
    // Data
    //

    PVISIBILITY_TESTER Data;
    bool FreeData;
};

} // namespace Iris

#endif // _VISIBILITY_TESTER_IRIS_PLUS_PLUS_