cmake_minimum_required(VERSION 3.23)
project(code C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_executable(knn distance.c ioutils.c knn.c)
target_link_libraries(knn m)

add_executable(kmeans distance.c ioutils.c kmeans.c)
target_link_libraries(kmeans m)

add_custom_command(
        TARGET knn POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/agk-ue08-p1.csv ${CMAKE_SOURCE_DIR}/iris.csv ${CMAKE_SOURCE_DIR}/mnist.csv ${CMAKE_SOURCE_DIR}/mnist-pca.csv
        ${CMAKE_CURRENT_BINARY_DIR})
add_custom_command(
        TARGET kmeans POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/supersimplistic.csv ${CMAKE_SOURCE_DIR}/mouse.csv ${CMAKE_SOURCE_DIR}/mnist-pca-short.csv ${CMAKE_SOURCE_DIR}/mnist-pca.csv
        ${CMAKE_CURRENT_BINARY_DIR})
