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
    name = "grid_pixel_sampler_generator",
    hdrs = ["grid_pixel_sampler_generator.h"],
    srcs = ["grid_pixel_sampler_generator.c"],
    deps = [
        ":grid_pixel_sampler",
        "//iris_camera:iris_camera",
    ],
)

cc_library(
    name = "orthographic_camera",
    hdrs = ["orthographic_camera.h"],
    srcs = ["orthographic_camera.c"],
    deps = [
        "//iris_camera:iris_camera",
    ],
)

cc_library(
    name = "pfm_writer",
    hdrs = ["pfm_writer.h"],
    srcs = ["pfm_writer.c"],
    deps = [
        "//iris_camera:iris_camera",
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