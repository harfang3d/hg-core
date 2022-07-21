// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.
#include "acutest.h"

//
extern void test_os();
extern void test_math();
extern void test_rand();
extern void test_units();
extern void test_string();
extern void test_path_tools();
extern void test_log();
extern void test_vec2();
extern void test_vec3();
extern void test_vec4();
extern void test_mat3();
extern void test_mat4();
extern void test_mat44();
extern void test_color();
extern void test_minmax();
extern void test_plane();
extern void test_projection();
extern void test_obb();
extern void test_easing();
extern void test_quaternion();
extern void test_frustum();
extern void test_vector_list();
extern void test_generational_vector_list();
extern void test_intrusive_shared_ptr_st();
extern void test_file();
extern void test_dir();
extern void test_data();
extern void test_rw_interface();
extern void test_data_rw_interface();
extern void test_file_rw_interface();
extern void test_clock();

//
TEST_LIST = {
	{"OS", test_os},
	{"Math", test_math},
	{"Rand", test_rand},
	{"Units", test_units},
	{"String", test_string},
	{"Path_tools", test_path_tools},
	{"Log", test_log},
	{"Vec2", test_vec2},
	{"Vec3", test_vec3},
	{"Vec4", test_vec4},
	{"Mat3", test_mat3},
	{"Mat4", test_mat4},
	{"Mat44", test_mat44},
	{"Color", test_color},
	{"MinMax", test_minmax},
	{"Plane", test_plane},
	{"Projection", test_projection},
	{"OBB", test_obb},
	{"Easing", test_easing},
    {"Quaternion", test_quaternion},
	{"Frustum", test_frustum},
	{"Vector_list", test_vector_list},
	{"Generational_vector_list", test_generational_vector_list},
	{"Intrusive_shared_ptr_st", test_intrusive_shared_ptr_st},
	{"File", test_file},
	{"Dir", test_dir},
	{"Data", test_data},
	{"RW_interface", test_rw_interface}, 
	{"Data_RW_interface", test_data_rw_interface},
	{"File_RW_interface", test_file_rw_interface},
	{"Clock", test_clock},

	{NULL, NULL},
};
