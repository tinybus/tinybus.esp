cmake_minimum_required(VERSION 3.10)

set(EXTRA_COMPONENT_DIRS "../../../")
set(EXCLUDE_COMPONENTS examples test_app test)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(tinybus_base_cpp)
