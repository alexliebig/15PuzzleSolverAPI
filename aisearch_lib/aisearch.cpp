//Alex Liebig, CS 482 Assignment 2
/*
This is the .cpp function that stores all the method definitions for aisearch.h
*/
#include "aisearch.h"
/*
The constructor here takes in the puzzle to be solved. It stores the current_puzzle into the StartingPuzzle,
calculates the Goal Puzzle, then calls the AStartSearch function. It also finds out which tile is the blank tile.
*/
aisearch::aisearch(std::vector<std::vector<int>> current_puzzle) {
    StartingPuzzle = current_puzzle; //stores current puzzle
    //loops to find empty tile
    for (int i = 0; i < (int)current_puzzle.size(); i++) {
        for (int j = 0; j < (int)current_puzzle[i].size(); j++) {
            if (current_puzzle[i][j] == 0) {
                BlankPositionRow = i;
                BlankPositionCol = j;
            }
        }
    }

    int count = 1;

    //creates goal state
    for (int i = 0; i < (int)current_puzzle.size(); i++) {
        std::vector<int> currentRow;
        for (int j = 0; j < (int)current_puzzle[i].size(); j++) {
            currentRow.push_back(count);
            count++;
        }
        GoalPuzzle.push_back(currentRow);
    }
    GoalPuzzle[GoalPuzzle.size() - 1][GoalPuzzle[0].size() - 1] = 0; //sets last node to zero

    AStarSearch(); //calls AStarSearch Algorithm
}

/*
This function is a helper function that outputs the path to the terminal. It sends std::cout as the output path (the terminal)
It prints the node statistics and it outputs the path to the terminal
*/
void aisearch::OutputPathToTerminal() {
    std::ostream& output = std::cout; //stores cout to output
    PrintNodeStatistics(output);
    OutputPath(cameFrom, GoalPuzzle, MovementDirection, output);
}

/*
This function outputs the path to the specified file. It prints the PrintNodeStatistics, and the OutputPath using the specified file.
*/
void aisearch::OutputPathToFile(std::string fileName) {
    std::ofstream outfile; //output file name
    outfile.open(fileName); //opens the output file by name (as a string)
    //tests to see if the output file will fail
    if (outfile.fail()) {
        std::cout << "OUTPUT HAS FAILED" << std::endl;
        return;
    }
    PrintNodeStatistics(outfile);
    OutputPath(cameFrom, GoalPuzzle, MovementDirection, outfile);
    outfile.close();
}

std::vector<std::vector<std::vector<int>>> aisearch::GetGoalPath()
{
    return goal_path;
}

std::vector<char> aisearch::GetGoalPathDirection()
{
    return goal_path_direction;
}

std::vector<std::vector<int>> aisearch::CreatePuzzle(int dimmension)
{
    if (dimmension <= 2)
        return std::vector<std::vector<int>>();
    std::vector<std::vector<int>> puzzle(dimmension, std::vector<int>(dimmension, -1));

    srand((unsigned int)time(nullptr));
    for (int i = 0; i < dimmension * dimmension; i++) {
        int row = rand() % dimmension;
        int col = rand() % dimmension;

        if (puzzle[row][col] == -1) {
            puzzle[row][col] = i;
        }
        else {
            for (auto& p_row : puzzle) {
                for (auto& p_col : p_row) {
                    if (p_col == -1) {
                        p_col = i;
                        goto end_loop;
                    }
                }
            }
        end_loop:;
        }
    }

    while (!aisearch::IsValidPuzzle(puzzle))
    {
        for (int i = 0; i < (int)puzzle.size(); i++) {
            for (int j = 0; j < (int)puzzle[i].size(); j++) {
                int x1 = i;
                int x2 = j;

                int y1 = j + 1 >= dimmension ? i + 1 : i;
                int y2 = (j + 1) % dimmension;

                if (y1 >= (int)puzzle.size())
                    continue;

                if (puzzle[x1][x2] > puzzle[y1][y2])
                {
                    int temp = puzzle[x1][x2];
                    puzzle[x1][x2] = puzzle[y1][y2];
                    puzzle[y1][y2] = temp;
                    goto end_create_puzzle_loop;
                }
            }
        }
    end_create_puzzle_loop:;
    }

    return puzzle;

}

bool aisearch::IsValidPuzzle(std::vector<std::vector<int>>& current_puzzle)
{
    int inversions = 0;
    std::map<int, bool> seen_values;
    std::pair<int, int> blank_tile;
    for (int i = 0; i < (int)current_puzzle.size(); i++)
    {
        for (int j = 0; j < (int)current_puzzle[i].size(); j++)
        {
            if (seen_values[current_puzzle[i][j]] == true)
                return false;
            if (current_puzzle[i][j] < 0 || current_puzzle[i][j] > (((int)current_puzzle.size()) * ((int)current_puzzle.size()) - 1))
                return false;
            seen_values[current_puzzle[i][j]] = true;
            blank_tile = current_puzzle[i][j] == 0 ? std::make_pair(i, j) : blank_tile;
            for (int count_down = current_puzzle[i][j]; count_down > 0; count_down--) {
                if (seen_values[count_down] == false)
                    inversions++;
            }
        }
    }

    //odd tile count
    if (current_puzzle.size() % 2 == 1) {
        if (inversions % 2 == 0)
            return true;
        else
            return false;
    }

    //even tile count
    if (current_puzzle.size() % 2 == 0)
    {
        int row_from_bottom = current_puzzle.size() - blank_tile.first;
        if (row_from_bottom % 2 != inversions % 2)
            return true;
        else
            return false;
    }
    return true;
}

/*
This function recursively outputs the output path of the goal states. It takes in a vector of where each node came from,
the current node (i.e. the starting puzzle), the movement direction to be outputted (left, right, up, down) corresponding
to that node, and the location of the output.
*/
void aisearch::OutputPath(std::map<std::vector<std::vector<int>>, std::vector<std::vector<int>>>& cameFrom, std::vector<std::vector<int>> current, std::map<std::vector<std::vector<int>>, char>& MovementDirection, std::ostream& outputLocation) {
    if (current == StartingPuzzle) {
        FormatNodeOutput(current, outputLocation);
        FormatTileMovement(MovementDirection[current], outputLocation);
        return;
    }
    OutputPath(cameFrom, cameFrom[current], MovementDirection, outputLocation);

    FormatTileMovement(MovementDirection[current], outputLocation);
    FormatNodeOutput(current, outputLocation);
}

/*
This function simply outputs the Goal path distance and the total number of nodes open. It outputs it to the
correct output location
*/
void aisearch::PrintNodeStatistics(std::ostream& outputLocation) {
    outputLocation << std::endl << "Total Nodes in Goal Path: " << GoalPathDistance << std::endl;
    outputLocation << "Total Nodes Opened: " << TotalNodesOpened << std::endl << std::endl;
}

/*
This function simply formats the node based on the character tile passed in along with a message "Moving Tile".
It displays this to the correct output location
*/
void aisearch::FormatTileMovement(char tile, std::ostream& outputLocation) {
    std::string outputString = ""; //string to be outputted
    switch (tile) {
    case 'l':
        outputString = "left";
        break;
    case 'r':
        outputString = "right";
        break;
    case 'u':
        outputString = "up";
        break;
    case 'd':
        outputString = "down";
        break;
    default:
        return;
    }
    outputLocation << std::endl << "Moving tile " + outputString << std::endl;
}

/*
This function formats the node output into a 4x4 grid. It takes the current node that it is on, and formats it into the
correct output. For ex:

-------------
| 1| 2| 3| 4|
-------------
| 6| 7|**| 8|
-------------
| 5| 9|11|12|
-------------
|13|10|14|15|
-------------

It then outputs it to the specified output location
*/
void aisearch::FormatNodeOutput(std::vector<std::vector<int>>& current, std::ostream& output)
{
    for (auto const& row : current)
    {
        int size = row.size() * 3 + 1;
        output << std::setfill('-') << std::setw(size) << "" << std::endl << std::setfill(' ');
        for (auto const& tile : row)
        {
            if (tile != 0)
                output << '|' << std::right << std::setw(2) << tile;
            else
                output << '|' << std::right << std::setw(2) << "**";
        }
        output << '|' << std::endl;
    }
    int size = current[0].size() * 3 + 1;
    output << std::setfill('-') << std::setw(size) << "" << std::endl << std::setfill(' ');
}

/*
This function gets the total path of the goal path. It takes in the cameFrom map, which returns the predecessor of that node,
and the current node that the function is on.
*/
void aisearch::GetTotalPath(std::map<std::vector<std::vector<int>>, std::vector<std::vector<int>>>& cameFrom, std::vector<std::vector<int>>& current) {
    GoalPathDistance++; //increments the goal distance
    if (current == StartingPuzzle) {
        goal_path.push_back(current);
        goal_path_direction.push_back(MovementDirection[current]);
        return;
    }
    GetTotalPath(cameFrom, cameFrom[current]);
    goal_path.push_back(current);
    goal_path_direction.push_back(MovementDirection[current]);
}

/*
This function is the heart of the entire project. This is the A* search. In this implementation, I used a priority queue to store the open
nodes. This function essentially loops through each open node, calculating F, which is the heuristic value H and the length of the current path
G from the starting node to the current node. It then uses this information to determine if the cost F *could* be in the goal path if its shorter
then the F of the parent node. It checks to see if it is in the open set of potential paths, and adds it to the open set. It uses the Manhatten distance
as the heuristic function
*/
void aisearch::AStarSearch()
{
    std::map<std::vector<std::vector<int>>, Node> fScore;
    auto compare = [&](std::vector<std::vector<int>> lhs, std::vector<std::vector<int>> rhs) //lambda function used to compare f scores in priority queue
    {
        return fScore[lhs].value > fScore[rhs].value;
    };

    std::priority_queue< std::vector<std::vector<int>>, std::vector<std::vector<std::vector<int>>>, decltype(compare)> open(compare); //priority queue that compares f scores
    std::map<std::vector<std::vector<int>>, bool> isInOpenSet; //map that helps with determining if a value is in the open set
    std::map<std::vector<std::vector<int>>, Node> gScore; //g score that is stored that calculates the weight of all the paths to the current node
    std::map<std::vector<std::vector<int>>, HeuristicNode> hScore; //heuristic score map, this is used to store the manhatten distances without looking them up more than once
    std::map<std::vector<std::vector<int>>, std::pair<int, int>> emptyPosition; //this map stores the pairs of the empty tile for fast look-up
    std::vector<std::vector<int>> current; //this is the current node being looked at
    std::vector<char> position = { 'l', 'r', 'u', 'd' }; //this is used mainly as an easy way to determine if the node can go left, right, up, or down

    open.push(StartingPuzzle); //pushes the starting node into the pq
    isInOpenSet[StartingPuzzle] = true; //sets the starting node to true
    gScore[StartingPuzzle].value = 0; //sets gscore to zero
    fScore[StartingPuzzle].value = ManhattenDistance(StartingPuzzle); //sets manhatten distance of starting puzzle
    emptyPosition[StartingPuzzle] = std::make_pair(BlankPositionRow, BlankPositionCol); //stores the empty square's location

    while (open.size() > 0) {
        current = open.top(); //gets the front of the priority queue since its value is f
        if (hScore[current].value == 0) { //will continue checking until its manhatten distance, or h score, is equal to the goal state
            GetTotalPath(cameFrom, current); //this finds the length of the path of the goal state
            return;
        }
        open.pop(); //pops the front of the queue
        isInOpenSet[current] = false; //since it is no longer on the queue, it sets current to false

        for (auto const& pos : position) //as mentioned earlier, this loops through all possible ways that the position can move: up down left and right
        {
            int row = emptyPosition[current].first; //gets the x value of the blank tile
            int col = emptyPosition[current].second;//gets the y value of the blank tile
            std::vector<std::vector<int>> neighbor = current; //stores the neighbor by getting the current value

            if (TryMovingTile(neighbor, pos, row, col)) //this function essentially moves the tile and checks to see if it can move. It sets
            {                                           //the row and column position, and creates the new neighbor based on how it moved
                emptyPosition[neighbor] = std::make_pair(row, col); //this sets the emptyPosition map to store the coordinates of the new position
                if (neighbor == cameFrom[current]) { //this is an optimizations. To avoid cycles by going to the beginning ot the loop if the neighbor
                    continue;                         //is equal to the grandfather node
                }
                int tentativeGScore = gScore[current].value + 1; //this is the tentativeGScore. It gets the g score + 1 since edge edge is weighted as 1
                if (tentativeGScore < gScore[neighbor].value) { //continue if the tentative gscore is smaller than the neighbor
                    if (isInOpenSet[neighbor] == false) { //only continues if the neighbor is not in the open set
                        cameFrom[neighbor] = current; //set cameFrom to current
                        MovementDirection[neighbor] = pos; //set direction in which it moved
                        gScore[neighbor].value = tentativeGScore; //sets the tentative gScore of the current path
                        hScore[neighbor].value = hScore[neighbor].value == -1 ? ManhattenDistance(neighbor) : hScore[neighbor].value; //sets h value
                        fScore[neighbor].value = gScore[neighbor].value + hScore[neighbor].value; //sets f value of neighbor           
                        open.push(neighbor); //pushes the neighbor onto the open set
                        TotalNodesOpened++; //increments the total number of nodes opened
                        isInOpenSet[neighbor] = true; //sets this value to true since it's on the open set
                    }
                }
            }
        }
    }
}

/*
This is the manhatten distance calculator. It determines how many tiles each tile is away from the goal tile.
It takes in the current puzzle as input, and loops through each value determining the offset
*/
int aisearch::ManhattenDistance(std::vector<std::vector<int>>& current_puzzle) {
    int TotalDistanceEstimate = 0; //stores the total distance of the manhatten
    int i = 1; //starts at 1 and goes to 16 to find offset

    for (auto current_row : current_puzzle) {
        for (auto current_tile : current_row) {
            if (current_tile == 0) { //skips if zero
                i++;
                continue;
            }

            int goalPositionRow = ((current_tile - 1) / current_puzzle.size()) + 1; //finds the row offset of goal
            int goalPositionCol = ((current_tile - 1) % current_row.size()) + 1; //finds the row offset of goal

            int currentPositionRow = ((i - 1) / current_puzzle.size()) + 1; //finds row offset of current tile
            int currentPositionCol = ((i - 1) % current_row.size()) + 1; //finds column offset of current tile

            int rowDifference = abs(currentPositionRow - goalPositionRow); //calculates the goal and current row offset
            int colDifference = abs(currentPositionCol - goalPositionCol); //calculates the goal and current column offset
            TotalDistanceEstimate += (rowDifference + colDifference); //adds the two offsets to determine the total distance offset

            i++;
        }
    }

    return TotalDistanceEstimate * 100; //returns total distance estimate by 100 for a better optimization of the distance estimate so it doesn't overestimate
}

/*
This function is primarily used to determine if a tile can be moved. It takes in the current tile, the movement direction, the row and column,
and modifies the current puzzle, row, and column if it can move in the specified TileMovement direction. It returns true or false if it can move.
*/
bool aisearch::TryMovingTile(std::vector<std::vector<int>>& current_puzzle, char TileMovement, int& PosRow, int& PosCol)
{
    int oldRow = PosRow; //stores old row
    int oldCol = PosCol; //stores old column
    //switch statement that determines movement and updates the row and column of the blank tile accordingly
    switch (TileMovement) {
    case 'l':
        PosCol--;
        break;
    case 'r':
        PosCol++;
        break;
    case 'u':
        PosRow--;
        break;
    case 'd':
        PosRow++;
        break;
    default:
        std::cout << "Error has occured" << std::endl;
        break;
    }
    //this if determines if the tile is moveable, and switches tiles if it is possible
    if ((PosCol < ((int)current_puzzle[0].size()) && PosCol >= 0) && (PosRow < ((int)current_puzzle.size()) && PosRow >= 0)) {
        auto temp = current_puzzle[PosRow][PosCol];
        current_puzzle[PosRow][PosCol] = current_puzzle[oldRow][oldCol];
        current_puzzle[oldRow][oldCol] = temp;
        return true;
    }
    //else it sets the old row and column back originally and returns false
    PosRow = oldRow;
    PosCol = oldCol;
    return false;
}