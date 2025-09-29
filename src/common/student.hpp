#ifndef STUDENT_HPP
#define STUDENT_HPP

#include <string>
#include <iostream>
#include <ctime>
#include <chrono>
#include <iomanip>

class Student {
private:
    int id;
    std::string lastName;
    std::string firstName;
    std::string birthDate; // format: DD.MM.YYYY
    std::tm birthDateTm;


    bool validateName(const std::string& name) const;
    bool validateDate(const std::string& date);
    bool parseDate(const std::string& date, std::tm& result) const;

public:
    Student();
    Student(int id, const std::string& firstName, const std::string& lastName,
            const std::string& birthDate);

    int getId() const { return id; }
    std::string getFirstName() const { return firstName; }
    std::string getLastName() const { return lastName; }
    std::string getBirthDate() const { return birthDate; }
    std::string getFullName() const { 
        return lastName + firstName;
    }

    bool setId(int newId);
    bool setFirstName(const std::string& name);
    bool setLastName(const std::string& name);
    bool setBirthDate(const std::string& date);

    bool isValid() const;

    bool operator==(const Student& other) const; // search for duplicate(FIO + date)
    bool operator<(const Student& other) const;  // sorting by FIO
    
    //helpers
    void print() const;
    std::string toString() const;
};


struct StudentNameComparator {
    bool operator()(const Student& a, const Student& b) const {
        return a < b;
    }
};

#endif