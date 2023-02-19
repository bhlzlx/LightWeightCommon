#pragma once
#include <cstdio>
#include <cstdint>
#include <cstdarg>
#include <thread>
#include <iostream>

namespace comm {
    namespace log {
        enum class Type {
            Vorbose,
            Warning,
            Error
        };

        class Logger {
        private:
            void*   _consoleHandle;
            bool    _allocated;
        private:
            void _log(comm::log::Type type, char const* format, ...);
        public:
            Logger();
            ~Logger();
            bool initialize();

            template<class ...ARGS>
            void log( comm::log::Type type, char const* format, ARGS&& ...args ) {
                _log(type, format, std::forward<ARGS>(args)...);
            }
        };

        extern Logger logger;

        void initialize();
        template<class ...ARGS>
        void log(comm::log::Type type, char const* format, ARGS&& ...args) {
            logger.log(type, format, std::forward<ARGS>(args)...);
        }
    }
}

#define COMMLOGV( format, ... ) comm::log::log( comm::log::Type::Vorbose, format, ##__VA_ARGS__ )
#define COMMLOGW( format, ... ) comm::log::log( comm::log::Type::Warning, format, ##__VA_ARGS__ )
#define COMMLOGE( format, ... ) comm::log::log( comm::log::Type::Error, format, ##__VA_ARGS__ )