# Set CMake minimum version and CMake policy required by UrhoCommon module
cmake_minimum_required (VERSION 3.2.3)
# Set project name
project (treegen)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (UNIX)
  set(ADDITIONAL_CXX_FLAGS "-Wall -Wextra -Wpedantic -Wno-switch -Wuninitialized  -Wshadow -Wredundant-decls -Wdisabled-optimization -Wnoexcept -Wsign-compare -Wsign-conversion -Wstrict-aliasing")
endif ()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ADDITIONAL_CXX_FLAGS}")


if (DEFINED RBFX_SUBDIR)
  set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT "False")
  set(URHO3D_EXTRAS "False")
  set(URHO3D_TOOLS "False")
  set(URHO3D_GRAPHICS_API "OpenGL")
  add_subdirectory(${RBFX_SUBDIR} rbfx)
else ()
  include ("${RBFX_PATH}/share/CMake/Urho3D.cmake")
  set(DEPS_FOLDERS_SET True)
  if (NOT DEFINED RBFX_PATH)
    message("RBFX_PATH is undefined!")
    set(DEPS_FOLDERS_SET False)
  endif()
endif()

if (NOT ${DEPS_FOLDERS_SET})
  message(FATAL_ERROR "required variables are undefined!")
endif()


add_executable(treegen WIN32
  src/main.cpp
  )

target_link_libraries(treegen Urho3D)
add_subdirectory(contrib/vcppbits/VcppBits/StateManager StateManager)
add_subdirectory(contrib/vcppbits/VcppBits/SimpleVector SimpleVector)
add_subdirectory(contrib/vcppbits/VcppBits/StringUtils StringUtils)
add_subdirectory(contrib/vcppbits/VcppBits/KeyFile KeyFile)
add_subdirectory(contrib/vcppbits/VcppBits/Settings Settings)
add_subdirectory(contrib/vcppbits/VcppBits/MathUtils MathUtils)
add_subdirectory(contrib/urhobits/UrhoBits/GeometryGenerator GeometryGenerator)
add_subdirectory(contrib/urhobits/UrhoBits/TreeGenerator TreeGenerator)
add_subdirectory(contrib/urhobits/UrhoBits/UrhoAppFramework UrhoAppFramework)
add_subdirectory(contrib/urhobits/UrhoBits/InputManager InputManager)
add_subdirectory(contrib/urhobits/UrhoBits/TpsCameraController TpsCameraController)


target_link_libraries(treegen VcppBits-StateManager)
target_link_libraries(treegen UrhoBits-TreeGenerator)
target_link_libraries(treegen UrhoBits-InputManager)
target_link_libraries(treegen UrhoBits-UrhoAppFramework)
target_link_libraries(treegen UrhoBits-TpsCameraController)


target_link_libraries(treegen UrhoBits-GeometryGenerator)
target_link_libraries(treegen VcppBits-MathUtils)
target_link_libraries(treegen VcppBits-Settings)
target_link_libraries(treegen VcppBits-KeyFile)
target_link_libraries(treegen VcppBits-SimpleVector)


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


if (HAS_MKLINK)
  execute_process (COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_SOURCE_DIR}/bin/Data ${CMAKE_BINARY_DIR}/Data)

  execute_process (COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_SOURCE_DIR}/bin/CoreData ${CMAKE_BINARY_DIR}/CoreData)

  message("BUILD TYPE IS ${CMAKE_BUILD_TYPE}")
  if (DEFINED RBFX_SUBDIR)
    execute_process (COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_BINARY_DIR}/bin/${CMAKE_CONFIGURATION_TYPE}/Urho3D.dll ${CMAKE_BINARY_DIR}/${CMAKE_CONFIGURATION_TYPE}/Urho3D.dll)
  else ()
    execute_process (COMMAND ${CMAKE_COMMAND} -E create_symlink ${RBFX_PATH}/bin/${CMAKE_CONFIGURATION_TYPE}/Urho3D.dll ${CMAKE_BINARY_DIR}/Urho3D.dll)
  endif()
else ()
  message(WARNING "MKLINK is not working properly, copy/link data yourself")
endif()
