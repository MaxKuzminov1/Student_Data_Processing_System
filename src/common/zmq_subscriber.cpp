/*
 * ZeroMQ подписчик для получения данных студентов
 * Обрабатывает входящие сообщения и преобразует их в объекты Student
 */

#include "zmq_subscriber.hpp"
#include <iostream>
#include <chrono>

ZmqSubscriber::ZmqSubscriber() 
    : running_(false), stop_requested_(false),
      messages_received_(0), students_received_(0) {
}

ZmqSubscriber::~ZmqSubscriber() {
    stop();
}

bool ZmqSubscriber::start(const std::string& endpoint, const std::string& filter) {
    if (running_) {
        std::cerr << "Subscriber already running" << std::endl;
        return false;
    }
    
    try {
        endpoint_ = endpoint;
        filter_ = filter;
        
        context_ = std::make_unique<zmq::context_t>(1);
        socket_ = std::make_unique<zmq::socket_t>(*context_, ZMQ_SUB);
        
        // Настройка socket
        socket_->set(zmq::sockopt::linger, 0);
        socket_->set(zmq::sockopt::rcvhwm, 1000);
        
        // Подключение и подписка
        socket_->connect(endpoint);
        socket_->set(zmq::sockopt::subscribe, filter);
        
        std::cout << "ZMQ Subscriber connected to " << endpoint 
                  << " with filter: '" << filter << "'" << std::endl;
        
        running_ = true;
        stop_requested_ = false;
        worker_thread_ = std::thread(&ZmqSubscriber::run, this);
        
        return true;
    } catch (const zmq::error_t& e) {
        std::cerr << "ZMQ Subscriber error: " << e.what() << std::endl;
        if (error_callback_) {
            error_callback_(e.what());
        }
        return false;
    }
}

void ZmqSubscriber::stop() {
    if (!running_) return;
    
    stop_requested_ = true;
    
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
    std::cout << "ZMQ Subscriber stopped" << std::endl;
}

void ZmqSubscriber::run() {
    while (!stop_requested_) {
        std::string message;
        
        if (receiveMessage(message)) {
            processMessage(message);
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
}

bool ZmqSubscriber::receiveMessage(std::string& message) {
    try {
        zmq::message_t zmq_message;
        zmq::recv_result_t result = socket_->recv(zmq_message, zmq::recv_flags::dontwait);
        
        if (result) {
            message = std::string(static_cast<char*>(zmq_message.data()), zmq_message.size());
            messages_received_++;
            return true;
        }
    } catch (const zmq::error_t& e) {
        std::cerr << "Receive error: " << e.what() << std::endl;
        if (error_callback_) {
            error_callback_(e.what());
        }
    }
    
    return false;
}

void ZmqSubscriber::processMessage(const std::string& message) {
    // Вызываем raw callback если установлен
    if (raw_message_callback_) {
        raw_message_callback_(message);
    }
    
    // Парсим и вызываем основной callback
    try {
        std::vector<Student> students = Serializer::deserializeStudents(message);
        students_received_ += students.size();
        
        if (message_callback_) {
            message_callback_(students);
        }
        
        std::cout << "Received " << students.size() << " students" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Message processing error: " << e.what() << std::endl;
        if (error_callback_) {
            error_callback_(e.what());
        }
    }
}