#pragma once
#include <queue>
#include <cstdint>

namespace comm {

    struct VersionedUID {
        union {
            struct {
                uint32_t ver : 8;
                uint32_t number : 24;
            };
            uint32_t uuid;
        };
    };
    static constexpr VersionedUID InvalidUID = {0, 0};

    static_assert(sizeof(VersionedUID) == sizeof(uint32_t), "");

    class VersionedUIDManager {
    private:
        uint32_t                    counter_;
        std::queue<VersionedUID>    freeList_;
    public:
        void free(VersionedUID id) {
            ++id.ver;
            freeList_.push(id);
        }
        VersionedUID alloc() {
            if(freeList_.size()) {
                auto rst = freeList_.front();
                freeList_.pop();
                return rst;
            } else {
                if(counter_ & ~0xffffff) { // 超过最大限了
                    return {}; // invalid id
                } else {
                    return VersionedUID{0, ++counter_}; // 从1开始计算
                }
            }
        }
    };

}