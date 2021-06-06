load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "aggregate",
    srcs = ["aggregate.c"],
    hdrs = ["aggregate.h"],
    deps = [
        "//iris_physx",
    ],
)

cc_library(
    name = "fresnel",
    srcs = ["fresnel.c"],
    hdrs = ["fresnel.h"],
    deps = [
        "//iris_physx",
    ],
)

cc_library(
    name = "lambertian",
    srcs = ["lambertian.c"],
    hdrs = ["lambertian.h"],
    deps = [
        "//iris_advanced_toolkit:sample_geometry",
        "//iris_physx",
    ],
)

cc_library(
    name = "microfacet",
    srcs = ["microfacet.c"],
    hdrs = ["microfacet.h"],
    deps = [
        "//iris_physx",
    ],
)

cc_library(
    name = "mirror",
    srcs = ["mirror.c"],
    hdrs = ["mirror.h"],
    deps = [
        "//iris_physx",
    ],
)

cc_library(
    name = "oren_nayar",
    srcs = ["oren_nayar.c"],
    hdrs = ["oren_nayar.h"],
    deps = [
        "//iris_advanced_toolkit:sample_geometry",
        "//iris_physx",
    ],
)