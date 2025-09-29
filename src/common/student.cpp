/*
 * Класс Student представляет данные студента
 * Включает валидацию данных и преобразование дат
 */

#include "student.hpp"
#include <cstring>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <iomanip>

Student::Student() : id(0), firstName(""), lastName(""), birthDate("") {
    std::memset(&birthDateTm, 0, sizeof(birthDateTm));
}

Student::Student(int id, const std::string& firstName, const std::string& lastName, const std::string& birthDate) 
    : id(id), firstName(firstName), lastName(lastName), birthDate(birthDate) {
    std::memset(&birthDateTm, 0, sizeof(birthDateTm));
    if (!parseDate(birthDate, birthDateTm)) {
        this->birthDate = "";
    }
}

bool Student::validateName(const std::string& name) const {
    
    for (char c : name) {
        if (!std::isalpha(c) && c != '-' && c != ' ' && c != '.') {
            return false;
        }
    }
    return true;
}

bool Student::parseDate(const std::string& date, std::tm& result) const {
    std::memset(&result, 0, sizeof(result));
    
    int day, month, year;
    char dot1, dot2;
    std::istringstream ss(date);
    
    if (!(ss >> day >> dot1 >> month >> dot2 >> year)) {
        return false;
    }
    
    if (dot1 != '.' || dot2 != '.') {
        return false;
    }
    
    result.tm_mday = day;
    result.tm_mon = month - 1;
    result.tm_year = year - 1900;
    
    std::tm check = result;
    std::time_t temp = std::mktime(&check);
    
    if (temp == -1) {
        return false;
    }
    
    if (check.tm_mday != day || check.tm_mon != month - 1 || check.tm_year != year - 1900) {
        return false;
    }
    
    return true;
}

bool Student::validateDate(const std::string& date) {
    std::tm parsedDate;
    if (parseDate(date, parsedDate)) {
        birthDateTm = parsedDate;
        birthDate = date;
        return true;
    }
    
    return false;
}

bool Student::setId(int newId) {
    if (newId >= 0) {
        id = newId;
        return true;
    }
    return false;
}

bool Student::setFirstName(const std::string& name) {
    if (validateName(name) && !name.empty()){
        firstName = name;
        return true;
    }
    return false;
}

bool Student::setLastName(const std::string& name) {
    if (validateName(name) && !name.empty()) {
        lastName = name;
        return true;
    }
    return false;
}

bool Student::setBirthDate(const std::string& date) {
    return validateDate(date);
}

bool Student::isValid() const {
    return id >= 0 && 
           !firstName.empty() && 
           !lastName.empty() && 
           !birthDate.empty() &&
           validateName(firstName) && 
           validateName(lastName) && 
           birthDateTm.tm_year != 0;
}

bool Student::operator==(const Student& other) const {
    
    return lastName == other.lastName &&
           firstName == other.firstName &&
           birthDate == other.birthDate;
}

bool Student::operator<(const Student& other) const {
    if (lastName != other.lastName) {
        return lastName < other.lastName;
    }
    return firstName < other.firstName;
    
}

void Student::print() const {
    std::cout << toString() << std::endl;
}

std::string Student::toString() const {
    std::stringstream ss;
    ss << "ID: " << id 
       << ", ФИО: " << lastName << " " << firstName;
    
    ss << ", Дата рождения: " << birthDate
       << " [" << (isValid() ? "VALID" : "INVALID") << "]";
    return ss.str();
}