#include <iostream>
#include <vector>
#include <cassert>
#include "../common/serializer.hpp"
#include "../common/student.hpp"

void testStudentSerialization() {
    std::cout << "=== ТЕСТ СЕРИАЛИЗАЦИИ ОДНОГО СТУДЕНТА ===" << std::endl;
    
    // Студент с отчеством
    Student s1(1, "Ivan", "Ivanov", "01.01.1990");
    auto json1 = Serializer::studentToJson(s1);
    
    std::cout << "Студент с отчеством: " << json1.dump() << std::endl;
    assert(json1["id"] == 1);
    assert(json1["first_name"] == "Ivan");
    assert(json1["last_name"] == "Ivanov");
    assert(json1["birth_date"] == "01.01.1990");
    
    // Обратная десериализация
    Student s1_back = Serializer::jsonToStudent(json1);
    assert(s1 == s1_back);
    

    
    std::cout << "Все тесты пройдены успешно!" << std::endl;
}

void testStudentsListSerialization() {
    std::cout << "\n=== ТЕСТ СЕРИАЛИЗАЦИИ СПИСКА СТУДЕНТОВ ===" << std::endl;
    
    std::vector<Student> students = {
        Student(1, "Ivan", "Ivanov", "01.01.1990"),
        Student(2, "Petr", "Petrov", "15.05.1991"),
        Student(3, "Anna", "Sidorova","20.03.1992")
    };
    
    std::string json_str = Serializer::serializeStudents(students);
    std::cout << "Сериализованный список: " << std::endl;
    std::cout << json_str << std::endl;
    
    // Десериализация
    auto students_back = Serializer::deserializeStudents(json_str);
    assert(students.size() == students_back.size());
    
    for (size_t i = 0; i < students.size(); ++i) {
        assert(students[i] == students_back[i]);
    }
    
    std::cout << "Список успешно сериализован и десериализован!" << std::endl;
}

void testValidation() {
    std::cout << "\n=== ТЕСТ ВАЛИДАЦИИ JSON ===" << std::endl;
    
    // Валидный JSON
    json valid_json = {
        {"id", 1},
        {"first_name", "Ivan"},
        {"last_name", "Ivanov"},
        {"patronymic", "Ivanovich"},
        {"birth_date", "01.01.1990"}
    };
    
    assert(Serializer::validateStudentJson(valid_json) == true);
    
    // Невалидный JSON - отсутствует поле
    json invalid_json1 = {
        {"id", 1},
        {"first_name", "Ivan"},
        {"last_name", "Ivanov"}
        // Отсутствует patronymic и birth_date
    };
    
    assert(Serializer::validateStudentJson(invalid_json1) == false);
    
    // Невалидный JSON - неверный тип данных
    json invalid_json2 = {
        {"id", "not_a_number"}, // Должно быть число
        {"first_name", "Ivan"},
        {"last_name", "Ivanov"},
        {"patronymic", "Ivanovich"},
        {"birth_date", "01.01.1990"}
    };
    
    assert(Serializer::validateStudentJson(invalid_json2) == false);
    
    std::cout << "Валидация работает корректно!" << std::endl;
}

void testEdgeCases() {
    std::cout << "\n=== ТЕСТ ГРАНИЧНЫХ СЛУЧАЕВ ===" << std::endl;
    
    // Пустой список
    std::vector<Student> empty_list;
    std::string empty_json = Serializer::serializeStudents(empty_list);
    auto empty_back = Serializer::deserializeStudents(empty_json);
    assert(empty_back.empty());
    
    // Специальные символы в именах
    Student special(1, "Jean-Claude", "O'Connor", "01.01.1990");
    auto special_json = Serializer::studentToJson(special);
    auto special_back = Serializer::jsonToStudent(special_json);
    assert(special == special_back);
    
    std::cout << "Граничные случаи обработаны корректно!" << std::endl;
}

int main() {
    try {
        testStudentSerialization();
        testStudentsListSerialization();
        testValidation();
        testEdgeCases();
        
        std::cout << "\n=== ВСЕ ТЕСТЫ СЕРИАЛИЗАЦИИ ПРОЙДЕНЫ УСПЕШНО! ===" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Тест провален: " << e.what() << std::endl;
        return 1;
    }
}