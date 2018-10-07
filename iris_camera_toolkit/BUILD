package(default_visibility = ["//visibility:public"])

cc_library(
    name = "grid_pixel_sampler",
    hdrs = ["grid_pixel_sampler.h"],
    srcs = ["grid_pixel_sampler.c"],
    deps = [
        "//iris_camera:iris_camera",
    ],
)

cc_library(
    name = "pcg_random_generator",
    hdrs = ["pcg_random_generator.h"],
    srcs = ["pcg_random_generator.c"],
    deps = [
        "//iris_advanced_toolkit:pcg_random",
        "//iris_camera:iris_camera",
        "//third_party/pcg_c_basic:pcg_basic",
    ],
)

cc_library(
    name = "pinhole_camera",
    hdrs = ["pinhole_camera.h"],
    srcs = ["pinhole_camera.c"],
    deps = [
        "//iris_camera:iris_camera",
    ],
)

cc_library(
    name = "thin_lens_camera",
    hdrs = ["thin_lens_camera.h"],
    srcs = ["thin_lens_camera.c"],
    deps = [
        "//iris_camera:iris_camera",
    ],
)