add_library(maths INTERFACE include/graphics/maths_headers.h)
find_package(glm REQUIRED)
#find_path(GLM_INCLUDE_DIR)

set_property(TARGET maths PROPERTY CXX_STANDARD 20)

target_include_directories(maths INTERFACE include)
#target_include_directories(maths PRIVATE src)

#target_link_libraries(maths PRIVATE logger util)
target_link_libraries(maths INTERFACE glm::glm)
