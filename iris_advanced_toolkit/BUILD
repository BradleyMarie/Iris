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
    name = "halton_sequence",
    srcs = ["halton_sequence.c"],
    hdrs = ["halton_sequence.h"],
    deps = [
        ":low_discrepancy_sequence",
        "//third_party/gruenschloss/halton:halton_enum",
        "//third_party/gruenschloss/halton:halton_sampler",
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
    name = "low_discrepancy_sequence_vtable",
    hdrs = ["low_discrepancy_sequence_vtable.h"],
    deps = [
        "//iris_advanced",
    ],
)

cc_library(
    name = "low_discrepancy_sequence",
    srcs = ["low_discrepancy_sequence.c"],
    hdrs = ["low_discrepancy_sequence.h"],
    deps = [
        ":low_discrepancy_sequence_vtable",
        "//common:alloc",
        "//common:safe_math",
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

cc_library(
    name = "sobol_sequence",
    srcs = ["sobol_sequence.c"],
    hdrs = ["sobol_sequence.h"],
    deps = [
        ":low_discrepancy_sequence",
        "//third_party/gruenschloss/double:sobol",
        "//third_party/gruenschloss/single:sobol",
        "//third_party/pbrt-v3:sobolmatrices",
    ],
)