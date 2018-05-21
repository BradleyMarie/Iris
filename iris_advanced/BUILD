package(default_visibility = ["//visibility:private"])

cc_library(
    name = "bounding_box",
    hdrs = ["bounding_box.h"],
    deps = [
        "//iris:iris",
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
        ":color",
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
    ],
    visibility = ["//visibility:public"],
)