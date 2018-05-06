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
    name = "const_pointer_list",
    hdrs = ["const_pointer_list.h"],
    deps = [
        ":safe_math",
    ],
)

cc_test(
    name = "const_pointer_list_test",
    srcs = ["const_pointer_list_test.cc"],
    deps = [
        ":const_pointer_list",
        "@com_google_googletest//:gtest_main"
    ],
)

cc_library(
    name = "dynamic_allocator",
    hdrs = ["dynamic_allocator.h"],
    deps = [
        ":alloc",
    ],
)

cc_test(
    name = "dynamic_allocator_test",
    srcs = ["dynamic_allocator_test.cc"],
    deps = [
        ":dynamic_allocator",
        "@com_google_googletest//:gtest_main"
    ],
)

cc_library(
    name = "pointer_list",
    hdrs = ["pointer_list.h"],
    deps = [
        ":safe_math",
    ],
)

cc_test(
    name = "pointer_list_test",
    srcs = ["pointer_list_test.cc"],
    deps = [
        ":pointer_list",
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

cc_library(
    name = "static_allocator",
    hdrs = ["static_allocator.h"],
    deps = [
        ":pointer_list",
    ],
)

cc_test(
    name = "static_allocator_test",
    srcs = ["static_allocator_test.cc"],
    deps = [
        ":static_allocator",
        "@com_google_googletest//:gtest_main"
    ],
)