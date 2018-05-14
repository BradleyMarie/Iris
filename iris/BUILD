package(default_visibility = ["//visibility:private"])

cc_library(
    name = "full_hit_context",
    hdrs = ["full_hit_context.h"],
    deps = [
        ":hit",
        ":hit_context",
        ":shared_hit_context",
    ],
)

cc_library(
    name = "hit",
    hdrs = ["hit.h"],
)

cc_library(
    name = "hit_allocator",
    hdrs = ["hit_allocator.h"],
    srcs = ["hit_allocator.c"],
    deps = [
        ":full_hit_context",
        ":hit",
        ":hit_allocator_internal",
        ":point",
        ":sal",
        ":status",
    ],
)

cc_test(
    name = "hit_allocator_test",
    srcs = ["hit_allocator_test.cc"],
    deps = [
        ":hit_allocator",
        "//test:test_util",
        "@com_google_googletest//:gtest_main"
    ],
)

cc_library(
    name = "hit_context",
    hdrs = ["hit_context.h"],
    deps = [
        ":sal",
    ],
)

cc_library(
    name = "hit_tester",
    hdrs = ["hit_tester.h"],
    srcs = ["hit_tester.c"],
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
        ":shared_hit_context_allocator",
        "//common:const_pointer_list"
    ],
)

cc_test(
    name = "hit_tester_test",
    srcs = ["hit_tester_test.cc"],
    deps = [
        ":hit_tester",
        ":hit_tester_internal",
        ":multiply",
        "//test:test_util",
        "@com_google_googletest//:gtest_main"
    ],
)

cc_library(
    name = "hit_allocator_internal",
    hdrs = ["hit_allocator_internal.h"],
    deps = [
        "//common:dynamic_allocator",
    ],
)

cc_library(
    name = "iris",
    hdrs = ["iris.h"],
    deps = [
        ":hit",
        ":hit_allocator",
        ":hit_tester",
        ":hit_context",
        ":matrix",
        ":multiply",
        ":point",
        ":ray",
        ":ray_tracer",
        ":sal",
        ":status",
        ":vector",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "matrix",
    hdrs = ["matrix.h"],
    srcs = ["matrix.c"],
    deps = [
        ":matrix_internal",
        ":status",
        ":vector",
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
        "//test:test_util",
        "@com_google_googletest//:gtest_main"
    ],
)

cc_library(
    name = "multiply",
    hdrs = ["multiply.h"],
    srcs = ["multiply.c"],
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
        "//test:test_util",
        "@com_google_googletest//:gtest_main"
    ],
)

cc_test(
    name = "multiply_test",
    srcs = ["multiply_test.cc"],
    deps = [
        ":multiply",
        "//test:test_util",
        "@com_google_googletest//:gtest_main"
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
        "//test:test_util",
        "@com_google_googletest//:gtest_main"
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
        "//test:test_util",
        "@com_google_googletest//:gtest_main"
    ],
)

cc_library(
    name = "ray_tracer",
    hdrs = ["ray_tracer.h"],
    srcs = ["ray_tracer.c"],
    deps = [
        ":hit_context",
        ":hit_tester",
        ":hit_tester_internal",
        ":multiply_internal",
        ":ray"
    ],
)

cc_test(
    name = "ray_tracer_test",
    srcs = ["ray_tracer_test.cc"],
    deps = [
        ":multiply",
        ":ray_tracer",
        "//test:test_util",
        "@com_google_googletest//:gtest_main"
    ],
)

cc_library(
    name = "sal",
    hdrs = ["sal.h"],
    visibility = ["//common:__pkg__"],
)

cc_library(
    name = "shared_hit_context",
    hdrs = ["shared_hit_context.h"],
    deps = [
        ":matrix",
        ":ray",
    ],
)

cc_library(
    name = "shared_hit_context_allocator",
    hdrs = ["shared_hit_context_allocator.h"],
    deps = [
        ":shared_hit_context",
        "//common:static_allocator",
    ],
)

cc_test(
    name = "shared_hit_context_allocator_test",
    srcs = ["shared_hit_context_allocator_test.cc"],
    deps = [
        ":shared_hit_context_allocator",
        "//test:test_util",
        "@com_google_googletest//:gtest_main"
    ],
)

cc_library(
    name = "status",
    hdrs = ["status.h"],
    deps = [
        ":sal",
    ],
)

cc_library(
    name = "vector",
    hdrs = ["vector.h"],
    deps = [
        ":sal",
    ],
)

cc_test(
    name = "vector_test",
    srcs = ["vector_test.cc"],
    deps = [
        ":vector",
        "//test:test_util",
        "@com_google_googletest//:gtest_main"
    ],
)