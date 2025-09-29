# Находим зависимости
find_package(PkgConfig REQUIRED)
pkg_check_modules(ZMQ REQUIRED libzmq)
find_package(nlohmann_json 3.11.2 REQUIRED)

# Клиентское приложение
add_executable(student_client main_client.cpp)
target_link_libraries(student_client 
    zmq_subscriber 
    serializer 
    student 
    ${ZMQ_LIBRARIES}
)
target_include_directories(student_client PRIVATE 
    ${CMAKE_SOURCE_DIR}/common 
    ${ZMQ_INCLUDE_DIRS}
)