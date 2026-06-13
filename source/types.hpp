#pragma once

#include <cmath>
#include <array>
#include <utility>
#include <type_traits>
#include <iostream>
#include <sstream>
#include <vector>
#include <stdexcept>
#include <cassert>

namespace Spår::Utility::Types
{
typedef float					float32_t;
typedef double					float64_t;
typedef long double				float128_t;

template<typename T, size_t N> requires std::is_arithmetic<T>::value
struct Vector
{
	static_assert(N > 0, "Vector must have more than 0 components.");

public:
	void markMagnitudeDirty() { magnitudeDirty = true; }

	constexpr static Vector<T, N> i()
	{
		std::array<T, N> result{0};
		result[0] = 1;

		return Vector<T, N>(result);
	}

	constexpr static Vector<T, N> j()
	{
		static_assert(N > 1, "j component requires at least 2 components.");
		std::array<T, N> result{0};
		result[1] = 1;

		return Vector<T, N>(result);
	}

	constexpr static Vector<T, N> k()
	{
		static_assert(N > 2, "k component requires at least 3 components.");
		std::array<T, N> result{0};
		result[2] = 1;

		return Vector<T, N>(result);
	}

	constexpr Vector() : components() {}
	constexpr Vector(std::array<T, N> components) : components(components) {}

	template<typename... Args>
	constexpr Vector(Args... args) : components{ static_cast<T>(args)... }
	{
		static_assert(N == sizeof...(Args), "Number of arguments must match vector size.");
	}

	constexpr T &operator[](size_t index)
	{
		assert(index < N && "Vector index out of bounds");

		return components[index];
	}

	constexpr const T &operator[](size_t index) const
	{
		assert(index < N && "Vector index out of bounds");

		return components[index];
	}

	// Bypass component array bound checking for performance heavy code
	constexpr T &unsafeAt(size_t index) { return components[index]; }
	constexpr const T &unsafeAt(size_t index) const { return components[index]; }

	/*
		void x(), y(), z() and w() are guaranteed to call markMagnitudeDirty()

		If you use the [] operator and store the reference, you have to manually
		call markMagnitudeDirty at the end of operations (it is marked dirty)
	*/
	[[nodiscard]] constexpr T x() const { return components[0]; }
	inline void x(T value) { components[0] = value; markMagnitudeDirty(); }
	[[nodiscard]] constexpr T y() const { static_assert(N > 1, "Vector has no y component."); return components[1]; }
	inline void y(T value) { static_assert(N > 1, "Vector has no y component."); components[1] = value; markMagnitudeDirty(); }
	[[nodiscard]] constexpr T z() const { static_assert(N > 2, "Vector has no z component."); return components[2]; }
	inline void z(T value) { static_assert(N > 2, "Vector has no z component."); components[2] = value; markMagnitudeDirty(); }
	[[nodiscard]] constexpr T w() const { static_assert(N > 3, "Vector has no w component."); return components[3]; }
	inline void w(T value) { static_assert(N > 3, "Vector has no w component."); components[3] = value; markMagnitudeDirty(); }

	constexpr Vector<T, N> operator+(const Vector<T, N> &other) const
	{
		std::array<T, N> result(components);
		for(size_t i = 0; i < N; i++)
		{
			result[i] += other.components[i];
		}

		return Vector<T, N>(result);
	}

	constexpr Vector<T, N> operator-(const Vector<T, N> &other) const
	{
		std::array<T, N> result(components);
		for(size_t i = 0; i < N; i++)
		{
			result[i] -= other.components[i];
		}

		return Vector<T, N>(result);
	}

	constexpr Vector<T, N> operator*(T scalar) const
	{
		std::array<T, N> result;
		for(size_t i = 0; i < N; i++)
		{
			result[i] = components[i] * scalar;
		}

		return Vector<T, N>(result);
	}

	friend constexpr Vector<T, N> operator*(T scalar, const Vector<T, N> &vector)
	{
		return vector * scalar;
	}

	// Check parallel
	constexpr bool operator||(const Vector<T, N> &other) const
	{
		if(components[0] == 0) return other.components[0] == 0;

		T scalar = other.components[0] / components[0];
		for(size_t i = 1; i < N; i++)
		{
			if(components[i] * scalar != other.components[i])
			{
				return false;
			}
		}

		return true;
	}

	constexpr T operator*(const Vector<T, N> &other) const
	{
		T result = 0;
		for(size_t i = 0; i < N; i++)
		{
			result += components[i] * other.components[i];
		}

		return result;
	}

	constexpr Vector<T, 3> cross(const Vector<T, 3> &other) const
	{
		static_assert(N == 3, "Cross product is only defined for 3D vectors.");
		return Vector<T, 3>(
			components[1] * other.components[2] - components[2] * other.components[1],
			components[2] * other.components[0] - components[0] * other.components[2],
			components[0] * other.components[1] - components[1] * other.components[0]
		);
	}

	constexpr Vector<T, N> operator-() const
	{
		std::array<T, N> result;
		for(size_t i = 0; i < N; i++)
		{
			result[i] = -components[i];
		}

		return Vector<T, N>(result);
	}

	[[nodiscard]] constexpr T magnitude() const
	{
		if(magnitudeDirty)
		{
			T result = 0;
			for(size_t i = 0; i < N; i++)
			{
				result += components[i] * components[i];
			}
	
			magnitudeDirty = false;
			return (magnitudeCache = std::sqrt(result));
		} else
		{
			return magnitudeCache;
		}
	}

	[[nodiscard]] constexpr Vector<T, N> normalise() const
	{
		T length = magnitude();
		if(0 == length) return Vector<T, N>();
		
		std::array<T, N> result;
		T inverseMagnitude = static_cast<T>(1) / length;
		for(size_t i = 0; i < N; i++)
		{
			result[i] = components[i] * inverseMagnitude;
		}

		magnitudeDirty = false;
		magnitudeCache = static_cast<T>(1);

		return Vector<T, N>(result);
	}

	void flip()
	{
		for(size_t i = 0; i < N; i++)
		{
			components[i] = -components[i];
		}
	}

	[[nodiscard]] constexpr T angle() const
	{
		static_assert(N >= 2, "Angle calculation requires at least 2 components.");

		return std::atan2(components[1], components[0]);
	}

	[[nodiscard]] constexpr T acuteAngleTo(const Vector<T, N> &other) const
	{
		static_assert(N >= 2, "Angle calculation requires at least 2 components.");
		
		T magProduct = magnitude() * other.magnitude();
		if(magProduct == 0) return 0;
		
		T dot = *this * other;
		return std::acos(dot / magProduct);
	}

	Vector<T, 2> toPolar() const
	{
		static_assert(N == 2, "Polar conversion requires 2 components.");
		return Vector<T, 2>(magnitude(), angle());
	}

	static Vector<T, 2> fromPolar(T r, T theta)
	{
		return Vector<T, 2>(r * std::cos(theta), r * std::sin(theta));
	}

	static Vector<T, 2> fromPolar(const Vector<T, 2> &input)
	{
		return Vector<T, 2>(input[0] * std::cos(input[1]), input[0] * std::sin(input[1]));
	}

	constexpr bool operator==(const Vector<T, N> &other) const
	{
		for(size_t i = 0; i < N; i++)
		{
			if(components[i] != other.components[i])
			{
				return false;
			}
		}

		return true;
	}

	constexpr bool operator!=(const Vector<T, N> &other) const
	{
		return !(*this == other);
	}

	std::string toString() const
	{
		std::ostringstream oss;
		
		oss << "<";
		for(size_t i = 0; i < N; ++i)
		{
			oss << components[i];
			if(i != N - 1) oss << ", ";
		}
		oss << ">";

		return oss.str();
	}

	friend std::ostream &operator<<(std::ostream &os, const Vector<T, N> &vector)
	{
		return os << vector.toString();
	}

private:
	std::array<T, N> components;

	mutable T magnitudeCache = 0;
	mutable bool magnitudeDirty = true;
};

typedef Vector<int32_t, 2> Vector2_i;
typedef Vector<float32_t, 2> Vector2;
typedef Vector<float64_t, 2> Vector2_d;
typedef Vector<int32_t, 3> Vector3_i;
typedef Vector<float32_t, 3> Vector3;
typedef Vector<float64_t, 3> Vector3_d;
typedef Vector<int32_t, 4> Vector4_i;
typedef Vector<float32_t, 4> Vector4;
typedef Vector<float64_t, 4> Vector4_d;

template<typename T, size_t sides> requires std::is_arithmetic<T>::value
constexpr std::vector<Vector<T, 2> > generatePolygon()
{
	static_assert(sides >= 3, "Polygon must have at least 3 sides.");

	std::vector<Vector<T, 2> > result;
	result.reserve(sides);

	T theta = 0;
	T dtheta = static_cast<T>(2) * M_PI / static_cast<T>(sides);
	for(size_t i = 0; i < sides; i++)
	{
		result.emplace_back(Vector<T, 2>::fromPolar(1, theta));
		theta += dtheta;
	}

	return result;
}
}