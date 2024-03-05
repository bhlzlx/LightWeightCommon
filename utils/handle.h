#pragma once
/**
 * @file handle.h
 * @author bhlzlx@hotmail.com
 * @brief weak ref for object
 * @version 0.1
 * @date 2024-01-31
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <atomic>

/**
 * @brief 
 *  a handle ref to a weak pointer
 */

namespace comm {

    class RefInfo {
    private:
        std::atomic<int>                ref_;
        void*                           ptr_; // data
    public:
        RefInfo(void* ptr)
            : ref_(1)
        {}

        void addRef() {
            ++ref_;
        }
        void deRef() {
            --ref_;
            if(ref_ == 0) {
                delete this;
            }
        }
        int refCount() const {
            return ref_;
        }
        void reset() {
            ptr_ = nullptr;
        }
        void*  data() const {
            return ptr_;
        }
    };

    class Handle {
        friend class ObjectHandle;
    private:
        RefInfo*    ref_;
    private:
    public:
        constexpr Handle() {
            ref_ = nullptr;
        }
        // 逻辑层不可手动创建Handle对象，只能通过GObject来获取弱引用
        Handle(void* ptr) {
            ref_ = new RefInfo(ptr);
        }
        Handle(Handle const& handle) {
            ref_ = handle.ref_;
            ref_->addRef();
        }
        Handle(Handle && handle) {
            ref_->deRef();
            ref_ = handle.ref_;
            handle.ref_ = nullptr;
        }
        Handle& operator = (Handle const& handle) {
            ref_ = handle.ref_;
            ref_->addRef();
            return *this;
        }
        Handle& operator = (Handle&& handle) {
            ref_->deRef();
            ref_ = handle.ref_;
            handle.ref_ = nullptr;
            return *this;
        }
        template<class T>
        T* as() const {
            if(!ref_) {
                return nullptr;
            }
            return (T*)ref_->data();
        }
        operator bool () const {
            return !!as<void*>();
        }
        bool operator == (nullptr_t) const {
            return !as<void*>();
        }
        ~Handle() {
            if(ref_) {
                ref_->deRef();
            }
        }
    };

    class ObjectHandle {
    private:
        Handle handle_;
    public:
        constexpr ObjectHandle() 
            :handle_()
        {
        }
        ObjectHandle(void* ptr = nullptr) 
            :handle_(ptr)
        {
        }
        ~ObjectHandle() {
            handle_.ref_->reset();
        }
        Handle handle() const {
            return handle_;
        }
        void reset() {
            handle_.ref_->reset();
        }
    };

}