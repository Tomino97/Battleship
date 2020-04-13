//
// Created by Tomi on 28. 2. 2020.
//

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

#include "plan.hpp"

#define DISCOUNT_FACTOR 0.99

namespace MCTS {
    class State {
    public:
        Plan board;
        bool lastHitShip;
        std::vector<std::pair<Coords, bool>> moves;
        std::vector<std::vector<std::vector<bool>>> possibleStates;

        State() : board{Plan(5, 5, true)}, lastHitShip{false} {
            for (const std::vector<Tile> &row : board.grid) {
                for (const Tile &tile : row) {
                    moves.emplace_back(tile.coords, true);
                    moves.emplace_back(tile.coords, false);
                }
            }
            Plan plan(5, 5, false);
            generateStates(plan, Coords(-1, -1), Plan::resolveDirection(UP));
        }

        void generateStates(Plan plan, Coords coords, const Vector& direction) {
            if(coords.x != -1 && coords.y != -1 && !plan.ships.empty()) {
                plan.placeShip(coords, plan.ships[0], direction);
                plan.ships.erase(plan.ships.begin());
            }
            if(plan.ships.empty()) {
                possibleStates.emplace_back(plan.transform());
                return;
            }
            for(short i = 0; i < board.width; i++) {
                for(short j = 0; j < board.height; j++) {
                    if(plan.validShip(i, j, plan.ships[0], RIGHT)) {
                        generateStates(plan, Coords(i ,j), Plan::resolveDirection(RIGHT));
                    }
                    if(plan.validShip(i, j, plan.ships[0], DOWN)) {
                        generateStates(plan, Coords(i ,j), Plan::resolveDirection(DOWN));
                    }
                }
            }
        }

        void doMove(const Coords &move) {
            lastHitShip = board.doMove(move);

            auto new_end = std::remove_if(moves.begin(), moves.end(),
                                          [&](std::pair<Coords, bool>& move_)
                                          { return move_.first == move; });
            moves.erase(new_end, moves.end());

            auto new_end2 = std::remove_if(possibleStates.begin(), possibleStates.end(),
                                          [&](const std::vector<std::vector<bool>>& grid)
                                          { return grid[move.x][move.y] != lastHitShip; });

            possibleStates.erase(new_end2, possibleStates.end());
        }

        void simulateMove(const std::pair<Coords, bool> &move) {
            lastHitShip = board.simulateMove(move);
            /*auto itr = moves.begin();
            for (; itr != moves.end() && *itr != move; ++itr);
            assert(itr != moves.end());
            std::iter_swap(itr, moves.end() - 1);
            moves.resize(moves.size() - 1);*/

            auto new_end = std::remove_if(moves.begin(), moves.end(),
                                          [&](std::pair<Coords, bool>& move_)
                                          { return move_.first == move.first; });
            moves.erase(new_end, moves.end());

            auto new_end2 = std::remove_if(possibleStates.begin(), possibleStates.end(),
                                          [&](const std::vector<std::vector<bool>>& grid)
                                          { return grid[move.first.x][move.first.y] != move.second; });

            possibleStates.erase(new_end2, possibleStates.end());
        }

        [[nodiscard]] std::vector<std::pair<Coords, bool>> getMoves() const {
            return moves;
        }

        bool gameEnded() {
            return board.gameEnded();
        }

        void doRandomMove() {
            std::mt19937 generator(
                    std::chrono::high_resolution_clock::now().time_since_epoch().count()); // seed the generator
            std::uniform_int_distribution<short> randMove(0, moves.size() - 1);
            std::pair<Coords, bool> mv = moves[randMove(generator)];
            simulateMove(mv);
        }

        double getScore() {
            if(possibleStates.empty()) {
                return 0;
            }
            return board.getScore();
        }
    };

    class Node {
        std::vector<std::pair<Coords, bool>> moves;
    public:
        double wins;
        double score;
        std::pair<Coords, bool> move;
        Node *parent;
        std::vector<Node *> children;
        int visits;

        explicit Node(const State& state): moves(state.getMoves()), wins{0}, score{0}, move{Coords(-1, -1), false}, parent{nullptr}, visits{0} {}

        Node(State& state, std::pair<Coords, bool>  move, Node* parent): moves(state.getMoves()), wins{0}, score{0}, move{std::move(move)}, parent{parent}, visits{0} {}

        ~Node() {
            for(auto child : children) {
                delete child;
            }
        }

        [[nodiscard]] std::vector<std::pair<Coords, bool>> getMoves() const {
            return moves;
        }

        [[nodiscard]] Node *selectChildUCT() const {
            for (auto child : children) {
                child->score = double(child->wins) / double(child->visits) +
                               std::sqrt(2.0 * std::log(double(this->visits)) / child->visits);
            }

            return *std::max_element(children.begin(), children.end(),
                                     [](Node *a, Node *b) { return a->score <= b->score; });
        }

        [[nodiscard]] bool hasUntriedMove() const {
            return !moves.empty();
        }

        [[nodiscard]] std::pair<Coords, bool> getRandomMove() const {
            std::mt19937 generator(
                    std::chrono::high_resolution_clock::now().time_since_epoch().count()); // seed the generator
            std::uniform_int_distribution<short> randMove(0, moves.size() - 1);
            std::pair<Coords, bool> mv = moves[randMove(generator)];
            return mv;
        }

        Node *addChild(State &oldState, const std::pair<Coords, bool> &currentMove) {
            auto node = new Node(oldState, currentMove, this);
            children.push_back(node);
            auto itr = moves.begin();
            for (; itr != moves.end() && *itr != currentMove; ++itr);
            assert(itr != moves.end());
            std::iter_swap(itr, moves.end() - 1);
            moves.resize(moves.size() - 1);
            return node;
        }

        void update(double result) {
            visits++;
            wins+= result;
        }
    };

    std::unique_ptr<Node> mcts(const State initialState, int maxIterations) {
        auto root = std::make_unique<Node>(initialState);

        for (int iter = 1; iter <= maxIterations; ++iter) {
            int horrizon = 0;
            auto node = root.get();
            State state = initialState;

            while(!node->hasUntriedMove()) {
                node = node->selectChildUCT();
                state.simulateMove(node->move);
                horrizon++;
            }

            if (node->hasUntriedMove()) {
                auto move = node->getRandomMove();
                state.simulateMove(move);
                node = node->addChild(state, move);
            }

            while(!state.possibleStates.empty() && !state.gameEnded()) {
                state.doRandomMove();
                horrizon++;
            }

            while(node != nullptr) {
                node->update(state.getScore() * std::pow(DISCOUNT_FACTOR, horrizon));
                node = node->parent;
            }
        }

        return root;
    }

    Coords calculateMove(const State initialState) {
        assert(initialState.moves > 0);
        if(initialState.moves.size() == 1) {
            return initialState.moves[0].first;
        }

        std::unique_ptr<Node> root = mcts(initialState, 10000);

        auto new_end = std::remove_if(root->children.begin(), root->children.end(),
                                      [](Node *a)
                                      { return !a->move.second; });
        root->children.erase(new_end, root->children.end());

        for(auto child : root->children) {
            std::cout << child->move.first << " " << child->move.second << " " << "WINS: " << child->wins << ", VISITS: " << child->visits << std::endl;
        }

        Node *best = *std::max_element(root->children.begin(), root->children.end(),
                                 [](Node *a, Node *b) { return a->visits < b->visits; });

        return best->move.first;
    }
}

#endif //BATTLESHIP_MCTS_H
