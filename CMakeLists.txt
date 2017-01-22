cmake_minimum_required(VERSION 2.4.0)

project(srf)

# Find source files
file(GLOB SOURCES src/*.c src/formats/*.c)

# Include header files
include_directories(include)

# Create shared library
add_library(${PROJECT_NAME} SHARED ${SOURCES})

# Install library
install(TARGETS ${PROJECT_NAME} DESTINATION lib/${PROJECT_NAME})

# Install library headers
# file(GLOB HEADERS include/*.h)
install(FILES include/libsrf.h DESTINATION include/${PROJECT_NAME}) 
