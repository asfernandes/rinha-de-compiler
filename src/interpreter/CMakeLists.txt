set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(rinha-de-compiler CXX)

file(GLOB_RECURSE SRC
	"*.h"
	"*.cpp"
)

file(GLOB_RECURSE TEST_SRC
	"*.test.cpp"
)

find_package(Boost REQUIRED COMPONENTS unit_test_framework)


foreach(item ${SRC})
	if(${item} MATCHES ".*\.test\.cpp$" OR ${item} MATCHES ".*main\.cpp$")
		list(REMOVE_ITEM SRC ${item})
	endif()
endforeach()


add_library(${PROJECT_NAME}-lib
	${SRC}
)

target_link_libraries(${PROJECT_NAME}-lib
	PUBLIC grammar
)


add_executable(${PROJECT_NAME}
	${SRC}
	main.cpp
)

target_link_libraries(${PROJECT_NAME}
	PUBLIC ${PROJECT_NAME}-lib
)


add_executable(${PROJECT_NAME}-test
	${TEST_SRC}
)

target_include_directories(${PROJECT_NAME}-test
	PUBLIC ${Boost_INCLUDE_DIR}
)

target_link_libraries(${PROJECT_NAME}-test
	PRIVATE ${PROJECT_NAME}-lib
	PRIVATE Boost::unit_test_framework
)

add_test(
	NAME ${PROJECT_NAME}-test
	COMMAND ${PROJECT_NAME}-test
)
