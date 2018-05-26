package(default_visibility = ["//visibility:private"])

cc_library(
    name = "spectrum",
    hdrs = ["spectrum.h"],
    srcs = ["spectrum.c"],
    deps = [
        ":spectrum_internal",
        "//common:alloc",
    ],
)

cc_library(
    name = "spectrum_internal",
    hdrs = ["spectrum_internal.h"],
    deps = [
        ":spectrum_vtable",
    ],
)

cc_library(
    name = "spectrum_vtable",
    hdrs = ["spectrum_vtable.h"],
    deps = [
        "//common:free_routine",
        "//common:sal",
        "//common:status",
    ],
)

cc_library(
    name = "reflector",
    hdrs = ["reflector.h"],
    srcs = ["reflector.c"],
    deps = [
        ":reflector_internal",
        "//common:alloc",
    ],
)

cc_library(
    name = "reflector_internal",
    hdrs = ["reflector_internal.h"],
    deps = [
        ":reflector_vtable",
    ],
)

cc_library(
    name = "reflector_vtable",
    hdrs = ["reflector_vtable.h"],
    deps = [
        "//common:free_routine",
        "//common:sal",
        "//common:status",
    ],
)