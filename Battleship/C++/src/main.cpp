#include <iostream>
#include <fstream>
#include <unistd.h>

#include "mcts.h"
#include "humanLikePlayer.cpp"
#include "heatMapPlayer.cpp"
#include "randomPlayer.cpp"
#include "evenPlayer.cpp"

int main() {
    char mode, x;
    short size, y;
    std::vector<int> ships = {3, 2};

    std::cout << "Choose one player you want to play against by typing one of the letters:\n\n"
                 "R - Random player - chooses completely random moves\n"
                 "H - HumanLike player - plays comparably to a human player\n"
                 "E - Even player - plays comparably to a human player with enhanced choosing\n"
                 "C - HeatMap player - calculates the most probable tile(only for a 5x5 game)\n"
                 "M - Monte Carlo Tree Search player - uses MCTS to calculate the best move\n";
    std::cin >> mode;

    std::cout << "Choose the size of the board (5 or 10):\n";
    std::cin >> size;

    if(size != 5 && size != 10) {
        std::cout << "Wrong size";
        return 0;
    }

    if(size == 10) {
        if(mode == 'C') {
            std::cout << "HeatMap player is only available for a 5x5 game\n";
            return 0;
        }
        ships.emplace_back(5);
        ships.emplace_back(4);
        ships.emplace_back(3);
    }

    Plan enemyBoard(size, size, true, ships);

    switch (mode) {
        case 'R': {
            std::mt19937_64 random_engine(std::chrono::high_resolution_clock::now().time_since_epoch().count());
            RandomPlayer::State state(size, ships);
            while(!enemyBoard.gameEnded() && !state.gameEnded()) {
                std::cout << "Enemy board: " << enemyBoard << "Your board: " << state.board;
                std::cout << "Select x-coordinates of your next move(A,B,...):\n";
                std::cin >> x;
                std::cout << "Select y-coordinates of your next move(1,2,...):\n";
                std::cin >> y;
                auto xx = static_cast<short>(x - 'A');
                if(xx > size - 1 || xx < 0 || y > size || y < 1) {
                    std::cout << "Wrong coordinates";
                    return 0;
                }
                enemyBoard.doMove(Coords(xx, static_cast<short>(y - 1)));
                state.doMove(&random_engine);
            }
            if(enemyBoard.gameEnded() && state.board.gameEnded()) {
                std::cout << "Enemy board: " << enemyBoard << "Your board: " << state.board;
                std::cout << "DRAW\n";
            } else if(enemyBoard.gameEnded()) {
                std::cout << "Enemy board: " << enemyBoard << "Your board: " << state.board;
                std::cout << "YOU WON\n";
            } else {
                std::cout << "Enemy board: " << enemyBoard << "Your board: " << state.board;
                std::cout << "YOU LOST\n";
            }
            break;
        }
        case 'H': {
            std::mt19937_64 random_engine(std::chrono::high_resolution_clock::now().time_since_epoch().count());
            HumanLikePlayer::State state(size, ships);
            while(!enemyBoard.gameEnded() && !state.gameEnded()) {
                std::cout << "Enemy board: " << enemyBoard << "Your board: " << state.board;
                std::cout << "Select x-coordinates of your next move(A,B,...):\n";
                std::cin >> x;
                std::cout << "Select y-coordinates of your next move(1,2,...):\n";
                std::cin >> y;
                auto xx = static_cast<short>(x - 'A');
                if(xx > size - 1 || xx < 0 || y > size || y < 1) {
                    std::cout << "Wrong coordinates";
                    return 0;
                }
                enemyBoard.doMove(Coords(xx, y - 1));
                state.doMove(&random_engine);
            }
            if(enemyBoard.gameEnded() && state.board.gameEnded()) {
                std::cout << "Enemy board: " << enemyBoard << "Your board: " << state.board;
                std::cout << "DRAW\n";
            } else if(enemyBoard.gameEnded()) {
                std::cout << "Enemy board: " << enemyBoard << "Your board: " << state.board;
                std::cout << "YOU WON\n";
            } else {
                std::cout << "Enemy board: " << enemyBoard << "Your board: " << state.board;
                std::cout << "YOU LOST\n";
            }
            break;
        }
        case 'E': {
            std::mt19937_64 random_engine(std::chrono::high_resolution_clock::now().time_since_epoch().count());
            EvenPlayer::State state(size, ships);
            while(!enemyBoard.gameEnded() && !state.gameEnded()) {
                std::cout << "Enemy board: " << enemyBoard << "Your board: " << state.board;
                std::cout << "Select x-coordinates of your next move(A,B,...):\n";
                std::cin >> x;
                std::cout << "Select y-coordinates of your next move(1,2,...):\n";
                std::cin >> y;
                auto xx = static_cast<short>(x - 'A');
                if(xx > size - 1 || xx < 0 || y > size || y < 1) {
                    std::cout << "Wrong coordinates";
                    return 0;
                }
                enemyBoard.doMove(Coords(xx, static_cast<short>(y - 1)));
                state.doMove(&random_engine);
            }
            if(enemyBoard.gameEnded() && state.board.gameEnded()) {
                std::cout << "Enemy board: " << enemyBoard << "Your board: " << state.board;
                std::cout << "DRAW\n";
            } else if(enemyBoard.gameEnded()) {
                std::cout << "Enemy board: " << enemyBoard << "Your board: " << state.board;
                std::cout << "YOU WON\n";
            } else {
                std::cout << "Enemy board: " << enemyBoard << "Your board: " << state.board;
                std::cout << "YOU LOST\n";
            }
            break;
        }
        case 'C': {
            HeatMapPlayer::State state(size, ships);
            while(!enemyBoard.gameEnded() && !state.gameEnded()) {
                std::cout << "Enemy board: " << enemyBoard << "Your board: " << state.board;
                std::cout << "Select x-coordinates of your next move(A,B,...):\n";
                std::cin >> x;
                std::cout << "Select y-coordinates of your next move(1,2,...):\n";
                std::cin >> y;
                auto xx = static_cast<short>(x - 'A');
                if(xx > size - 1 || xx < 0 || y > size || y < 1) {
                    std::cout << "Wrong coordinates";
                    return 0;
                }
                enemyBoard.doMove(Coords(xx, static_cast<short>(y - 1)));
                state.doMove();
            }
            if(enemyBoard.gameEnded() && state.board.gameEnded()) {
                std::cout << "Enemy board: " << enemyBoard << "Your board: " << state.board;
                std::cout << "DRAW\n";
            } else if(enemyBoard.gameEnded()) {
                std::cout << "Enemy board: " << enemyBoard << "Your board: " << state.board;
                std::cout << "YOU WON\n";
            } else {
                std::cout << "Enemy board: " << enemyBoard << "Your board: " << state.board;
                std::cout << "YOU LOST\n";
            }
            break;
        }
        case 'M': {
            MCTS::State state(size, ships);
            while(!enemyBoard.gameEnded() && !state.gameEnded()) {
                std::cout << "Enemy board: " << enemyBoard << "Your board: " << state.board;
                std::cout << "Select x-coordinates of your next move(A,B,...):\n";
                std::cin >> x;
                std::cout << "Select y-coordinates of your next move(1,2,...):\n";
                std::cin >> y;
                auto xx = static_cast<short>(x - 'A');
                if(xx > size - 1 || xx < 0 || y > size || y < 1) {
                    std::cout << "Wrong coordinates";
                    return 0;
                }
                enemyBoard.doMove(Coords(xx, static_cast<short>(y - 1)));
                Coords move = MCTS::calculateMove(state, 1000);
                state.doMove(move);
            }
            if(enemyBoard.gameEnded() && state.board.gameEnded()) {
                std::cout << "Enemy board: " << enemyBoard << "Your board: " << state.board;
                std::cout << "DRAW\n";
            } else if(enemyBoard.gameEnded()) {
                std::cout << "Enemy board: " << enemyBoard << "Your board: " << state.board;
                std::cout << "YOU WON\n";
            } else {
                std::cout << "Enemy board: " << enemyBoard << "Your board: " << state.board;
                std::cout << "YOU LOST\n";
            }
            break;
        }
        default:
            std::cout << "Wrong choice selected.";
    }

    return 0;
}
