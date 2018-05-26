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