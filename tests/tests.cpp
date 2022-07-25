// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.
#include "acutest.h"

// foundation tests
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

// engine tests
extern void test_vertex_layout();
extern void test_anim();
extern void test_node();

//
TEST_LIST = {
	{"foundation.os", test_os},
	{"foundation.math", test_math},
	{"foundation.rand", test_rand},
	{"foundation.units", test_units},
	{"foundation.string", test_string},
	{"foundation.path_tools", test_path_tools},
	{"foundation.log", test_log},
	{"foundation.vec2", test_vec2},
	{"foundation.vec3", test_vec3},
	{"foundation.vec4", test_vec4},
	{"foundation.mat3", test_mat3},
	{"foundation.mat4", test_mat4},
	{"foundation.mat44", test_mat44},
	{"foundation.color", test_color},
	{"foundation.minMax", test_minmax},
	{"foundation.plane", test_plane},
	{"foundation.projection", test_projection},
	{"foundation.obb", test_obb},
	{"foundation.easing", test_easing},
    {"foundation.quaternion", test_quaternion},
	{"foundation.frustum", test_frustum},
	{"foundation.vector_list", test_vector_list},
	{"foundation.generational_vector_list", test_generational_vector_list},
	{"foundation.intrusive_shared_ptr_st", test_intrusive_shared_ptr_st},
	{"foundation.file", test_file},
	{"foundation.dir", test_dir},
	{"foundation.data", test_data},
	{"foundation.rw_interface", test_rw_interface}, 
	{"foundation.data_rw_interface", test_data_rw_interface},
	{"foundation.file_rw_interface", test_file_rw_interface},
	{"foundation.clock", test_clock},

	{"engine.vertex_layout", test_vertex_layout},
	{"engine.anim", test_anim},
	{"engine.node", test_node},

	{NULL, NULL},
};
