#pragma once

#include <vector>
#include <random>
#include <iostream>
#include <chrono>
#include <cassert>

#include "geometry.hpp"

#define OCEAN_CHAR    ' '
#define BOAT_CHAR    '*'
#define UNKNOWN_CHAR    '?'

enum TileType {
    UNKNOWN,
    OCEAN,    /* '-' */
    BOAT    /* '*' */
};

struct Tile {
    Coords coords;
    TileType type;
    bool opened;

    //constructors for the different inputs
    Tile(short x, short y, TileType type) : coords{x, y}, type{type}, opened{false} {}

    Tile(const Coords &coords, TileType type) : coords{coords}, type{type}, opened{false} {}

    Tile(short x, short y, TileType type, bool opened) : coords{x, y}, type{type}, opened{opened} {}

    Tile(const Coords &coords, TileType type, bool opened) : coords{coords}, type{type}, opened{opened} {}

    friend bool operator==(const Tile &tile, const Tile &other);

    friend bool operator!=(const Tile &tile, const Tile &other);
};

class Plan {
    int hits;

    //method to print out the plan
    friend std::ostream &operator<<(std::ostream &out, const Plan &plan);

    void randomize() {
        std::mt19937 generator(
                std::chrono::high_resolution_clock::now().time_since_epoch().count()); // seed the generator
        std::uniform_int_distribution<short> xAxis(0, width - 1); // define the range
        std::uniform_int_distribution<short> yAxis(0, height - 1); // define the range
        std::uniform_int_distribution<short> randDirection(0, DIRECTIONS_SIZE - 1);
        for (short ship : ships) {
            bool valid = false;
            while (!valid) {
                short x = xAxis(generator);
                short y = yAxis(generator);
                auto dir = static_cast<DirectionName>(randDirection(generator));
                if (validShip(x, y, ship, dir)) {
                    Coords coords = Coords(x, y);
                    Vector direction = resolveDirection(dir);
                    placeShip(coords, ship, direction);
                    valid = true;
                }
            }
        }
        for (short i = 0; i < width; i++) {
            for (short j = 0; j < height; j++) {
                if (grid[i][j].type != BOAT) {
                    grid[i][j].type = OCEAN;
                }
            }
        }
    }

public:
    int moves;
    std::vector<std::vector<Tile>> grid;
    short height;
    short width;
    std::vector<short> ships;

    //copy constructor of the new Plan, based on the old one's attributes
    Plan(const Plan &old) : hits{old.hits}, moves{old.moves}, height{old.height}, width{old.width}, ships(old.ships) {
        for (const std::vector<Tile> &row : old.grid) {
            std::vector<Tile> v;
            v.reserve(row.size());
            for (const Tile &tile : row) {
                v.emplace_back(Tile(tile.coords, tile.type, tile.opened));
            }
            grid.push_back(v);
        }
    };

    //basic constructor for the (width x height) board with inputShips on it, with a rand option to randomly place the ships
    Plan(short height, short width, bool rand, const std::vector<int> &inputShips) : hits{0}, height{height},
                                                                                     width{width} {
        for (int i : inputShips) {
            ships.emplace_back(i);
        }
        moves = 0;
        for (short i = 0; i < width; i++) {
            std::vector<Tile> v;
            for (short j = 0; j < height; j++) {
                v.emplace_back(Tile(i, j, UNKNOWN));
            }
            grid.push_back(v);
        }
        if (rand) {
            randomize();
        }
    };

    //a function to simulate a certain move, assuming and rewriting a certain tile type to the move one, returning if we hit
    bool simulateMove(const std::pair<Coords, bool> &move) {
        grid[move.first.x][move.first.y].opened = true;
        moves++;
        if (move.second) {
            grid[move.first.x][move.first.y].type = BOAT;
            hits++;
            return true;
        }
        grid[move.first.x][move.first.y].type = OCEAN;
        return false;
    }

    //a function that openes a certain tile, returning whether we hit a part of the ship
    bool doMove(const Coords &coords) {
        assert(!grid[coords.x][coords.y].opened);
        grid[coords.x][coords.y].opened = true;
        bool hit = grid[coords.x][coords.y].type == BOAT;
        if (hit) {
            hits++;
        }
        moves++;
        return hit;
    }

    //the value from the board
    double getScore() const {
        assert(gameEnded());
        double perMove = 1.0 / double(totalOcean() - totalShip());
        if (moves > totalOcean()) {
            return 0;
        }
        return 1.0 - ((moves - totalShip()) * perMove);
    }

    //calculates the total number of ship tiles
    int totalShip() const {
        int sum = 0;
        for (int ship : ships) {
            sum += ship;
        }
        return sum;
    }

    //calculates the total number of ocean tiles
    int totalOcean() const {
        return height * width - totalShip();
    }

    //check for a end of the game
    bool gameEnded() const {
        return totalShip() == hits || moves == width * height;
    }

    //a function to place the ship at given coords, length in a given direction
    void placeShip(Coords coords, short length, const Vector &direction) {
        for (short i = 0; i < length; i++) {
            grid[coords.x][coords.y].type = BOAT;
            coords += direction;
        }
    }

    //check for a valid ship placement
    bool validShip(short x, short y, short length, DirectionName dir) const {
        Coords coords = Coords(x, y);
        Vector direction = resolveDirection(dir);
        for (int i = 0; i < length; i++) {
            for (short ix = -1; ix < 2; ix++) {
                for (short iy = -1; iy < 2; iy++) {
                    //std::cout << "Coords: " << coords.x << ", " << coords.y << "\n";
                    //std::cout << "Dir: " << direction.first << ", " << direction.second << ", " << ix << ", " << iy << "\n";
                    if (ix == 0 && iy == 0 &&
                        (!validCoordinates(coords.x, coords.y) || !emptyCoordinates(coords.x, coords.y))) {
                        return false;
                    }
                    if ((ix != 0 || iy != 0) &&
                        validCoordinates(static_cast<short>(coords.x + ix), static_cast<short>(coords.y + iy)) &&
                        !emptyCoordinates(static_cast<short>(coords.x + ix), static_cast<short>(coords.y + iy))) {
                        return false;
                    }
                }
            }
            coords += direction;
        }
        return true;
    }

    //valid coordinates check
    bool validCoordinates(short x, short y) const {
        return !(x >= width || y >= height || x < 0 || y < 0);
    }

    //check for a coordinates to be unopened or the ocean
    bool emptyCoordinates(short x, short y) const {
        return grid[x][y].type == UNKNOWN || grid[x][y].type == OCEAN;
    }

    //returns the direction enum, in which the coordinates vector point
    static DirectionName getDirection(const Coords &coords) {
        if (coords.x > 0) {
            return DirectionName::DOWN;
        }
        if (coords.x < 0) {
            return DirectionName::UP;
        }
        if (coords.y < 0) {
            return DirectionName::LEFT;
        }
        return DirectionName::RIGHT;
    }

    //returns the coordinates vector depending on the direction enum
    static std::pair<short, short> resolveDirection(DirectionName dir) {
        switch (dir) {
            case UP:
                return std::make_pair(-1, 0);
            case DOWN:
                return std::make_pair(1, 0);
            case RIGHT:
                return std::make_pair(0, 1);
            case LEFT:
                return std::make_pair(0, -1);
        }
        std::cout << "BAAAAAAAAAAAAAAAAAD";
        return std::make_pair(0, -1);
    }

    //function to transform the board into a true/false grid
    std::vector<std::vector<bool>> transform() const {
        std::vector<std::vector<bool>> result;
        for (const std::vector<Tile> &row : grid) {
            std::vector<bool> v;
            v.reserve(row.size());
            for (const Tile &tile : row) {
                v.emplace_back(tile.type == BOAT);
            }
            result.push_back(v);
        }
        return result;
    }
};
