set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(rinha-de-compiler CXX)

file(GLOB_RECURSE SRC
	"*.h"
	"*.cpp"
)

add_executable(${PROJECT_NAME}
	${SRC}
)

target_link_libraries(${PROJECT_NAME}
	PUBLIC grammar
)
