/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisplusplus_irispointer.h

Abstract:

    This file contains the definitions for the 
    Iris++ iris pointer type.

--*/

#include <irisplusplus.h>

#ifndef _IRISPOINTER_IRIS_PLUS_PLUS_
#define _IRISPOINTER_IRIS_PLUS_PLUS_

namespace Iris {

//
// Types
//

template <typename T>
class IrisPointer final
{
public:
    IrisPointer(
        _In_ T* Pointer
        )
    : Data(Pointer)
    {
        if (Pointer == nullptr)
        {
            throw std::invalid_argument("Pointer");
        }
    }

    IrisPointer(
        _In_ const IrisPointer & ToCopy
        )
    : Data(ToCopy.Data)
    {
        Data->Retain();
    }

    T &
    operator*(
        void
        ) const
    {
        return *Data;
    }

    T *
    operator->(
        void
        ) const
    {
        return Data;
    }

    IrisPointer & 
    operator=(
        _In_ const IrisPointer & ToCopy
        )
    {
        if (this != &ToCopy)
        {
            Data->Release();
            Data = ToCopy.Data;
            Data->Retain();
        }

        return *this;
    }

    ~IrisPointer(
        void
        )
    {
        Data->Release();
    }

private:
    T* Data;
};

} // namespace Iris

#endif // _IRISPOINTER_IRIS_PLUS_PLUS_