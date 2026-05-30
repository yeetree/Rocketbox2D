#ifndef ENGINE_CORE_FLAGS
#define ENGINE_CORE_FLAGS

namespace Engine
{
    template <typename T>
    class Flags {
        // Type must be an enum
        static_assert(std::is_enum<T>::value, "Flags type parameter must be an enum!");

    public:
        using underlying_type = typename std::underlying_type<T>::type;

        Flags() : value_(0) {}
        Flags(T e) : value_(static_cast<underlying_type>(e)) {}

        Flags& operator|=(const Flags& other) { value_ |= other.value_; return *this; }
        Flags& operator&=(const Flags& other) { value_ &= other.value_; return *this; }
        
        Flags operator|(const Flags& other) const { return Flags(value_ | other.value_); }
        Flags operator&(const Flags& other) const { return Flags(value_ & other.value_); }
        Flags operator~() const { return Flags(~value_); }

        bool Has(T e) const { return (value_ & static_cast<underlying_type>(e)) != 0; }
        explicit operator bool() const { return value_ != 0; }

    private:
        explicit Flags(underlying_type val) : value_(val) {}
        underlying_type value_;
    };
} // namespace Engine


#endif // ENGINE_CORE_FLAGS
