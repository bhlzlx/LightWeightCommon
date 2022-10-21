#include "archive.h"

namespace comm {

    class FileIStream : public IStream {
    private:
        FILE*   _file;
        size_t  _size;
    public:
        FileIStream(FILE* file, size_t size)
            : _file(file)
            , _size(size)
        {}

        virtual int64_t read( void* buffer, int64_t size ) override;
        virtual int64_t seek( SeekOption option, int offset ) override;
        virtual int64_t tell() const override;
        virtual int64_t size() const override;
        virtual bool seekable() const override;
        /**
         * @brief close 会回收资源，销毁对象，回收内存
         * 
         */
        virtual void close() override;
        virtual ~FileIStream() override{}
    };

    class FileOStream : public OStream {
    private:
        FILE*       _file;
        size_t      _size;
    public:
        FileOStream(FILE* file, size_t size)
            : _file(file)
            , _size(size)
        {}
        virtual int64_t write( const void* buffer, int64_t size ) override;
        virtual int64_t seek(SeekOption option, int offset ) override;
        virtual int64_t tell() const override;
        virtual int64_t size() const override;
        virtual bool seekable() const override;
        /**
         * @brief close 会回收资源，销毁对象，回收内存
         */
        virtual void close() override;
        virtual ~FileOStream(){}
    };

    class FileSystemArchive : public IArchive {
    private:
        std::string _rootpath;
    public:
        FileSystemArchive(const std::string root)
            : _rootpath(root)
        {}
        virtual IStream* openIStream( const std::string& path, BitFlags<ReadFlag> flags) override;
        virtual OStream* openOStream( const std::string& path, BitFlags<WriteFlag> flags) override;
        virtual bool testExist( const std::string& path ) override;
        virtual bool supportListFeature() const override;
        virtual std::vector<FileEntity> listFiles( const std::string& path ) override;
        virtual std::string rootPath() override;
        virtual bool readonly() const override;
        virtual void destroy() override;
        virtual ~FileSystemArchive() override {}
    };

}