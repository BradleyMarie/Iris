load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

package(default_visibility = ["//visibility:private"])

cc_library(
    name = "full_hit_context",
    hdrs = ["full_hit_context.h"],
    deps = [
        ":hit",
        ":hit_context",
        ":matrix",
        ":point",
        "//common:dynamic_allocator",
    ],
)

cc_library(
    name = "hit",
    hdrs = ["hit.h"],
)

cc_library(
    name = "hit_allocator",
    srcs = ["hit_allocator.c"],
    hdrs = ["hit_allocator.h"],
    deps = [
        ":full_hit_context",
        ":hit",
        ":hit_allocator_internal",
        ":point",
        "//common:sal",
        "//common:status",
    ],
)

cc_test(
    name = "hit_allocator_test",
    srcs = ["hit_allocator_test.cc"],
    deps = [
        ":hit_allocator",
        "//test_util:equality",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "hit_context",
    hdrs = ["hit_context.h"],
    deps = [
        "//common:sal",
    ],
)

cc_library(
    name = "hit_tester",
    srcs = ["hit_tester.c"],
    hdrs = ["hit_tester.h"],
    deps = [
        ":hit_allocator",
        ":hit_allocator_internal",
        ":hit_tester_internal",
        ":matrix",
        ":multiply_internal",
        ":ray",
    ],
)

cc_library(
    name = "hit_tester_internal",
    hdrs = ["hit_tester_internal.h"],
    deps = [
        ":full_hit_context",
        ":hit_allocator_internal",
    ],
)

cc_test(
    name = "hit_tester_test",
    srcs = ["hit_tester_test.cc"],
    deps = [
        ":hit_tester",
        ":hit_tester_internal",
        ":multiply",
        "//test_util:equality",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "hit_allocator_internal",
    hdrs = ["hit_allocator_internal.h"],
    deps = [
        ":ray",
        "//common:dynamic_allocator",
    ],
)

cc_library(
    name = "iris",
    hdrs = ["iris.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":hit",
        ":hit_allocator",
        ":hit_context",
        ":hit_tester",
        ":matrix",
        ":multiply",
        ":point",
        ":ray",
        ":ray_tracer",
        ":vector",
    ],
)

cc_library(
    name = "matrix",
    srcs = ["matrix.c"],
    hdrs = ["matrix.h"],
    deps = [
        ":matrix_internal",
        ":vector",
        "//common:status",
    ],
)

cc_library(
    name = "matrix_internal",
    hdrs = ["matrix_internal.h"],
)

cc_test(
    name = "matrix_test",
    srcs = ["matrix_test.cc"],
    deps = [
        ":matrix",
        "//test_util:equality",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "multiply",
    srcs = ["multiply.c"],
    hdrs = ["multiply.h"],
    deps = [
        ":matrix",
        ":multiply_internal",
        ":ray",
    ],
)

cc_library(
    name = "multiply_internal",
    hdrs = ["multiply_internal.h"],
    deps = [
        ":matrix_internal",
        ":ray",
    ],
)

cc_test(
    name = "multiply_internal_test",
    srcs = ["multiply_internal_test.cc"],
    deps = [
        ":matrix",
        ":multiply_internal",
        "//test_util:equality",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "multiply_test",
    srcs = ["multiply_test.cc"],
    deps = [
        ":multiply",
        "//test_util:equality",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "point",
    hdrs = ["point.h"],
    deps = [
        ":vector",
    ],
)

cc_test(
    name = "point_test",
    srcs = ["point_test.cc"],
    deps = [
        ":point",
        "//test_util:equality",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "ray",
    hdrs = ["ray.h"],
    deps = [
        ":point",
        ":vector",
    ],
)

cc_test(
    name = "ray_test",
    srcs = ["ray_test.cc"],
    deps = [
        ":ray",
        "//test_util:equality",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "ray_tracer",
    srcs = ["ray_tracer.c"],
    hdrs = ["ray_tracer.h"],
    deps = [
        ":hit_context",
        ":hit_tester",
        ":hit_tester_internal",
        ":multiply_internal",
        ":ray",
    ],
)

cc_test(
    name = "ray_tracer_test",
    srcs = ["ray_tracer_test.cc"],
    deps = [
        ":multiply",
        ":ray_tracer",
        "//test_util:equality",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "vector",
    hdrs = ["vector.h"],
    deps = [
        "//common:sal",
    ],
)

cc_test(
    name = "vector_test",
    srcs = ["vector_test.cc"],
    deps = [
        ":vector",
        "//test_util:equality",
        "@com_google_googletest//:gtest_main",
    ],
)
