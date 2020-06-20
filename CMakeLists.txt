# Set CMake minimum version and CMake policy required by UrhoCommon module
cmake_minimum_required (VERSION 3.2.3)
# Set project name
project (treegen)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/;${CMAKE_MODULE_PATH}")

include(GetGitRevisionDescription)
get_git_head_revision(GIT_REFSPEC URHOTREEGEN_GIT_SHA1)
string(SUBSTRING ${URHOTREEGEN_GIT_SHA1} 0 10 URHOTREEGEN_VERSION_GIT_REVISION)



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
  src/AppSettings.cpp
  src/TreeEditState.cpp
  src/TreeConfigCache.cpp
  src/UrhoToGltf.cpp
  src/SettingsUi.cpp
  src/TreeGen.cpp
  src/RootState.cpp
  )

if (UNIX)
  set(ADDITIONAL_CXX_FLAGS "-Wall -Wextra -Wpedantic -Wno-switch -Wuninitialized  -Wshadow -Wredundant-decls -Wdisabled-optimization -Wnoexcept -Wsign-compare -Wsign-conversion -Wstrict-aliasing")
  separate_arguments(ADDITIONAL_CXX_FLAGS)
  target_compile_options(treegen PRIVATE "${ADDITIONAL_CXX_FLAGS}")
endif ()

target_link_libraries(treegen Urho3D)
add_subdirectory(contrib/vcppbits/VcppBits/StateManager StateManager)
add_subdirectory(contrib/vcppbits/VcppBits/SimpleVector SimpleVector)
add_subdirectory(contrib/vcppbits/VcppBits/StringUtils StringUtils)
add_subdirectory(contrib/vcppbits/VcppBits/KeyFile KeyFile)
add_subdirectory(contrib/vcppbits/VcppBits/Settings Settings)
add_subdirectory(contrib/vcppbits/VcppBits/MathUtils MathUtils)
add_subdirectory(contrib/rbfxbits/UrhoBits/GeometryGenerator GeometryGenerator)
add_subdirectory(contrib/rbfxbits/UrhoBits/TreeGenerator TreeGenerator)
add_subdirectory(contrib/rbfxbits/UrhoBits/UrhoAppFramework UrhoAppFramework)
add_subdirectory(contrib/rbfxbits/UrhoBits/InputManager InputManager)
add_subdirectory(contrib/rbfxbits/UrhoBits/TpsCameraController TpsCameraController)


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

set(RBFX_BUILD_TYPE "Release")
if (NOT DEFINED RBFX_SUBDIR)
  get_filename_component(RBFX_FOLDER_NAME ${RBFX_PATH} NAME)
  message("FOLDER NAME ${RBFX_FOLDER_NAME}")
  set(RBFX_DEBUG_BUILD_TYPE_FOUND)
  string(FIND "${RBFX_FOLDER_NAME}" "Debug" RBFX_DEBUG_BUILD_TYPE_POS)
  if (NOT ${RBFX_DEBUG_BUILD_TYPE_POS} EQUAL -1)
    set(RBFX_BUILD_TYPE "Debug")
  endif()
endif()
message("RBFX_BUILD_TYPE is ${RBFX_BUILD_TYPE}")



if (HAS_MKLINK)
  execute_process (COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_SOURCE_DIR}/bin/Data ${CMAKE_BINARY_DIR}/Data)

  execute_process (COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_SOURCE_DIR}/bin/CoreData ${CMAKE_BINARY_DIR}/CoreData)

  message("BUILD TYPE IS ${CMAKE_BUILD_TYPE}")
  message("CONFIGURATION_TYPE IS ${CMAKE_CONFIGURATION_TYPE}")
  if (DEFINED RBFX_SUBDIR)
    set(URHO3D_DLL_LOCATION "${CMAKE_BINARY_DIR}/bin/${CMAKE_CONFIGURATION_TYPE}/Urho3D.dll")
    execute_process (COMMAND ${CMAKE_COMMAND} -E create_symlink  ${CMAKE_BINARY_DIR}/${CMAKE_CONFIGURATION_TYPE}/Urho3D.dll)
  else ()
    set(URHO3D_DLL_LOCATION "${RBFX_PATH}/bin/${RBFX_BUILD_TYPE}/Urho3D.dll")
    execute_process (COMMAND ${CMAKE_COMMAND} -E create_symlink ${RBFX_PATH}/bin/${RBFX_BUILD_TYPE}/Urho3D.dll ${CMAKE_BINARY_DIR}/Urho3D.dll)
  endif()
else ()
  message(WARNING "MKLINK is not working properly, copy/link data yourself")
endif()


set(URHOTREEGEN_VERSION_MAJOR 0)
set(URHOTREEGEN_VERSION_MINOR 0)
set(URHOTREEGEN_VERSION_BUGFIX 0)

set(URHOTREEGEN_VERSION_AS_STRING
  "${URHOTREEGEN_VERSION_MAJOR}.${URHOTREEGEN_VERSION_MINOR}.${URHOTREEGEN_VERSION_BUGFIX}")

# install msvc runtime bloat
set(CMAKE_INSTALL_UCRT_LIBRARIES Yes)
set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION ".")
set(CMAKE_INSTALL_DEBUG_LIBRARIES Yes)
set(CMAKE_INSTALL_UCRT_LIBRARIES Yes)
include(InstallRequiredSystemLibraries)


install(TARGETS treegen
  DESTINATION ".")

install(DIRECTORY bin/CoreData bin/Data
  DESTINATION ".")

if(WIN32)
  install(FILES "${URHO3D_DLL_LOCATION}"
    DESTINATION ".")
endif()

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
