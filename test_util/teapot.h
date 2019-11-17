/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    teapot.h

Abstract:

    A model of the Utah teapot.

--*/

#ifndef _TEST_UTIL_TEAPOT_
#define _TEST_UTIL_TEAPOT_

#include "iris/iris.h"

#if __cplusplus 
extern "C" {
#endif // __cplusplus

//
// Teapot Data
//

#define TEAPOT_VERTEX_COUNT 3240
extern const POINT3 teapot_vertices[TEAPOT_VERTEX_COUNT];

#define TEAPOT_NORMAL_COUNT 3252
extern const VECTOR3 teapot_normals[TEAPOT_NORMAL_COUNT];

#define TEAPOT_FACE_COUNT 6320
extern const size_t teapot_face_vertices[TEAPOT_FACE_COUNT][3];
extern const size_t teapot_face_normals[TEAPOT_FACE_COUNT][3];

#if __cplusplus 
}
#endif // __cplusplus

#endif // _TEST_UTIL_TEAPOT_