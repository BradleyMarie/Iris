load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

package(default_visibility = ["//visibility:private"])

cc_library(
    name = "bounding_box",
    hdrs = ["bounding_box.h"],
    deps = [
        "//iris",
    ],
)

cc_test(
    name = "bounding_box_test",
    srcs = ["bounding_box_test.cc"],
    deps = [
        ":bounding_box",
        "//test_util:equality",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "color",
    srcs = ["color.c"],
    hdrs = ["color.h"],
    deps = [
        "//iris",
    ],
)

cc_test(
    name = "color_test",
    srcs = ["color_test.cc"],
    deps = [
        "//test_util:equality",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "intersection",
    hdrs = ["intersection.h"],
    deps = [
        ":ray_differential",
    ],
)

cc_test(
    name = "intersection_test",
    srcs = ["intersection_test.cc"],
    deps = [
        ":intersection",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "iris_advanced",
    hdrs = ["iris_advanced.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":bounding_box",
        ":color",
        ":intersection",
        ":math",
        ":random",
        ":ray_differential",
        "//iris",
    ],
)

cc_library(
    name = "math",
    srcs = ["math.c"],
    hdrs = ["math.h"],
    deps = [
        "//iris",
    ],
)

cc_library(
    name = "random",
    srcs = ["random.c"],
    hdrs = ["random.h"],
    deps = [
        "//common:alloc",
        "//common:free_routine",
        "//iris",
    ],
)

cc_test(
    name = "random_test",
    srcs = ["random_test.cc"],
    deps = [
        ":random",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "ray_differential",
    hdrs = ["ray_differential.h"],
    deps = [
        "//iris",
    ],
)

cc_test(
    name = "ray_differential_test",
    srcs = ["ray_differential_test.cc"],
    deps = [
        ":ray_differential",
        "//test_util:equality",
        "@com_google_googletest//:gtest_main",
    ],
)
