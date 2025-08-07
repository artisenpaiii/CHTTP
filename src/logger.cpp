#include "logger.h"

std::string Logger::get_current_time() const {
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);

    std::ostringstream oss;
    oss << std::put_time(std::localtime(&now_time), "%H:%M:%S");
    return oss.str();

};

void Logger::log_info(std::string info) const {
    std::cout << "[" << this->get_current_time() 
              << "][INFO] - " << info << std::endl; 
};

void Logger::log_error(std::string info) const {
    std::cout << "[" << this->get_current_time() << "][ERROR] - " 
              << info << std::endl; 
};

void Logger::log_method(std::string method, std::string text) const {
    std::cout << "[" << this->get_current_time() << "][" << method << "] - " 
    << text << std::endl; 


}
