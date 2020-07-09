#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <array>
#include <cmath>
#include <ctime>
#include <chrono>

using namespace std;

class UT
{
public:
    std::array<int, 2> currentBoardInit{-1, -1};
    std::array<int, 2> currentBoard;

    int currentPlayerInit = 7;
    int currentPlayer;

    bool bGameOverInit = false;
    bool bGameOver;

    int finalResultInit = 0;
    int finalResult;

    // 3x3x3x3 array of zeros
    std::array<std::array<std::array<std::array<int, 3>, 3>, 3>, 3> fullBoardInit{};
    std::array<std::array<std::array<std::array<int, 3>, 3>, 3>, 3> fullBoard;
    // 3x3 array of zeros
    std::array<std::array<int, 3>, 3> bigBoardInit{};
    std::array<std::array<int, 3>, 3> bigBoard;
    // 8x3x2 list of lines for checking the result of the board
    std::array<std::array<std::array<int, 2>, 3>, 8> winningLines;

    // Constructor
    UT()
    {
        // Fill in winningLines
        setWinningLines();
        // Initialise state values
        restart();
    }

    void setWinningLines()
    {
        // 3xRows
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                winningLines[i][j][0] = i;
                winningLines[i][j][1] = j;
            }
        }
        // 3xColumns
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                winningLines[i + 3][j][0] = j;
                winningLines[i + 3][j][1] = i;
            }
        }
        // Diagonal1
        for (int j = 0; j < 3; j++)
        {
            winningLines[6][j][0] = j;
            winningLines[6][j][1] = j;
        }
        // Diagonal2
        for (int j = 0; j < 3; j++)
        {
            winningLines[7][j][0] = j;
            winningLines[7][j][1] = 2 - j;
        }
    }

    void restart()
    {
        currentBoard = currentBoardInit;
        currentPlayer = currentPlayerInit;
        bGameOver = bGameOverInit;
        finalResult = finalResultInit;
        fullBoard = fullBoardInit;
        bigBoard = bigBoardInit;
    }

    void updateInitValues()
    {
        currentBoardInit = currentBoard;
        currentPlayerInit = currentPlayer;
        bGameOverInit = bGameOver;
        finalResultInit = finalResult;
        fullBoardInit = fullBoard;
        bigBoardInit = bigBoard;
    }

    int evaluate3x3Board(std::array<std::array<int, 3>, 3> smallBoard)
    {
        // Check if there is a winning line -> 7(player1)/9(player2)
        for (int i = 0; i < 8; i++)
        {
            int total = 0;
            for (int j = 0; j < 3; j++)
                total = total + smallBoard[winningLines[i][j][0]][winningLines[i][j][1]];
            if (total == 21)
                return 7;
            else if (total == 27)
                return 9;
        }

        // Check if there is any unoccupied square -> 0(incomplete)
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++)
                if (smallBoard[i][j] == 0)
                    return 0;

        // Else -> 8(Draw)
        return 8;
    }

    std::vector<std::array<int, 4>> getPlayableMoves()
    {
        std::vector<std::array<int, 4>> playableMoves;
        // If we are free to choose any board, retrieve all unoccupied coordinates
        if (currentBoard[0] == -1)
        {
            for (int a = 0; a < 3; a++)
                for (int b = 0; b < 3; b++)
                    for (int c = 0; c < 3; c++)
                        for (int d = 0; d < 3; d++)
                            if (fullBoard[a][b][c][d] == 0)
                                playableMoves.push_back(std::array<int, 4>{a, b, c, d});
        }
        // Else, retrieve all unoccupied coordinates in the currentBoard
        else
            for (int i = 0; i < 3; i++)
                for (int j = 0; j < 3; j++)
                    if (fullBoard[currentBoard[0]][currentBoard[1]][i][j] == 0)
                        playableMoves.push_back(std::array<int, 4>{currentBoard[0], currentBoard[1], i, j});
        return playableMoves;
    }

    void makeMove(std::array<int, 4> move)
    {
        // Update full board
        fullBoard[move[0]][move[1]][move[2]][move[3]] = currentPlayer;

        // Evaluate the result of the 3x3 board
        int result = evaluate3x3Board(fullBoard[move[0]][move[1]]);

        // If the board completed...
        if (result != 0)
        {
            // Update big board
            bigBoard[move[0]][move[1]] = result;
            // Update the full board
            for (int i = 0; i < 3; i++)
                for (int j = 0; j < 3; j++)
                    fullBoard[move[0]][move[1]][i][j] = result;

            // Check for the final result
            finalResult = evaluate3x3Board(bigBoard);

            // If game over...
            if (finalResult != 0)
            {
                bGameOver = true;
                // If there is no 3 in a row but the big board is full...
                if (finalResult == 8)
                {
                    // count who won most boards and return winner accordingly
                    int count = 0;
                    for (int i = 0; i < 3; i++)
                        for (int j = 0; j < 3; j++)
                            count = count + bigBoard[i][j] - 8;
                    if (count > 0)
                        count = 1;
                    else if (count < 0)
                        count = -1;
                    finalResult = count + 8;
                }
            }
        }
        // Update current board
        currentBoard = std::array<int, 2>{{move[2], move[3]}};

        // If the current board is unplayable, convert it to [-1, -1]
        if (bigBoard[currentBoard[0]][currentBoard[1]] != 0)
            currentBoard = std::array<int, 2>{{-1, -1}};

        // Update current player 7 <-> 9
        currentPlayer = 16 - currentPlayer;
    }

    std::array<int, 4> convert2DTo4D(std::array<int, 4> move2D)
    {
        return std::array<int, 4>{{floor(move2D[0] / 3), floor(move2D[1] / 3), move2D[0] % 3, move2D[1] % 3}};
    }

    std::array<int, 2> convert4DTo2D(std::array<int, 2> move4D)
    {
        return std::array<int, 2>{{3 * move4D[0] + move4D[2], 3 * move4D[1] + move4D[3]}};
    }

    string displayPlayer(int player)
    {
        if (player == 7)
            return "O";
        else if (player == 9)
            return "X";
        else
            return " ";
    }

    string displaySmallRow(std::array<int, 3> row)
    {
        return " " + displayPlayer(row[0]) + " | " + displayPlayer(row[1]) + " | " + displayPlayer(row[2]) + " ";
    }

    string displayBigRow(std::array<std::array<int, 3>, 3> rows)
    {
        return displaySmallRow(rows[0]) + " # " + displaySmallRow(rows[1]) + " # " + displaySmallRow(rows[2]) + "\n";
    }

    string displayThinUnderline()
    {
        return "----------- # ----------- # -----------\n";
    }

    string displayThickUnderline()
    {
        return "            #             #            \n#######################################\n            #             #            \n";
    }

    void display()
    {
        cout << endl;
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                cout << displayBigRow(std::array<std::array<int, 3>, 3>{{fullBoard[i][0][j], fullBoard[i][1][j], fullBoard[i][2][j]}});
                if (j < 2)
                    cout << displayThinUnderline();
            }
            if (i < 2)
                cout << displayThickUnderline();
        }
        cout << endl;
    }
};

class Timer
{
public:
    std::chrono::_V2::steady_clock::time_point time_start;
    void start()
    {
        time_start = std::chrono::steady_clock::now();
    }
    int time()
    {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - time_start);
        return elapsed.count();
    }
};

int main()
{
    srand(time(NULL));
    UT ut;
    Timer timer;

    int n = 0;
    timer.start();
    while (timer.time() < 100)
    {
        ut.restart();
        // ut.display();
        while (!ut.bGameOver)
        {
            std::vector<std::array<int, 4U>> playableMoves = ut.getPlayableMoves();
            int rand_index = rand() % playableMoves.size();
            ut.makeMove(playableMoves[rand_index]);
            // ut.display();
        }
        n++;
    }
    cout << n << "simulations ran.";

    // string result;
    // if (ut.finalResult == 8)
    //     result = "Draw!";
    // else
    //     result = ut.displayPlayer(ut.finalResult) + " Wins!";

    // cout << result;

    return 0;
}