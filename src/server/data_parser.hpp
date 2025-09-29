#ifndef DATA_PARSER_HPP
#define DATA_PARSER_HPP

#include <string>
#include <vector>
#include <memory>
#include <fstream> 
#include "../common/student.hpp"

class DataParser {
public:
    struct ParseResult {
        std::vector<Student> students;
        std::vector<std::string> errors;
        int totalLines;
        int validLines;
        
        ParseResult() : totalLines(0), validLines(0) {}
    };

    ParseResult parseStudentFile(const std::string& filename);
    
    std::vector<Student> parseMultipleFiles(const std::vector<std::string>& filenames);
    
    void printParseStatistics(const ParseResult& result, const std::string& filename) const;

private:
    std::unique_ptr<Student> parseLine(const std::string& line, int lineNumber, std::string& errorMessage);
    
    std::unique_ptr<Student> createStudent(int id, const std::string& firstName, 
                                         const std::string& lastName, const std::string& patronymic,
                                         const std::string& birthDate, std::string& errorMessage);
    
    std::string cleanLine(const std::string& line);
    
    std::vector<std::string> tokenizeLine(const std::string& line, int lineNumber, std::string& errorMessage);
};

#endif // DATA_PARSER_HPP