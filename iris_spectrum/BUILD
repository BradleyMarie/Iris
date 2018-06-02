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
    name = "spectrum_compositor",
    hdrs = ["spectrum_compositor.h"],
    deps = [
        ":reflector",
        ":reflector_internal",
        ":spectrum",
        ":spectrum_compositor_internal",
        ":spectrum_internal",
    ],
)

cc_library(
    name = "spectrum_compositor_context",
    hdrs = ["spectrum_compositor_context.h"],
    srcs = ["spectrum_compositor_context.c"],
    deps = [
        ":spectrum_compositor",
        ":spectrum_compositor_internal",
        ":spectrum_internal",
    ],
)

cc_library(
    name = "spectrum_compositor_internal",
    hdrs = ["spectrum_compositor_internal.h"],
    deps = [
        ":reflector",
        ":spectrum_internal",
        "//common:static_allocator",
    ],
)

cc_library(
    name = "spectrum_internal",
    hdrs = ["spectrum_internal.h"],
    deps = [
        ":spectrum_vtable",
    ],
)

cc_test(
    name = "spectrum_test",
    srcs = ["spectrum_test.cc"],
    deps = [
        ":spectrum",
        "@com_google_googletest//:gtest_main"
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