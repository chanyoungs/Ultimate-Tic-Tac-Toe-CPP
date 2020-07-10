// Headers
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <array>
#include <cmath>
#include <ctime>
#include <chrono>

// Namespaces
using namespace std;

// Global variables
double k = sqrt(2);
enum eState
{
    ePlayer1 = 7,
    eDraw = 8,
    ePlayer2 = 9,
    eIncomplete = 0
};

// Classes
class Timer
{
public:
    std::chrono::_V2::steady_clock::time_point time_start;
    Timer()
    {
        start();
    }
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

class UT
{
public:
    std::array<int, 2> currentBoardInit{-1, -1};
    std::array<int, 2> currentBoard;

    eState currentPlayerInit = ePlayer1;
    eState currentPlayer;

    bool bGameOverInit = false;
    bool bGameOver;

    eState finalResultInit = eIncomplete;
    eState finalResult;

    // Integer storing the number of complete boards
    int finalCountInit = 0;
    int finalCount;

    // 3x3x3x3 array storing the state of each of the 81 squares
    std::array<std::array<std::array<std::array<eState, 3>, 3>, 3>, 3> fullBoardInit{};
    std::array<std::array<std::array<std::array<eState, 3>, 3>, 3>, 3> fullBoard;
    // 3x3 array storing the state of each of the 9 boards
    std::array<std::array<eState, 3>, 3> bigBoardStateInit{};
    std::array<std::array<eState, 3>, 3> bigBoardState;
    // 3x3 array storing the number of moves made in each of the 9 boards
    std::array<std::array<int, 3>, 3> bigBoardCountInit{};
    std::array<std::array<int, 3>, 3> bigBoardCount;

    // Constructor
    UT()
    {
        // Initialise state values
        restart();
    }

    void restart()
    {
        currentBoard = currentBoardInit;
        currentPlayer = currentPlayerInit;
        bGameOver = bGameOverInit;
        finalResult = finalResultInit;
        finalCount = finalCountInit;
        fullBoard = fullBoardInit;
        bigBoardState = bigBoardStateInit;
        bigBoardCount = bigBoardCountInit;
    }

    void updateInitValues()
    {
        currentBoardInit = currentBoard;
        currentPlayerInit = currentPlayer;
        bGameOverInit = bGameOver;
        finalResultInit = finalResult;
        finalCountInit = finalCount;
        fullBoardInit = fullBoard;
        bigBoardStateInit = bigBoardState;
        bigBoardCountInit = bigBoardCount;
    }

    eState evaluateLine(std::array<eState, 3> line)
    {
        auto state = line[0];
        for (int i = 1; i < 3; i++)
            // If any of the elements in the line are different, return "Draw"
            if (line[i] != state)
                return eIncomplete;
        // If all the elements in the line are the same, return the winning player
        return state;
    }

    eState evaluate3x3Board(std::array<std::array<eState, 3>, 3> smallBoard, std::array<int, 2> currentPosition)
    {
        // Check for corresponding row
        auto result = evaluateLine({smallBoard[currentPosition[0]][0], smallBoard[currentPosition[0]][1], smallBoard[currentPosition[0]][2]});
        if (result != eIncomplete)
            return result;
        // Check for corresponding column
        result = evaluateLine({smallBoard[0][currentPosition[1]], smallBoard[1][currentPosition[1]], smallBoard[2][currentPosition[1]]});
        if (result != eIncomplete)
            return result;
        // Check for first diagonal
        if (currentPosition[0] == currentPosition[1])
        {
            result = evaluateLine({smallBoard[0][0], smallBoard[1][1], smallBoard[2][2]});
            if (result != eIncomplete)
                return result;
        }
        // Check for first diagonal
        if (currentPosition[0] == currentPosition[1])
        {
            result = evaluateLine({smallBoard[0][0], smallBoard[1][1], smallBoard[2][2]});
            if (result != eIncomplete)
                return result;
        }
        // Check for second diagonal
        if (currentPosition[0] == 2 - currentPosition[1])
        {
            result = evaluateLine({smallBoard[0][2], smallBoard[1][1], smallBoard[2][0]});
            if (result != eIncomplete)
                return result;
        }

        // Otherwise, return "Unfilled" (We'll check later whether or not it's actually a "Draw")
        return eIncomplete;
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
                                playableMoves.push_back({a, b, c, d});
        }
        // Else, retrieve all unoccupied coordinates in the currentBoard
        else
            for (int i = 0; i < 3; i++)
                for (int j = 0; j < 3; j++)
                    if (fullBoard[currentBoard[0]][currentBoard[1]][i][j] == 0)
                        playableMoves.push_back({currentBoard[0], currentBoard[1], i, j});
        return playableMoves;
    }

    void makeMove(std::array<int, 4> move)
    {
        // Update full board
        fullBoard[move[0]][move[1]][move[2]][move[3]] = currentPlayer;
        // Update big board count
        bigBoardCount[move[0]][move[1]]++;

        // Evaluate the result of the 3x3 board
        auto result = evaluate3x3Board(fullBoard[move[0]][move[1]], {move[2], move[3]});

        // If no one won and the board is full, set result to "Draw"
        if (result == eIncomplete && bigBoardCount[move[0]][move[1]] == 9)
            result = eDraw;

        // If the board completed...
        if (result != eIncomplete)
        {
            // Update big board
            bigBoardState[move[0]][move[1]] = result;
            // Update final count
            finalCount++;

            // Update the full board
            for (int i = 0; i < 3; i++)
                for (int j = 0; j < 3; j++)
                    fullBoard[move[0]][move[1]][i][j] = result;

            // Check for the final result
            finalResult = evaluate3x3Board(bigBoardState, {move[0], move[1]});
            // If no one won and the board is full, set result to "Draw"
            if (finalResult == eIncomplete && finalCount == 9)
                finalResult = eDraw;

            // If game over...
            if (finalResult != eIncomplete)
            {
                bGameOver = true;
                // If there is no 3 in a row but the big board is full...
                if (finalResult == eDraw)
                {
                    // count who won most boards and return winner accordingly
                    int count = 0;
                    for (int i = 0; i < 3; i++)
                        for (int j = 0; j < 3; j++)
                            count = count + bigBoardState[i][j] - 8;

                    // Update final result
                    if (count == 0)
                        finalResult = eDraw;
                    else
                        finalResult = count < 0 ? ePlayer1 : ePlayer2;
                }
            }
        }

        // Update current board
        currentBoard = std::array<int, 2>{{move[2], move[3]}};

        // If the current board is unplayable, convert it to [-1, -1]
        if (bigBoardState[currentBoard[0]][currentBoard[1]] != 0)
            currentBoard = std::array<int, 2>{{-1, -1}};

        // Update current player 7 <-> 9
        currentPlayer = currentPlayer == ePlayer1 ? ePlayer2 : ePlayer1;
    }

    std::array<int, 4> convert2DTo4D(std::array<int, 2> move2D)
    {
        return std::array<int, 4>{{move2D[0] / 3, move2D[1] / 3, move2D[0] % 3, move2D[1] % 3}};
    }

    std::array<int, 2> convert4DTo2D(std::array<int, 4> move4D)
    {
        return std::array<int, 2>{{3 * move4D[0] + move4D[2], 3 * move4D[1] + move4D[3]}};
    }

    string displaySquare(eState square)
    {
        if (square == ePlayer1)
            return "O";
        else if (square == ePlayer2)
            return "X";
        else if (square == eDraw)
            return "D";
        else
            return " ";
    }

    string displaySmallRow(std::array<eState, 3> row)
    {
        return " " + displaySquare(row[0]) + " | " + displaySquare(row[1]) + " | " + displaySquare(row[2]) + " ";
    }

    void displayBigRow(std::array<std::array<eState, 3>, 3> rows)
    {
        cout << displaySmallRow(rows[0]) + " # " + displaySmallRow(rows[1]) + " # " + displaySmallRow(rows[2]);
    }

    void displayThinUnderline()
    {
        cout << "---+---+--- # ---+---+--- # ---+---+---";
    }

    void displayThickUnderline()
    {
        cout << "            #             #            \n#######################################\n            #             #            \n";
    }

    eState rollOut(bool bDisplay = false)
    {
        if (bDisplay)
            display();
        while (!bGameOver)
        {
            auto playableMoves = getPlayableMoves();
            int rand_index = rand() % playableMoves.size();
            makeMove(playableMoves[rand_index]);
            if (bDisplay)
                display();
        }
        return finalResult;
    }

    void display()
    {
        cout << endl;
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {

                displayBigRow({fullBoard[i][0][j], fullBoard[i][1][j], fullBoard[i][2][j]});
                if (i == 1)
                    cout << "      " + displaySmallRow(bigBoardState[j]);
                cout << endl;

                if (j < 2)
                {
                    displayThinUnderline();
                    if (i == 1)
                        cout << "      ---+---+---";
                    cout << endl;
                }
            }
            if (i < 2)
                displayThickUnderline();
        }
        cout << endl;
    }
};

class Node
{
public:
    // ##############################################
    // Public properties
    // ##############################################
    bool bRootNode;
    Node *pParent;
    int index;
    int score = 0;
    int visits = 0;
    std::array<int, 4> move;
    std::vector<Node *> pChildren;

    // ##############################################
    // Constructors and Destructures
    // ##############################################
    // Root constructor
    Node()
    {
        pParent = NULL;
        bRootNode = true;
    }
    // Non-root constructor
    Node(Node *pP, int i, std::array<int, 4> m)
    {
        pParent = pP;
        bRootNode = false;
        index = i;
        move = m;
    }

    ~Node()
    {
        // for (int i = 0; i < pChildren.size(); i++)
        //     delete pChildren[i];
    }

    // ##############################################
    // Public Methods
    // ##############################################
    // This assumes that the visits are non-zero
    double getWeight(int childScore, int childVisits, int parentVisits)
    {
        return childScore / childVisits + k * sqrt(log(parentVisits) / childVisits);
    }

    Node *getBestChild()
    {
        double maxWeight = 0;
        Node *bestChild;
        for (size_t i = 0; i < pChildren.size(); i++)
        {
            if (pChildren[i]->visits == 0)
                return pChildren[i];
            double weight = getWeight(pChildren[i]->score, pChildren[i]->visits, visits);
            if (weight > maxWeight)
            {
                maxWeight = weight;
                bestChild = pChildren[i];
            }
        }
        return bestChild;
    }
};

class MCTS : public UT
{
public:
    Node *rootNode;
    MCTS()
    {
        rootNode = new Node();
        iterate();
        iterate();
    }

    void iterate(Node *node = NULL)
    {
        if (node == NULL)
        {
            node = rootNode;
            restart();
        }
        if (bGameOver)
        {
            rolloutAndBackpropagate(node);
        }
        else
        {
            // If not leaf node, pick a child with the highest weight and iterate
            if (node->pChildren.size() != 0)
            {
                auto bestChild = node->getBestChild();
                makeMove(bestChild->move);
                iterate(bestChild);
                // If leaf node...
            }
            else
            {
                // If first time visiting, rollout and backpropagate
                if (node->visits == 0)
                {
                    rolloutAndBackpropagate(node);
                    // If revisiting, expand
                }
                else
                {
                    auto playableMoves = getPlayableMoves();

                    // Update children nodes
                    for (size_t i = 0; i < playableMoves.size(); i++)
                    {
                        node->pChildren.push_back(new Node(node, i, playableMoves[i]));
                    }
                    // Rollout on the first child
                    makeMove(playableMoves[0]);
                    rolloutAndBackpropagate(node->pChildren[0]);
                }
            }
        }
    }

    void backpropagate(Node *node, int score)
    {
        node->score += score;
        node->visits++;
        if (!node->bRootNode)
            backpropagate(node->pParent, -score);
    }

    void rolloutAndBackpropagate(Node *node)
    {
        // Current player 7(player1), 9(player2) at the time of leaf node before rollout
        auto player = currentPlayer;
        // Final result gives 7(player1 wins), 8(draw), 9(player2 wins)
        auto result = rollOut();
        int score;
        if (result == eDraw)
            score = 0;
        else
            score = result == player ? -1 : 1;
        backpropagate(node, score);
    }

    void recursivelyDeleteChildren(Node *node)
    {
        for (size_t i = 0; i < node->pChildren.size(); i++)
            recursivelyDeleteChildren(node->pChildren[i]);
        delete node;
    }

    void moveAndUpdateRootNode(Node *chosenChildNode)
    {
        restart();
        makeMove(chosenChildNode->move);
        // Delete unnecessary nodes
        for (size_t i = 0; i < rootNode->pChildren.size(); i++)
        {
            auto childNode = rootNode->pChildren[i];

            // Recursively delete all other siblings and their children
            if (childNode != chosenChildNode)
                recursivelyDeleteChildren(rootNode->pChildren[i]);
        }
        updateInitValues();
        // Update root node
        rootNode = chosenChildNode;
        rootNode->bRootNode = true;
        // Delete previous root node
        delete rootNode->pParent;

        iterate();
        iterate();
    }

    std::array<int, 4> outputMove()
    {
        // Get greedy maximal scoring child
        int maxScore;
        Node *maxChildNode;
        for (size_t i = 0; i < rootNode->pChildren.size(); i++)
        {
            if (i == 0)
            {
                maxChildNode = rootNode->pChildren[0];
                maxScore = maxChildNode->score;
            }
            else
            {
                auto childNode = rootNode->pChildren[i];
                auto score = childNode->score;
                if (score > maxScore)
                {
                    maxScore = score;
                    maxChildNode = childNode;
                }
            }
        }
        auto move4D = maxChildNode->move;
        auto move2D = convert4DTo2D(move4D);
        int score = maxChildNode->score;
        int visits = maxChildNode->visits;

        moveAndUpdateRootNode(maxChildNode);

        return {move2D[0], move2D[1], score, visits};
    }

    void inputMove(std::array<int, 2> inputMove2D)
    {
        auto inputMove4D = convert2DTo4D(inputMove2D);
        for (size_t i = 0; i < rootNode->pChildren.size(); i++)
        {
            auto child = rootNode->pChildren[i];
            auto move = child->move;
            if (move[0] == inputMove4D[0] &&
                move[1] == inputMove4D[1] &&
                move[2] == inputMove4D[2] &&
                move[3] == inputMove4D[3])
            {
                return moveAndUpdateRootNode(child);
            }
        }
        throw std::invalid_argument("No Move Match Found!");
    }
};

eState selfPlay(int iterations = 100, bool bDisplay = true)
{
    UT ut;
    bool bFirstMove = true;
    MCTS *mcts1 = new MCTS;
    MCTS *mcts2 = new MCTS;
    std::array<MCTS *, 2> players{mcts1, mcts2};
    int player = 0;
    std::array<int, 2> move2D;

    int n = 0;

    while (!ut.bGameOver && n < 100)
    {
        if (bFirstMove)
            bFirstMove = false;
        else
            players[player]->inputMove(move2D);
        for (int i = 0; i < iterations; i++)
            players[player]->iterate();
        auto output = players[player]->outputMove();
        move2D = {output[0], output[1]};
        int score = output[2];
        int visits = output[3];
        auto move4D = ut.convert2DTo4D(move2D);
        ut.makeMove(move4D);
        // cerr << "4D: (" << move4D[0] << "," << move4D[1] << "," << move4D[2] << "," << move4D[3] << ") 2D: (" << move2D[0] << "," << move2D[1] << ")" << endl;
        cerr << "Confidence: " << score << "/" << visits << endl;
        if (bDisplay)
            ut.display();
        player = 1 - player;
        n++;
    }
    return ut.finalResult;
};

eState selfPlayTimed(int timeLimit = 100, bool bDisplay = true)
{
    Timer timer;
    UT ut;
    bool bFirstMove = true;
    MCTS *mcts1 = new MCTS;
    MCTS *mcts2 = new MCTS;
    std::array<MCTS *, 2> players{mcts1, mcts2};
    int player = 0;
    std::array<int, 2> move2D;

    while (!ut.bGameOver)
    {
        if (bFirstMove)
            bFirstMove = false;
        else
        {
            timer.start();
            players[player]->inputMove(move2D);
        }
        int n = 0;
        while (timer.time() < timeLimit)
        {
            players[player]->iterate();
            n++;
        }
        auto output = players[player]->outputMove();

        move2D = {output[0], output[1]};
        int score = output[2];
        int visits = output[3];
        cerr << "Confidence: " << score << "/" << visits << " Simulations: " << n << " Actual time taken: " << timer.time() << endl;
        ut.makeMove(ut.convert2DTo4D(move2D));
        if (bDisplay)
            ut.display();
        player = 1 - player;
        n++;
    }
    return ut.finalResult;
};

void codinGameTurn(int timeLimit = 100, MCTS *pmcts = NULL)
{
    // Initialise timer
    Timer timer;

    // Input values
    int opponentRow;
    int opponentCol;
    cin >> opponentRow >> opponentCol;
    cin.ignore();
    int validActionCount;
    cin >> validActionCount;
    cin.ignore();
    for (int i = 0; i < validActionCount; i++)
    {
        int row;
        int col;
        cin >> row >> col;
        cin.ignore();
    }

    // Conditionally accept move
    if (opponentRow != -1)
        pmcts->inputMove({opponentRow, opponentCol});

    // Iterate until timelimit
    int n = 0;
    while (timer.time() < timeLimit)
    {
        pmcts->iterate();
        n++;
    }

    // Output move output = [moveRow, moveCol, score, visits]
    auto output = pmcts->outputMove();
    int score = output[2];
    int visits = output[3];
    cout << output[0] << " " << output[1] << endl;
    cerr << "#Simulations: " << n << endl;
    cerr << "Confidence: " << score << "/" << visits << endl;
}

void codinGame()
{
    MCTS *pmcts = new MCTS;

    // Initial move
    codinGameTurn(3000, pmcts);
    while (1)
    {
        codinGameTurn(100, pmcts);
    }
}

int main()
{
    srand(time(NULL));
    selfPlayTimed();
    return 0;
}