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
    IRISPLUSPLUSAPI
    SceneBase(
        void
        );

    virtual
    void
    Trace(
        _In_ RayTracerReference RayTracerRef
        ) const = 0;

    virtual
    ~SceneBase(
        void
        )
    { }

public:
    virtual
    void
    Add(
        _In_ const Shape & ShapeRef,
        _In_ const Matrix & ModelToWorldRef,
        _In_ bool Premultiplied
        ) = 0;
        
    _Ret_
    Scene
    AsScene(
        void
        )
    {
        return Scene(Data, true);
    }

private:
    void
    Retain(
        void
        )
    {
        SceneRetain(Data);
    }

    IRISPLUSPLUSAPI
    void 
    Release(
        void
        )
    {
        SceneRelease(Data);
    }

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

    friend class IrisPointer<SceneBase>;
    
    PSCENE Data;
};

} // namespace Iris

#endif // _SCENE_BASE_IRIS_PLUS_PLUS_