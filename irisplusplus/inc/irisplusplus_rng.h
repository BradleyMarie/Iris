/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisplusplus_rng.h

Abstract:

    This file contains the definitions for the 
    Iris++ random number generator type.

--*/

#include <irisplusplus.h>

#ifndef _RANDOM_IRIS_PLUS_PLUS_
#define _RANDOM_IRIS_PLUS_PLUS_

namespace Iris {

//
// Types
//

class Random {
protected:
    IRISPLUSPLUSAPI
    Random(
        void
        );

public:
    _Ret_
    PRANDOM
    AsPRANDOM(
        void
        ) const
    {
        return Data;
    }

    _Ret_range_(Minimum, Maximum)
    virtual
    FLOAT
    GenerateFloat(
        _In_ FLOAT Minimum,
        _In_ FLOAT Maximum
        ) const = 0;

    _Ret_range_(Minimum, Maximum)
    virtual
    SIZE_T
    GenerateIndex(
        _In_ SIZE_T Minimum,
        _In_ SIZE_T Maximum
        ) const = 0;

    virtual
    ~Random(
        void
        )
    { }

private:
    PRANDOM Data;

    IRISPLUSPLUSAPI
    Random(
        _In_ PRANDOM Rng
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

    friend class IrisPointer<Random>;
    friend class CRandom;
};

class CRandom final : public Random {
public:
    IRISPLUSPLUSAPI
    static
    IrisPointer<Random>
    Create(
        _In_ PRANDOM Rng
        );

    _Ret_range_(Minimum, Maximum)
    IRISPLUSPLUSAPI
    virtual
    FLOAT
    GenerateFloat(
        _In_ FLOAT Minimum,
        _In_ FLOAT Maximum
        ) const;

    _Ret_range_(Minimum, Maximum)
    IRISPLUSPLUSAPI
    virtual
    SIZE_T
    GenerateIndex(
        _In_ SIZE_T Minimum,
        _In_ SIZE_T Maximum
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
    CRandom(
        _In_ PRANDOM Rng
        );

    std::atomic_size_t References;
};

} // namespace Iris

#endif // _RANDOM_IRIS_PLUS_PLUS_