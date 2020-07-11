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
    name = "pcg_random",
    srcs = ["pcg_random.c"],
    hdrs = ["pcg_random.h"],
    deps = [
        "//iris_advanced",
        "@pcg_c_basic//:pcg_basic",
    ],
)
