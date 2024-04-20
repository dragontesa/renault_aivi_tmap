#pragma once

namespace SKT
{

template<typename def, typename inner = typename def::type>
class TSafeEnum : public def
{
    typedef typename def::type type;
	inner val;

public:

    TSafeEnum(type v) : val(v) {}
	inner underlying() const { return val; }

    friend bool operator == (const TSafeEnum & lhs, const TSafeEnum & rhs) { return lhs.val == rhs.val; }
    friend bool operator != (const TSafeEnum & lhs, const TSafeEnum & rhs) { return lhs.val != rhs.val; }
    friend bool operator <  (const TSafeEnum & lhs, const TSafeEnum & rhs) { return lhs.val <  rhs.val; }
    friend bool operator <= (const TSafeEnum & lhs, const TSafeEnum & rhs) { return lhs.val <= rhs.val; }
    friend bool operator >  (const TSafeEnum & lhs, const TSafeEnum & rhs) { return lhs.val >  rhs.val; }
    friend bool operator >= (const TSafeEnum & lhs, const TSafeEnum & rhs) { return lhs.val >= rhs.val; }
};

}

#define T_SAFE_ENUM(enumType,...) \
	struct enumType##_def {\
		enum type {\
			__VA_ARGS__\
		};\
	};\
    typedef SKT::TSafeEnum<enumType##_def> enumType;

