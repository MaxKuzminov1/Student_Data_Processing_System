#ifndef ZMQ_PUBLISHER_HPP
#define ZMQ_PUBLISHER_HPP

#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <zmq.hpp>
#include "student.hpp"
#include "serializer.hpp"

class ZmqPublisher {
public:
    ZmqPublisher();
    ~ZmqPublisher();
    
    // Запуск и остановка publisher
    bool start(const std::string& endpoint = "tcp://*:5556");
    void stop();
    
    // Отправка данных
    void publish(const std::vector<Student>& students);
    void publish(const std::string& message);
    
    // Статус
    bool isRunning() const { return running_; }
    size_t getQueueSize() const;
    
    // Статистика
    size_t getMessagesSent() const { return messages_sent_; }
    size_t getStudentsSent() const { return students_sent_; }

private:
    void run();
    bool sendMessage(const std::string& message);
    
private:
    std::unique_ptr<zmq::context_t> context_;
    std::unique_ptr<zmq::socket_t> socket_;
    std::thread worker_thread_;
    std::atomic<bool> running_;
    std::atomic<bool> stop_requested_;
    
    // Очередь сообщений
    std::queue<std::string> message_queue_;
    mutable std::mutex queue_mutex_;
    std::condition_variable queue_cv_;
    
    // Статистика
    std::atomic<size_t> messages_sent_;
    std::atomic<size_t> students_sent_;
    
    std::string endpoint_;
};

#endif // ZMQ_PUBLISHER_HPP