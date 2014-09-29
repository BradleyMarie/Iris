/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    scene.cpp

Abstract:

    This file contains the definitions for the 
    Iris++ scene type.

--*/

#include <irisplusplus.h>
#include <sstream>

namespace Iris {

//
// SceneTracer/SceneObject Functions
//

void
SceneObject::Release(
    void
    )
{
    SceneObjectFree(Data);
}

SceneObject::SceneObject(
    _In_ PSCENE_OBJECT IrisSceneObject
    )
: Data(IrisSceneObject)
{ }

void
SceneTracer::Trace(
    _In_ const SceneObject & SceneObjectRef
    )
{
    PCSCENE_OBJECT IrisSceneObject = SceneObjectRef.AsPCSCENE_OBJECT();

    ISTATUS Status = SceneTracerTraceGeometry(Data, IrisSceneObject);

    if (Status != ISTATUS_SUCCESS)
    {
        std::ostringstream error;
        error << "Iris Error: " << Status;
        throw std::runtime_error(error.str());
    } 
}

SceneTracer::SceneTracer(
    _Inout_ PSCENE_TRACER SceneTracer
    )
: Data(SceneTracer)
{ }

//
// Static Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
static
ISTATUS 
SceneAddObject(
    _Inout_ PVOID Context,
    _In_ PSCENE_OBJECT IrisSceneObject
    )
{
    assert(Context != NULL);
    assert(IrisSceneObject != NULL);

    SceneObject SceneObjectWrapper(IrisSceneObject);

    Scene *ScenePtr = static_cast<Scene*>(Context);
    ScenePtr->AddObject(SceneObjectWrapper);
    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
static
ISTATUS 
SceneTrace(
    _In_ PCVOID Context, 
    _In_ RAY IrisWorldRay,
    _Inout_ PSCENE_TRACER IrisSceneTracer
    )
{
    assert(Context != NULL);
    assert(IrisSceneTracer != NULL);

    SceneTracer SceneTracerWrapper(IrisSceneTracer);
    Ray WorldRay(IrisWorldRay);

    const Scene *ScenePtr = static_cast<const Scene*>(Context);
    ScenePtr->Trace(WorldRay, SceneTracerWrapper);
    return ISTATUS_SUCCESS;
}

static
VOID
SceneFree(
    _In_ _Post_invalid_ PVOID Context
    )
{
    assert(Context != NULL);

    Scene *ScenePtr = static_cast<Scene*>(Context);
    delete ScenePtr;
}

//
// Static Variables
//

const static SCENE_VTABLE InteropVTable = {
    SceneAddObject,
    SceneTrace,
    SceneFree
};

//
// Functions
//

IRISPLUSPLUSAPI
Scene::Scene(
    void
    )
{
    Scene *ScenePtr = this;

    PSCENE IrisScene = SceneAllocate(&InteropVTable,
                                     &ScenePtr,
                                     sizeof(Scene*),
                                     sizeof(Scene*));

    if (IrisScene == NULL)
    {
        throw std::bad_alloc();
    }

    Data = IrisScene;
}

void
Scene::AddObject(
    _In_ IrisPointer<Shape> ShapePtr,
    _In_opt_ Matrix ModelToWorld,
    _In_ bool Premultiplied
    )
{
    PDRAWING_SHAPE DrawingShape = ShapePtr->AsPDRAWING_SHAPE();
    PMATRIX IrisMatrix = ModelToWorld.AsPMATRIX();
    BOOL IrisPremultiplied = (Premultiplied == false) ? FALSE : TRUE;

    ISTATUS Status = SceneAddObject(Data,
                                    DrawingShape,
                                    IrisMatrix,
                                    IrisPremultiplied);

    if (Status != ISTATUS_SUCCESS)
    {
        std::ostringstream error;
        error << "Iris Error: " << Status;
        throw std::runtime_error(error.str());
    }
}

Scene::Scene(
    _In_ PSCENE IrisScene
    )
: Data(IrisScene)
{ }

void
Scene::Reference(
    void
    )
{
    SceneReference(Data);
}

void 
Scene::Dereference(
    void
    )
{
    SceneDereference(Data);
}

IrisPointer<Scene>
CScene::Create(
    _In_ PSCENE IrisScene
    )
{
    if (IrisScene == NULL)
    {
        throw std::invalid_argument("IrisScene");
    }

    CScene* Allocated = new CScene(IrisScene);
    return IrisPointer<Scene>(Allocated);
}

void
CScene::AddObject(
    _In_ SceneObject & SceneObjectPtr
    )
{
    throw std::logic_error("This Code Should Not Be Reachable");
}

void
CScene::Trace(
    _In_ const Ray & WorldRay,
    _Inout_ SceneTracer & SceneTracerRef
    ) const
{
    throw std::logic_error("This Code Should Not Be Reachable");
}

CScene::CScene(
    _In_ PSCENE IrisScene
    )
: Scene(IrisScene), References(1)
{ }

void
CScene::Reference(
    void
    )
{
    Scene::Reference();
    References += 1;
}

void 
CScene::Dereference(
    void
    )
{
    Scene::Dereference();
    References -= 1;

    if (References == 0)
    {
        delete this;
    }
}

} // namespace Iris