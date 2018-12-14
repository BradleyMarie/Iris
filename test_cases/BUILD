package(default_visibility = ["//visibility:private"])

cc_test(
    name = "empty_physx",
    srcs = ["empty_physx.cc"],
    data = [
        "//test_results:blank.pfm",
    ],
    deps = [
        "//iris_advanced_toolkit:pcg_random",
        "//iris_camera_toolkit:grid_pixel_sampler",
        "//iris_camera_toolkit:pinhole_camera",
        "//iris_physx_toolkit:all_light_sampler",
        "//iris_physx_toolkit:list_scene",
        "//iris_physx_toolkit:path_tracer",
        "//iris_physx_toolkit:sample_tracer",
        "//test_util:pfm",
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
        "//iris_physx_toolkit:all_light_sampler",
        "//iris_physx_toolkit:constant_material",
        "//iris_physx_toolkit:lambertian_brdf",
        "//iris_physx_toolkit:list_scene",
        "//iris_physx_toolkit:path_tracer",
        "//iris_physx_toolkit:point_light",
        "//iris_physx_toolkit:sample_tracer",
        "//iris_physx_toolkit:sphere",
        "//test_util:spectra",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "single_triangle",
    srcs = ["single_triangle.cc"],
    data = [
        "//test_results:blank.pfm",
        "//test_results:single_emissive_triangle_xy_triangle.pfm",
        "//test_results:single_emissive_triangle_xz_triangle.pfm",
        "//test_results:single_emissive_triangle_yz_triangle.pfm",
        "//test_results:single_triangle_xy_triangle.pfm",
        "//test_results:single_triangle_xz_triangle.pfm",
        "//test_results:single_triangle_yz_triangle.pfm",
    ],
    deps = [
        "//iris_advanced_toolkit:pcg_random",
        "//iris_camera_toolkit:grid_pixel_sampler",
        "//iris_camera_toolkit:orthographic_camera",
        "//iris_physx_toolkit:all_light_sampler",
        "//iris_physx_toolkit:attenuated_reflector",
        "//iris_physx_toolkit:constant_emissive_material",
        "//iris_physx_toolkit:constant_material",
        "//iris_physx_toolkit:directional_light",
        "//iris_physx_toolkit:lambertian_brdf",
        "//iris_physx_toolkit:list_scene",
        "//iris_physx_toolkit:path_tracer",
        "//iris_physx_toolkit:sample_tracer",
        "//iris_physx_toolkit:triangle",
        "//test_util:pfm",
        "//test_util:spectra",
        "@com_google_googletest//:gtest_main",
    ],
)