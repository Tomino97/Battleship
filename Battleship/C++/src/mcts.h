//The inspiration for a basic layout comes from: https://github.com/PetterS/monte-carlo-tree-search

#ifndef BATTLESHIP_MCTS_H
#define BATTLESHIP_MCTS_H

#include <cstddef>
#include <memory>
#include <random>
#include <algorithm>
#include <sstream>
#include <memory>
#include <cassert>
#include <map>
#include <future>
#include <iostream>
#include <iomanip>
#include <utility>
#include <set>

#include "plan.hpp"


namespace MCTS {
    //class for the current State representation
    class State {
    public:
        Plan board;
        std::vector<std::pair<Coords, bool>> moves;
        std::mt19937_64 engine;

        //constructor to create (size x size) board with the vector of ships on it
        State(short size, const std::vector<int> &ships) : board{Plan(size, size, true, ships)} {
            for (const std::vector<Tile> &row : board.grid) {
                for (const Tile &tile : row) {
                    moves.emplace_back(Coords(tile.coords.x, tile.coords.y), true);
                    moves.emplace_back(Coords(tile.coords.x, tile.coords.y), false);
                }
            }
            std::random_device device;
            std::seed_seq seq{std::chrono::high_resolution_clock::now().time_since_epoch().count()};
            engine.seed(seq);
        }

        //a function that checks the valid layout of ships and returns the number of correct ship tiles
        int validLayout() const {
            std::vector<Coords> visitedShips;
            std::vector<short> ships = board.ships;
            int uncoveredShips = 0;
            for (const std::vector<Tile> &row : board.grid) {
                for (const Tile &tile : row) {
                    if (tile.opened && tile.type == BOAT &&
                        std::find(visitedShips.begin(), visitedShips.end(), tile.coords) == visitedShips.end()) {
                        visitedShips.emplace_back(tile.coords);
                        bool correct = true;
                        for (auto dir : {Plan::resolveDirection(DirectionName::RIGHT),
                                         Plan::resolveDirection(DirectionName::DOWN)}) {
                            Coords tmp = tile.coords;
                            short length = 1;
                            tmp += dir;
                            while (board.validCoordinates(tmp.x, tmp.y) && board.grid[tmp.x][tmp.y].opened &&
                                   board.grid[tmp.x][tmp.y].type == BOAT) {
                                visitedShips.emplace_back(tmp);
                                length++;
                                tmp += dir;
                            }
                            if (length == 1) {
                                continue;
                            }
                            for (int i = 0; i < length + 2; i++) {
                                for (int ix : {-1, 1}) {
                                    if (dir.first == 0) {
                                        if (board.validCoordinates(static_cast<short>(tmp.x + ix), tmp.y) &&
                                            board.grid[tmp.x + ix][tmp.y].opened &&
                                            !board.emptyCoordinates(static_cast<short>(tmp.x + ix), tmp.y)) {
                                            correct = false;
                                        }
                                    } else {
                                        if (board.validCoordinates(tmp.x, static_cast<short>(tmp.y + ix)) &&
                                            board.grid[tmp.x][tmp.y + ix].opened &&
                                            !board.emptyCoordinates(tmp.x, static_cast<short>(tmp.y + ix))) {
                                            correct = false;
                                        }
                                    }
                                }
                                tmp -= dir;
                            }
                            auto it = std::find(ships.begin(), ships.end(), length);
                            if (it == ships.end()) {
                                correct = false;
                            }
                            if (!correct) {
                                break;
                            }
                            uncoveredShips += *it;
                            std::iter_swap(it, ships.end() - 1);
                            ships.erase(ships.end() - 1);
                            break;
                        }
                    }
                }
            }
            return uncoveredShips;
        }

        //do a certain move, remove it from the moves vector
        void doMove(const Coords &move) {
            board.doMove(move);

            auto new_end = std::remove_if(moves.begin(), moves.end(),
                                          [&](std::pair<Coords, bool> &move_) { return move_.first == move; });
            moves.erase(new_end, moves.end());
        }

        //simulate a certain move, throwing away the real content of the tile
        void simulateMove(const std::pair<Coords, bool> &move) {
            board.simulateMove(move);

            auto new_end = std::remove_if(moves.begin(), moves.end(),
                                          [&](std::pair<Coords, bool> &move_) { return move_.first == move.first; });
            moves.erase(new_end, moves.end());
        }

        std::vector<std::pair<Coords, bool>> getMoves() const {
            return moves;
        }

        //check if the game is finished for a given state
        bool gameEnded() const {
            return board.gameEnded();
        }


        void simulateRandomMove() {
            std::uniform_int_distribution<> randMove(0, moves.size() - 1);
            std::pair<Coords, bool> mv = moves[randMove(engine)];
            simulateMove(mv);
        }

        double getScore() const {
            int uncoveredShips = validLayout();
            /*if(uncoveredShips != board.totalShip()) {         //uncomment to put emphasis on full ship findings
                return 0;
            }*/
            return std::pow((double) uncoveredShips / board.totalShip(), 2) * board.getScore();
        }
    };

    //class for a single node in the MCTS tree representation
    class Node {
        std::vector<std::pair<Coords, bool>> moves;
    public:
        double payoff;
        double score;
        int visits;
        std::pair<Coords, bool> move;
        Node *parent;
        std::vector<std::unique_ptr<Node>> children;

        //constructor for the root node
        explicit Node(const State &state) : moves(state.getMoves()), payoff{0}, score{0},
                                            visits{0}, move{Coords(-1, -1), false}, parent{nullptr} {}

        //constructor for the child nodes
        Node(State &state, std::pair<Coords, bool> move, Node *parent) : moves(state.getMoves()), payoff{0}, score{0},
                                                                         visits{0}, move{std::move(move)},
                                                                         parent{parent} {}

        //UCT calculation of the most promising child
        Node *selectChildUCT() {
            for (std::unique_ptr<Node> &child : children) {
                child->score = double(child->payoff) / double(child->visits) +
                               std::sqrt(2.0 * std::log(double(this->visits)) / child->visits);
            }

            return std::max_element(children.begin(), children.end(),
                                    [](const std::unique_ptr<Node> &a, const std::unique_ptr<Node> &b) {
                                        return a->score < b->score;
                                    })->get();
        }

        bool hasUntriedMove() const {
            return !moves.empty();
        }

        std::pair<Coords, bool> getRandomMove(std::mt19937_64 *engine) const {
            std::uniform_int_distribution<int> randMove(0, moves.size() - 1);
            std::pair<Coords, bool> mv = moves[randMove(*engine)];
            return mv;
        }

        //add a new child into the tree
        Node *addChild(State &oldState, const std::pair<Coords, bool> &currentMove) {
            children.push_back(std::make_unique<Node>(oldState, currentMove, this));
            auto itr = moves.begin();
            for (; itr != moves.end() && *itr != currentMove; ++itr);
            assert(itr != moves.end());
            std::iter_swap(itr, moves.end() - 1);
            moves.resize(moves.size() - 1);
            return children.back().get();
        }

        //update the payoff and number of visits of the node
        void update(double result) {
            visits++;
            payoff += result;
        }
    };

    std::unique_ptr<Node> mcts(const State initialState, int maxIterations) {
        auto root = std::make_unique<Node>(initialState);

        for (int iter = 1; iter <= maxIterations; ++iter) {
            auto node = root.get();
            State state = initialState;                     //copy initial state

            //SELECTION
            while (!node->hasUntriedMove() && !node->children.empty()) {
                node = node->selectChildUCT();
                state.simulateMove(node->move);
            }

            //EXPANSION
            if (node->hasUntriedMove() && !state.gameEnded()) {
                auto move = node->getRandomMove(&state.engine);
                state.simulateMove(move);
                node = node->addChild(state, move);
            }

            //SIMULATION
            while (!state.gameEnded()) {
                state.simulateRandomMove();
            }

            //BACKPROPAGATION
            double score = state.getScore();
            while (node != nullptr) {
                node->update(score);
                node = node->parent;
            }
        }

        return root;
    }

    Coords calculateMove(const State initialState, int iterationsPerMove) {
        assert(initialState.moves > 0);
        if (initialState.moves.size() == 1) {
            return initialState.moves[0].first;
        }

        //calculate the tree of moves
        std::unique_ptr<Node> root = mcts(initialState,
                                          static_cast<int>(initialState.moves.size()) * iterationsPerMove);

        //remove the missing simulations
        auto new_end = std::remove_if(root->children.begin(), root->children.end(),
                                      [](std::unique_ptr<Node> &a) { return !a->move.second; });
        root->children.erase(new_end, root->children.end());

        //print of the payoff and visits for all children
        /*for(const auto& child : root->children) {
            std::cout << child->move.first << " " << child->move.second << " " << "PAYOFF: " << child->payoff << ", VISITS: " << child->visits << std::endl;
        }*/

        Node *best = std::max_element(root->children.begin(), root->children.end(),
                                      [](const std::unique_ptr<Node> &a, const std::unique_ptr<Node> &b) {
                                          return a->visits < b->visits;
                                      })->get();

        return best->move.first;
    }
}

#endif //BATTLESHIP_MCTS_H
