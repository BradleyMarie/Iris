/*++

Copyright (c) 2016 Brad Weinberger

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

class Scene final {
public:
    Scene(
        _In_ PSCENE ScenePtr,
        _In_ bool Retain
        )
    : Data(ScenePtr)
    { 
        if (Retain)
        {
            SceneRetain(ScenePtr);
        }
    }
    
    Scene(
        _In_ const Scene & ToCopy
        )
    : Data(ToCopy.Data)
    { 
        SceneRetain(Data);
    }
    
    Scene(
        _In_ Scene && ToMove
        )
    : Data(ToMove.Data)
    { 
        ToMove.Data = nullptr;
    }
    
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

    IRISPLUSPLUSAPI
    Scene & 
    operator=(
        _In_ const Scene & ToCopy
        );

    ~Scene(
        void
        )
    { 
        SceneRelease(Data);
    }

private:
    PSCENE Data;
};

} // namespace Iris

#endif // _SCENE_IRIS_PLUS_PLUS_