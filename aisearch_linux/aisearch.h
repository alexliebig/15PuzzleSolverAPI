//Alex Liebig
/*
This is the header file for the AI search class that stores all the member methods and variable declarations
*/
#pragma once
#include <iostream> 
#include <string>
#include <vector>
#include <cmath>
#include <map>
#include <algorithm> 
#include <queue>
#include <iomanip>
#include <fstream>

/*
This is a struct that is used to store the default value of 1000000.
This was the simplest way I found to store default values as 1 million.
*/
struct Node {
    int value = 1000000;
};

/*
Same as the previous struct, except it stores values by default as -1
*/
struct HeuristicNode {
    int value = -1;
};

class aisearch
{
public:
    aisearch(std::vector<std::vector<int>>); //This is the default constructor, it takes in the original puzzle to be solved
    void OutputPathToTerminal(); //This method outputs values to the terminal
    void OutputPathToFile(std::string fileName); //this method outputs values to the specified file taken as a string
    std::vector<std::vector<std::vector<int>>> GetGoalPath(); //this method gets the goal path
    std::vector<char> GetGoalPathDirection(); //this method gets the direction of the goal path

    static std::vector<std::vector<int>> CreatePuzzle(int);
    static bool IsValidPuzzle(std::vector<std::vector<int>>&);
private:
    //member variables
    std::vector<std::vector<int>> StartingPuzzle; //this vector stores the starting puzzle
    std::vector<std::vector<int>> GoalPuzzle; //this vector stores the goal puzzle, computer in the constructor
    std::map<std::vector<std::vector<int>>, std::vector<std::vector<int>>> cameFrom; //this map stores where each node came from
    std::map<std::vector<std::vector<int>>, char> MovementDirection; //this map stores the movement direction, such as left right up and down
    std::vector<char> goal_path_direction; //returns the direction of the goal path
    std::vector<std::vector<std::vector<int>>> goal_path; //stores the goal path
    int BlankPositionRow; //this stores the starting x position of the blank tile
    int BlankPositionCol; //this stores the starting y position of the blank tile
    int GoalPathDistance = 0; //this stores the number of nodes in the goal path
    int TotalNodesOpened = 0; //this stores the number of nodes that were visited in total

    //member methods
    void AStarSearch(); //this is the AStarSearch used to find the shortest goal path
    int ManhattenDistance(std::vector<std::vector<int>>&); //this method calculates the manhatten distance as the heuristic function
    bool TryMovingTile(std::vector<std::vector<int>>&, char, int&, int&); //this method tries moving the tile. If true, it returns the moved tile along with the blank position
    void FormatNodeOutput(std::vector<std::vector<int>>&, std::ostream&); //this method formats the tiles using iomanip
    void GetTotalPath(std::map<std::vector<std::vector<int>>, std::vector<std::vector<int>>>& cameFrom, std::vector<std::vector<int>>& current); //this method calculates the goal path distance
    void OutputPath(std::map<std::vector<std::vector<int>>, std::vector<std::vector<int>>>&, std::vector<std::vector<int>>, std::map<std::vector<std::vector<int>>, char>&, std::ostream&); //this method outputs the goal path
    void PrintNodeStatistics(std::ostream&); //this method outputs the node statistics
    void FormatTileMovement(char, std::ostream&); //this method is used to output the tile's movement direction
};


