#include "name.h"
#include "../memory/memory.h"

namespace comm {

    Name::Name( prototype_t* ptr  )
        : _heapPtr(ptr)
    {}
    Name::Name( const Name& name ) {
        _heapPtr = name._heapPtr;
    }
    Name& Name::operator = ( const Name& name ) {
        _heapPtr = name._heapPtr;
        return *this;
    }
    char const* Name::text() const {
        return _heapPtr->str;
    }
    uint16_t Name::length() const {
        return _heapPtr->length;
    }
    //
    NamePool::NamePool()
        : _nameSet()
        , _mutex()
        , _totalBytes(0)
    {}

    Name NamePool::getName( char const* str, uint16_t length ) {
        static Name::prototype_t NullName = {};
        if(!str) {
            return Name(&NullName);
        }
        if(0 == length) {
            length = (uint16_t)strlen(str);
        }
        /**
         * @brief 如果先看set里有没有，则需要查找两次，如果提前准备好内存，则只需要插入一次
         * 不过也有其缺点，如果数据已经存在就会有一次内存回收的操作。
         */
        size_t bytes = sizeof(sizeof(Name::prototype_t) + length);
        Name::prototype_t* prototype_t = (Name::prototype_t*)(uint8_t*)comm_alloc(bytes);
        strcpy(prototype_t->str, str);
        prototype_t->length = length;

        std::unique_lock<std::mutex> lock(_mutex);
        auto rst = _nameSet.insert(prototype_t);
        this->_totalBytes += bytes;
        lock.unlock();

        if(!rst.second) {
            comm_free(prototype_t);
        }
        return Name(*rst.first);
    }

    NamePool::~NamePool() {
        for(auto prototype_t : _nameSet) {
            comm_free(prototype_t);
        }
    }

}