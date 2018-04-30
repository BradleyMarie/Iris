package(default_visibility = ["//visibility:public"])

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
    visibility = ["//iris:__pkg__"],
)

cc_library(
    name = "multiply",
    hdrs = ["multiply.h"],
    srcs = ["multiply.c"],
    deps = [
        ":matrix",
        ":point",
        ":multiply_internal",
        ":vector",
    ],
)

cc_library(
    name = "multiply_internal",
    hdrs = ["multiply_internal.h"],
    deps = [
        ":matrix_internal",
        ":point",
        ":vector",
    ],
    visibility = ["//iris:__pkg__"],
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
    name = "sal",
    hdrs = ["sal.h"],
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