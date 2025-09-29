# Находим зависимости
find_package(PkgConfig REQUIRED)
pkg_check_modules(ZMQ REQUIRED libzmq)
find_package(nlohmann_json 3.11.2 REQUIRED)

# Библиотека DataParser
add_library(data_parser data_parser.cpp)
target_link_libraries(data_parser student)
target_include_directories(data_parser PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/common
)

# Библиотека DataManager
add_library(data_manager data_manager.cpp)
target_link_libraries(data_manager student)
target_include_directories(data_manager PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/common
)

# Основной сервер
add_executable(student_server main_server.cpp)
target_link_libraries(student_server 
    data_parser 
    data_manager 
    zmq_publisher 
    serializer 
    student 
    ${ZMQ_LIBRARIES}
)
target_include_directories(student_server PRIVATE 
    ${CMAKE_SOURCE_DIR}/common 
    ${ZMQ_INCLUDE_DIRS}
)
