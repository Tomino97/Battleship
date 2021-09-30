#ifndef BATTLESHIP_HEATMAPPLAYER_H
#define BATTLESHIP_HEATMAPPLAYER_H

#include <random>
#include <algorithm>

#include "plan.hpp"

namespace HeatMapPlayer {

    //class for the current State representation
    class State {
        std::vector<Coords> moves;
        std::vector<std::vector<std::vector<bool>>> possibleStates;
        bool lastHitShip;

        //to calculate the most probable tile to contain the ship
        Coords calculateMove() {
            int max = 0;
            Coords result;
            for (auto move : moves) {
                int count = std::count_if(possibleStates.begin(), possibleStates.end(),
                                          [&](const auto &state) { return state[move.x][move.y]; });
                if (count > max) {
                    max = count;
                    result = move;
                }
            }
            return result;
        }

        void eraseMove(const Coords &coords) {
            auto new_end = std::remove_if(moves.begin(), moves.end(),
                                          [&](Coords &move_) { return move_ == coords; });
            moves.erase(new_end, moves.end());

            auto new_end2 = std::remove_if(possibleStates.begin(), possibleStates.end(),
                                           [&](const std::vector<std::vector<bool>> &grid) {
                                               return grid[coords.x][coords.y] != lastHitShip;
                                           });

            possibleStates.erase(new_end2, possibleStates.end());
        }

        //generating the possible states, saving them into possibleStates vector
        void generateStates(Plan plan, Coords coords, const Vector &direction) {
            if (coords.x != -1 && coords.y != -1 && !plan.ships.empty()) {
                plan.placeShip(coords, plan.ships[0], direction);
                plan.ships.erase(plan.ships.begin());
            }
            if (plan.ships.empty()) {
                possibleStates.emplace_back(plan.transform());
                return;
            }
            for (short i = 0; i < board.width; i++) {
                for (short j = 0; j < board.height; j++) {
                    if (plan.validShip(i, j, plan.ships[0], RIGHT)) {
                        generateStates(plan, Coords(i, j), Plan::resolveDirection(RIGHT));
                    }
                    if (plan.validShip(i, j, plan.ships[0], DOWN)) {
                        generateStates(plan, Coords(i, j), Plan::resolveDirection(DOWN));
                    }
                }
            }
        }

    public:
        Plan board;

        //constructor to create (size x size) board with the vector of ships on it, also generating the possible states
        State(short size, const std::vector<int> &ships) : lastHitShip{false}, board{Plan(size, size, true, ships)} {
            for (const std::vector<Tile> &row : board.grid) {
                for (const Tile &tile : row) {
                    moves.emplace_back(tile.coords);
                }
            }
            Plan plan(size, size, false, ships);
            generateStates(plan, Coords(-1, -1), Plan::resolveDirection(UP));
        }

        //method to make a move
        void doMove() {
            Coords move = calculateMove();
            lastHitShip = board.doMove(move);
            eraseMove(move);
        }

        //method to print out the probability of each tile to contain a part of the ship, based on possibleStates
        void printHeatMap(std::ostream &output) {
            for (short x = 0; x < board.width; x++) {
                for (short y = 0; y < board.height; y++) {
                    if (std::find(moves.begin(), moves.end(), Coords(x, y)) != moves.end()) {
                        output << double(std::count_if(possibleStates.begin(), possibleStates.end(),
                                                       [&](const auto &state) { return state[x][y]; })) /
                                  possibleStates.size();
                    } else {
                        if (board.grid[x][y].type == BOAT) {
                            output << 1;
                        } else {
                            output << 0;
                        }
                    }
                    if (y != board.width - 1) {
                        output << " ";
                    }
                }
                output << std::endl;
            }
            output << std::endl;
        }

        //check if the game is finished for a given state
        bool gameEnded() {
            return board.gameEnded();
        }
    };
}

#endif //BATTLESHIP_HEATMAPPLAYER_H
