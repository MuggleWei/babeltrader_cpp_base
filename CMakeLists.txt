cmake_minimum_required (VERSION 3.18.6)
project(babeltrader_cpp_base)

################################
# general config
################################

message("-- use c compiler ${CMAKE_C_COMPILER}")
message("-- use c++ compiler ${CMAKE_CXX_COMPILER}")

# set compile parameter
if (${CMAKE_C_COMPILER_ID} STREQUAL GNU)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=native -Wall -Wextra")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -Wall -Wextra")
elseif (${CMAKE_C_COMPILER_ID} MATCHES Clang)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=native -Wall -Wextra -Wno-missing-field-initializers")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -Wall -Wextra -Wno-missing-field-initializers")
elseif (${CMAKE_C_COMPILER_ID} STREQUAL MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS=1 -D_UNICODE -DUNICODE)
endif()

# set standard and print features
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

message("-- c compiler support features: ")
foreach(feature ${CMAKE_C_COMPILE_FEATURES})
	message("support feature: ${feature}")
endforeach()

# for vim plugin - YCM
if (NOT ${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
	set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

# set output directory
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# set use folder in vs
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

################################
# options
################################

option(BUILD_SHARED_LIBS "Build shared or static library" ON)
option(BABELTRADER_CPP_BUILD_EXAMPLE "babeltrader cpp build example" OFF)
option(BABELTRADER_CPP_INSTALL_EXAMPLE "babeltrader cpp install example" OFF)

################################
# show inputs
################################

# CMAKE_PREFIX_PATH
foreach(path ${CMAKE_PREFIX_PATH})
	message("# CMake Prefix Path: ${path}")
endforeach()

# build flags
message("# BUILD_SHARED_LIBS: ${BUILD_SHARED_LIBS}")
message("# BUILD_TESTING: ${BUILD_TESTING}")
message("# BABELTRADER_CPP_BUILD_EXAMPLE: ${BABELTRADER_CPP_BUILD_EXAMPLE}")
message("# BABELTRADER_CPP_INSTALL_EXAMPLE: ${BABELTRADER_CPP_INSTALL_EXAMPLE}")

################################
# deps
################################

message("# find package mugglec")
find_package(mugglec REQUIRED)
if (NOT mugglec_FOUND)
	message(FATAL_ERROR "failed found mugglec")
else()
	message("Find mugglec package: ${mugglec_DIR}")
endif()

message("# find package mugglecpp")
find_package(mugglecpp REQUIRED)
if (NOT mugglecpp_FOUND)
	message(FATAL_ERROR "failed found mugglecpp")
else()
	message("Find mugglecpp package: ${mugglecpp_DIR}")
endif()

if (BUILD_TESTING)
	message("# find package GTest")
	find_package(GTest REQUIRED)
	if (NOT GTest_FOUND)
		message(FATAL_ERROR "failed found GTest")
	else()
		message("Find GTest package: ${GTest_DIR}")
	endif()
endif()

################################
# configure
################################

file(STRINGS "version.txt" babeltrader_cpp_version)
string(REPLACE "-" ";" babeltrader_cpp_semver_ext ${babeltrader_cpp_version})
list(GET babeltrader_cpp_semver_ext 0 babeltrader_cpp_semver)
string(REPLACE "." ";" babeltrader_cpp_semver_list ${babeltrader_cpp_semver})

list(GET babeltrader_cpp_semver_list 0 BABELTRADER_CPP_VERSION_MAJOR)
list(GET babeltrader_cpp_semver_list 1 BABELTRADER_CPP_VERSION_MINOR)
list(GET babeltrader_cpp_semver_list 2 BABELTRADER_CPP_VERSION_PATCH)
set(BABELTRADER_CPP_VERSION "${BABELTRADER_CPP_VERSION_MAJOR}.${BABELTRADER_CPP_VERSION_MINOR}.${BABELTRADER_CPP_VERSION_PATCH}")


set(babeltrader_src_dir ${CMAKE_CURRENT_LIST_DIR}/src)
set(babeltrader_gen_dir ${CMAKE_CURRENT_BINARY_DIR}/generated)
if (BUILD_SHARED_LIBS)
	set(BABELTRADER_CPP_USE_DLL ON)
	set(BABELTRADER_CPP_LIB_TYPE SHARED)
else()
	set(BABELTRADER_CPP_USE_DLL OFF)
	set(BABELTRADER_CPP_LIB_TYPE STATIC)
endif()

include(CheckIncludeFile)
check_include_file(threads.h BABELTRADER_CPP_HAVE_THREADS)

configure_file(
	${babeltrader_src_dir}/babeltrader/cpp/config.h.in
	${babeltrader_gen_dir}/babeltrader/cpp/config.h)
FILE(GLOB_RECURSE babeltrader_cpp_gen_h "${babeltrader_gen_dir}/babeltrader/cpp/*.h")

################################
# babeltrader cpp
################################
FILE(GLOB_RECURSE babeltrader_cpp_src_cpp "${babeltrader_src_dir}/babeltrader/cpp/*.cpp")
FILE(GLOB_RECURSE babeltrader_cpp_src_h "${babeltrader_src_dir}/babeltrader/cpp/*.h")
add_library(babeltradercpp ${BABELTRADER_CPP_LIB_TYPE}
	${babeltrader_cpp_src_cpp}
	${babeltrader_cpp_src_h}
	${babeltrader_cpp_gen_h})
target_include_directories(babeltradercpp PUBLIC
	$<BUILD_INTERFACE:${babeltrader_src_dir}>
	$<BUILD_INTERFACE:${babeltrader_gen_dir}>
	$<INSTALL_INTERFACE:include>)
target_link_libraries(babeltradercpp mugglecpp)

target_compile_definitions(babeltradercpp
	PRIVATE BABELTRADER_CPP_EXPORTS MUGGLE_HOLD_LOG_MACRO)
set_target_properties(babeltradercpp PROPERTIES
	DEBUG_POSTFIX d
	VERSION ${BABELTRADER_CPP_VERSION}
	SOVERSION ${BABELTRADER_CPP_VERSION_MAJOR})

################################
# install
################################
if (APPLE)
	set_target_properties(babeltradercpp
		PROPERTIES
		INSTALL_RPATH "@executable_path/../lib"
		)
elseif (UNIX)
	set_target_properties(babeltradercpp
		PROPERTIES
		INSTALL_RPATH "\$ORIGIN/../lib"
		)
endif()

include(GNUInstallDirs)
install(TARGETS babeltradercpp
	EXPORT babeltradercppTargets
	RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
	ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
	LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}")
install(DIRECTORY
	"${babeltrader_src_dir}/babeltrader"
	"${babeltrader_gen_dir}/babeltrader"
	DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
	FILES_MATCHING
	PATTERN "*.h"
	PATTERN "*.hpp")

# cmake package config
set(babeltrader_cpp_export_dir "${CMAKE_INSTALL_LIBDIR}/cmake/babeltradercpp")
install(EXPORT babeltradercppTargets
	FILE babeltradercppTargets.cmake
	DESTINATION "${babeltrader_cpp_export_dir}")

include(CMakePackageConfigHelpers)
configure_package_config_file(
	"${CMAKE_CURRENT_LIST_DIR}/cmake/babeltradercppConfig.cmake.in"
	"${babeltrader_gen_dir}/babeltradercppConfig.cmake"
	INSTALL_DESTINATION "${babeltrader_cpp_export_dir}"
	NO_SET_AND_CHECK_MACRO
	NO_CHECK_REQUIRED_COMPONENTS_MACRO)
write_basic_package_version_file(
	"${babeltrader_gen_dir}/babeltradercppConfigVersion.cmake"
	VERSION ${BABELTRADER_CPP_VERSION}
	COMPATIBILITY SameMajorVersion)
install(FILES
	"${babeltrader_gen_dir}/babeltradercppConfig.cmake"
	"${babeltrader_gen_dir}/babeltradercppConfigVersion.cmake"
	DESTINATION "${babeltrader_cpp_export_dir}")

# pkgconfig
configure_file(
	"${CMAKE_CURRENT_LIST_DIR}/cmake/babeltradercpp.pc.in"
	"${babeltrader_gen_dir}/babeltradercpp.pc" @ONLY)
install(FILES
	"${babeltrader_gen_dir}/babeltradercpp.pc"
	DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig")

# cpack
include(InstallRequiredSystemLibraries)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_LIST_DIR}/LICENSE")
set(CPACK_PACKAGE_VERSION_MAJOR ${BABELTRADER_CPP_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${BABELTRADER_CPP_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${BABELTRADER_CPP_VERSION_PATCH})
set(CPACK_SOURCE_GENERATOR "TGZ")
include(CPack)

################################
# example
################################

function(add_example name folder)
	message("add example ${name} ${folder}")
	
	set(name example_${name})

	file(GLOB_RECURSE tmp_h ${folder}/*.h)
	file(GLOB_RECURSE tmp_c ${folder}/*.c)
	file(GLOB_RECURSE tmp_cpp ${folder}/*.cpp)
	file(GLOB_RECURSE tmp_cc ${folder}/*.cc)

	if (WIN32)
		add_executable(${name} ${tmp_h} ${tmp_c} ${tmp_cpp} ${tmp_cc})
		set_target_properties(${name}
			PROPERTIES
			FOLDER "example"
			VS_DEBUGGER_WORKING_DIRECTORY "$(OutDir)"
		)
	else()
		add_executable(${name} ${tmp_c} ${tmp_cpp} ${tmp_cc})
		if (APPLE)
			set_target_properties(${name}
				PROPERTIES
				INSTALL_RPATH "@executable_path/../lib")
		elseif (UNIX)
			set_target_properties(${name}
				PROPERTIES
				INSTALL_RPATH "\$ORIGIN/../lib"
				)
		endif()
	endif(WIN32)
	add_dependencies(${name} babeltradercpp)
	target_include_directories(${name} PUBLIC
		${CMAKE_CURRENT_LIST_DIR}/example_include)
	target_link_libraries(${name} mugglecpp babeltradercpp)
	target_compile_definitions(${name} PUBLIC MUGGLE_HOLD_LOG_MACRO)

	if (BABELTRADER_CPP_INSTALL_EXAMPLE)
		install(TARGETS ${name} RUNTIME DESTINATION bin)
	endif()
endfunction()

if (BABELTRADER_CPP_BUILD_EXAMPLE)
	set(example_root_dir ${CMAKE_CURRENT_LIST_DIR}/example)
	FILE(GLOB subdirs RELATIVE ${example_root_dir} ${example_root_dir}/*)
	FOREACH(subdir ${subdirs})
		IF(IS_DIRECTORY ${example_root_dir}/${subdir})
			add_example(${subdir} ${example_root_dir}/${subdir})
		ENDIF()
	ENDFOREACH()
endif()

################################
# mock
################################
set(mock_dir ${CMAKE_CURRENT_LIST_DIR}/mock)
FILE(GLOB_RECURSE babeltradercpp_mock_cpp "${mock_dir}/*.cpp")
FILE(GLOB_RECURSE babeltradercpp_mock_h "${mock_dir}/*.h")
set(mock babeltradercpp_mock)
add_library(${mock} ${BABELTRADER_CPP_LIB_TYPE}
	${babeltradercpp_mock_cpp}
	${babeltradercpp_mock_h})
target_link_libraries(${mock} mugglecpp babeltradercpp)
target_include_directories(${mock} PUBLIC "${mock_dir}")
target_compile_definitions(${mock}
	PRIVATE BABELTRADER_CPP_MOCK_EXPORTS MUGGLE_HOLD_LOG_MACRO)
set_target_properties(${mock} PROPERTIES
	DEBUG_POSTFIX d
	VERSION ${BABELTRADER_CPP_VERSION}
	SOVERSION ${BABELTRADER_CPP_VERSION_MAJOR})

################################
# test
################################

function(add_unittest name folder)
	message("add test ${name} ${folder}")
	
	set(name test_${name})

	file(GLOB_RECURSE tmp_h ${folder}/*.h)
	file(GLOB_RECURSE tmp_c ${folder}/*.c)
	file(GLOB_RECURSE tmp_cpp ${folder}/*.cpp)
	file(GLOB_RECURSE tmp_cc ${folder}/*.cc)

	if (WIN32)
		add_executable(${name} ${tmp_h} ${tmp_c} ${tmp_cpp} ${tmp_cc})
		set_target_properties(${name}
			PROPERTIES
			FOLDER "example"
			VS_DEBUGGER_WORKING_DIRECTORY "$(OutDir)"
		)
	else()
		add_executable(${name} ${tmp_c} ${tmp_cpp} ${tmp_cc})
		if (APPLE)
			set_target_properties(${name}
				PROPERTIES
				INSTALL_RPATH "@executable_path/../lib")
		elseif (UNIX)
			set_target_properties(${name}
				PROPERTIES
				INSTALL_RPATH "\$ORIGIN/../lib"
				)
		endif()
	endif(WIN32)
	add_dependencies(${name} babeltradercpp)
	target_include_directories(${name} PUBLIC
		${CMAKE_CURRENT_LIST_DIR}/example_include
		${CMAKE_CURRENT_LIST_DIR}/mock)
	target_link_libraries(${name} mugglecpp babeltradercpp ${mock} ${GTEST_BOTH_LIBRARIES})
	target_compile_definitions(${name} PUBLIC MUGGLE_HOLD_LOG_MACRO)

	add_test(NAME ${name} COMMAND ${name})
endfunction()

if (BUILD_TESTING)
	enable_testing()

	set(test_root_dir ${CMAKE_CURRENT_LIST_DIR}/test)
	FILE(GLOB subdirs RELATIVE ${test_root_dir} ${test_root_dir}/*)
	FOREACH(subdir ${subdirs})
		IF(IS_DIRECTORY ${test_root_dir}/${subdir})
			add_unittest(${subdir} ${test_root_dir}/${subdir})
		ENDIF()
	ENDFOREACH()
endif()
