#pragma once

class Attribute {
  public:
	Attribute(int max, int value) : m_max(max), value(value) {};
	Attribute(int max) : m_max(max), value(max) {};
	int value{ 1 };
	bool inline constexpr isMax() { return value >= m_max; };
	bool inline constexpr isMin() { return value <= 0; };
	void inline maximize() { value = m_max; };
	void inline minimize() { value = 0; };
	bool inline constexpr operator == (const int& rhs) { return value == rhs; };
	bool inline constexpr operator != (const int& rhs) { return value != rhs; };
	bool inline constexpr operator >= (const int& rhs) { return value >= rhs; };
	bool inline constexpr operator <= (const int& rhs) { return value <= rhs; };
	bool inline constexpr operator > (const int& rhs) { return value > rhs; };
	bool inline constexpr operator < (const int& rhs) { return value < rhs; };
	bool inline constexpr operator == (const Attribute& rhs){ return value == rhs.value; };
	bool inline constexpr operator != (const Attribute& rhs) { return value != rhs.value; };
	bool inline constexpr operator >= (const Attribute& rhs) { return value >= rhs.value; };
	bool inline constexpr operator <= (const Attribute& rhs) { return value <= rhs.value; };
	bool inline constexpr operator > (const Attribute& rhs) { return value > rhs.value; };
	bool inline constexpr operator < (const Attribute& rhs) { return value < rhs.value; };
	Attribute inline operator -- () { --value; return *this; };
	Attribute inline operator ++ () { ++value; return *this; };
	Attribute inline operator = (const int& rhs) { value = rhs; return *this; };
	Attribute inline operator -= (const int& rhs) { value -= rhs; return *this; };
	Attribute inline operator += (const int& rhs) { value += rhs; return *this; };
	Attribute inline operator *= (const int& rhs) { value *= rhs; return *this; };
	Attribute inline operator /= (const int& rhs) { value /= rhs; return *this; };
	Attribute inline operator -= (const Attribute& rhs) { value -= rhs.value; return *this; };
	Attribute inline operator += (const Attribute& rhs) { value += rhs.value; return *this; };
	Attribute inline operator *= (const Attribute& rhs) { value *= rhs.value; return *this; };
	Attribute inline operator /= (const Attribute& rhs) { value /= rhs.value; return *this; };
	Attribute inline operator - (const int& rhs) { return Attribute(m_max, value - rhs); };
	Attribute inline operator + (const int& rhs) { return Attribute(m_max, value + rhs); };
	Attribute inline operator * (const int& rhs) { return Attribute(m_max, value * rhs); };
	Attribute inline operator / (const int& rhs) { return Attribute(m_max, value / rhs); };
	Attribute inline operator - (const Attribute& rhs) { return Attribute(m_max, value - rhs.value); };
	Attribute inline operator + (const Attribute& rhs) { return Attribute(m_max, value + rhs.value); };
	Attribute inline operator * (const Attribute& rhs) { return Attribute(m_max, value * rhs.value); };
	Attribute inline operator / (const Attribute& rhs) { return Attribute(m_max, value / rhs.value); };
	void inline constexpr setMax(int max) { m_max = max; }
  private:
	int m_max{ 1 };
};