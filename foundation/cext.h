// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#pragma once

#include "foundation/assert.h"
#include <algorithm>
#include <cstring>
#include <limits>
#include <memory>
#include <string>

namespace hg {

#if __cplusplus <= 201103L // upto C++11
#define nullptr NULL
#endif

template <typename T> inline bool asbool(const T &v) { return v ? true : false; }

#define forever while (true)
#define unused(ARG)

#define __ERR__(err_exp, ret_exp)                                                                                                                              \
	{                                                                                                                                                          \
		(err_exp);                                                                                                                                             \
		return (ret_exp);                                                                                                                                      \
	}
#define __ERRRAW__(err_exp)                                                                                                                                    \
	{                                                                                                                                                          \
		(err_exp);                                                                                                                                             \
		return;                                                                                                                                                \
	}

/// Set 'variable' value if it does not compare equal to 'value', return true if 'variable' was modified.
template <typename T> inline bool set_if_not_equal(T &variable, const T &value) {
	bool r = variable != value;
	if (r)
		variable = value;
	return r;
}

#if __cplusplus >= 201103L
/// Set 'variable' value if it does not compare equal to 'value', return true if 'variable' was modified.
template <typename T> inline bool set_if_not_equal(T &variable, const T &&value) {
	bool r = variable != value;
	if (r)
		variable = std::move(value);
	return r;
}
#endif

/// Set a bool variable to false prior to returning its original value.
inline bool bool_gate(bool &cond) {
	bool _cond = cond;
	cond = false;
	return _cond;
}

#if __cplusplus >= 201103L

/// Bind shared_ptr<T> to functions expecting T&, const T&, T* or const T*.
template <typename T> struct shared_ref_ {
	explicit shared_ref_(const std::shared_ptr<T> &r_) : r(r_) {}
	explicit shared_ref_(std::shared_ptr<T> &&r_) : r(std::move(r_.r)) {}
	operator T &() const { return *r; }
	operator T *() const { return r.get(); }

	std::shared_ptr<T> r;
};

template <typename T> shared_ref_<T> shared_ref(const std::shared_ptr<T> &p) { return shared_ref_<T>(p); }
template <typename T> shared_ref_<T> shared_ref(const std::shared_ptr<T> &&p) { return shared_ref_<T>(p); }

template <typename T, typename D> shared_ref_<T> shared_ref_cast(const std::shared_ptr<D> &p) { return shared_ref<T>(std::static_pointer_cast<D>(p)); }

/// Bind std::string to functions expecting const char *.
struct bind_string {
	explicit bind_string(const char *s_) : str(s_) {}
	explicit bind_string(std::string s_) : str(std::move(s_)) {}
	explicit bind_string(const std::string &&s_) : str(std::move(s_)) {}
	operator const char *() const { return str.c_str(); }

	std::string str;
};

// plain raw memory block
template <typename T> struct memory_block {
	memory_block() = default;
	memory_block(const T *data_, size_t size_) : data(new char[size_]), size(size_) {
		if (data_)
			memcpy(data, data_, size_);
	}
	~memory_block() { delete[] data; }

	operator const T *() const { return reinterpret_cast<T *>(data); }

	T *get_data() const { return reinterpret_cast<T *>(data); }
	size_t get_size() const { return size; }

private:
	char *data{};
	size_t size{};
};

template <typename C, typename V> bool push_back_unique(C &c, const V &v) {
	auto i = std::find(c.begin(), c.end(), v);
	if (i != c.end())
		return false;
	c.push_back(v);
	return true;
}

template <typename C, typename V> bool find_erase(C &c, const V &v) {
	auto i = std::find(c.begin(), c.end(), v);
	if (i == c.end())
		return false;
	c.erase(i);
	return true;
}

template <typename C, typename V> bool contains(const C &c, const V &v) { return std::find(c.begin(), c.end(), v) != c.end(); }

// until C++17 and std::enable_shared_from_this::weak_from_this()
template <typename T> std::weak_ptr<T> weak_from_this(std::enable_shared_from_this<T> *s) { return std::weak_ptr<T>(s->shared_from_this()); }
template <typename T> std::weak_ptr<const T> weak_from_this(const std::enable_shared_from_this<T> *s) { return std::weak_ptr<const T>(s->shared_from_this()); }

// from https://channel9.msdn.com/Events/GoingNative/2013/Cpp-Seasoning
template <typename I> auto slide(I f, I l, I p) -> std::pair<I, I> { // I model RandomAccessIterator
	if (p < f)
		return {p, std::rotate(p, f, l)};
	if (l < p)
		return {std::rotate(f, l, p), p};
	return {f, l};
}

#undef min
#undef max

// perform a numeric cast after asserting that the input value fits in the target type
template <class CastType, class Type> CastType numeric_cast(Type v) {
	static_assert(std::is_integral<CastType>::value && std::is_integral<Type>::value, "numeric_cast is only valid for integral types");

#if _DEBUG
	auto mn = std::numeric_limits<CastType>::min(), mx = std::numeric_limits<CastType>::max();

	bool r;
	if (std::is_signed<CastType>::value && !std::is_signed<Type>::value) { // int/size_t
		r = size_t(v) <= size_t(mx);
	} else if (!std::is_signed<CastType>::value && std::is_signed<Type>::value) {
		r = v >= 0 && size_t(v) <= size_t(mx);
	} else if (std::is_signed<CastType>::value && std::is_signed<Type>::value) {
		r = ptrdiff_t(v) >= ptrdiff_t(mn) && ptrdiff_t(v) <= ptrdiff_t(mx);
	} else { // both non-signed
		r = size_t(v) >= size_t(mn) && size_t(v) <= size_t(mx);
	}

	if (!r)
		trigger_assert(__FILE__, __LINE__, __FUNCTION__, "numeric_cast", "Cast operation will result in truncation");
#endif

	return CastType(v);
}

template <typename T, typename... Args> std::unique_ptr<T> make_unique(Args &&... args) { return std::unique_ptr<T>(new T(std::forward<Args>(args)...)); }

template <typename C, typename V> int index_of(const C &c, const V &v, int if_missing = -1) {
	const auto i = std::find(c.begin(), c.end(), v);
	if (i == std::end(c))
		return if_missing;
	return numeric_cast<int, size_t>(std::distance(std::begin(c), i));
}
#endif

} // namespace hg

#if __cplusplus < 201103L
typedef uint32_t char32_t;

namespace std {
typedef basic_string<char32_t, char_traits<char32_t>, allocator<char32_t> > u32string;
}
#endif

#if __cplusplus >= 201103L
#include <array>
#else
namespace std {
// swap is not implemented
template <typename T, size_t N> class array {
public:
	typedef T *iterator;
	typedef const T *const_iterator;
	typedef std::reverse_iterator<iterator> reverse_iterator;
	typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

	array() {}
	array(const T &v) { std::fill(begin(), end(), v); }
	array(const T *ptr) { std::copy(ptr, ptr + N, begin()); }
	array(const array &a) { std::copy(a.begin(), a.end(), begin()); }

	inline T &at(size_t offset) { return _elmnt[offset]; }
	inline const T &at(size_t offset) const { return _elmnt[offset]; }

	inline T &operator[](size_t offset) { return at(offset); }
	inline const T &operator[](size_t offset) const { return at(offset); }

	inline T &front() { return at(0); }
	inline const T &front() const { return at(0); }

	inline T &back() { return at(N - 1); }
	inline const T &back() const { return N ? at(N - 1) : at(N); }

	T *data() { return &_elmnt[0]; }
	const T *data() const { return &_elmnt[0]; }

	size_t size() const { return N; }
	bool empty() const { return size() == 0; }

	iterator begin() { return iterator(data()); }
	const_iterator begin() const { return const_iterator(data()); }

	iterator end() { return iterator(data() + N); }
	const_iterator end() const { return const_iterator(data() + N); }

	reverse_iterator rbegin() { return reverse_iterator(end()); }
	const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }

	reverse_iterator rend() { return reverse_iterator(begin()); }
	const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }

	array &operator=(const array &a) {
		std::copy(a.begin(), a.end(), begin());
		return *this;
	}

	void fill(const T &value) { std::fill(begin(), end(), value); }

	private:
	T _elmnt[N];
};

template <typename T, size_t N> bool operator==(const array<T, N> &lhs, const array<T, N> &rhs) { return std::equal(lhs.begin(), lhs.end(), rhs.begin()); }
template <typename T, size_t N> bool operator!=(const array<T, N> &lhs, const array<T, N> &rhs) { return !(lhs == rhs); }
template <typename T, size_t N> bool operator<(const array<T, N> &lhs, const array<T, N> &rhs) {
	return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}
template <typename T, size_t N> bool operator>(const array<T, N> &lhs, const array<T, N> &rhs) { return rhs > lhs; }
template <typename T, size_t N> bool operator<=(const array<T, N> &lhs, const array<T, N> &rhs) { return !(lhs > rhs); }
template <typename T, size_t N> bool operator>=(const array<T, N> &lhs, const array<T, N> &rhs) { return !(lhs < rhs); }

}
#endif

#if __cplusplus >= 201103L
#include <type_traits>
#else 
namespace std {

template <typename T> struct remove_const { typedef T type; };
template <typename T> struct remove_const<const T> { typedef T type; };

template <typename T> struct remove_volatile { typedef T type; };
template <typename T> struct remove_volatile<volatile T> { typedef T type; };

template <typename T> struct remove_cv { typedef T type; };
template <typename T> struct remove_cv<const T> { typedef T type; };
template <typename T> struct remove_cv<volatile T> { typedef T type; };
template <typename T> struct remove_cv<const volatile T> { typedef T type; };

template <class T, T v> struct integral_constant {
	typedef T value_type;
	typedef integral_constant<T, v> type;
	operator value_type() const { return value; }
	static const T value = v;
};
typedef integral_constant<bool, true> true_type;
typedef integral_constant<bool, false> false_type;

namespace detail {
template <typename T> class is_integral : std::false_type {};
template <> struct is_integral<bool> : public std::integral_constant<bool, true> {};
template <> struct is_integral<char> : public std::integral_constant<bool, true> {};
template <> struct is_integral<signed char> : public std::integral_constant<bool, true> {};
template <> struct is_integral<unsigned char> : public std::integral_constant<bool, true> {};
template <> struct is_integral<short> : public std::integral_constant<bool, true> {};
template <> struct is_integral<unsigned short> : public std::integral_constant<bool, true> {};
template <> struct is_integral<int> : public std::integral_constant<bool, true> {};
template <> struct is_integral<unsigned int> : public std::integral_constant<bool, true> {};
template <> struct is_integral<long> : public std::integral_constant<bool, true> {};
template <> struct is_integral<unsigned long> : public std::integral_constant<bool, true> {};
template <> struct is_integral<long long> : public std::integral_constant<bool, true> {};
template <> struct is_integral<unsigned long long> : public std::integral_constant<bool, true> {};

template <typename> struct is_floating_point : public std::false_type {};
template <> struct is_floating_point<float> : public std::integral_constant<bool, true> {};
template <> struct is_floating_point<double> : public std::integral_constant<bool, true> {};
template <> struct is_floating_point<long double> : public std::integral_constant<bool, true> {};
} // namespace detail

template <typename T> struct is_integral : public integral_constant<bool, detail::is_integral<typename remove_cv<T>::type>::value> {};
template <typename T> struct is_floating_point : public integral_constant<bool, detail::is_floating_point<typename remove_cv<T>::type>::value> {};

template <typename T> struct is_arithmetic : public integral_constant<bool, is_integral<T>::value || is_floating_point<T>::value> {};

namespace detail {
template <typename T, bool = std::is_arithmetic<T>::value> struct is_signed : public std::integral_constant<bool, (T(-1) < T(0))> {};
template <typename T> struct is_signed<T, false> : public std::false_type {};
} // namespace detail

template <typename T> struct is_signed : public detail::is_signed<T>::type {};

}
#endif