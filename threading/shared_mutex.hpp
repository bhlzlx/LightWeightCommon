#pragma once

/**
 * @file shared_mutex.hpp
 * @author 李新
 * @brief 读写锁（共享锁）
 * 如果当前版本的编译器标准库中没有shared_mutex那么可以用它
 * 从 boost 库中抄出来的，并添加了关键注释
 * @version 0.1
 * 
 */

#include <condition_variable>
#include <mutex>
#include <cassert>

namespace comm {

    class SharedMutex {
    private:
        constexpr static uint32_t           _writerMask = 1<<31;
        constexpr static uint32_t           _readerMask = UINT32_MAX>>1;
    private:
        mutable std::mutex                  _mutex;
        mutable std::condition_variable     _sharedCV;          // read condition variable
        mutable std::condition_variable     _exclusiveCV;       // write condition variable
        mutable uint32_t                    _statusBits;
    private:
        bool writerEntered() const { return !!(_statusBits&_writerMask); }
        bool writable() const { return 0 == _statusBits; }
        uint32_t readerCount() const { return _statusBits&_readerMask; }
        void setReaderCount( uint32_t count ) const { _statusBits&=~_readerMask; _statusBits|=count; }
    public:
        SharedMutex()
            : _mutex()
            , _sharedCV()
            , _exclusiveCV()
            , _statusBits(0)
        {}

        void lock() const {
            std::unique_lock<std::mutex> lock(_mutex);
            // 第一阶段
            _sharedCV.wait(lock,[this]()->bool{
                return !writerEntered();
            });
            _statusBits |= _writerMask;
            // 第二阶段
            _exclusiveCV.wait(lock,[this]()->bool{
                return readerCount() == 0;
            });
        }

        bool try_lock() const {
            std::unique_lock<std::mutex> lock(_mutex);
            if(!writable()) {
                return false;
            }
            _statusBits = _writerMask;
            return true;
        }

        void unlock() const {
            // TODO: 防止调用者胡乱调用应该加一些断言
            std::unique_lock<std::mutex> lock(_mutex);
            _statusBits = 0;
            /**
             * @brief 解析（李新）
             * 通知其它写线程和读线程
             */
            _sharedCV.notify_all();
        }

        void lock_shared() const {
            std::unique_lock<std::mutex> lock(_mutex);
            /**
             * @brief 解析（李新）
             * 这里主要是要等待两个条件
             * *1 写锁在等待的时候会更改写位，如果这个写位被修改了则读锁要等待
             * *2 读锁有个计数上限，超过了也不行
             * 所以这里需要判断这两个条件，而读锁计数上限是 _readerMask ，
             * 小于它写位必然为0，所在这里直接判断小于 _readerMask即可
             */
            _sharedCV.wait(lock, [this]()->bool{
                return _readerMask>_statusBits;
            });
            ++_statusBits;
        }

        bool try_lock_shared() const {
            std::unique_lock<std::mutex> lock(_mutex);
            if(_readerMask <= _statusBits) {
                return false;
            }
            ++_statusBits;
            return true;
        }

        void unlock_shared() const {
            // TODO: 防止调用者胡乱调用应该加一些断言
            std::unique_lock<std::mutex> lock(_mutex);
            auto readerNum = readerCount()-1;
            setReaderCount(readerNum);
            if(writerEntered()) {
                // 表明写锁lock已经进入第一阶段，优先唤醒写锁
                if(readerCount() == 0) {
                    _exclusiveCV.notify_one();
                }
            } else {
                // 从读满的状态到不满的状态，通知写锁进入第一阶段或者读线程唤醒
                if(readerNum == _readerMask-1) {
                    _sharedCV.notify_one();
                }
            }
        }
    };

}