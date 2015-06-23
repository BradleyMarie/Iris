/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisplusplus_refcountedptr.h

Abstract:

    This file contains the definitions for the 
    Iris++ ReferenceCountedPtr type.

--*/

#include <irisplusplus.h>

#ifndef _REF_COUNTED_POINTER_IRIS_PLUS_PLUS_
#define _REF_COUNTED_POINTER_IRIS_PLUS_PLUS_

namespace Iris {

//
// Types
//

template <typename T, void DeletionFunction(_In_opt_ _Post_invalid_ T)>
class ReferenceCountedPointer final {
public:
    ReferenceCountedPointer(
        _In_ T Pointer
        )
    : Data(Pointer), References(new std::atomic_size_t(1))
    {
        if (Pointer == NULL)
        {
            throw std::invalid_argument("Pointer");
        }
    }

    ReferenceCountedPointer(
        _In_ const ReferenceCountedPointer & ToCopy
        )
    : Data(ToCopy.Data), References(ToCopy.References)
    {
        *References += 1;
    }

    T
    GetPointer(
        void
        )
    {
        return Data;
    }

    const T
    GetPointer(
        void
        ) const
    {
        return Data;
    }

    ReferenceCountedPointer &
    operator=(
        _In_ const ReferenceCountedPointer & ToCopy
        )
    {
        if (this != &ToCopy)
        {
            *References -= 1;

            if (References == 0)
            {
                DeletionFunction(Data);
            }

            References = ToCopy.References;

            *References += 1;

            Data = ToCopy.Data;
        }

        return *this;
    }

    ~ReferenceCountedPointer(
        void
        )
    {
        *References -= 1;

        if (References == 0)
        {
            DeletionFunction(Data);
        }
    }

private:
    std::shared_ptr<std::atomic_size_t> References;
    T Data;
};

} // namespace Iris

#endif // _REF_COUNTED_POINTER_IRIS_PLUS_PLUS_