cmake_minimum_required(VERSION 3.10)
project(SatelliteFormation C)

# ==================== 设置编译标准 ====================
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic -O2 -fPIC")

# 调试模式下添加更多信息
if(CMAKE_BUILD_TYPE MATCHES Debug)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0 -DDEBUG")
endif()

# ==================== 项目结构 ====================
set(PROJECT_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(PROJECT_OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/build)
set(PROJECT_CONFIG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/config)

# 包含目录
include_directories(${PROJECT_INCLUDE_DIR})

# ==================== 源文件列表 ====================

# 基础模块
set(BASE_SOURCES
    ${PROJECT_SOURCE_DIR}/vector3.c
    ${PROJECT_SOURCE_DIR}/quaternion.c
    ${PROJECT_SOURCE_DIR}/satellite.c
    ${PROJECT_SOURCE_DIR}/orbit.c
    ${PROJECT_SOURCE_DIR}/attitude.c
)

# 编队模块
set(FORMATION_SOURCES
    ${PROJECT_SOURCE_DIR}/formation/formation_base.c
    ${PROJECT_SOURCE_DIR}/formation/formation_inspect.c
    ${PROJECT_SOURCE_DIR}/formation/formation_around.c
    ${PROJECT_SOURCE_DIR}/formation/formation_circumnavigate.c
    ${PROJECT_SOURCE_DIR}/formation/formation_retreat.c
)

# 决策模块
set(DECISION_SOURCES
    ${PROJECT_SOURCE_DIR}/decision/decision_tree.c
    ${PROJECT_SOURCE_DIR}/decision/differential_game.c
    ${PROJECT_SOURCE_DIR}/decision/formation_manager.c
)

# 其他模块
set(OTHER_SOURCES
    ${PROJECT_SOURCE_DIR}/config/config.c
    ${PROJECT_SOURCE_DIR}/kinematics.c
)

# 所有源文件
set(ALL_SOURCES
    ${BASE_SOURCES}
    ${FORMATION_SOURCES}
    ${DECISION_SOURCES}
    ${OTHER_SOURCES}
)

# ==================== 库目标 ====================

# 创建静态库
add_library(libsatellite_static STATIC
    ${ALL_SOURCES}
)
set_target_properties(libsatellite_static PROPERTIES
    OUTPUT_NAME "satellite"
    LIBRARY_OUTPUT_DIRECTORY "${PROJECT_OUTPUT_DIR}/lib"
    ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_OUTPUT_DIR}/lib"
)

# 创建动态库（可选）
# add_library(libsatellite_shared SHARED
#     ${ALL_SOURCES}
# )
# set_target_properties(libsatellite_shared PROPERTIES
#     OUTPUT_NAME "satellite"
#     LIBRARY_OUTPUT_DIRECTORY "${PROJECT_OUTPUT_DIR}/lib"
# )

# ==================== 可执行文件目标 ====================

# 主程序
add_executable(satellite_sim
    ${PROJECT_SOURCE_DIR}/main.c
    ${ALL_SOURCES}
)
set_target_properties(satellite_sim PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_OUTPUT_DIR}/bin"
)

# 链接库
target_link_libraries(satellite_sim m)  # 数学库

# ==================== 单元测试（可选） ====================

# 启用测试
enable_testing()

# 测试可执行文件列表
set(TEST_TARGETS
    # test_vector3
    # test_quaternion
    # test_orbit
    # test_satellite
)

# 为每个测试创建可执行文件（如果存在）
# foreach(TEST ${TEST_TARGETS})
#     if(EXISTS "${PROJECT_SOURCE_DIR}/tests/${TEST}.c")
#         add_executable(${TEST}
#             ${PROJECT_SOURCE_DIR}/tests/${TEST}.c
#             ${ALL_SOURCES}
#         )
#         set_target_properties(${TEST} PROPERTIES
#             RUNTIME_OUTPUT_DIRECTORY "${PROJECT_OUTPUT_DIR}/test"
#         )
#         target_link_libraries(${TEST} m)
#         add_test(NAME ${TEST} COMMAND ${TEST})
#     endif()
# endforeach()

# ==================== 输出目录 ====================

file(MAKE_DIRECTORY "${PROJECT_OUTPUT_DIR}/bin")
file(MAKE_DIRECTORY "${PROJECT_OUTPUT_DIR}/lib")
file(MAKE_DIRECTORY "${PROJECT_OUTPUT_DIR}/obj")
file(MAKE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/output")

# ==================== 编译信息输出 ====================

message(STATUS "========================================")
message(STATUS "卫星编队仿真系统 - C语言版本")
message(STATUS "========================================")
message(STATUS "CMAKE_C_COMPILER: ${CMAKE_C_COMPILER}")
message(STATUS "CMAKE_C_FLAGS: ${CMAKE_C_FLAGS}")
message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message(STATUS "Project Include Dir: ${PROJECT_INCLUDE_DIR}")
message(STATUS "Project Source Dir: ${PROJECT_SOURCE_DIR}")
message(STATUS "Project Output Dir: ${PROJECT_OUTPUT_DIR}")
message(STATUS "========================================")
message(STATUS "构建目标:")
message(STATUS "  - 静态库: libsatellite")
message(STATUS "  - 可执行文件: satellite_sim")
message(STATUS "========================================")

# ==================== 安装规则（可选） ====================

# install(TARGETS satellite_sim
#     RUNTIME DESTINATION bin
# )
# 
# install(TARGETS libsatellite_static
#     ARCHIVE DESTINATION lib
# )
# 
# install(DIRECTORY ${PROJECT_INCLUDE_DIR}/
#     DESTINATION include
# )
