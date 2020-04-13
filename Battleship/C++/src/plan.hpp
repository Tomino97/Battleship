#pragma once

#include <vector>
#include <random>
#include <iostream>
#include <chrono>
#include <cassert>

#include "geometry.hpp"

#define OCEAN_CHAR	'-'
#define BOAT_CHAR	'*'
#define UNKNOWN_CHAR	'?'

enum TileType {
    UNKNOWN,
	OCEAN,	/* '-' */
	BOAT	/* '*' */
};

struct Tile {
	Coords coords;
	TileType type;
	bool opened;

    Tile(short x, short y, TileType type) : coords{x, y}, type{type}, opened{false} { }
    Tile(const Coords& coords, TileType type) : coords{coords}, type{type}, opened{false} { }
	Tile(short x, short y, TileType type, bool opened) : coords{x, y}, type{type}, opened{opened} { }
    Tile(const Coords& coords, TileType type, bool opened) : coords{coords}, type{type}, opened{opened} { }

    friend bool operator==(const Tile& tile, const Tile& other);
    friend bool operator!=(const Tile& tile, const Tile& other);
};

class Plan {
public:
	std::vector<std::vector<Tile>> grid;
	short height;
	short width;
	std::vector<short> ships;
    int moves;
    int hits;

    Plan(const Plan& old): height{old.height}, width{old.width}, ships(old.ships), moves{old.moves}, hits{old.hits} {
        for (const std::vector<Tile>& row : old.grid) {
            std::vector<Tile> v;
            v.reserve(row.size());
            for (const Tile& tile : row) {
                v.emplace_back(Tile(tile.coords, tile.type, tile.opened));
            }
            grid.push_back(v);
        }
    };

	Plan(short height, short width, bool rand): height{height}, width{width}, hits{0} {
	    ships.emplace_back(3);
	    ships.emplace_back(2);
	    moves = 0;
        for(short i = 0; i < width; i++) {
            std::vector<Tile> v;
            for(short j = 0; j < height; j++) {
                v.emplace_back(Tile(i, j, UNKNOWN));
            }
            grid.push_back(v);
        }
        if(rand) {
            randomize();
        }
	};

    friend std::ostream& operator<<(std::ostream &out, const Plan&plan);

    bool simulateMove(const std::pair<Coords, bool> &move) {
        grid[move.first.x][move.first.y].opened = true;
        moves++;
        if(move.second) {
            grid[move.first.x][move.first.y].type = BOAT;
            hits++;
            return true;
        }
        grid[move.first.x][move.first.y].type = OCEAN;
        return false;
    }

    bool doMove(const Coords& coords) {
        assert(!grid[coords.x][coords.y].opened);
        grid[coords.x][coords.y].opened = true;
        bool hit = grid[coords.x][coords.y].type == BOAT;
        if(hit) {
            hits++;
        }
        moves++;
        return hit;
    }

    double getScore() {
        assert(gameEnded());
        double perMove = 1.0 / double(height*width - totalShip());
        if(moves == totalShip()) {
            return 1.0;
        }
        return 1.0 - ((moves - 5) * perMove);
    }

    int totalShip() {
        int sum = 0;
        for(int ship : ships) {
            sum += ship;
        }
        return sum;
    }

    bool gameEnded() {
        return totalShip() == hits;
    }

	void randomize() {
        std::mt19937 generator(std::chrono::high_resolution_clock::now().time_since_epoch().count()); // seed the generator
        std::uniform_int_distribution<short> xAxis(0, width - 1); // define the range
        std::uniform_int_distribution<short> yAxis(0, height - 1); // define the range
        std::uniform_int_distribution<short> randDirection(0, DIRECTIONS_SIZE - 1);
        for (short ship : ships) {
            bool valid = false;
            while(!valid) {
                short x = xAxis(generator);
                short y = yAxis(generator);
                auto dir = static_cast<DirectionName>(randDirection(generator));
                if(validShip(x, y, ship, dir)) {
                    Coords coords = Coords(x,y);
                    Vector direction = resolveDirection(dir);
                    placeShip(coords, ship, direction);
                    valid = true;
                }
            }
        }
        for(short i = 0; i < width; i++) {
            for(short j = 0; j < height; j++) {
                if(grid[i][j].type != BOAT) {
                    grid[i][j].type = OCEAN;
                }
            }
        }
	}

	void placeShip(Coords& coords, short length, const Vector& direction) {
        for(short i = 0; i < length; i++) {
            grid[coords.x][coords.y].type = BOAT;
            coords += direction;
        }
    }

	bool validShip(short x, short y, short length, DirectionName dir) {
	    Coords coords = Coords(x,y);
	    Vector direction = resolveDirection(dir);
	    for(int i = 0; i < length; i++) {
	        for(short ix = -1; ix < 2; ix++) {
                for (short iy = -1; iy < 2; iy++) {
                    //std::cout << "Coords: " << coords.x << ", " << coords.y << "\n";
                    //std::cout << "Dir: " << direction.first << ", " << direction.second << ", " << ix << ", " << iy << "\n";
                    if (ix == 0 && iy == 0 && (!validCoordinates(coords.x, coords.y) || !emptyCoordinates(coords.x, coords.y))) {
                        return false;
                    }
                    if((ix != 0 || iy != 0) && validCoordinates(coords.x + ix, coords.y + iy) &&
                            !emptyCoordinates(coords.x + ix, coords.y + iy)) {
                        return false;
                    }
                }
            }
	        coords += direction;
	    }
	    return true;
	}

	bool validCoordinates(short x, short y) {
        return !(x >= width || y >= height || x < 0 || y < 0);
	}

    bool emptyCoordinates(short x, short y) {
        return grid[x][y].type == UNKNOWN;
    }

	static std::pair<short, short> resolveDirection(DirectionName dir) {
        switch(dir) {
            case UP:
                return std::make_pair(0, -1);
            case DOWN:
                return std::make_pair(0, 1);
            case RIGHT:
                return std::make_pair(1, 0);
            case LEFT:
                return std::make_pair(-1, 0);
        }
        std::cout << "BAAAAAAAAAAAAAAAAAD";
        return std::make_pair(0, -1);
	}

	std::vector<std::vector<bool>> transform() {
        std::vector<std::vector<bool>> result;
        for (const std::vector<Tile>& row : grid) {
            std::vector<bool> v;
            v.reserve(row.size());
            for (const Tile& tile : row) {
                v.emplace_back(tile.type == BOAT);
            }
            result.push_back(v);
        }
        return result;
    }
};
