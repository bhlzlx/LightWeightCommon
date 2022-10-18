#include "filesystem_archive.h"
#ifdef _WIN32
#include <windows.h>
#endif

namespace common {

std::string FormatFilePath(const std::string& _filepath)
{
    int nSec = 0;
    std::string curSec;
    std::string fpath;
    if (_filepath[0] == '/') {
        fpath.push_back('/');
    }
    const char* ptr = _filepath.c_str();
    while (*ptr != 0) {
        if (*ptr == '\\' || *ptr == '/') {
            if (curSec.length() > 0) {
                if (curSec == ".") {
                } else if (curSec == ".." && nSec >= 2) {
                    int secleft = 2;
                    while (!(fpath.empty() && secleft == 0)) {
                        if (fpath.back() == '\\' || fpath.back() == '/') {
                            --secleft;
                            break;
                        }
                        fpath.pop_back();
                    }
                    fpath.pop_back(); // pop back '/'
                } else {
                    if (!fpath.empty() && fpath.back() != '/')
                        fpath.push_back('/');
                    fpath.append(curSec);
                    ++nSec;
                }
                curSec.clear();
            }
        } else {
            curSec.push_back(*ptr);
            if (*ptr == ':') {
                --nSec;
            }
        }
        ++ptr;
    }
    if (curSec.length() > 0) {
        if (!fpath.empty() && fpath.back() != '/')
            fpath.push_back('/');
        fpath.append(curSec);
    }
    return fpath;
}

int64_t FileIStream::read(void* buffer, int64_t size)
{
    return fread(buffer, 1, size, _file);
}
int64_t FileIStream::seek(SeekOption option, int offset)
{
    switch (option) {
    case SeekOption::begin:
        return fseek(_file, SEEK_SET, offset);
    case SeekOption::current:
        return fseek(_file, SEEK_CUR, offset);
    case SeekOption::end:
        return fseek(_file, SEEK_END, offset);
    default:
        break;
    }
    return 0;
}
int64_t FileIStream::tell() const
{
    return ftell(_file);
}
int64_t FileIStream::size() const
{
    return _size;
}
bool FileIStream::seekable() const
{
    return true;
}
void FileIStream::close()
{
    if (_file) {
        fclose(_file);
    }
    _size = 0;
    _file = nullptr;
    delete this;
}

int64_t FileOStream::write(const void* buffer, int64_t size)
{
    return fwrite(buffer, 1, size, _file);
}
int64_t FileOStream::seek(SeekOption option, int offset)
{
    switch (option) {
    case SeekOption::begin:
        return fseek(_file, offset, SEEK_SET);
    case SeekOption::current:
        return fseek(_file, offset, SEEK_CUR);
    case SeekOption::end:
        return fseek(_file, offset, SEEK_END);
    }
    return 0;
}
int64_t FileOStream::tell() const
{
    return ftell(_file);
}
int64_t FileOStream::size() const
{
    return _size;
}
bool FileOStream::seekable() const
{
    return true;
}
void FileOStream::close()
{
    if (_file) {
        fclose(_file);
        _file = nullptr;
    }
    delete this;
}

IStream* FileSystemArchive::openIStream(const std::string& path, BitFlags<ReadFlag> flags)
{
    std::string fullpath = _rootpath + "/" + path;
    char const* openFlag = nullptr; 
    if( flags.test(ReadFlag::binary)) {
        openFlag = "rb";
    } else {
        openFlag = "r";
    }
    auto file = fopen(fullpath.c_str(), openFlag );
    if(!file) {
        return nullptr;
    } else {
        fseek( file, 0, SEEK_END);
        int64_t fileSize = ftell(file);
        fseek( file, 0, SEEK_SET);
        void *ptr = malloc(sizeof(FileIStream));
        FileIStream* istream = new (ptr) FileIStream(file, fileSize);
        return istream;
    }
}

OStream* FileSystemArchive::openOStream(const std::string& path, BitFlags<WriteFlag> flags)
{
    std::string fullpath = _rootpath + "/" + path;
    std::string openFlag = "w";
    if(flags.test(WriteFlag::binary)){
        openFlag.push_back('b');
    }
    if(flags.test(WriteFlag::append)){
        openFlag.push_back('a');
    }
    // fullpath = FormatFilePath(fullpath);
    auto file = fopen(fullpath.c_str(), openFlag.c_str() );
    if(!file) {
        return nullptr;
    } else {
        fseek( file, 0, SEEK_END);
        int64_t fileSize = ftell(file);
        fseek( file, 0, SEEK_SET);
        void *ptr = malloc(sizeof(FileOStream));
        FileOStream* ostream = new (ptr) FileOStream(file, fileSize);
        return ostream;
    }
    return nullptr;
}

bool FileSystemArchive::testExist(const std::string& path) {
    auto rst = this->openIStream(path, (uint32_t)ReadFlag::binary);
    if(rst) {
        rst->close();
        return true;
    }
    return false;
}

bool FileSystemArchive::supportListFeature() const {
    return true;
}

std::vector<IArchive::FileEntity> FileSystemArchive::listFiles(const std::string& path) {
    /**
     * @brief  win32 实现
     * 
     */
    std::vector<IArchive::FileEntity> rst;
    #ifdef _WIN32
    std::string absDirPath = _rootpath + "/" + path + "/*";
    WIN32_FIND_DATA data;
    HANDLE hContent = FindFirstFile(absDirPath.c_str(), &data);
    while (FindNextFile(hContent, &data)) {
        if (!(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            rst.emplace_back(data.cFileName, IArchive::FileEntityType::file);
        } else {
            rst.emplace_back(data.cFileName, IArchive::FileEntityType::directory);
        }
    }
    #endif
    return rst;
}

std::string FileSystemArchive::rootPath() {
    return _rootpath;
}

bool FileSystemArchive::readonly() const {
    return false;
}

void FileSystemArchive::destroy() {
    this->~FileSystemArchive();
    free(this);
}

IArchive* CreateFSArchive(const std::string& rootPath) {
    auto memptr = malloc(sizeof(FileSystemArchive));
    return new (memptr) FileSystemArchive(rootPath);
}

}