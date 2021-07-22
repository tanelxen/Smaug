#pragma once
#include <glm/vec3.hpp>

// Dummy stuff so when we rewire this to be bgfx independent, it's easier
typedef uint16_t texture_t;
#define INVALID_TEXTURE UINT16_MAX

const double PI = 3.141592653589793238463;


// Pitch, Yaw, Roll
void Directions(glm::vec3 angles, glm::vec3* forward = nullptr, glm::vec3* right = nullptr, glm::vec3* up = nullptr);
glm::vec3 Angles(glm::vec3 forward, glm::vec3* up = nullptr);


template<typename T>
constexpr T max(T a, T b)
{
	if (a > b)
		return a;
	return b;
}

template<typename T>
constexpr T min(T a, T b)
{
	if (a < b)
		return a;
	return b;
}


template<typename T>
constexpr T clamp(T value, T min, T max)
{
	if (value > max)
		return max;
	if (value < min)
		return min;
	return value;
}

template<bool unordered = false>
inline constexpr bool inRange(float min, float max, float value)
{
	if constexpr (unordered)
	{
		if (min > max)
		{
			float t = min;
			min = max;
			max = t;
		}
	}
	return (value >= min) && (value <= max);
}

template<bool unordered = false>
inline constexpr bool rangeInRange(float largerMin, float largerMax, float smallerMin, float smallerMax)
{
	return inRange<unordered>(largerMin, largerMax, smallerMin) || inRange<unordered>(largerMin, largerMax, smallerMax);
}

template<bool unordered1 = false, bool unordered2 = false>
inline constexpr bool rangeOverlap(float min1, float max1, float min2, float max2)
{
	return rangeInRange<unordered1>(min1, max1, min2, max2) || rangeInRange<unordered2>(min2, max2, min1, max1);
}


inline constexpr bool closeTo(float value, float target, float threshold = 0.0001f)
{
	return inRange(target - threshold, target + threshold, value);
}


inline constexpr glm::vec3 dirMask(glm::vec3 vec)
{
	return { 1.0f - fabs(vec.x), 1.0f - fabs(vec.y), 1.0f - fabs(vec.z) };
}


#define MAKE_BITFLAG(enumName) \
inline enumName& operator |=  (enumName& a, enumName b) { a = static_cast<enumName>(a | b);  return a; } \
inline enumName& operator &=  (enumName& a, enumName b) { a = static_cast<enumName>(a & b);  return a; } \
inline enumName& operator ^=  (enumName& a, enumName b) { a = static_cast<enumName>(a ^ b);  return a; } \
inline enumName& operator <<= (enumName& a, int b)      { a = static_cast<enumName>(a >> b); return a; } \
inline enumName& operator >>= (enumName& a, int b)      { a = static_cast<enumName>(a << b); return a; } \
inline enumName  operator ~   (enumName a) { return static_cast<enumName>(~(char)a); }