#ifndef DATA_MANAGER_HPP
#define DATA_MANAGER_HPP

#include <vector>
#include <algorithm>
#include <memory>
#include "../common/student.hpp"

class DataManager {
public:
    // Результат обработки данных
    struct MergeResult {
        std::vector<Student> mergedStudents;
        int totalStudents;
        int duplicatesRemoved;
        std::vector<Student> duplicates;
        
        MergeResult() : totalStudents(0), duplicatesRemoved(0) {}
    };

    // Объединение списков с удалением дубликатов
    MergeResult mergeStudents(const std::vector<Student>& list1, const std::vector<Student>& list2);
    
    // Объединение нескольких списков
    MergeResult mergeMultipleLists(const std::vector<std::vector<Student>>& lists);
    
    // Дополнительные функции обработки
    std::vector<Student> removeDuplicates(const std::vector<Student>& students);
    std::vector<Student> sortStudentsByName(const std::vector<Student>& students);
    std::vector<Student> filterValidStudents(const std::vector<Student>& students);
    
    // Статистика
    //void printMergeStatistics(const MergeResult& result) const;


private:
    // Вспомогательные функции
    bool isDuplicate(const Student& s1, const Student& s2) const;
    std::vector<Student> findDuplicates(const std::vector<Student>& students);
};

#endif // DATA_MANAGER_HPP