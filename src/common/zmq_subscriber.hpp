#ifndef ZMQ_SUBSCRIBER_HPP
#define ZMQ_SUBSCRIBER_HPP

#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <functional>
#include <zmq.hpp>
#include "student.hpp"
#include "serializer.hpp"

class ZmqSubscriber {
public:
    using MessageCallback = std::function<void(const std::vector<Student>&)>;
    using RawMessageCallback = std::function<void(const std::string&)>;
    using ErrorCallback = std::function<void(const std::string&)>;
    
    ZmqSubscriber();
    ~ZmqSubscriber();
    
    // Подключение и отключение
    bool start(const std::string& endpoint = "tcp://localhost:5556", 
               const std::string& filter = "");
    void stop();
    
    // Callback функции
    void setMessageCallback(MessageCallback callback) { message_callback_ = callback; }
    void setRawMessageCallback(RawMessageCallback callback) { raw_message_callback_ = callback; }
    void setErrorCallback(ErrorCallback callback) { error_callback_ = callback; }
    
    // Статус
    bool isRunning() const { return running_; }
    
    // Статистика
    size_t getMessagesReceived() const { return messages_received_; }
    size_t getStudentsReceived() const { return students_received_; }

private:
    void run();
    bool receiveMessage(std::string& message);
    void processMessage(const std::string& message);
    
private:
    std::unique_ptr<zmq::context_t> context_;
    std::unique_ptr<zmq::socket_t> socket_;
    std::thread worker_thread_;
    std::atomic<bool> running_;
    std::atomic<bool> stop_requested_;
    
    // Callback функции
    MessageCallback message_callback_;
    RawMessageCallback raw_message_callback_;
    ErrorCallback error_callback_;
    
    // Статистика
    std::atomic<size_t> messages_received_;
    std::atomic<size_t> students_received_;
    
    std::string endpoint_;
    std::string filter_;
};

#endif // ZMQ_SUBSCRIBER_HPP