#include "aisearch.h"

#define EXPORT __declspec(dllexport)

extern "C"
{
    EXPORT int * __stdcall create_puzzle(int size)
    {
        int* return_puzzle = new int[size * size];
        std::vector<std::vector<int>> random_puzzle = aisearch::CreatePuzzle(size);
        int i = 0;
        for (auto const& row : random_puzzle) {
            for (auto const& val : row) {
                return_puzzle[i] = (int)val;
                i++;
            }
        }
        return return_puzzle;
    }

    EXPORT int __stdcall release_memory(int* pArray)
    {
        delete[] pArray;
        return 0;
    }

    EXPORT bool __stdcall is_valid_puzzle(int* pArray, int boardSize)
    {
        std::vector<std::vector<int>> puzzle;
        std::vector<int> row(boardSize);
        for (int i = 0; i < boardSize; i++)
            puzzle.push_back(row);
        for (int i = 0; i < boardSize * boardSize; i++) {
            puzzle[i / boardSize][i % boardSize] = pArray[i];
        }
        return aisearch::IsValidPuzzle(puzzle);
    }

    EXPORT int * __stdcall solve_puzzle(int* pArray, int boardSize, int & solvedPuzzleSize)
    {
        std::vector<std::vector<int>> puzzle;
        std::vector<int> row(boardSize);
        for (int i = 0; i < boardSize; i++)
            puzzle.push_back(row);
        for (int i = 0; i < boardSize * boardSize; i++) {
            puzzle[i / boardSize][i % boardSize] = pArray[i];
        }
        aisearch current_puzzle(puzzle);

        //convert to string
        std::vector<char> goalPath = current_puzzle.GetGoalPathDirection();
        int* returnPath = new int[goalPath.size()];
        int i = 0;
        for (auto const& val : goalPath) {
            returnPath[i] = val;
            i++;
        }

        solvedPuzzleSize = goalPath.size();
        return returnPath;
    }
}

std::string VectorCharToString(std::vector<char> path)
{
    std::string returnString = "";
    for (char c : path) {
        returnString += c;
    }
    return returnString;
}

int main()
{
    return 0;
}


