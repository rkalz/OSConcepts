cmake_minimum_required (VERSION 3.8)

add_executable(OsConcepts api_io.c)

add_custom_command(TARGET OsConcepts POST_BUILD
					COMMAND ${CMAKE_COMMAND} -E copy
					${CMAKE_SOURCE_DIR}/win32/chapter_2/src.txt
					${CMAKE_CURRENT_BINARY_DIR}/src.txt)