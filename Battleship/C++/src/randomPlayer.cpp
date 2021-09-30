#ifndef BATTLESHIP_RANDOMPLAYER_H
#define BATTLESHIP_RANDOMPLAYER_H

#include <random>
#include <algorithm>

#include "plan.hpp"

namespace RandomPlayer {
    //class for the current State representation
    class State {
        std::vector<Coords> moves;

        void eraseMove(const Coords& coords) {
            auto new_end = std::remove_if(moves.begin(), moves.end(),
                                          [&](Coords& move_)
                                          { return move_ == coords; });
            moves.erase(new_end, moves.end());
        }

        void doRandomMove(std::mt19937_64* engine) {
            std::uniform_int_distribution<short> randMove(0, moves.size() - 1);
            Coords mv = moves[randMove(*engine)];
            eraseMove(mv);
            board.doMove(mv);
        }
    public:
        Plan board;

        //constructor to create (size x size) board with the vector of ships on it
        State(short size, const std::vector<int>& ships) : board{Plan(size, size, true, ships)} {
            for (const std::vector<Tile> &row : board.grid) {
                for (const Tile &tile : row) {
                    moves.emplace_back(tile.coords);
                }
            }
        }

        //method to make a move, requires a std::mt19937_64* engine to make random moves
        void doMove(std::mt19937_64* engine) {
            doRandomMove(engine);
        }

        //check if the game is finished for a given state
        bool gameEnded() {
            return board.gameEnded();
        }
    };
}

#endif //BATTLESHIP_RANDOMPLAYER_H
