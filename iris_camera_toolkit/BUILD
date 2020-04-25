load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "grid_image_sampler",
    srcs = ["grid_image_sampler.c"],
    hdrs = ["grid_image_sampler.h"],
    deps = [
        "//iris_camera",
    ],
)

cc_library(
    name = "halton_image_sampler",
    srcs = ["halton_image_sampler.c"],
    hdrs = ["halton_image_sampler.h"],
    deps = [
        "//common:safe_math",
        "//iris_camera",
        "//third_party/gruenschloss/halton:halton_enum",
    ],
)

cc_library(
    name = "orthographic_camera",
    srcs = ["orthographic_camera.c"],
    hdrs = ["orthographic_camera.h"],
    deps = [
        "//iris_camera",
    ],
)

cc_library(
    name = "pfm_writer",
    srcs = ["pfm_writer.c"],
    hdrs = ["pfm_writer.h"],
    deps = [
        "//iris_camera",
    ],
)

cc_library(
    name = "pinhole_camera",
    srcs = ["pinhole_camera.c"],
    hdrs = ["pinhole_camera.h"],
    deps = [
        "//iris_camera",
    ],
)

cc_library(
    name = "sobol_image_sampler",
    srcs = ["sobol_image_sampler.c"],
    hdrs = ["sobol_image_sampler.h"],
    deps = [
        "//iris_camera",
        "//third_party/gruenschloss/double:sobol",
        "//third_party/pbrt-v3:sobolmatrices",
    ],
)

cc_library(
    name = "thin_lens_camera",
    srcs = ["thin_lens_camera.c"],
    hdrs = ["thin_lens_camera.h"],
    deps = [
        "//iris_camera",
    ],
)
