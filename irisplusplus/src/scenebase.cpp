/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    scenebase.cpp

Abstract:

    This file contains the definitions for the 
    Iris++ SceneBase type.

--*/

#include <irisplusplus.h>

namespace Iris {

//
// Static Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS 
SceneBase::Trace(
    _In_ PCVOID Context, 
    _Inout_ PRAYTRACER_REFERENCE RayTracerRef
    )
{
    assert(Context != NULL);
    assert(RayTracerRef != NULL);

    RayTracerReference Tracer(RayTracerRef);

    const SceneBase **SceneBasePtr = (const SceneBase**) Context;
    (*SceneBasePtr)->Trace(Tracer);
    return ISTATUS_SUCCESS;
}

VOID 
SceneBase::Free(
    _In_ _Post_invalid_ PVOID Context
    )
{
    assert(Context != NULL);

    const SceneBase **SceneBasePtr = (const SceneBase**) Context;
    delete *SceneBasePtr;
}

//
// Static Variables
//

const SCENE_VTABLE SceneBase::InteropVTable = {
    SceneBase::Trace, SceneBase::Free
};

//
// Protected Functions
//

SceneBase::SceneBase(
    void
    )
{
    SceneBase *SceneBasePtr = this;

    ISTATUS Success = SceneAllocate(&InteropVTable,
                                    &SceneBasePtr,
                                    sizeof(SceneBase*),
                                    alignof(SceneBase*),
                                    &Data);

    if (Success != ISTATUS_SUCCESS)
    {
        throw std::bad_alloc();
    }
}

} // namespace Iris