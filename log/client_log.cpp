#include "client_log.h"

#include <ctime>
#include <iomanip>

#ifdef _WIN32
#include <Windows.h>
#endif


#ifdef _WIN32
namespace comm {
    namespace log{

        constexpr uint32_t LogBufferSize = 4096;
        constexpr uint32_t ThreadIDBufferSize = 32;
        constexpr uint32_t TimeBufferSize = 32;
        //
        thread_local char LogBuffer[LogBufferSize];
        thread_local char ThreadIDBuffer[ThreadIDBufferSize];
        thread_local char TimeBuffer[TimeBufferSize];

        Logger::Logger()
            : _consoleHandle(nullptr)
            , _allocated(false)
        {}

        Logger::~Logger() {
            if(_allocated) {
                FreeConsole();
            }
        }
        bool Logger::initialize() {
            _consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
            if(!_consoleHandle) {
                auto rst = AllocConsole();
                if(rst == TRUE) {
                    _consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
                    _allocated = true;
                } else {
                    return false;
                }
            }
            return true;
        }

        template<class T>
        inline std::enable_if_t<sizeof(T) == sizeof(uint64_t), void> printOpaqueID( char* buffer, T id ) {
            sprintf(buffer,"[%llu]",*(uint64_t*)&id);
        }
        template<class T>
        inline std::enable_if_t<sizeof(T) == sizeof(int32_t), void> printOpaqueID( char* buffer, T id ) {
            sprintf(buffer,"[%u]",*(uint32_t*)&id);
        }

        void Logger::_log( Type type, char const* format, ...) {
            va_list vaList;
            va_start(vaList, format);
            auto logLength = vsnprintf( LogBuffer, LogBufferSize, format, vaList );
            va_end(vaList);
            //
            switch (type)   
            {
            case comm::log::Type::Vorbose:
                SetConsoleTextAttribute(_consoleHandle, FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_GREEN); break;
            case comm::log::Type::Warning:
                SetConsoleTextAttribute(_consoleHandle, FOREGROUND_RED|FOREGROUND_GREEN|BACKGROUND_BLUE); break;
            case comm::log::Type::Error:
                SetConsoleTextAttribute(_consoleHandle, FOREGROUND_GREEN|FOREGROUND_BLUE|BACKGROUND_RED); break;
                break;
            default:
                break;
            }
            auto threadID = std::this_thread::get_id();
            printOpaqueID(ThreadIDBuffer, threadID);
            time_t currentTime;
            std::time(&currentTime);
            auto tm = localtime(&currentTime);
            sprintf( TimeBuffer, "|%d-%d-%d-%d:%d:%d|", tm->tm_year+1900, tm->tm_mon, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
            std::cout<<"[comm]"<<std::setw(16)<<TimeBuffer<<ThreadIDBuffer<<std::setw(8)<<LogBuffer<<std::endl;
            //
            SetConsoleTextAttribute(_consoleHandle, FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_GREEN);
        }

        Logger logger;

        void initialize() {
            logger.initialize();
        }
        
    }
}

#else
//todo:FIX ME 假装已实现 其实什么都没做 
namespace comm{
    namespace log{
        Logger::Logger(){
        }
        Logger::~Logger() {
        }
        bool Logger::initialize() {
            return true;
        }
        void Logger::_log( Type type, char const* format, ...) {
        }
        Logger logger;
        void initialize() {
            logger.initialize();
        }
    }
}
#endif

