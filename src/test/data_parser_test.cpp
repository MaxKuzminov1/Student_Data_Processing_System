#include <iostream>
#include <vector>
#include "../server/data_parser.hpp"
#include "../common/student.hpp"

void createTestFiles() {
    // Создаем тестовые файлы
    std::ofstream file1("student_file_1.txt");
    file1 << "1 Ivan Ivanovich 01.01.1988\n";
    file1 << "2 Petr Petrov 04.07.1988\n";
    file1 << "3 Denis Denisov 43.01.1988\n"; // Невалидная дата
    file1 << "4 Vladimir Jukov 04.5.1987\n";
    file1 << "5 Vladimir Kochkin 11.10.1989\n";
    file1 << "6 Test User With Very Long Patronymic Name 15.03.1990\n"; // Длинное отчество
    file1.close();
    
    std::ofstream file2("student_file_2.txt");
    file2 << "34 Ivan Ivanovich 01.01.1988\n";
    file2 << "54 Petr Kazakov 05.07.1986\n";
    file2 << "23 Denis Denisov 04.03.1988\n";
    file2 << "12 Vladimir Jukov 04.5.1987\n";
    file2 << "43 Vladimir Kochkin 11.11.1989\n";
    file2 << "99 Invalid Date Student 32.13.1990\n"; // Невалидная дата
    file2 << "100 Short Data\n"; // Недостаточно данных
    file2.close();
}

void testDataParser() {
    std::cout << "=== ТЕСТИРОВАНИЕ ПАРСЕРА ДАННЫХ ===" << std::endl;
    
    // Создаем тестовые файлы
    createTestFiles();
    
    DataParser parser;
    
    // Тест 1: Парсинг одного файла
    std::cout << "\n1. Парсинг одного файла:" << std::endl;
    auto result1 = parser.parseStudentFile("student_file_1.txt");
    
    std::cout << "Найдено студентов: " << result1.students.size() << std::endl;
    for (const auto& student : result1.students) {
        student.print();
    }
    
    // Тест 2: Парсинг нескольких файлов
    std::cout << "\n2. Парсинг нескольких файлов:" << std::endl;
    std::vector<std::string> filenames = {"../database/student_file_1.txt", "../database/student_file_2.txt"};
    auto allStudents = parser.parseMultipleFiles(filenames);
    
    std::cout << "Всего загружено студентов: " << allStudents.size() << std::endl;
    
    // Тест 3: Вывод первых нескольких студентов
    std::cout << "\n3. Первые 5 загруженных студентов:" << std::endl;
    for (size_t i = 0; i < std::min(allStudents.size(), size_t(5)); ++i) {
        allStudents[i].print();
    }
}

int main() {
    testDataParser();
    return 0;
}