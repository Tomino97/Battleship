#ifndef BATTLESHIP_EVENPLAYER_H
#define BATTLESHIP_EVENPLAYER_H

#include <random>
#include <algorithm>
#include <stack>

#include "plan.hpp"

namespace EvenPlayer {
    enum Mode {
        hunt, target, destroy
    };

    //class for the current State representation
    class State {
        std::vector<Coords> moves;
        Mode mode;
        std::stack<Coords> targets;
        Coords lastHit;

        void eraseMove(const Coords &coords) {
            auto new_end = std::remove_if(moves.begin(), moves.end(),
                                          [&](Coords &move_) { return move_ == coords; });
            moves.erase(new_end, moves.end());
        }

        void doRandomMove(std::mt19937_64 *engine) {
            std::uniform_int_distribution<short> randMove(0, moves.size() - 1);
            Coords mv = moves[randMove(*engine)];
            eraseMove(mv);
            if (board.doMove(mv)) {
                lastHit = mv;
                addAdjacent(mv);
                mode = Mode::target;
            }
        }

        void addAdjacent(const Coords &move) {
            for (int i = 0; i < DIRECTIONS_SIZE; i++) {
                add(move + Plan::resolveDirection(static_cast<DirectionName>(i)));
            }
        }

        void add(const Coords &coords) {
            if (board.validCoordinates(coords.x, coords.y) && !board.grid[coords.x][coords.y].opened) {
                targets.push(coords);
            }
        }

    public:
        Plan board;

        //constructor to create (size x size) board with the vector of ships on it
        State(short size, const std::vector<int> &ships) : mode{Mode::hunt}, lastHit(Coords(-1, -1)),
                                                           board{Plan(size, size, true, ships)} {
            bool evenRow = true;
            bool evenColumn;
            for (const std::vector<Tile> &row : board.grid) {
                evenRow = !evenRow;
                evenColumn = evenRow;
                for (const Tile &tile : row) {
                    evenColumn = !evenColumn;
                    if (evenColumn) {
                        moves.emplace_back(tile.coords);
                    }
                }
            }
        }

        //method to make a move, requires a std::mt19937_64* engine to make random moves
        void doMove(std::mt19937_64 *engine) {
            Coords mv;
            if (targets.empty()) {
                mode = Mode::hunt;
            } else {
                mv = targets.top();
                targets.pop();
                eraseMove(mv);
            }
            if (mode == Mode::hunt) {
                doRandomMove(engine);
            } else if (mode == Mode::target) {
                if (board.doMove(mv)) {
                    while (!targets.empty()) {
                        targets.pop();
                    }
                    mode = Mode::destroy;
                    DirectionName direction = Plan::getDirection(mv - lastHit);
                    add(mv + Plan::resolveDirection(direction));
                    add(lastHit - Plan::resolveDirection(direction));
                }
            } else {
                if (board.doMove(mv)) {
                    DirectionName direction = Plan::getDirection(mv - lastHit);
                    add(mv + Plan::resolveDirection(direction));
                }
            }
        }

        //check if the game is finished for a given state
        bool gameEnded() {
            return board.gameEnded();
        }
    };
}

#endif //BATTLESHIP_EVENLIKEPLAYER_H
