#ifndef SERIALIZER_HPP
#define SERIALIZER_HPP

#include <string>
#include <vector>
#include "student.hpp"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class Serializer {
public:
    // Сериализация одного студента в JSON
    static json studentToJson(const Student& student);
    
    // Десериализация JSON в студента
    static Student jsonToStudent(const json& j);
    
    // Сериализация списка студентов в JSON строку
    static std::string serializeStudents(const std::vector<Student>& students);
    
    // Десериализация JSON строки в список студентов
    static std::vector<Student> deserializeStudents(const std::string& data);
    
    // Валидация JSON данных
    static bool validateStudentJson(const json& j);

private:
    static const std::string STUDENTS_KEY;
    static const std::string ID_KEY;
    static const std::string FIRST_NAME_KEY;
    static const std::string LAST_NAME_KEY;
    static const std::string BIRTH_DATE_KEY;
};

#endif // SERIALIZER_HPP