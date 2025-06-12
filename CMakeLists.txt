cmake_minimum_required(VERSION 3.16)
project(SimpleLang)

set(CMAKE_CXX_STANDARD 17)

set(ANTLR_JAR ${PROJECT_SOURCE_DIR}/ThirdParty/antlr4/antlr-4.13.2-complete.jar)
set(GRAMMAR_FILE ${CMAKE_SOURCE_DIR}/SimpleLang.g4)
set(GENERATED_SRC_DIR ${CMAKE_BINARY_DIR}/generated)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

file(MAKE_DIRECTORY ${GENERATED_SRC_DIR})
add_custom_command(
  OUTPUT ${GENERATED_SRC_DIR}/SimpleLangLexer.cpp ${GENERATED_SRC_DIR}/SimpleLangParser.cpp
  COMMAND java -jar ${ANTLR_JAR} -Dlanguage=Cpp -visitor -o ${GENERATED_SRC_DIR} ${GRAMMAR_FILE}
  DEPENDS ${GRAMMAR_FILE}
  COMMENT "Generating parser with ANTLR"
)

include_directories(
  ${GENERATED_SRC_DIR}
  ThirdParty/antlr4/runtime-cpp
)

add_subdirectory(ThirdParty/antlr4/runtime-cpp)

add_executable(SimpleLang
  main.cpp
  test.cpp
  binary_ops.cpp
  byte_stack.cpp
  byte_code_vm.cpp
  byte_code_vm_debugger.cpp
  byte_code_generator.cpp
  byte_code_printer.cpp
  byte_code_enum_translation.cpp
  compiler.cpp
  compiler_visitor.cpp
  program.cpp
  ${GENERATED_SRC_DIR}/SimpleLangLexer.cpp
  ${GENERATED_SRC_DIR}/SimpleLangParser.cpp
)

target_link_libraries(SimpleLang antlr4_static)

if(MSVC)
    target_compile_options(SimpleLang PRIVATE /W4)
else()
    target_compile_options(SimpleLang PRIVATE -Wall -Wextra)
endif()