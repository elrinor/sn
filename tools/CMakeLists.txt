cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

# Headergen is not here because it has to be added before src, see the comment in the root CMakeLists.txt.
add_subdirectory(intgen)
add_subdirectory(ppgen)
