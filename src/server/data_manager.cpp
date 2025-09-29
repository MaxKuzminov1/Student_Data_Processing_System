/*
 * Менеджер данных для обработки списков студентов
 * Включает объединение, удаление дубликатов и сортировку
 */

#include "data_manager.hpp"
#include <iostream>
#include <unordered_set>
#include <functional>

DataManager::MergeResult DataManager::mergeStudents(const std::vector<Student>& list1, const std::vector<Student>& list2) {
    MergeResult result;
    
    std::vector<Student> allStudents;
    allStudents.reserve(list1.size() + list2.size());
    allStudents.insert(allStudents.end(), list1.begin(), list1.end());
    allStudents.insert(allStudents.end(), list2.begin(), list2.end());
    
    result.totalStudents = allStudents.size();
    
    result.mergedStudents = removeDuplicates(allStudents);
    result.duplicatesRemoved = result.totalStudents - result.mergedStudents.size();
    result.duplicates = findDuplicates(allStudents);
    
    result.mergedStudents = sortStudentsByName(result.mergedStudents);
    
    return result;
}

DataManager::MergeResult DataManager::mergeMultipleLists(const std::vector<std::vector<Student>>& lists) {
    MergeResult result;
    
    if (lists.empty()) {
        return result;
    }
    
    std::vector<Student> allStudents;
    for (const & list : lists) {
        allStudents.insert(allStudents.end(), list.begin(), list.end());
        result.totalStudents += list.size();
    }
    
    result.mergedStudents = removeDuplicates(allStudents);
    result.duplicatesRemoved = result.totalStudents - result.mergedStudents.size();
    result.duplicates = findDuplicates(allStudents);
    
    result.mergedStudents = sortStudentsByName(result.mergedStudents);
    
    return result;
}

std::vector<Student> DataManager::removeDuplicates(const std::vector<Student>& students) {
    std::vector<Student> uniqueStudents;
    
     studentHash = [](const Student& s) {
        return std::hash<std::string>{}(s.getLastName() + s.getFirstName() + 
                                       s.getBirthDate());
    };
    
     studentEqual = [](const Student& s1, const Student& s2) {
        return s1 == s2;
    };
    
    std::unordered_set<Student, decltype(studentHash), decltype(studentEqual)> 
        seenStudents(students.size(), studentHash, studentEqual);
    
    for (const & student : students) {
        if (seenStudents.insert(student).second) {
            uniqueStudents.push_back(student);
        }
    }
    
    return uniqueStudents;
}

std::vector<Student> DataManager::sortStudentsByName(const std::vector<Student>& students) {
    std::vector<Student> sortedStudents = students;
    std::sort(sortedStudents.begin(), sortedStudents.end());
    return sortedStudents;
}

std::vector<Student> DataManager::filterValidStudents(const std::vector<Student>& students) {
    std::vector<Student> validStudents;
    
    for (const & student : students) {
        if (student.isValid()) {
            validStudents.push_back(student);
        }
    }
    
    return validStudents;
}

bool DataManager::isDuplicate(const Student& s1, const Student& s2) const {
    return s1 == s2;
}

std::vector<Student> DataManager::findDuplicates(const std::vector<Student>& students) {
    std::vector<Student> duplicates;
    std::vector<bool> isDuplicate(students.size(), false);
    
    for (size_t i = 0; i < students.size(); ++i) {
        if (isDuplicate[i]) continue;
        
        for (size_t j = i + 1; j < students.size(); ++j) {
            if (students[i] == students[j]) {
                if (!isDuplicate[i]) {
                    duplicates.push_back(students[i]);
                    isDuplicate[i] = true;
                }
                duplicates.push_back(students[j]);
                isDuplicate[j] = true;
            }
        }
    }
    
    return duplicates;
}


