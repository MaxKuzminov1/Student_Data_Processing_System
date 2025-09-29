#include "data_parser.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cctype>

// Вспомогательная функция для трима строки
std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) return "";
    
    size_t end = str.find_last_not_of(" \t\n\r");
    return str.substr(start, end - start + 1);
}

DataParser::ParseResult DataParser::parseStudentFile(const std::string& filename) {
    ParseResult result;
    
    std::ifstream file(filename);
    if (!file.is_open()) {
        result.errors.push_back("Ошибка: Не удалось открыть файл " + filename);
        return result;
    }
    
    std::string line;
    int lineNumber = 0;
    
    while (std::getline(file, line)) {
        lineNumber++;
        result.totalLines++;
        
        // Пропускаем пустые строки и комментарии
        std::string cleanedLine = cleanLine(line);
        if (cleanedLine.empty()) {
            continue;
        }
        
        // Парсим строку
        std::string errorMessage;
        std::unique_ptr<Student> student= parseLine(cleanedLine, lineNumber, errorMessage);
        
        if (student && student->isValid()) {
            result.students.push_back(*student);
            result.validLines++;
        } else {
            result.errors.push_back("Файл " + filename + ", строка " + 
                                  std::to_string(lineNumber) + ": " + errorMessage);
        }
    }
    
    file.close();
    return result;
}

std::vector<Student> DataParser::parseMultipleFiles(const std::vector<std::string>& filenames) {
    std::vector<Student> allStudents;
    
    for (const std::string &filename : filenames) {
        ParseResult result = parseStudentFile(filename);
        
        // Выводим статистику по каждому файлу
        printParseStatistics(result, filename);
        
        // Добавляем валидных студентов
        allStudents.insert(allStudents.end(), result.students.begin(), result.students.end());
        
        // Выводим ошибки
        for (const std::string &error : result.errors) {
            std::cerr << "⚠️  " << error << std::endl;
        }
    }
    
    return allStudents;
}

std::string DataParser::cleanLine(const std::string& line) {
    std::string cleaned = trim(line);
    
    // Пропускаем строки, начинающиеся с комментариев
    if (cleaned.empty() || cleaned[0] == '#' || cleaned[0] == ';') {
        return "";
    }
    
    // Удаляем лишние пробелы между словами
    std::stringstream ss(cleaned);
    std::string token, result;
    while (ss >> token) {
        if (!result.empty()) result += " ";
        result += token;
    }
    
    return result;
}

std::vector<std::string> DataParser::tokenizeLine(const std::string& line, int lineNumber, std::string& errorMessage) {
    std::vector<std::string> tokens;
    std::stringstream ss(line);
    std::string token;
    
    while (ss >> token) {
        tokens.push_back(token);
    }
        // Проверяем минимальное количество токенов (ID, Фамилия, Имя, Дата)
    if (tokens.size() < 4) {
        errorMessage = "Недостаточно данных. Ожидается: ID Фамилия Имя [Отчество] ДатаРождения";
        return {};
    }
    
    // Обрабатываем разные случаи количества токенов
    if (tokens.size() == 4) {
        // Нет отчества: ID Фамилия Имя Дата
        tokens.insert(tokens.begin() + 3, ""); // Вставляем пустое отчество
    } else if (tokens.size() == 5) {
        // Есть отчество: ID Фамилия Имя Отчество Дата
        // Ничего не меняем
    } else if (tokens.size() > 5) {
        // Если больше 5 токенов, возможно, в ФИО есть пробелы
        // Объединяем токены с 3 по предпоследний как отчество
        std::string patronymic = tokens[3];
        for (size_t i = 4; i < tokens.size() - 1; ++i) {
            patronymic += " " + tokens[i];
        }
        tokens[3] = patronymic;
        tokens.resize(5); // Оставляем только 5 токенов
    }
    
    return tokens;
}

std::unique_ptr<Student> DataParser::parseLine(const std::string& line, int lineNumber, std::string& errorMessage) {

    std::vector<std::string> tokens = tokenizeLine(line, lineNumber, errorMessage);
    if (tokens.empty()) {
        return nullptr;
    }

    int id;
    try {
        id = std::stoi(tokens[0]);
        if (id < 0) {
            errorMessage = "ID не может быть отрицательным: " + tokens[0];
            return nullptr;
        }
    } catch (const std::exception& e) {
        errorMessage = "Неверный формат ID: " + tokens[0];
        return nullptr;
    }
    

    std::string lastName = tokens[1];
    std::string firstName = tokens[2];
    std::string patronymic = tokens[3];
    std::string birthDate = tokens[4];
    
    return createStudent(id, firstName, lastName, patronymic, birthDate, errorMessage);
}

std::unique_ptr<Student> DataParser::createStudent(int id, const std::string& firstName, 
                                                 const std::string& lastName, const std::string& patronymic,
                                                 const std::string& birthDate, std::string& errorMessage) {
    std::unique_ptr<Student> student = std::make_unique<Student>();
    
    if (!student->setId(id)) {
        errorMessage = "Неверный ID: " + std::to_string(id);
        return nullptr;
    }
    
    if (!student->setLastName(lastName)) {
        errorMessage = "Неверная фамилия: '" + lastName + "'";
        return nullptr;
    }
    
    if (!student->setFirstName(firstName)) {
        errorMessage = "Неверное имя: '" + firstName + "'";
        return nullptr;
    }
    
    if (!student->setBirthDate(birthDate)) {
        errorMessage = "Неверная дата рождения: '" + birthDate + "'";
        return nullptr;
    }
    
    if (!student->isValid()) {
        errorMessage = "Невалидные данные студента";
        return nullptr;
    }
    
    return student;
}

void DataParser::printParseStatistics(const ParseResult& result, const std::string& filename) const {
    std::cout << "   Результаты парсинга файла: " << filename << std::endl;
    std::cout << "   Всего строк: " << result.totalLines << std::endl;
    std::cout << "   Валидных записей: " << result.validLines << std::endl;
    std::cout << "   Ошибок: " << result.errors.size() << std::endl;
    
    if (result.validLines > 0) {
        std::cout << "   Успешно загружено студентов: " << result.students.size() << std::endl;
    }
    std::cout << std::endl;
}