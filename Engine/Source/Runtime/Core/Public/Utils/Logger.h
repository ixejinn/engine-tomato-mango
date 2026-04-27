#ifndef MANGO_LOGGER_H
#define MANGO_LOGGER_H

#include <iostream>
#include <sstream>
#include <string>
#include <source_location>

namespace tomato {
    class Logger {
    public:
        enum Level { Info, Debug, Warn, Error };

        Logger(const Level lv, const std::source_location loc = std::source_location::current())
        : level_(lv), location_(loc) {}

        template<typename T>
        Logger& operator<<(const T& msg) {
            buffer_ << msg;
            return *this;
        }

        ~Logger() {
            std::cout << Prefix(level_) << buffer_.str();

            if (level_ > Level::Info) {
                std::cout << "\n"
                << location_.file_name() << ":" << location_.line()
                << ": " << location_.function_name();
            }
            std::cout << "\n";
        }

    private:
        static const char* Prefix(Level lv) {
            switch (lv) {
                case Info:  return "[INFO] ";
                case Debug: return "[DEBUG] ";
                case Warn:  return "[WARN] ";
                case Error: return "[ERROR] ";
            }
            return "";
        }

        Level level_;
        std::source_location location_;

        std::ostringstream buffer_;
    };
}

// !!!!! 나중에 옮기기 !!!!!
#ifndef NDEBUG
#define TOMATO_DEBUG
#endif

#ifdef _DEBUG
#define TOMATO_DEBUG
#endif
// !!!!! 나중에 옮기기 !!!!!

#ifdef TOMATO_DEBUG
#define TMT_INFO Logger(tomato::Logger::Level::Info)
#define TMT_DEBUG Logger(tomato::Logger::Level::Debug)
#define TMT_WARN Logger(tomato::Logger::Level::Warn)
#define TMT_ERR Logger(tomato::Logger::Level::Error)
#else
#define TMT_INFO
#define TMT_DEBUG
#define TMT_WARN
#define TMT_ERR
#endif

#endif //MANGO_LOGGER_H