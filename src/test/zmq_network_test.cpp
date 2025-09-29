#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include <vector>
#include "../common/zmq_publisher.hpp"
#include "../common/zmq_subscriber.hpp"
#include "../common/student.hpp"
#include "../common/serializer.hpp"

class NetworkTest {
private:
    std::atomic<bool> message_received_;
    std::atomic<bool> raw_message_received_;
    std::vector<Student> received_students_;
    std::string received_raw_message_;
    std::mutex data_mutex_;

public:
    NetworkTest() : message_received_(false), raw_message_received_(false) {}

    void runTests() {
        std::cout << "=== ТЕСТИРОВАНИЕ СЕТЕВЫХ МОДУЛЕЙ ZMQ ===" << std::endl;
        
        testSerializer();
        testPublisherSubscriber();
        testStress();
        testErrorHandling();
        
        std::cout << "=== ВСЕ ТЕСТЫ ЗАВЕРШЕНЫ ===" << std::endl;
    }

private:
    void testSerializer() {
        std::cout << "\n1. ТЕСТ СЕРИАЛИЗАЦИИ:" << std::endl;
        
        // Создаем тестовых студентов
        Student s1(1, "Ivan", "Ivanov", "01.01.1990");
        Student s2(2, "Petr", "Petrov", "15.05.1991");
        Student s3(3, "Anna", "Sidorova", "20.03.1992");
        
        std::vector<Student> students = {s1, s2, s3};
        
        // Тест сериализации
        std::string json_data = Serializer::serializeStudents(students);
        std::cout << "Сериализованные данные:" << std::endl;
        std::cout << json_data << std::endl;
        
        // Тест десериализации
        try {
            auto deserialized_students = Serializer::deserializeStudents(json_data);
            std::cout << "Успешно десериализовано: " << deserialized_students.size() << " студентов" << std::endl;
            
            // Проверяем целостность данных
            bool data_ok = true;
            for (size_t i = 0; i < students.size(); ++i) {
                if (!(students[i] == deserialized_students[i])) {
                    data_ok = false;
                    break;
                }
            }
            
            std::cout << "Целостность данных: " << (data_ok ? "OK" : "FAIL") << std::endl;
            
        } catch (const std::exception& e) {
            std::cerr << "Ошибка десериализации: " << e.what() << std::endl;
        }
        
        // Тест невалидного JSON
        std::cout << "Тест невалидного JSON: ";
        try {
            auto invalid_students = Serializer::deserializeStudents("invalid json");
            std::cout << "FAIL - ожидалась ошибка" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "OK - " << e.what() << std::endl;
        }
    }

    void testPublisherSubscriber() {
        std::cout << "\n2. ТЕСТ PUBLISHER-SUBSCRIBER:" << std::endl;
        
        ZmqPublisher publisher;
        ZmqSubscriber subscriber;
        
        // Настраиваем callback для subscriber
        subscriber.setMessageCallback([this](const std::vector<Student>& students) {
            std::lock_guard<std::mutex> lock(data_mutex_);
            received_students_ = students;
            message_received_ = true;
            std::cout << "Callback: получено " << students.size() << " студентов" << std::endl;
        });
        
        subscriber.setRawMessageCallback([this](const std::string& message) {
            std::lock_guard<std::mutex> lock(data_mutex_);
            received_raw_message_ = message;
            raw_message_received_ = true; // Устанавливаем флаг для сырых сообщений
            std::cout << "Raw callback: получено сообщение длиной " << message.length() << " байт" << std::endl;
        });
        
        subscriber.setErrorCallback([](const std::string& error) {
            std::cerr << "Subscriber error: " << error << std::endl;
        });
        
        // Запускаем publisher и subscriber
        std::string endpoint = "tcp://localhost:5556";
        
        if (!publisher.start(endpoint)) {
            std::cerr << "Не удалось запустить publisher" << std::endl;
            return;
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        if (!subscriber.start(endpoint)) {
            std::cerr << "Не удалось запустить subscriber" << std::endl;
            publisher.stop();
            return;
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        // Тест 1: Отправка студентов
        std::cout << "--- Тест отправки студентов ---" << std::endl;
        Student s1(1, "Ivan", "Ivanov", "01.01.1990");
        Student s2(2, "Petr", "Petrov", "15.05.1991");
        Student s3(3, "Anna", "Sidorova", "20.03.1992");
        std::vector<Student> test_students = {s1, s2, s3};
        
        message_received_ = false;
        publisher.publish(test_students);
        
        std::cout << "Данные отправлены, ожидание получения..." << std::endl;
        
        if (waitForMessage(5000)) {
            std::lock_guard<std::mutex> lock(data_mutex_);
            std::cout << "Данные успешно получены!" << std::endl;
            std::cout << "Получено студентов: " << received_students_.size() << std::endl;
            
            bool data_correct = true;
            if (received_students_.size() == test_students.size()) {
                for (size_t i = 0; i < test_students.size(); ++i) {
                    if (!(received_students_[i] == test_students[i])) {
                        data_correct = false;
                        break;
                    }
                }
            } else {
                data_correct = false;
            }
            
            std::cout << "Целостность данных: " << (data_correct ? "OK" : "FAIL") << std::endl;
            std::cout << "Статистика publisher: " << publisher.getMessagesSent() 
                      << " сообщений, " << publisher.getStudentsSent() << " студентов" << std::endl;
            std::cout << "Статистика subscriber: " << subscriber.getMessagesReceived() 
                      << " сообщений, " << subscriber.getStudentsReceived() << " студентов" << std::endl;
        } else {
            std::cout << "Таймаут получения данных!" << std::endl;
        }
        
        // Тест 2: Отправка сырого сообщения
        std::cout << "\n--- Тест сырых сообщений ---" << std::endl;
        message_received_ = false;
        raw_message_received_ = false;
        
        // Временно отключаем обработку ошибок, чтобы не видеть ошибки парсинга
        subscriber.setErrorCallback(nullptr);
        
        publisher.publish("RAW TEST MESSAGE");
        
        std::cout << "Сырое сообщение отправлено, ожидание получения..." << std::endl;
        
        // Ждем сырое сообщение (не JSON)
        if (waitForRawMessage(2000)) {
            std::lock_guard<std::mutex> lock(data_mutex_);
            std::cout << "Сырое сообщение получено!" << std::endl;
            std::cout << "Содержимое: '" << received_raw_message_ << "'" << std::endl;
            std::cout << "Длина: " << received_raw_message_.length() << " байт" << std::endl;
        } else {
            std::cout << "Таймаут получения сырого сообщения!" << std::endl;
        }
        
        // Восстанавливаем обработку ошибок
        subscriber.setErrorCallback([](const std::string& error) {
            std::cerr << "Subscriber error: " << error << std::endl;
        });
        
        // Останавливаем
        subscriber.stop();
        publisher.stop();
    }

    void testStress() {
        std::cout << "\n3. СТРЕСС-ТЕСТ:" << std::endl;
        
        ZmqPublisher publisher;
        ZmqSubscriber subscriber;
        
        std::atomic<int> messages_received{0};
        std::atomic<int> students_received{0};
        
        subscriber.setMessageCallback([&](const std::vector<Student>& students) {
            messages_received++;
            students_received += students.size();
        });
        
        std::string endpoint = "tcp://localhost:5557";
        
        publisher.start(endpoint);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        subscriber.start(endpoint);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        // Генерируем много студентов
        const int NUM_BATCHES = 10;
        const int STUDENTS_PER_BATCH = 100;
        
        std::cout << "Отправка " << NUM_BATCHES << " пачек по " << STUDENTS_PER_BATCH << " студентов..." << std::endl;
        
        auto start_time = std::chrono::steady_clock::now();
        
        for (int i = 0; i < NUM_BATCHES; ++i) {
            std::vector<Student> batch;
            for (int j = 0; j < STUDENTS_PER_BATCH; ++j) {
                int id = i * STUDENTS_PER_BATCH + j;
                batch.emplace_back(id, "Name" + std::to_string(id), 
                                  "LastName" + std::to_string(id),
                                  "01.01.1990");
            }
            publisher.publish(batch);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        
        auto end_time = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        std::cout << "Статистика стресс-теста:" << std::endl;
        std::cout << "  Время выполнения: " << duration.count() << " мс" << std::endl;
        std::cout << "  Получено сообщений: " << messages_received << std::endl;
        std::cout << "  Получено студентов: " << students_received << std::endl;
        std::cout << "  Ожидалось студентов: " << (NUM_BATCHES * STUDENTS_PER_BATCH) << std::endl;
        std::cout << "  Потеряно: " << (NUM_BATCHES * STUDENTS_PER_BATCH - students_received) << " студентов" << std::endl;
        
        subscriber.stop();
        publisher.stop();
    }

    void testErrorHandling() {
        std::cout << "\n4. ТЕСТ ОБРАБОТКИ ОШИБОК:" << std::endl;
        
        // Тест неверного endpoint
        std::cout << "Тест неверного endpoint: ";
        ZmqPublisher publisher;
        bool result = publisher.start("invalid_endpoint");
        std::cout << (result ? "FAIL" : "OK") << std::endl;
        
        if (result) {
            publisher.stop();
        }
        
        // Тест отправки когда publisher не запущен
        std::cout << "Тест отправки без запуска: ";
        ZmqPublisher publisher2;
        publisher2.publish("test");
        std::cout << "OK" << std::endl;
        
        // Тест двойного запуска
        std::cout << "Тест двойного запуска: ";
        ZmqPublisher publisher3;
        if (publisher3.start("tcp://*:5558")) {
            bool second_start = publisher3.start("tcp://*:5559");
            std::cout << (second_start ? "FAIL" : "OK") << std::endl;
            publisher3.stop();
        }
        
        // Тест невалидных JSON данных
        std::cout << "Тест невалидных JSON данных: ";
        ZmqPublisher publisher4;
        ZmqSubscriber subscriber;
        
        std::atomic<bool> error_received{false};
        subscriber.setErrorCallback([&](const std::string& error) {
            error_received = true;
        });
        
        publisher4.start("tcp://*:5560");
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        subscriber.start("tcp://localhost:5560");
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        publisher4.publish("invalid json data");
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        std::cout << (error_received ? "OK" : "FAIL") << std::endl;
        
        subscriber.stop();
        publisher4.stop();
    }

    bool waitForMessage(int timeout_ms) {
        for (int i = 0; i < timeout_ms / 10; ++i) {
            if (message_received_) {
                return true;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        return false;
    }

    bool waitForRawMessage(int timeout_ms) {
        for (int i = 0; i < timeout_ms / 10; ++i) {
            if (raw_message_received_) {
                return true;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        return false;
    }
};

int main() {
    NetworkTest test;
    test.runTests();
    return 0;
}