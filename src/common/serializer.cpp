/*
 * Сериализатор/десериализатор для преобразования объектов Student в JSON и обратно
 * Использует библиотеку nlohmann/json для работы с JSON
 */

#include "serializer.hpp"
#include <sstream>

const std::string Serializer::STUDENTS_KEY = "students";
const std::string Serializer::ID_KEY = "id";
const std::string Serializer::FIRST_NAME_KEY = "first_name";
const std::string Serializer::LAST_NAME_KEY = "last_name";
const std::string Serializer::BIRTH_DATE_KEY = "birth_date";

json Serializer::studentToJson(const Student& student) {
    json j;
    j[ID_KEY] = student.getId();
    j[FIRST_NAME_KEY] = student.getFirstName();
    j[LAST_NAME_KEY] = student.getLastName();
    j[BIRTH_DATE_KEY] = student.getBirthDate();
    return j;
}

Student Serializer::jsonToStudent(const json& j) {
    if (!validateStudentJson(j)) {
        throw std::invalid_argument("Invalid student JSON");
    }
    
    int id = j[ID_KEY].get<int>();
    std::string firstName = j[FIRST_NAME_KEY].get<std::string>();
    std::string lastName = j[LAST_NAME_KEY].get<std::string>();
    std::string birthDate = j[BIRTH_DATE_KEY].get<std::string>();
    
    return Student(id, firstName, lastName, birthDate);
}

std::string Serializer::serializeStudents(const std::vector<Student>& students) {
    json j;
    j[STUDENTS_KEY] = json::array();
    
    for (const Student& student : students) {
        j[STUDENTS_KEY].push_back(studentToJson(student));
    }
    
    // Красивое форматирование для отладки, в production можно использовать j.dump()
    return j.dump(4);
}

std::vector<Student> Serializer::deserializeStudents(const std::string& data) {
    std::vector<Student> students;
    
    try {
        json j = json::parse(data);
        
        if (!j.contains(STUDENTS_KEY) || !j[STUDENTS_KEY].is_array()) {
            throw std::invalid_argument("Invalid students JSON format");
        }
        
        for (const nlohmann::json_abi_v3_11_3::json &studentJson : j[STUDENTS_KEY]) {
            if (validateStudentJson(studentJson)) {
                students.push_back(jsonToStudent(studentJson));
            }
        }
    } catch (const json::parse_error& e) {
        throw std::invalid_argument("JSON parse error: " + std::string(e.what()));
    }
    
    return students;
}

bool Serializer::validateStudentJson(const json& j) {
    return j.contains(ID_KEY) && j[ID_KEY].is_number_integer() &&
           j.contains(FIRST_NAME_KEY) && j[FIRST_NAME_KEY].is_string() &&
           j.contains(LAST_NAME_KEY) && j[LAST_NAME_KEY].is_string() &&
           j.contains(BIRTH_DATE_KEY) && j[BIRTH_DATE_KEY].is_string();
}