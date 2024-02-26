#pragma once 
#include <cstdint>
#include <initializer_list>
#include <type_traits>
#include <cassert>

namespace comm {

    template<typename T>
    requires std::is_enum_v<T>
    class BitFlags {
    private:
        using underlying_type = typename std::underlying_type<T>::type;
        underlying_type _flags;
        // std::underlying_type<T> _flags;
    private:
        template<typename F>
        constexpr bool is_2_power(F x) const {
            return (x != 0) && ((x & (x-1))) == 0;
        }
        template<typename F, typename ...Args>
        constexpr underlying_type compose(F val, Args ...args) const {
            return compose(val) | compose(args...);
        }
        template<typename F>
        constexpr underlying_type compose(F val) const{
            underlying_type result = (underlying_type)val;
            assert(is_2_power(result)&&"value is not a power of 2");
            return result;
        }
        constexpr underlying_type compose() const {
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
        constexpr operator underlying_type() const {
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
