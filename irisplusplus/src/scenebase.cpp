/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    scenebase.cpp

Abstract:

    This file contains the definitions for the 
    Iris++ SceneBase type.

--*/

#include <irisplusplusp.h>

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
    assert(Context != nullptr);
    assert(RayTracerRef != nullptr);

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
    assert(Context != nullptr);

    const SceneBase **SceneBasePtr = (const SceneBase**) Context;
    delete *SceneBasePtr;
}

Scene
SceneBase::Create(
    _In_ std::unique_ptr<SceneBase> SceneBasePtr
    )
{
    if (!SceneBasePtr)
    {
        throw std::invalid_argument("SceneBasePtr");
    }
    
    SceneBase *UnmanagedSceneBasePtr = SceneBasePtr.release();
    PSCENE ScenePtr;

    ISTATUS Success = SceneAllocate(&InteropVTable,
                                    &UnmanagedSceneBasePtr,
                                    sizeof(SceneBase*),
                                    alignof(SceneBase*),
                                    &ScenePtr);

    if (Success != ISTATUS_SUCCESS)
    {
        throw std::bad_alloc();
    }
    
    return Scene(ScenePtr, false);
}

//
// Static Variables
//

const SCENE_VTABLE SceneBase::InteropVTable = {
    SceneBase::Trace, SceneBase::Free
};

} // namespace Iris