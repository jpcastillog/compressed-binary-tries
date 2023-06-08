add_library(intersection ../src/intersection.cpp ../include/intersection.hpp)
add_executable(test_encoding test_encoding.cpp)
target_link_libraries(test_encoding PUBLIC sdsl divsufsort divsufsort64)