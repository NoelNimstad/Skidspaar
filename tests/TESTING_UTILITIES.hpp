#pragma once

template<typename T>
constexpr bool WITHIN_RANGE(T a, T b, T d)
{
	return ((b) - (d) < (a) && (b) + (d) > (a));
}

#define ε 0.00001
template<typename T>
constexpr bool WITHIN_ε(T a, T b)
{
	return ((b) - static_cast<T>(ε) < (a) && (b) + static_cast<T>(ε) > (a));
}