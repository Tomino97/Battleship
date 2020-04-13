#pragma once

#include <utility>
#include <ostream>

using Vector = std::pair<short, short>;

#define DIRECTIONS_SIZE 4
enum DirectionName {
	UP,
	RIGHT,
	DOWN,
	LEFT
};

Vector operator+(const Vector& v1, const Vector& v2);
Vector operator-(const Vector& v1, const Vector& v2);
Vector operator*(const Vector& v, int x);
Vector operator*(int x, const Vector& v);

struct Coords {
    short x, y;

	Coords()		 : x{0}, y{0} { }
	Coords(short x, short y) : x{x}, y{y} { }

	Coords(const Coords& coords) = default;

    Coords& operator+=(const Vector& v)
    {
        x = static_cast<short>(x + v.first);
        y = static_cast<short>(y + v.second);
        return *this;
    }

    Coords& operator-=(const Vector& v)
    {
        x = static_cast<short>(x - v.first);
        y = static_cast<short>(y - v.second);
        return *this;
    }

	friend bool operator==(const Coords& c1, const Coords& c2);
	friend bool operator!=(const Coords& c1, const Coords& c2);
	friend Coords operator+(const Coords& c, const Vector& v);
	friend Coords operator-(const Coords& c, const Vector& v);
    friend std::ostream& operator<<(std::ostream &out, const Coords&plan);
};
