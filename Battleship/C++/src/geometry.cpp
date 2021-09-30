#include <vector>
#include "geometry.hpp"

bool operator==(const Coords &c1, const Coords &c2) {
    return c1.x == c2.x && c1.y == c2.y;
}

Coords operator+(const Coords &c, const Vector &v) {
    return {static_cast<short>((c.x + v.first)),
            static_cast<short>(c.y + v.second)};
}

Coords operator-(const Coords &c, const Vector &v) {
    return {static_cast<short>(c.x - v.first),
            static_cast<short>(c.y - v.second)};
}

bool operator!=(const Coords &c1, const Coords &c2) {
    return !(c1 == c2);
}

Vector operator+(const Vector &v1, const Vector &v2) {
    return std::make_pair(static_cast<short>(v1.first + v2.first),
                          static_cast<short>(v1.second + v2.second));
}

Vector operator-(const Vector &v1, const Vector &v2) {
    return std::make_pair(static_cast<short>(v1.first - v2.first),
                          static_cast<short>(v1.second - v2.second));
}

Vector operator*(const Vector &v, int x) {
    return std::make_pair(static_cast<short>(v.first * x),
                          static_cast<short>(v.second * x));
}

Vector operator*(int x, const Vector &v) {
    return v * x;
}

std::ostream &operator<<(std::ostream &out, const Coords &coords) {
    out << "[" << coords.x << "," << coords.y << "] ";
    return out;
}

Coords operator-(const Coords &c, const Coords &c2) {
    return Coords(static_cast<short>(c.x - c2.x), static_cast<short>(c.y - c2.y));
}

Coords operator+(const Coords &c, const Coords &c2) {
    return Coords(static_cast<short>(c.x + c2.x), static_cast<short>(c.y + c2.y));
}
