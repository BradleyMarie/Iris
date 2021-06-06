load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//iris_physx_toolkit:__subpackages__"])

cc_library(
    name = "microfacet_distribution",
    hdrs = ["microfacet_distribution.h"],
    deps = [
        "//iris_advanced",
    ],
)

cc_library(
    name = "trowbridge_reitz",
    srcs = ["trowbridge_reitz.c"],
    hdrs = ["trowbridge_reitz.h"],
    deps = [
        ":microfacet_distribution"
    ],
)