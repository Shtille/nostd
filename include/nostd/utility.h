#ifndef __NOSTD_UTILITY_H__
#define __NOSTD_UTILITY_H__

namespace nostd {
namespace utility {

	template <typename T>
	T&& move(T&& t)
	{
		return static_cast<T&&>(t);
	}
	template <typename T>
	T&& move(T& t)
	{
		return static_cast<T&&>(t);
	}

	template <typename T>
	T&& forward(T&& t)
	{
		return static_cast<T&&>(t);
	}
	template <typename T>
	T&& forward(T& t)
	{
		return static_cast<T&&>(t);
	}

	template <typename T>
	void swap(T& lhs, T& rhs)
	{
		T t = lhs;
		lhs = rhs;
		rhs = t;
	}

	template <typename A, typename B>
	struct pair
	{
		A first;
		B second;

		pair() : first(), second() {}
		pair(const A& a, const B& b) : first(a), second(b) {}
		pair(const pair& other) : first(other.first), second(other.second) {}
		pair(pair&& other) : first(move(other.first)), second(move(other.second)) {}
		pair& operator =(const pair& other)
		{
			first = other.first;
			second = other.second;
			return *this;
		}
		void operator =(pair&& other)
		{
			first = move(other.first);
			second = move(other.second);
			return *this;
		}
		bool operator ==(const pair& other) const
		{
			return first == other.first && second == other.second;
		}
		bool operator !=(const pair& other) const
		{
			return first != other.first || second != other.second;
		}
	};

} // namespace utility
} // namespace nostd

#endif