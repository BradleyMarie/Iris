/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisplusplus_scene.h

Abstract:

    This file contains the definitions for the 
    Iris++ scene type.

--*/

#include <irisplusplus.h>

#ifndef _SCENE_IRIS_PLUS_PLUS_
#define _SCENE_IRIS_PLUS_PLUS_

namespace Iris {

//
// Types
//

class RayTracer final {
private:
    Scene(
        _In_ PSCENE ScenePtr
        )
    : Data(ScenePtr)
    { }
    
public:
    _Ret_
    PSCENE
    AsPSCENE(
        void
        )
    {
        return Data;
    }

    _Ret_
    PCSCENE
    AsPCSCENE(
        void
        ) const
    {
        return Data;
    }

    virtual
    void
    Trace(
        _In_ const RayTracer & RayTracerRef
        ) const = 0;

    virtual
    ~Scene(
        void
        )
    { }

private:
    PSCENE Data;

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

    friend class IrisPointer<Scene>;
    friend class CScene;
};

} // namespace Iris

#endif // _SCENE_IRIS_PLUS_PLUS_