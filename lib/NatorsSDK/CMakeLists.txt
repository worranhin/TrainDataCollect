cmake_minimum_required(VERSION 3.10) # 设置CMake的最低版本需求

project(NatorsSDK VERSION 0.1) # 定义项目名称

# 设置C标准
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

add_library(NTSDK SHARED IMPORTED GLOBAL)

set_target_properties(NTSDK PROPERTIES
  IMPORTED_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/lib64/NTControl.lib"
)

set_target_properties(NTSDK PROPERTIES
  IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib64/NTControl.dll"
)

# cmake_minimum_required(VERSION 3.10) # 设置CMake的最低版本需求

# project(NatorsSDK VERSION 0.1) # 定义项目名称

# # # 设置C标准
# set(CMAKE_C_STANDARD 11)
# set(CMAKE_C_STANDARD_REQUIRED True)

# add_library(NTSDK SHARED IMPORTED)

# set_target_properties(NTSDK PROPERTIES
#   IMPORTED_IMPLIB "${PROJECT_SOURCE_DIR}/lib64/NTControl.lib"
# )

# set_target_properties(NTSDK PROPERTIES
# IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/lib64/NTControl.dll"
# )

# add_library(NTSDK SHARED IMPORTED)

# set_target_properties(NTSDK PROPERTIES
#   IMPORTED_IMPLIB "lib64/NTControl.lib"
# )

# set_target_properties(NTSDK PROPERTIES
# IMPORTED_LOCATION "lib64/NTControl.dll"
# )

# add_executable(main main.c)

# target_link_libraries(main NatorControl)