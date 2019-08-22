package(default_visibility = ["//visibility:public"])

cc_library(
    name = "pcg_random",
    hdrs = ["pcg_random.h"],
    srcs = ["pcg_random.c"],
    deps = [
        "//iris_advanced:iris_advanced",
        "//third_party/pcg_c_basic:pcg_basic",
    ],
)

cc_library(
    name = "ply_reader",
    hdrs = ["ply_reader.h"],
    srcs = ["ply_reader.c"],
    deps = [
        "//common:safe_math",
        "//iris_advanced:iris_advanced",
        "//third_party/rply:rply",
    ],
)

cc_test(
    name = "ply_reader_test",
    srcs = ["ply_reader_test.cc"],
    deps = [
        ":ply_reader",
        "//test_util:equality",
        "@com_google_googletest//:gtest_main"
    ],
)