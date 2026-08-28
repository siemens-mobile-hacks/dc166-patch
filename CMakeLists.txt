cmake_minimum_required(VERSION 3.20)

project(dc166 NONE)

include(CTest)
find_program(MAKE_EXECUTABLE make REQUIRED)
set(DC166_RUNTIME_WORK "${CMAKE_BINARY_DIR}/runtime" CACHE PATH
    "TASKING runtime extraction and library build directory")

add_custom_target(runtime-libraries ALL
  COMMAND "${MAKE_EXECUTABLE}" -C "${CMAKE_CURRENT_SOURCE_DIR}"
          "WORK=${CMAKE_BINARY_DIR}/work"
          "RUNTIME_WORK=${DC166_RUNTIME_WORK}" libraries
  USES_TERMINAL
)

add_custom_target(patched-linker ALL
  COMMAND "${MAKE_EXECUTABLE}" -C "${CMAKE_CURRENT_SOURCE_DIR}"
          "WORK=${CMAKE_BINARY_DIR}/work"
          "RUNTIME_WORK=${DC166_RUNTIME_WORK}"
          "${CMAKE_BINARY_DIR}/work/l166-patched.exe"
  DEPENDS runtime-libraries
  USES_TERMINAL
)

add_custom_target(patched-debugger ALL
  COMMAND "${MAKE_EXECUTABLE}" -C "${CMAKE_CURRENT_SOURCE_DIR}"
          "WORK=${CMAKE_BINARY_DIR}/work"
          "RUNTIME_WORK=${DC166_RUNTIME_WORK}"
          "${CMAKE_BINARY_DIR}/work/xfw166-patched.exe"
  COMMAND "${CMAKE_COMMAND}" -E copy_if_different
          "${CMAKE_BINARY_DIR}/work/xfw166-patched.exe"
          "${DC166_RUNTIME_WORK}/extracted/SourceFiles/bin/xfw166-patched.exe"
  DEPENDS patched-linker
  USES_TERMINAL
)

add_subdirectory(tests)
