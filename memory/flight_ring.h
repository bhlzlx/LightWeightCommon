#pragma once
#include <cstdint>

namespace comm {
    // constexpr uint32_t FlightCount = 2;
    template<size_t FlightCount = 2>
    class FlightRing {
        struct flight_range_t {
            uint32_t begin;
            uint32_t end;
        };
    private:
        uint32_t            size_;
        uint32_t            align_;
        uint32_t            flight_;
        flight_range_t      range_;
        flight_range_t      flightRanges_[FlightCount];
    public:
        static constexpr uint32_t InvalidAlloc = ~0;
        FlightRing(uint32_t size, uint32_t alignSize)
            : size_(size)
            , align_(alignSize-1)
            , flight_(0)
            , range_{}
            , flightRanges_{}
        {}
        void reset(uint32_t size) {
            size_ = size;
            flight_ = 0;
            range_ = {};
            for(auto& range: flightRanges_) {
                range = {};
            }
        }
        flight_range_t const& flight() const {
            return flightRanges_[flight_];
        }
        flight_range_t& flight() {
            return flightRanges_[flight_];
        }
        void prepareNextFlight() {
            ++flight_; 
            flight_%=FlightCount;
            range_.begin = flightRanges_[flight_].end; // 回收上一帧
            flightRanges_[flight_].begin = flightRanges_[flight_].end = range_.end; // 准备下一帧
        }
        uint32_t alloc(uint32_t size) {
            size = (size + align_) & ~align_;
            uint32_t max = size_;
            if(range_.end < range_.begin) {
                max = range_.begin;
            }
            while(true) {
                uint32_t availSize = max - range_.end;
                uint32_t offset = range_.end;
                if(availSize >= size) {
                    range_.end = flightRanges_[flight_].end = offset + size;
                    return offset;
                } else {
                    if(range_.begin > 0 && range_.end > range_.begin) {
                        range_.end = 0;
                        max = range_.begin;
                    } else {
                        return InvalidAlloc;
                    }
                }
            }
        }
    };
}

