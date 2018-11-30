package(default_visibility = ["//visibility:private"])

cc_test(
    name = "empty_physx",
    srcs = ["empty_physx.cc"],
    deps = [
        "//iris_advanced_toolkit:pcg_random",
        "//iris_camera_toolkit:grid_pixel_sampler",
        "//iris_camera_toolkit:pinhole_camera",
        "//iris_physx_camera_toolkit:physx_sample_tracer",
        "//iris_physx_toolkit:all_light_sampler",
        "//iris_physx_toolkit:list_scene",
        "//iris_physx_toolkit:path_tracer",
        "//test_util:spectra",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "single_sphere",
    srcs = ["single_sphere.cc"],
    deps = [
        "//iris_advanced_toolkit:pcg_random",
        "//iris_camera_toolkit:grid_pixel_sampler",
        "//iris_camera_toolkit:pfm_writer",
        "//iris_camera_toolkit:pinhole_camera",
        "//iris_physx_camera_toolkit:physx_sample_tracer",
        "//iris_physx_toolkit:all_light_sampler",
        "//iris_physx_toolkit:constant_material",
        "//iris_physx_toolkit:lambertian_brdf",
        "//iris_physx_toolkit:list_scene",
        "//iris_physx_toolkit:path_tracer",
        "//iris_physx_toolkit:point_light",
        "//iris_physx_toolkit:sphere",
        "//test_util:spectra",
        "@com_google_googletest//:gtest_main",
    ],
)