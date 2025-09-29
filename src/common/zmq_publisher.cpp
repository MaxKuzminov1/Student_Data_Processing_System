/*
 * ZeroMQ издатель для отправки данных студентов
 * Использует очередь сообщений для асинхронной отправки
 */

#include "zmq_publisher.hpp"
#include <iostream>
#include <chrono>

ZmqPublisher::ZmqPublisher() 
    : running_(false), stop_requested_(false), 
      messages_sent_(0), students_sent_(0) {
}

ZmqPublisher::~ZmqPublisher() {
    stop();
}

bool ZmqPublisher::start(const std::string& endpoint) {
    if (running_) {
        std::cerr << "Publisher already running" << std::endl;
        return false;
    }
    
    try {
        endpoint_ = endpoint;
        context_ = std::make_unique<zmq::context_t>(1);
        socket_ = std::make_unique<zmq::socket_t>(*context_, ZMQ_PUB);
        
        // Настройка socket
        socket_->set(zmq::sockopt::linger, 0);
        socket_->set(zmq::sockopt::sndhwm, 1000);
        
        // Привязка к endpoint
        socket_->bind(endpoint);
        
        std::cout << "ZMQ Publisher started on " << endpoint << std::endl;
        
        running_ = true;
        stop_requested_ = false;
        worker_thread_ = std::thread(&ZmqPublisher::run, this);
        
        return true;
    } catch (const zmq::error_t& e) {
        std::cerr << "ZMQ Publisher error: " << e.what() << std::endl;
        return false;
    }
}

void ZmqPublisher::stop() {
    if (!running_) return;
    
    stop_requested_ = true;
    queue_cv_.notify_all();
    
    if (worker_thread_.joinable()) {
        worker_thread_.join();
    }
    
    if (socket_) {
        socket_->close();
    }
    
    if (context_) {
        context_->close();
    }
    
    running_ = false;
    std::cout << "ZMQ Publisher stopped" << std::endl;
}

void ZmqPublisher::publish(const std::vector<Student>& students) {
    if (!running_) {
        std::cerr << "Publisher not running" << std::endl;
        return;
    }
    
    std::string message = Serializer::serializeStudents(students);
    publish(message);
    students_sent_ += students.size();
}

void ZmqPublisher::publish(const std::string& message) {
    if (!running_) return;
    
    {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        message_queue_.push(message);
    }
    
    queue_cv_.notify_one();
}

size_t ZmqPublisher::getQueueSize() const {
    std::lock_guard<std::mutex> lock(queue_mutex_);
    return message_queue_.size();
}

void ZmqPublisher::run() {
    while (!stop_requested_ || !message_queue_.empty()) {
        std::string message;
        
        {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            if (message_queue_.empty()) {
                queue_cv_.wait_for(lock, std::chrono::milliseconds(100));
                continue;
            }
            
            message = message_queue_.front();
            message_queue_.pop();
        }
        
        if (sendMessage(message)) {
            messages_sent_++;
        } else {
            std::cerr << "Failed to send message" << std::endl;
        }
    }
}



bool ZmqPublisher::sendMessage(const std::string& message) {
    if (!socket_ || !context_) {
        std::cerr << "Socket is not initialized" << std::endl;
        std::cout << "Отправка сообщения длиной " << message.length() << " байт" << std::endl;
        return false;
    }
    try {
        zmq::message_t zmq_message(message.size());
        memcpy(zmq_message.data(), message.c_str(), message.size());
        zmq::send_result_t result = socket_->send(zmq_message, zmq::send_flags::dontwait);
        
        // Простое преобразование в bool
        return static_cast<bool>(result);
    } catch (const zmq::error_t& e) {
        std::cerr << "ZeroMQ send error: " << e.what() << std::endl;
        return false;
    }
}
