# CMakeList.txt: MySTL 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)

project (MYSTL)

# 将源代码添加到此项目的可执行文件。
add_executable (${PROJECT_NAME}  demo.cpp "MySTL_Dir/algo.h" "MySTL_Dir/heap_algo.h" "MySTL_Dir/functional.h" "MySTL_Dir/memory.h" "MySTL_Dir/allocator.h" "MySTL_Dir/algorithm.h" "MySTL_Dir/set_algo.h" "MySTL_Dir/exceptdef.h" "MySTL_Dir/vector.h" "MySTL_Dir/deque.h" "MySTL_Dir/hashtable.h" "MySTL_Dir/list.h" "MySTL_Dir/unordered_map.h" "MySTL_Dir/stack.h" "MySTL_Dir/queue.h" "MySTL_Dir/rb_tree.h")

target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/MySTL_Dir)


if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)
endif()

# TODO: 如有需要，请添加测试并安装目标。
