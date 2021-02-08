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
    name = "low_discrepancy_image_sampler",
    srcs = ["low_discrepancy_image_sampler.c"],
    hdrs = ["low_discrepancy_image_sampler.h"],
    deps = [
        "//iris_advanced_toolkit:low_discrepancy_sequence",
        "//iris_camera"
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
    name = "status_bar_progress_reporter",
    srcs = ["status_bar_progress_reporter.c"],
    hdrs = ["status_bar_progress_reporter.h"],
    deps = [
        "//iris_camera",
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
