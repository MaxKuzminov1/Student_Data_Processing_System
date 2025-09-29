#include <iostream>
#include <vector>
#include <algorithm>
#include "../common/student.hpp"

void testStudent() {
    std::cout << "=== ТЕСТИРОВАНИЕ КЛАССА STUDENT ===" << std::endl;
    
    // Тест 1: Создание валидного студента
    std::cout << "\n1. Валидный студент:" << std::endl;
    Student s1(1, "Ivanov", "Ivan", "01.01.1990");
    s1.print();
    
    // Тест 2: Создание невалидного студента
    std::cout << "\n2. Невалидный студент (неправильная дата):" << std::endl;
    Student s2(2, "Petrov", "Petr", "32.13.1990");
    s2.print();

    // Тест 3: Создание невалидного студента
    std::cout << "\n3. Невалидный студент (неправильная дата):" << std::endl;
    Student s3(3, "Ivanov", "Ivan", "01.01.2110");
    s3.print();
    
    // Тест 4: Создание валидного студента (29.02.2000 - високосный год)
    std::cout << "\n4. Валидный студент (високосный год):" << std::endl;
    Student s4(4, "Petrov", "Petr", "29.02.2000");
    s4.print();

    // Тест 5: Создание валидного студента
    std::cout << "\n5. Валидный студент:" << std::endl;
    Student s5(5, "Ivanov", "Ivan", "01.01.1900");
    s5.print();
    
    // Тест 6: Создание невалидного студента
    std::cout << "\n6. Невалидный студент (неправильная дата):" << std::endl;
    Student s6(6, "Petrov", "Petr", "31.04.2000");
    s6.print();
    
    // Тест 7: Сравнение студентов
    std::cout << "\n7. Сравнение студентов:" << std::endl;
    Student s7(7, "Ivanov", "Ivan", "01.01.1990");
    Student s8(8, "Ivanov", "Ivan", "01.01.1990"); // Дубликат
    Student s9(9, "Sidorov", "Alexey", "15.05.1991");
    
    std::cout << "s1 == s7: " << (s1 == s7) << std::endl;
    std::cout << "s7 == s8: " << (s7 == s8) << std::endl;
    std::cout << "s1 == s9: " << (s1 == s9) << std::endl;
    
    // Тест 8: Сортировка по ФИО
    std::cout << "\n8. Сортировка студентов по ФИО:" << std::endl;
    std::vector<Student> students = {
        Student(1, "Petrov", "Petr", "10.10.1990"),
        Student(2, "Ivanov", "Ivan", "01.01.1990"),
        Student(3, "Sidorova", "Anna", "15.05.1991"),
        Student(4, "Alexeev", "Alexey", "20.03.1989")
    };
    
    std::cout << "До сортировки:" << std::endl;
    for (const auto& s : students) {
        s.print();
    }
    
    std::sort(students.begin(), students.end());
    
    std::cout << "\nПосле сортировки:" << std::endl;
    for (const auto& s : students) {
        s.print();
    }
    
    // Тест 9: Валидация данных через сеттеры
    std::cout << "\n9. Тест валидации через сеттеры:" << std::endl;
    Student s10(10, "Smirnov", "Sergey", "01.01.1995");
    std::cout << "Установка валидной фамилии: " << s10.setLastName("Kuznetsova") << std::endl;
    std::cout << "Установка невалидной фамилии: " << s10.setLastName("Kuznetsov123") << std::endl;
    std::cout << "Установка валидного имени: " << s10.setFirstName("Maria") << std::endl;
    std::cout << "Установка невалидного имени: " << s10.setFirstName("Mari4a") << std::endl;
    std::cout << "Установка валидной даты: " << s10.setBirthDate("25.12.1992") << std::endl;
    std::cout << "Установка невалидной даты: " << s10.setBirthDate("40.12.1992") << std::endl;
    
    // Тест 10: Вывод студента после изменений
    std::cout << "\n10. Студент после изменений:" << std::endl;
    s10.print();
}

int main() {
    testStudent();
    return 0;
}