package(default_visibility = ["//iris:__pkg__"])

cc_library(
    name = "alloc",
    hdrs = ["alloc.h"],
    deps = [
        ":safe_math",
    ],
)

cc_test(
    name = "alloc_test",
    srcs = ["alloc_test.cc"],
    deps = [
        ":alloc",
        "@com_google_googletest//:gtest_main"
    ],
)

cc_library(
    name = "safe_math",
    hdrs = ["safe_math.h"],
    deps = [
        "//iris:sal",
    ],
)

cc_test(
    name = "safe_math_test",
    srcs = ["safe_math_test.cc"],
    deps = [
        ":safe_math",
        "@com_google_googletest//:gtest_main"
    ],
)