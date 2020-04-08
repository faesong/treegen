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

set(DEPS_FOLDERS_SET True)
if (NOT DEFINED CPPBITS_FOLDER)
  message("CPPBITS_FOLDER is undefined!")
  set(DEPS_FOLDERS_SET False)
endif()
if (NOT DEFINED URHOBITS_FOLDER)
  message("URHOBITS_FOLDER is undefined!")
  set(DEPS_FOLDERS_SET False)
endif()
if (NOT DEFINED RBFX_PATH)
  message("RBFX_PATH is undefined!")
  set(DEPS_FOLDERS_SET False)
endif()

if (NOT ${DEPS_FOLDERS_SET})
  message(FATAL_ERROR "required variables are undefined!")
endif()


include ("${RBFX_PATH}/share/CMake/Urho3D.cmake")

add_executable(treegen WIN32
  src/main.cpp
  )

target_link_libraries(treegen Urho3D)
add_subdirectory(${CPPBITS_FOLDER}/VcppBits/StateManager StateManager)
add_subdirectory(${CPPBITS_FOLDER}/VcppBits/SimpleVector SimpleVector)
add_subdirectory(${CPPBITS_FOLDER}/VcppBits/StringUtils StringUtils)
add_subdirectory(${CPPBITS_FOLDER}/VcppBits/KeyFile KeyFile)
add_subdirectory(${CPPBITS_FOLDER}/VcppBits/Settings Settings)
add_subdirectory(${CPPBITS_FOLDER}/VcppBits/MathUtils MathUtils)
add_subdirectory(${URHOBITS_FOLDER}/UrhoBits/GeometryGenerator GeometryGenerator)
add_subdirectory(${URHOBITS_FOLDER}/UrhoBits/TreeGenerator TreeGenerator)
add_subdirectory(${URHOBITS_FOLDER}/UrhoBits/UrhoAppFramework UrhoAppFramework)
add_subdirectory(${URHOBITS_FOLDER}/UrhoBits/InputManager InputManager)
add_subdirectory(${URHOBITS_FOLDER}/UrhoBits/TpsCameraController TpsCameraController)


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

# TODO hardcoded path
#add_subdirectory(C:/wip/rbfx/Source/ThirdParty/Assimp Assimp)
#target_link_libraries(treegen Assimp)

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
else ()
  message(WARNING "MKLINK is not working properly, copy/link data yourself")
endif()
