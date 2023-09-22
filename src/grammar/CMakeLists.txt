set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(grammar CXX)

find_package(antlr4-generator REQUIRED)
find_package(antlr4-runtime CONFIG REQUIRED)

antlr4_generate(
	grammar
	${CMAKE_CURRENT_SOURCE_DIR}/Rinha.g4
	BOTH
	TRUE
	FALSE
	"rinha::grammar"
)

add_library(${PROJECT_NAME}
	${ANTLR4_SRC_FILES_grammar}
)

target_compile_definitions(${PROJECT_NAME}
	PUBLIC -DANTLR4CPP_STATIC
)

target_include_directories(${PROJECT_NAME}
	PUBLIC ${ANTLR4_INCLUDE_DIR}
	PUBLIC ${ANTLR4_INCLUDE_DIR_grammar}/..
)

if(NOT MSVC)
	target_link_libraries(${PROJECT_NAME}
		PRIVATE antlr4_static
	)
else()
	target_link_libraries(${PROJECT_NAME}
		PRIVATE antlr4_shared
	)
endif()
