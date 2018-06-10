package(default_visibility = ["//visibility:private"])

cc_library(
    name = "iris_spectrum",
    hdrs = ["iris_spectrum.h"],
    deps = [
        ":reflector",
        ":reflector_allocator_context",
        ":spectrum",
        ":spectrum_compositor_context",
    ],
    visibility = ["//visibility:public"],
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
    name = "reflector_allocator",
    hdrs = ["reflector_allocator.h"],
    srcs = ["reflector_allocator.c"],
    deps = [
        ":reflector",
        ":reflector_allocator_internal",
        "//common:dynamic_allocator",
    ],
)

cc_library(
    name = "reflector_allocator_context",
    hdrs = ["reflector_allocator_context.h"],
    srcs = ["reflector_allocator_context.c"],
    deps = [
        ":reflector_allocator",
        ":reflector_allocator_internal",
    ],
)

cc_library(
    name = "reflector_allocator_internal",
    hdrs = ["reflector_allocator_internal.h"],
    deps = [
        ":reflector_internal",
        "//common:dynamic_allocator",
    ],
)

cc_library(
    name = "reflector_internal",
    hdrs = ["reflector_internal.h"],
    deps = [
        ":reflector_vtable",
    ],
)

cc_test(
    name = "reflector_test",
    srcs = ["reflector_test.cc"],
    deps = [
        ":reflector",
        "@com_google_googletest//:gtest_main"
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
    srcs = ["spectrum_compositor.c"],
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

cc_test(
    name = "spectrum_compositor_context_test",
    srcs = ["spectrum_compositor_context_test.cc"],
    deps = [
        ":spectrum_compositor_context",
        "@com_google_googletest//:gtest_main"
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

cc_test(
    name = "spectrum_compositor_test",
    srcs = ["spectrum_compositor_test.cc"],
    deps = [
        ":spectrum_compositor",
        ":test_util",
        "@com_google_googletest//:gtest_main"
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
    name = "test_util",
    hdrs = ["test_util.h"],
    srcs = ["test_util.c"],
    testonly = 1,
    deps = [
        ":spectrum_compositor",
        ":spectrum_compositor_internal",
    ],
)