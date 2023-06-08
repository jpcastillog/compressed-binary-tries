# add_library(intersection ../src/intersection.cpp ../include/intersection.hpp)

add_executable(build build_tries.cpp)
add_executable(queries intersection_query_log.cpp)

target_link_libraries(queries PUBLIC intersection pthread sdsl divsufsort divsufsort64)
target_link_libraries(build PUBLIC sdsl divsufsort divsufsort64)