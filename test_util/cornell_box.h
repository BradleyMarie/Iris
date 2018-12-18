/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    cornell_box.h

Abstract:

    The parameters of the Cornell box scene transformed to be left-handed.

--*/

#ifndef _TEST_UTIL_CORNELL_BOX_
#define _TEST_UTIL_CORNELL_BOX_

#include "iris/iris.h"

#if __cplusplus 
extern "C" {
#endif // __cplusplus

//
// Camera Data
//

extern const POINT3 cornell_box_camera_location;
extern const VECTOR3 cornell_box_camera_up;
extern const VECTOR3 cornell_box_camera_direction;
extern float_t cornell_box_focal_length;
extern float_t cornell_box_camera_width;
extern float_t cornell_box_camera_height;

//
// Scene Data
//

extern const POINT3 cornell_box_left_wall[4];
extern const POINT3 cornell_box_right_wall[4];
extern const POINT3 cornell_box_back_wall[4];
extern const POINT3 cornell_box_ceiling[4];
extern const POINT3 cornell_box_floor[4];
extern const POINT3 cornell_box_light[4];
extern const POINT3 cornell_box_short_box[5][4];
extern const POINT3 cornell_box_tall_box[5][4];

#if __cplusplus 
}
#endif // __cplusplus

#endif // _TEST_UTIL_CORNELL_BOX_