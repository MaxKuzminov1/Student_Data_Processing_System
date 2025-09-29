/*
 * Сервер обработки данных студентов
 * Загружает данные из файлов, обрабатывает и публикует через ZeroMQ
 */

#include <iostream>
#include <thread>
#include <chrono>
#include <unordered_map>
#include <filesystem>
#include <fstream>
#include <vector>
#include "data_parser.hpp"
#include "data_manager.hpp"
#include "../common/zmq_publisher.hpp"

namespace fs = std::filesystem;

class StudentServer {
private:
    DataParser parser_;
    DataManager data_manager_;
    ZmqPublisher publisher_;
    std::string database_dir_;
    std::unordered_map<std::string, fs::file_time_type> file_timestamps_;
    bool first_run_ = true;   // <-- флаг для первой отправки

    // Проверяет новые или изменённые файлы и возвращает список их путей
    std::vector<std::string> detectChangedFiles() {
        std::vector<std::string> changed_files;

        for (const class std::filesystem::__cxx11::directory_entry& entry : fs::directory_iterator(database_dir_)) {
            if (!entry.is_regular_file()) continue;

            std::filesystem::__cxx11::path path = entry.path();
            if (path.extension() != ".txt") continue;
            if (path.filename().string().find("student_file_") != 0) continue;

            std::filesystem::file_time_type last_write = fs::last_write_time(path);
            std::string file_path = path.string();

            if (!file_timestamps_.count(file_path) ||
                file_timestamps_[file_path] != last_write)
            {
                file_timestamps_[file_path] = last_write;
                changed_files.push_back(file_path);
            }
        }

        return changed_files;
    }

    void processAndPublish(const std::vector<std::string>& files) {
        if (files.empty()) return;

        // Если это первая отправка – даём клиентам 5 секунд на подключение
        if (first_run_) {
            std::cout << "\n[INFO] Первая отправка. Ожидание 5 секунд для подключения клиентов..." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(5));
            first_run_ = false;
        }

        std::cout << "\n[INFO] Изменения обнаружены, обработка файлов..." << std::endl;

        std::vector<Student> students = parser_.parseMultipleFiles(files);
        if (students.empty()) {
            std::cerr << "[WARN] Не удалось загрузить данные из изменённых файлов" << std::endl;
            return;
        }

        students = data_manager_.filterValidStudents(students);
        students = data_manager_.removeDuplicates(students);
        students = data_manager_.sortStudentsByName(students);

        std::cout << "[INFO] Отправка данных. Студентов: " << students.size() << std::endl;
        publisher_.publish(students);
    }

public:
    StudentServer(const std::string& database_dir = "../database")
        : database_dir_(database_dir) {}

    bool run() {
        std::cout << "Сервер обработки студентов" << std::endl;
        std::cout << "==========================" << std::endl;

        if (!publisher_.start("tcp://*:5556")) {
            std::cerr << "[ERROR] Ошибка запуска ZMQ publisher" << std::endl;
            return false;
        }

        std::cout << "[INFO] Мониторинг директории: " << database_dir_ << std::endl;

        while (true) {
            try {
                std::vector<std::string> changed_files = detectChangedFiles();
                processAndPublish(changed_files);
            } catch (const std::exception& ex) {
                std::cerr << "[ERROR] " << ex.what() << std::endl;
            }

            // Проверяем папку каждые 2 секунды
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }

        publisher_.stop();
        return true;
    }
};

int main() {
    StudentServer server;
    return server.run() ? 0 : 1;
}