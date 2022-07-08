add_executable(geo_viewer geo_viewer.cpp)
target_link_libraries(geo_viewer PUBLIC app_glfw engine)

add_executable(scn_viewer scn_viewer.cpp)
target_link_libraries(scn_viewer PUBLIC app_glfw engine)
