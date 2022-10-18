#pragma once 
#include <cstdint>
#include <type_traits>
#include <cassert>

namespace common {

    template<typename T, typename IntType = uint8_t, typename validation = std::enable_if_t<std::is_enum<T>::value, bool>>
    class BitFlags {
    private:
        IntType _flags;
    private:
        template<typename F>
        constexpr bool is_2_power(F x) const {
            return (x != 0) && ((x & (x-1))) == 0;
        }
        template<typename F, typename ...Args>
        constexpr IntType compose(F val, Args ...args) const {
            return compose(val) | compose(args...);
        }
        template<typename F>
        constexpr IntType compose(F val) const{
            IntType result = (IntType)val;
            assert(is_2_power(result)&&"value is not a power of 2");
            return result;
        }
        constexpr IntType compose() const {
            return 0;
        }
    public:
        template<typename... Args>
        constexpr BitFlags(Args ...args)
            : _flags(compose(args...))
        {
        }
        BitFlags(std::initializer_list<T> const& list) : _flags(0) {
            for(auto const& val : list) {
                _flags |= compose(val);
            }
        }
        constexpr bool test(T val) const {
            return (_flags & compose(val)) == compose(val);
        }
        constexpr operator IntType() const {
            return _flags;
        }
        void append(T val) {
            _flags |= compose(val);
        }
        void remove(T val) {
            _flags &= ~compose(val);
        }
    };

}
