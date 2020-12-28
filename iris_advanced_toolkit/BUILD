load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "color_io",
    srcs = ["color_io.c"],
    hdrs = ["color_io.h"],
    deps = [
        "//iris_advanced",
    ],
)

cc_library(
    name = "lanczos_upscale",
    srcs = ["lanczos_upscale.c"],
    hdrs = ["lanczos_upscale.h"],
    deps = [
        "//iris_advanced",
    ],
)

cc_library(
    name = "pcg_random",
    srcs = ["pcg_random.c"],
    hdrs = ["pcg_random.h"],
    deps = [
        "//iris_advanced",
        "@pcg_c_basic//:pcg_basic",
    ],
)

cc_library(
    name = "sample_geometry",
    srcs = ["sample_geometry.c"],
    hdrs = ["sample_geometry.h"],
    deps = [
        "//iris_advanced",
    ],
)