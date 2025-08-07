#ifndef LOGGER
#define LOGGER

#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>

class Logger{

    public:
        void log_info(std::string& info) const;

    private:
        std::string get_current_time() const;
};

#endif