package(default_visibility = ["//visibility:private"])

cc_library(
    name = "bounding_box",
    hdrs = ["bounding_box.h"],
    deps = [
        "//iris:iris",
    ],
)

cc_library(
    name = "color",
    hdrs = ["color.h"],
    deps = [
        "//iris:iris",
    ],
)

cc_test(
    name = "color_test",
    srcs = ["color_test.cc"],
    deps = [
        "//test:test_util",
        "@com_google_googletest//:gtest_main"
    ],
)

cc_test(
    name = "bounding_box_test",
    srcs = ["bounding_box_test.cc"],
    deps = [
        ":bounding_box",
        "//test:test_util",
        "@com_google_googletest//:gtest_main"
    ],
)

cc_library(
    name = "iris_advanced",
    hdrs = ["iris_advanced.h"],
    deps = [
        ":bounding_box",
        ":color",
        ":random",
        "//iris:iris"
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "random",
    hdrs = ["random.h"],
    srcs = ["random.c"],
    deps = [
        "//common:alloc",
        "//common:free_routine",
        "//iris:iris",
    ],
)

cc_test(
    name = "random_test",
    srcs = ["random_test.cc"],
    deps = [
        ":random",
        "@com_google_googletest//:gtest_main"
    ],
)