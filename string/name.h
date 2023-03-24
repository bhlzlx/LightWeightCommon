#pragma once
#include <cstdint>
#include <cstring>
#include <set>
#include <mutex>

namespace comm {

    class Name {
    friend class NamePool;
    public:
        struct prototype_t {
            uint16_t    length;
            char        str[1];
        };
    private:
        // prototype_t 只是为了方便调试的时候观察值
    private:
        prototype_t* _heapPtr;
        Name(prototype_t* ptr);
    public:
        Name( const Name& name );
        Name& operator = ( const Name& name );
        char const* text() const;
        uint16_t length() const;
        operator size_t() const {
            return (size_t)_heapPtr->str;
        }
        ~Name() {}
    };

    /**
     * @brief TODO: NamePool 不能用读写锁，所以直接用互斥量保证线程安全吧
     *
     */

    class NamePool {
    private:
        using nameproto = Name::prototype_t*;
        struct prototype_less {
            bool operator() ( const Name::prototype_t* a, const Name::prototype_t* b) const{
                if(a->length < b->length) {
                    return true;
                } else if(a->length > b->length) {
                    return false;
                } else {
                    return strcmp(a->str, b->str) < 0;
                }
            }
        };
        std::set<Name::prototype_t*, prototype_less>    _nameSet;
        std::mutex                                      _mutex;
        size_t                                          _totalBytes;
    public:
        NamePool();
        Name getName(char const* str, uint16_t length);
        size_t bytesTotal() const {
            return _totalBytes;
        }
        ~NamePool();
    };

}