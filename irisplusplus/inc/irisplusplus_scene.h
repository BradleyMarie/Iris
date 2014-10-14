/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisplusplus_scene.h

Abstract:

    This file contains the prototypes for the 
    Iris++ scene type.

--*/

#include <irisplusplus.h>

#ifndef _SCENE_IRIS_PLUS_PLUS_
#define _SCENE_IRIS_PLUS_PLUS_

namespace Iris {

//
// Types
//

class SceneObject final {
public:
    PCSCENE_OBJECT
    AsPCSCENE_OBJECT(
        void
        ) const
    {
        return Data.GetPointer();
    }

    PSCENE_OBJECT
    AsPSCENE_OBJECT(
        void
        )
    {
        return Data.GetPointer();
    }

private:
    SceneObject(
        _In_ PSCENE_OBJECT IrisSceneObject
        );

    friend 
    _Check_return_
    _Success_(return == ISTATUS_SUCCESS)
    ISTATUS 
    SceneAddObject(
        _Inout_ PVOID Context,
        _In_ PSCENE_OBJECT SceneObject
        );

    ReferenceCountedPointer<PSCENE_OBJECT, SceneObjectFree> Data;
};

class SceneTracer final {
    void
    Trace(
        _In_ const SceneObject & SceneObjectRef
        );

private:
    SceneTracer(
        _Inout_ PSCENE_TRACER SceneTracer
        );

    friend 
    _Check_return_
    _Success_(return == ISTATUS_SUCCESS)
    ISTATUS 
    SceneTrace(
        _In_ PCVOID Context, 
        _In_ RAY IrisWorldRay,
        _Inout_ PSCENE_TRACER IrisSceneTracer
        );

    PSCENE_TRACER Data;
};

class Scene {
protected:
    IRISPLUSPLUSAPI
    Scene(
        void
        );

public:
    _Ret_
    PSCENE
    AsPSCENE(
        void
        ) const
    {
        return Data;
    }

    void
    AddObject(
        _In_ IrisPointer<Shape> ShapePtr,
        _In_opt_ Matrix ModelToWorld,
        _In_ bool Premultiplied
        );

    virtual
    ~Scene(
        void
        )
    { }

protected:
    virtual
    void
    AddObject(
        _In_ SceneObject & SceneObjectPtr
        ) = 0;

    virtual
    void
    Trace(
        _In_ const Ray & WorldRay,
        _Inout_ SceneTracer & SceneTracerRef
        ) const = 0;

private:
    PSCENE Data;

    IRISPLUSPLUSAPI
    Scene(
        _In_ PSCENE IrisScene
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

    friend 
    _Check_return_
    _Success_(return == ISTATUS_SUCCESS)
    ISTATUS 
    SceneAddObject(
        _Inout_ PVOID Context,
        _In_ PSCENE_OBJECT SceneObject
        );

    friend
    _Check_return_
    _Success_(return == ISTATUS_SUCCESS)
    ISTATUS 
    SceneTrace(
        _In_ PCVOID Context, 
        _In_ RAY IrisWorldRay,
        _Inout_ PSCENE_TRACER IrisSceneTracer
        );

    friend class IrisPointer<Scene>;
    friend class CScene;
};

class CScene final : public Scene {
public:
    IRISPLUSPLUSAPI
    static
    IrisPointer<Scene>
    Create(
        _In_ PSCENE IrisScene
        );

protected:
    IRISPLUSPLUSAPI
    virtual
    void
    AddObject(
        _In_ SceneObject & SceneObjectPtr
        );

    IRISPLUSPLUSAPI
    virtual
    void
    Trace(
        _In_ const Ray & WorldRay,
        _Inout_ SceneTracer & SceneTracerRef
        ) const;

private:
    CScene(
        _In_ PSCENE Scene
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

    std::atomic_size_t References;
};

} // namespace Iris

#endif // _SCENE_IRIS_PLUS_PLUS_