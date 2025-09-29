
/*
 * Клиент для просмотра данных студентов через ZeroMQ
 * Подключается к серверу и отображает полученные данные в табличном формате
 * 
 * Команды:
 * - p/print - показать текущие данные
 * - s/status - показать статус подключения
 * - q/quit - выход
 */

#include <iostream>
#include <thread>
#include <chrono>
#include <signal.h>
#include <mutex>
#include <iomanip>
#include "../common/zmq_subscriber.hpp"
#include "../common/student.hpp"
#include "../common/serializer.hpp"

class StudentClient {
private:
    std::atomic<bool> running_;
    ZmqSubscriber subscriber_;
    std::string server_endpoint_;
    std::vector<Student> received_students_;
    std::mutex data_mutex_;
    int update_count_;

public:
    StudentClient(const std::string& endpoint = "tcp://localhost:5556")
        : running_(false), server_endpoint_(endpoint), update_count_(0) {}

    ~StudentClient() {
        stop();
    }

    bool start() {
        if (running_) {
            std::cout << "Клиент уже запущен" << std::endl;
            return false;
        }

        std::cout << "ЗАПУСК КЛИЕНТА ДЛЯ ПРОСМОТРА СТУДЕНТОВ" << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << "Подключение к серверу: " << server_endpoint_ << std::endl;
        std::cout << std::endl;

        // Настраиваем callback функции
        subscriber_.setMessageCallback([this](const std::vector<Student>& students) {
            onStudentsReceived(students);
        });

        subscriber_.setRawMessageCallback([this](const std::string& message) {
            onRawMessageReceived(message);
        });

        subscriber_.setErrorCallback([this](const std::string& error) {
            onErrorReceived(error);
        });

        if (!subscriber_.start(server_endpoint_)) {
            std::cerr << "Ошибка подключения к серверу" << std::endl;
            return false;
        }

        running_ = true;
        update_count_ = 0;

        std::cout << "Клиент успешно подключен к серверу" << std::endl;
        std::cout << "Ожидание данных от сервера..." << std::endl;

        displayLoop();

        return true;
    }

    void stop() {
        if (running_) {
            running_ = false;
            subscriber_.stop();
            std::cout << "Клиент остановлен" << std::endl;
        }
    }

    bool isRunning() const {
        return running_;
    }

    void printCurrentData() {
        std::lock_guard<std::mutex> lock(data_mutex_);
        
        if (received_students_.empty()) {
            std::cout << "Данные не получены" << std::endl;
            return;
        }

        displayStudentsTable(received_students_);
    }

private:
    void onStudentsReceived(const std::vector<Student>& students) {
        std::lock_guard<std::mutex> lock(data_mutex_);
        
        received_students_ = students;
        update_count_++;

        std::cout << "Получено обновление #" << update_count_ << std::endl;
        std::cout << "   Количество студентов: " << students.size() << std::endl;
        std::cout << "   Статистика: " << subscriber_.getMessagesReceived() 
                  << " сообщений, " << subscriber_.getStudentsReceived() << " студентов" << std::endl;
        
        // Автоматически отображаем данные при первом получении
        if (update_count_ == 1) {
            std::cout << std::endl;
            displayStudentsTable(students);
        } else {
            std::cout << "   Для просмотра данных введите 'p'" << std::endl;
        }
    }

    void onRawMessageReceived(const std::string& message) {
        std::cout << "Получено сырое сообщение: " 
                  << message.substr(0, 100) 
                  << (message.length() > 100 ? "..." : "") << std::endl;
    }

    void onErrorReceived(const std::string& error) {
        std::cerr << "Ошибка: " << error << std::endl;
    }

    void displayStudentsTable(const std::vector<Student>& students) {
        if (students.empty()) {
            std::cout << "Нет данных для отображения" << std::endl;
            return;
        }

        std::cout << "ТАБЛИЦА СТУДЕНТОВ (" << students.size() << " записей)" << std::endl;
        std::cout << "==========================================================================================================" << std::endl;
        std::cout << std::left 
                  << std::setw(5) << "ID" 
                  << std::setw(20) << "Фамилия" 
                  << std::setw(15) << "Имя"  
                  << std::setw(15) << "Дата рождения"
                  << std::endl;
        std::cout << "----------------------------------------------------------------------------------------------------------" << std::endl;

        for (const Student& student : students) {
            std::cout << std::left 
                      << std::setw(5) << student.getId()

                      << std::setw(15) << (student.getFirstName().length() > 13 ? student.getFirstName().substr(0, 10) + "..." : student.getFirstName())
                                            << std::setw(20) << (student.getLastName().length() > 18 ? student.getLastName().substr(0, 15) + "..." : student.getLastName())
                      << std::setw(15) << student.getBirthDate()
                      << std::endl;
        }
        std::cout << "==========================================================================================================" << std::endl;
        
        // Статистика
        int valid_count = 0;
        for (const Student& student : students) {
            if (student.isValid()) valid_count++;
        }

        std::cout << "Статистика: " 
                  << "Валидных: " << valid_count << "/" << students.size()

                  << std::endl;
        std::cout << std::endl;
    }

    void displayLoop() {
        std::string command;
        
        while (running_) {
            std::cout << "Команды: [p]rint, [s]tatus, [q]uit: ";
            std::getline(std::cin, command);
            
            if (!running_) break;

            if (command == "p" || command == "print") {
                printCurrentData();
            } else if (command == "s" || command == "status") {
                printStatus();
            } else if (command == "q" || command == "quit") {
                std::cout << "Завершение работы..." << std::endl;
                break;
            } else if (!command.empty()) {
                std::cout << "Неизвестная команда. Доступные команды: p, s, q" << std::endl;
            }
        }
    }

    void printStatus() {
        std::lock_guard<std::mutex> lock(data_mutex_);
        
        std::cout << "СТАТУС КЛИЕНТА" << std::endl;
        std::cout << "==================" << std::endl;
        std::cout << "Подключение: " << (subscriber_.isRunning() ? "Активно" : "Неактивно") << std::endl;
        std::cout << "Получено обновлений: " << update_count_ << std::endl;
        std::cout << "Текущее количество студентов: " << received_students_.size() << std::endl;
        std::cout << "Всего сообщений: " << subscriber_.getMessagesReceived() << std::endl;
        std::cout << "Всего студентов: " << subscriber_.getStudentsReceived() << std::endl;
        std::cout << std::endl;
    }
};



void printUsage(const std::string& program_name) {
    std::cout << "Использование: " << program_name << " [опции]" << std::endl;
    std::cout << "Опции:" << std::endl;
    std::cout << "  -s, --server <адрес>    Адрес сервера (по умолчанию: tcp://localhost:5556)" << std::endl;
    std::cout << "  -h, --help              Показать эту справку" << std::endl;
}

int main(int argc, char* argv[]) {
    // Обработка аргументов командной строки
    std::string server_endpoint = "tcp://localhost:5556";

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "-s" || arg == "--server") {
            if (i + 1 < argc) {
                server_endpoint = argv[++i];
            } else {
                std::cerr << "Ошибка: отсутствует значение для " << arg << std::endl;
                return 1;
            }
        } else if (arg == "-h" || arg == "--help") {
            printUsage(argv[0]);
            return 0;
        } else {
            std::cerr << "Неизвестный аргумент: " << arg << std::endl;
            printUsage(argv[0]);
            return 1;
        }
    }


    // Создаем и запускаем клиент
    StudentClient client(server_endpoint);
    
    if (!client.start()) {
        std::cerr << "Не удалось запустить клиент" << std::endl;
        return 1;
    }

    // Ожидаем сигнал остановки
    while (client.isRunning()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // Останавливаем клиент
    client.stop();

    std::cout << "Клиент завершил работу" << std::endl;
    return 0;
}