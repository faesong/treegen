cmake_minimum_required (VERSION 3.2.3)
# Set project name
project (treegen)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/;${CMAKE_MODULE_PATH}")

include(GetGitRevisionDescription)
get_git_head_revision(GIT_REFSPEC URHOTREEGEN_GIT_SHA1)
string(SUBSTRING ${URHOTREEGEN_GIT_SHA1} 0 10 URHOTREEGEN_VERSION_GIT_REVISION)



include(contrib/rbfxbits/CMake/RbfxSdkUtils.cmake)


add_executable(treegen WIN32
  src/main.cpp
  src/AppSettings.cpp
  src/TreeEditState.cpp
  src/TreeConfigCache.cpp
  src/UrhoToGltf.cpp
  src/SettingsUi.cpp
  src/TreeGen.cpp
  src/RootState.cpp
  )

set(ADDITIONAL_CXX_FLAGS "")
if (UNIX)
  set(ADDITIONAL_CXX_FLAGS "-Wall -Wextra -Wpedantic -Wno-switch -Wuninitialized  -Wshadow -Wredundant-decls -Wdisabled-optimization -Wsign-compare -Wsign-conversion -Wstrict-aliasing")
  if (CMAKE_COMPILER_IS_GNUCXX)
    set(ADDITIONAL_CXX_FLAGS "${ADDITIONAL_CXX_FLAGS} -Wnoexcept")
  endif ()
  separate_arguments(ADDITIONAL_CXX_FLAGS)
endif ()

# a workaround for GCC8
# https://stackoverflow.com/questions/56615841/passing-stdfilesystempath-to-a-function-segfaults
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU"
    AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9)
  message(WARNING "gcc ${CMAKE_CXX_COMPILER_VERSION}")
  target_link_libraries(treegen "-lstdc++fs")
endif()

target_compile_options(treegen PRIVATE ${ADDITIONAL_CXX_FLAGS})

target_link_libraries(treegen Urho3D)

set_target_properties(treegen PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/Debug"
  RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/Release"
  )


include(contrib/vcppbits/CMake/AddBit.cmake)


# Translation bit wants this variable to point to our Translation IDs
set(TRANSLATION_IDS_FILE ${CMAKE_CURRENT_SOURCE_DIR}/src/TranslationIds.hpp)

add_bit(
  TARGET treegen
  BITS_DIR contrib/vcppbits/VcppBits
  LIBNAME_PREFIX VcppBits
  BITS StateManager SimpleVector StringUtils KeyFile Settings MathUtils Translation
  )

add_bit(
  TARGET treegen
  BITS_DIR contrib/rbfxbits/UrhoBits
  LIBNAME_PREFIX UrhoBits
  BITS GeometryGenerator TreeGenerator UrhoAppFramework InputManager TpsCameraController
  )

add_bit(
  BITS_DIR contrib/vcppbits/VcppBits
  LIBNAME_PREFIX VcppBits
  BITS TestsDriver
  )

set_target_properties(UrhoBits-TreeGenerator-Tests PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/Debug"
  RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/Release"
  )


target_include_directories(treegen SYSTEM PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/contrib")


set_property(TARGET treegen PROPERTY CXX_STANDARD 17)


#symlink stuff
if (CMAKE_HOST_WIN32)
  set (NULL_DEVICE nul)
  if (NOT DEFINED HAS_MKLINK)
    # Test whether the host system is capable of setting up symbolic link
    execute_process (COMMAND cmd /C mklink test-link CMakeCache.txt WORKING_DIRECTORY ${CMAKE_BINARY_DIR} RESULT_VARIABLE MKLINK_EXIT_CODE OUTPUT_QUIET ERROR_QUIET)
    if (MKLINK_EXIT_CODE EQUAL 0)
      set (HAS_MKLINK TRUE)
      file (REMOVE ${CMAKE_BINARY_DIR}/test-link)
    else ()
      set (HAS_MKLINK FALSE)
    endif()
    set (HAS_MKLINK ${HAS_MKLINK} CACHE INTERNAL "MKLINK capability")
  endif ()
endif ()


# this part is crap, although it works for couple of specific uses...


if (HAS_MKLINK OR NOT CMAKE_HOST_WIN32)
  execute_process (COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR}/_srcdir)

  execute_process (COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_SOURCE_DIR}/bin/Data ${CMAKE_BINARY_DIR}/Data)

  execute_process (COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_SOURCE_DIR}/bin/CoreData ${CMAKE_BINARY_DIR}/CoreData)
endif()

set(URHOTREEGEN_VERSION_MAJOR 0)
set(URHOTREEGEN_VERSION_MINOR 0)
set(URHOTREEGEN_VERSION_BUGFIX 0)

set(URHOTREEGEN_VERSION_AS_STRING
  "${URHOTREEGEN_VERSION_MAJOR}.${URHOTREEGEN_VERSION_MINOR}.${URHOTREEGEN_VERSION_BUGFIX}")

# install msvc runtime bloat
set(CMAKE_INSTALL_UCRT_LIBRARIES Yes)
set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION ".")
set(CMAKE_INSTALL_DEBUG_LIBRARIES No)
set(CMAKE_INSTALL_UCRT_LIBRARIES Yes)
include(InstallRequiredSystemLibraries)


install(TARGETS treegen
  DESTINATION ".")

install(DIRECTORY bin/CoreData bin/Data
  DESTINATION ".")

if(WIN32)
  set(CPACK_GENERATOR "ZIP")
  set(URHOTREEGEN_SYSTEM_NAME "windows")
elseif(UNIX)
  set(URHOTREEGEN_SYSTEM_NAME "linux")
  set(CPACK_GENERATOR "TGZ")
endif()

set(URHOTREEGEN_ARCH "i686")
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(URHOTREEGEN_ARCH "x86_64")
endif()

set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Vitalii Minnakhmetov") #required
set(CPACK_PACKAGE_VERSION_MAJOR ${URHOTREEGEN_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${URHOTREEGEN_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${URHOTREEGEN_VERSION_BUGFIX})
set(CPACK_PACKAGE_FILE_NAME "treegen-${URHOTREEGEN_VERSION_AS_STRING}-${URHOTREEGEN_VERSION_GIT_REVISION}-${URHOTREEGEN_SYSTEM_NAME}-${URHOTREEGEN_ARCH}")
set(CPACK_PACKAGE_VERSION ${URHOTREEGEN_VERSION_AS_STRING})
include(CPack)
