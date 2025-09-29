# Находим зависимости
find_package(PkgConfig REQUIRED)
pkg_check_modules(ZMQ REQUIRED libzmq)
find_package(nlohmann_json 3.11.2 REQUIRED)

# Основной тест студента
add_executable(student_test student_test.cpp)
target_link_libraries(student_test student)

# Тест парсера
add_executable(parser_test data_parser_test.cpp)
target_link_libraries(parser_test data_parser student)

# Тест менеджера данных
add_executable(manager_test data_manager_test.cpp)
target_link_libraries(manager_test data_manager data_parser student)

# Тест сериализации
add_executable(serializer_test serializer_unit_test.cpp)
target_link_libraries(serializer_test serializer student nlohmann_json::nlohmann_json)

# Тест сетевых модулей
add_executable(network_test zmq_network_test.cpp)
target_link_libraries(network_test zmq_publisher zmq_subscriber serializer student ${ZMQ_LIBRARIES})
target_include_directories(network_test PRIVATE ${CMAKE_SOURCE_DIR}/common ${ZMQ_INCLUDE_DIRS})


# Группируем все тесты
add_custom_target(run_all_tests
    COMMAND echo "=== Запуск всех тестов ==="
    COMMAND ./student_test
    COMMAND echo "=== Тест студента завершен ==="
    COMMAND ./parser_test
    COMMAND echo "=== Тест парсера завершен ==="
    COMMAND ./manager_test
    COMMAND echo "=== Тест менеджера данных завершен ==="
    COMMAND ./serializer_test
    COMMAND echo "=== Тест сериализации завершен ==="
    COMMAND ./network_test
    COMMAND echo "=== Тест сети завершен ==="
    COMMAND echo "=== Все тесты завершены ==="
    DEPENDS student_test parser_test manager_test serializer_test network_test integration_test
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)