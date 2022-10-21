#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <utils/enum_class_bits.h>

namespace comm {

    enum class SeekOption {
        begin   = 0,
        current = 1,
        end     = 2
    };

    enum class ReadFlag {
        binary = 1,         // 二进制模式打开
    };

    enum class WriteFlag {
        append  = 1,        // 只允许在文件尾写
        binary  = 2,        // 二进制格式
        trunc   = 4,        // 如果存在就抹掉
    };

    enum class AssetManagerType {
        FileSystem,     // PC,iOS Documents
        AndroidAsset,   // Android 自带的资源文件系统（是只读的）
        Pkg,            // iOS,Android 加密压缩资源包 
    };

    class IStream {
    public:
        virtual int64_t read( void* buffer, int64_t size ) = 0;
        virtual int64_t seek( SeekOption option, int offset ) = 0;
        virtual int64_t tell() const = 0;
        virtual int64_t size() const = 0;
        virtual bool seekable() const = 0;
        /**
         * @brief close 会回收资源，销毁对象，回收内存
         * 
         */
        virtual void close() = 0;
        virtual ~IStream(){}
    };

    class OStream {
    public:
        virtual int64_t write( const void* buffer, int64_t size ) = 0;
        virtual int64_t seek(SeekOption option, int offset ) = 0;
        virtual int64_t tell() const = 0;
        virtual int64_t size() const = 0;
        virtual bool seekable() const = 0;
        /**
         * @brief close 会回收资源，销毁对象，回收内存
         */
        virtual void close() = 0;
        virtual ~OStream(){}
    };

    class IArchive {
    private:
    public:
        enum class FileEntityType {
            file        = 0,
            directory   = 1
        };
        struct FileEntity {
            FileEntity( std::string&& name, FileEntityType type)
                : name(name)
                , type(type)
            {}
            std::string     name;
            FileEntityType  type;
        };
    public:
        virtual IStream* openIStream( const std::string& path, BitFlags<ReadFlag> flags) = 0;
        virtual OStream* openOStream( const std::string& path, BitFlags<WriteFlag> flags) = 0;
        virtual bool testExist( const std::string& path ) = 0;
        virtual bool supportListFeature() const = 0;
        virtual std::vector<FileEntity> listFiles( const std::string& path ) = 0;
        virtual std::string rootPath() = 0;
        virtual bool readonly() const = 0;
        virtual void destroy() = 0;
        virtual ~IArchive() {};
    };

    IArchive* CreateFSArchive(const std::string& rootPath);

}