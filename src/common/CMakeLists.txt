# Находим пакеты
find_package(PkgConfig REQUIRED)
pkg_check_modules(ZMQ REQUIRED libzmq)
find_package(nlohmann_json 3.11.2 REQUIRED)

# Библиотека Student
add_library(student student.cpp)
target_include_directories(student PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Библиотека Serializer
add_library(serializer serializer.cpp)
target_link_libraries(serializer student nlohmann_json::nlohmann_json)
target_include_directories(serializer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Библиотека ZMQ Publisher
add_library(zmq_publisher zmq_publisher.cpp)
target_link_libraries(zmq_publisher serializer student ${ZMQ_LIBRARIES})
target_include_directories(zmq_publisher PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${ZMQ_INCLUDE_DIRS})

# Библиотека ZMQ Subscriber
add_library(zmq_subscriber zmq_subscriber.cpp)
target_link_libraries(zmq_subscriber serializer student ${ZMQ_LIBRARIES})
target_include_directories(zmq_subscriber PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${ZMQ_INCLUDE_DIRS})