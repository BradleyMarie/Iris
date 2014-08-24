/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisplusplus_normal.h

Abstract:

    This file contains the definitions for the 
    Iris++ normal type.

--*/

#include <irisplusplus.h>

#ifndef _NORMAL_IRIS_PLUS_PLUS_
#define _NORMAL_IRIS_PLUS_PLUS_

namespace Iris {

//
// Types
//

class Normal {
protected:
    IRISPLUSPLUSAPI
    Normal(
        bool Prenormalized
        );

public:
    _Ret_
    PNORMAL
    AsPNORMAL(
        void
        ) const
    {
        return Data;
    }

    virtual
    VECTOR3
    ComputeNormal(
        _In_ const Point & ModelHitPoint,
        _In_opt_ const void *AdditionalData
        ) const = 0;

    virtual
    BOOL
    Prenormalized(
        void
        ) const = 0;

    virtual
    ~Normal(
        void
        )
    { }

private:
    PNORMAL Data;

    IRISPLUSPLUSAPI
    Normal(
        _In_ PNORMAL IrisNormal
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

    friend class IrisPointer<Normal>;
    friend class CNormal;
};

class CNormal final : public Normal {
public:
    IRISPLUSPLUSAPI
    static
    IrisPointer<Normal>
    Create(
        _In_ PNORMAL IrisNormal
        );

    IRISPLUSPLUSAPI
    virtual
    VECTOR3
    ComputeNormal(
        _In_ const Point & ModelHitPoint,
        _In_opt_ const void *AdditionalData
        ) const;

    IRISPLUSPLUSAPI
    virtual
    BOOL
    Prenormalized(
        void
        ) const;

private:
    IRISPLUSPLUSAPI
    CNormal(
        _In_ PNORMAL IrisNormal
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

    size_t References;
};

} // namespace Iris

#endif // _NORMAL_IRIS_PLUS_PLUS_