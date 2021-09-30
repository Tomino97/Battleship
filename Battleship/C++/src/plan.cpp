#include "geometry.hpp"
#include "plan.hpp"

bool operator==(const Tile &tile, const Tile &other) {
    return tile.coords == other.coords &&
           tile.type == other.type &&
           tile.opened == other.opened;
}

bool operator!=(const Tile &tile, const Tile &other) {
    return !(tile == other);
}

std::ostream &operator<<(std::ostream &out, const Plan &plan) {
    out << "\n" << "  ";
    for (int i = 1; i <= plan.width; i++) {
        out << i;
    }
    out << "\n" << " ";
    for (int i = 0; i < plan.width + 2; i++) {
        out << "#";
    }
    out << "\n";
    char tmp = 'A';
    for (const std::vector<Tile> &row : plan.grid) {
        out << tmp << '#';
        tmp++;
        for (const Tile &tile : row) {
            if (!tile.opened) {
                out << UNKNOWN_CHAR;
                continue;
            }
            switch (tile.type) {
                case OCEAN:
                    out << OCEAN_CHAR;
                    break;
                case BOAT:
                    out << BOAT_CHAR;
                    break;
                case UNKNOWN:
                    out << UNKNOWN_CHAR;
                    break;
            }
        }
        out << '#' << "\n";
    }
    out << " ";
    for (int i = 0; i < plan.width + 2; i++) {
        out << "#";
    }
    out << "\n";
    return out;
}
