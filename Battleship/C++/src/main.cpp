#include <iostream>

#include "mcts.h"

void test(const MCTS::State initialState, int maxIterations) {
    auto root = std::make_unique<MCTS::Node>(initialState);
    for(int a = 0; a < maxIterations; a++) {
        auto node = root.get();
        MCTS::State state = initialState;
        /*for (auto c : state.getMoves()) {
            std::cout << c.first;
        }
        std::cout << std::endl;
        for (auto c : node->getMoves()) {
            std::cout << c.first;
        }
        std::cout << std::endl;*/
        auto move = node->getRandomMove();
        state.simulateMove(move);
        node = node->addChild(state, move);
        std::cout << move.first << move.second;
        std::cout << state.board;
        for(const auto& x : state.possibleStates) {
            std::cout << "\n";
            for(const auto& i : x) {
                for(auto j : i) {
                    if(j) {
                        std::cout << "X";
                    } else {
                        std::cout << ".";
                    }
                }
                std::cout << "\n";
            }
        }
        while(!state.possibleStates.empty() && !state.gameEnded()) {
            state.doRandomMove();
            std::cout << state.board;
            for(const auto& x : state.possibleStates) {
                std::cout << "\n";
                for(const auto& i : x) {
                    for(auto j : i) {
                        if(j) {
                            std::cout << "X";
                        } else {
                            std::cout << ".";
                        }
                    }
                    std::cout << "\n";
                }
            }
        }
    }
}

int main()
{
    MCTS::State state;
    //test(state, 1);
    /*state.simulateMove({Coords(1, 3), true});
    for(const auto& x : state.possibleStates) {
        std::cout << "\n";
        for(const auto& i : x) {
            for(auto j : i) {
                if(j) {
                    std::cout << "X";
                } else {
                    std::cout << ".";
                }
            }
            std::cout << "\n";
        }
    }
    for(auto x : state.moves) {
        std::cout << x.first << x.second << std::endl;
    }
    state.simulateMove({Coords(3, 0), true});
    for(const auto& x : state.possibleStates) {
        std::cout << "\n";
        for(const auto& i : x) {
            for(auto j : i) {
                if(j) {
                    std::cout << "X";
                } else {
                    std::cout << ".";
                }
            }
            std::cout << "\n";
        }
    }
    for(auto x : state.moves) {
        std::cout << x.first << x.second << std::endl;
    }*/
    while(!state.gameEnded()) {
        std::cout << state.board;

        Coords move = MCTS::calculateMove(state);
        state.doMove(move);
    }
    std::cout << state.board << std::endl << state.board.moves;
	return 0;
}
