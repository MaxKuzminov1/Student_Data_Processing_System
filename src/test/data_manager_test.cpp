#include <iostream>
#include <vector>
#include "../server/data_manager.hpp"
#include "../common/student.hpp"

// Создаем тестовые данные
std::vector<Student> createTestList1() {
    return {
        Student(1, "Ivan", "Ivanov", "01.01.1990"),
        Student(2, "Petr", "Petrov", "15.05.1991"),
        Student(3, "Anna", "Sidorova", "20.03.1992"),
        Student(1, "Ivan", "Ivanov", "01.01.1990"), // Дубликат
        Student(4, "Maria", "Popova", "10.08.1993") // Без отчества
    };
}

std::vector<Student> createTestList2() {
    return {
        Student(5, "Sergey", "Sergeev",  "25.12.1989"),
        Student(2, "Petr", "Petrov", "15.05.1991"), // Дубликат
        Student(6, "Olga", "Orlova", "20.07.1990"), // Без отчества
        Student(7, "Ivan", "Ivanov", "01.01.1990"), // Дубликат
        Student(8, "Dmitry", "Dmitriev", "25.09.1994")
    };
}

std::vector<Student> createTestList3() {
    return {
        Student(9, "Elena", "Elenova", "30.11.1995"),
        Student(10, "Ivan", "Ivanov", "01.01.1990"), // Дубликат
        Student(11, "Alexey", "Alexeev", "15.02.1996")
    };
}

void testDataManager() {
    std::cout << "=== ТЕСТИРОВАНИЕ МОДУЛЯ ОБРАБОТКИ ДАННЫХ ===" << std::endl;
    
    DataManager manager;
    
    // Тест 1: Объединение двух списков
    std::cout << "\n1. ОБЪЕДИНЕНИЕ ДВУХ СПИСКОВ:" << std::endl;
    std::vector<Student> list1 = createTestList1();
    std::vector<Student> list2 = createTestList2();
    
    // Тест 3: Анализ данных
    std::cout << "\n3. АНАЛИЗ ДАННЫХ:" << std::endl;
    
    // Тест 5: Удаление дубликатов из одного списка
    std::cout << "\n5. УДАЛЕНИЕ ДУБЛИКАТОВ ИЗ ОДНОГО СПИСКА:" << std::endl;
    auto duplicatesList = createTestList1(); // Содержит дубликаты
    auto uniqueList = manager.removeDuplicates(duplicatesList);
    std::cout << "Было: " << duplicatesList.size() << " студентов" << std::endl;
    std::cout << "Стало: " << uniqueList.size() << " студентов" << std::endl;
    
    // Тест 6: Сортировка студентов
    std::cout << "\n6. СОРТИРОВКА СТУДЕНТОВ ПО ФИО:" << std::endl;
    auto unsortedList = createTestList2();
    auto sortedList = manager.sortStudentsByName(unsortedList);
    
    std::cout << "До сортировки:" << std::endl;
    for (const auto& student : unsortedList) {
        std::cout << "  ";
        student.print();
    }
    
    std::cout << "После сортировки:" << std::endl;
    for (const auto& student : sortedList) {
        student.print();
    }
}

int main() {
    testDataManager();
    return 0;
}