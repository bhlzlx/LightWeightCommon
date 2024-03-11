#pragma once
#include <utility>

namespace comm {

    template<class T>
    class Singleton {
    protected:
        static T* inst_;
        Singleton() {}
    public:
        template<class ...ARGS>
        static T* Instance(ARGS&& ...args) {
            if(inst_) {
                return inst_;
            }
            inst_ = new T(std::forward<ARGS>(args)...);
            return inst_;
        }
    };

    template<class T>
    T* Singleton<T>::inst_ = nullptr;


}