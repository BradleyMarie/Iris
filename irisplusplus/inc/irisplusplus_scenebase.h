/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisplusplus_scenebase.h

Abstract:

    This file contains the definitions for the 
    Iris++ SceneBase type.

--*/

#include <irisplusplus.h>

#ifndef _SCENE_BASE_IRIS_PLUS_PLUS_
#define _SCENE_BASE_IRIS_PLUS_PLUS_

namespace Iris {

//
// Types
//

class SceneBase {
protected:
    virtual
    void
    Trace(
        _In_ RayTracerReference RayTracerRef
        ) const = 0;

    IRISPLUSPLUSAPI
    static
    Scene
    Create(
        _In_ std::unique_ptr<SceneBase> SceneBasePtr
        );

public:
    virtual
    ~SceneBase(
        void
        )
    { }
    
private:        
    _Check_return_
    _Success_(return == ISTATUS_SUCCESS)
    static
    ISTATUS 
    Trace(
        _In_ PCVOID Context, 
        _Inout_ PRAYTRACER_REFERENCE RayTracerReference
        );
    
    static
    VOID 
    Free(
        _In_ _Post_invalid_ PVOID Context
        );
    
    const static SCENE_VTABLE InteropVTable;
};

} // namespace Iris

#endif // _SCENE_BASE_IRIS_PLUS_PLUS_