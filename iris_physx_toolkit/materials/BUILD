load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "alpha",
    srcs = ["alpha.c"],
    hdrs = ["alpha.h"],
    deps = [
        "//iris_physx",
        "//iris_physx_toolkit:float_texture",
        "//iris_physx_toolkit/bsdfs:alpha",
    ],
)

cc_library(
    name = "constant",
    srcs = ["constant.c"],
    hdrs = ["constant.h"],
    deps = [
        "//iris_physx",
    ],
)

cc_library(
    name = "matte",
    srcs = ["matte.c"],
    hdrs = ["matte.h"],
    deps = [
        "//iris_physx_toolkit:float_texture",
        "//iris_physx_toolkit:reflector_texture",
        "//iris_physx_toolkit/bsdfs:lambertian",
        "//iris_physx_toolkit/bsdfs:oren_nayar",
    ],
)

cc_library(
    name = "mirror",
    srcs = ["mirror.c"],
    hdrs = ["mirror.h"],
    deps = [
        "//iris_physx",
        "//iris_physx_toolkit:reflector_texture",
        "//iris_physx_toolkit/bsdfs:mirror",
    ],
)

cc_library(
    name = "plastic",
    srcs = ["plastic.c"],
    hdrs = ["plastic.h"],
    deps = [
        "//iris_physx",
        "//iris_physx_toolkit:float_texture",
        "//iris_physx_toolkit:reflector_texture",
        "//iris_physx_toolkit/bsdfs:aggregate",
        "//iris_physx_toolkit/bsdfs:lambertian",
        "//iris_physx_toolkit/bsdfs:microfacet",
        "//iris_physx_toolkit/bsdfs/fresnels:dielectric",
        "//iris_physx_toolkit/bsdfs/microfacet_distributions:trowbridge_reitz",
    ],
)