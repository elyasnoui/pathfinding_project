#include <iostream>
#include <fstream>
#include <xstring>
#include <set>
#include <stack>

using namespace std;

typedef pair<int, int> Pair;
typedef pair<double, pair<int, int>> pPair;
typedef char** Grid;

int ROW, COLUMN;
Pair ORIGIN;
Pair DESTINATION;

struct node {
    int parent_i, parent_j;
    double f_cost, g_cost, h_cost;
};

// Checking if source or destination is within grid bounds
bool isInBounds(const int& curr_row, const int& curr_column) {
    return (curr_row >= 0) && (curr_row < ROW) &&
           (curr_column >= 0) && (curr_column < COLUMN);
}

bool wallCheck(const Grid& grid, const int& row, const int& column) {
    if (grid[row][column] != 'x') return true;
    else return false;
}

bool spaceCheck(const Grid& grid, const int& row, const int& column) {
    if (grid[row][column] != '.') return false;
    else return true;
}

bool isDestination(const int& row, const int& column, const Pair& destination) {
    if (row == destination.first && column == destination.second) return true;
    else return false;
}

double calcHueristic(const int& row, const int& column, const Pair& destination) {
    return ((double)sqrt((row - destination.first) * (ROW - destination.first)
                         + (column - destination.second) * (column - destination.second)));
}

void displayPath(node**& pNode, const Pair& destination, const string& fileName) {
    int row = destination.first;
    int column = destination.second;

    // Reverses order as they pop, since the originally traced from destination back to origin
    stack<Pair> path;

    int temp_row, temp_column;
    while (!(pNode[row][column].parent_i == row && pNode[row][column].parent_j == column)) {
        path.push(make_pair(row, column));
        temp_row = pNode[row][column].parent_i;
        temp_column = pNode[row][column].parent_j;
        row = temp_row;
        column = temp_column;
    }

    path.push(make_pair(row, column));
    string route = "";
    while (!path.empty()) {
        pair<int,int> p1 = path.top();
        path.pop();

        if (!path.empty()) {
            pair<int,int> p2 = path.top();

            // Determining directions
            if ((p1.first == (p2.first - 1)) && (p1.second == p2.second))
                route.append("S");
            else if ((p1.first == (p2.first +1)) && (p1.second == p2.second))
                route.append("N");
            else if ((p1.first == (p2.first)) && (p1.second == p2.second+1))
                route.append("W");
            else if ((p1.first == (p2.first)) && (p1.second == p2.second-1))
                route.append("E");
        }

        if (p1.first == destination.first && p1.second == destination.second)
            break;
    }
    cout << fileName << ": " << route << endl;
}

void aStarPath(const Grid& grid, const Pair& origin, const Pair& destination, const string& fileName) {
    if (!isInBounds(origin.first, origin.second) ||
        !isInBounds(destination.first, destination.second)) {
        cout << "Source/Destination is out of bounds" << endl;
        return;
    }

    if (!wallCheck(grid, origin.first, origin.second) ||
        !wallCheck(grid, destination.first, destination.second)) {
        cout << "Source or the destination is blocked" << endl;
        return;
    }

    if (isDestination(origin.first, origin.second, destination)) return;

    bool** closedList = new bool*[ROW];
    node** openList = new node*[ROW];

    for (int i=0; i<ROW; i++) {
        closedList[i] = new bool [COLUMN];
        openList[i] = new node[COLUMN];
    }

    int i, j;
    for (i=0; i<ROW; i++)
        for(j=0; j<COLUMN; j++) {
            closedList[i][j] = false;
            openList[i][j].f_cost = -1;
            openList[i][j].g_cost = -1;
            openList[i][j].h_cost = -1;
            openList[i][j].parent_i = -1;
            openList[i][j].parent_j = -1;
        }

    i = origin.first, j = origin.second;
    openList[i][j].f_cost = 0.0;
    openList[i][j].g_cost = 0.0;
    openList[i][j].h_cost = 0.0;
    openList[i][j].parent_i = i;
    openList[i][j].parent_j = j;

    set<pPair> pSet;
    set<pPair>::iterator itr;
    pSet.insert(make_pair(0.0, make_pair(i, j)));
    double minimum = LONG_MAX;
    pPair p;

    while (!pSet.empty()) {
        minimum = (*pSet.begin()).first;
        p = *pSet.begin();

        for (itr = pSet.begin(); itr != pSet.end(); ++itr)
            if ((*itr).first < minimum)
                p = *itr;
        pSet.erase(p);

        i = p.second.first, j = p.second.second;
        closedList[i][j] = true;

        double f_cost_new, g_cost_new, h_cost_new;

        // Checking NORTH
        if (isInBounds(i, j+1)) {
            // If the destination cell is the same as the current successor
            if (isDestination(i, j+1, destination)) {

                // Set the Parent of the destination cell
                openList[i][j+1].parent_i = i;
                openList[i][j+1].parent_j = j;

                displayPath(openList, destination, fileName);
                return;
            }
            else if (!closedList[i][j+1] && wallCheck(grid, i, j+1) && spaceCheck(grid, i, j+1)) {
                g_cost_new = openList[i][j].g_cost + 1.0;
                h_cost_new = calcHueristic(i, j+1, destination);
                f_cost_new = g_cost_new + h_cost_new;
                if (openList[i][j+1].f_cost == -1 || openList[i][j+1].f_cost > f_cost_new) {
                    pSet.insert(make_pair(f_cost_new,
                                          make_pair(i, j + 1)));

                    // Update the details of this cell
                    openList[i][j+1].f_cost = f_cost_new;
                    openList[i][j+1].g_cost = g_cost_new;
                    openList[i][j+1].h_cost = h_cost_new;
                    openList[i][j+1].parent_i = i;
                    openList[i][j+1].parent_j = j;
                }
            }
        }

        // Checking EAST
        if (isInBounds(i+1, j)) {
            // If the destination node is the same as the current successor
            if (isDestination(i+1, j, destination)) {

                // Set the Parent of the destination cell
                openList[i+1][j].parent_i = i;
                openList[i+1][j].parent_j = j;

                displayPath(openList, destination, fileName);
                return;
            }

            else if (!closedList[i+1][j] && wallCheck(grid, i+1, j) && spaceCheck(grid, i+1, j)) {
                g_cost_new = openList[i][j].g_cost + 1.0;
                h_cost_new = calcHueristic(i+1, j, destination);
                f_cost_new = g_cost_new + h_cost_new;

                if (openList[i+1][j].f_cost == -1 || openList[i+1][j].f_cost > f_cost_new) {
                    pSet.insert(make_pair(f_cost_new, make_pair(i+1, j)));
                    openList[i+1][j].f_cost = f_cost_new;
                    openList[i+1][j].g_cost = g_cost_new;
                    openList[i+1][j].h_cost = h_cost_new;
                    openList[i+1][j].parent_i = i;
                    openList[i+1][j].parent_j = j;
                }
            }
        }

        // Checking SOUTH
        if (isInBounds(i, j-1)) {

            // Check to see if we've arrived at the destination
            if (isDestination(i, j - 1, destination)) {

                // Set the Parent of the destination node
                openList[i][j-1].parent_i = i;
                openList[i][j-1].parent_j = j;

                displayPath(openList, destination, fileName);
                return;
            }

                // Not arrived at destination
            else if (!closedList[i][j-1] && wallCheck(grid, i, j-1) && spaceCheck(grid, i, j-1)) {
                g_cost_new = openList[i][j].g_cost + 1.0;
                h_cost_new = calcHueristic(i, j-1, destination);
                f_cost_new = g_cost_new + h_cost_new;

                if (openList[i][j - 1].f_cost == -1 || openList[i][j - 1].f_cost > f_cost_new) {
                    pSet.insert(make_pair(f_cost_new,
                                          make_pair(i, j - 1)));

                    // Updating list with new costs and position
                    openList[i][j-1].f_cost = f_cost_new;
                    openList[i][j-1].g_cost = g_cost_new;
                    openList[i][j-1].h_cost = h_cost_new;
                    openList[i][j-1].parent_i = i;
                    openList[i][j-1].parent_j = j;
                }
            }
        }

        // Checking WEST
        if (isInBounds(i-1, j)) {
            if (isDestination(i-1, j, destination)) {
                openList[i-1][j].parent_i = i;
                openList[i-1][j].parent_j = j;

                displayPath(openList, destination, fileName);
                return;
            }


            else if (!closedList[i-1][j] && wallCheck(grid, i-1, j) && spaceCheck(grid, i-1, j)) {
                g_cost_new = openList[i][j].g_cost + 1.0;
                h_cost_new = calcHueristic(i-1, j, destination);
                f_cost_new = g_cost_new + h_cost_new;

                /*
                If it isn’t on the open list, add it to
                the open list. Make the current square
                the parent of this square. Record the
                f_cost, g_cost, and h_cost of the node
                                 OR
                If it is on the open list already, check
                to see if this path to that square is better,
                using f_cost.
                */

                if (openList[i - 1][j].f_cost == -1 || openList[i - 1][j].f_cost > f_cost_new) {
                    pSet.insert(make_pair(f_cost_new,
                                          make_pair(i - 1, j)));

                    // Updating list with new costs and position
                    openList[i-1][j].f_cost = f_cost_new;
                    openList[i-1][j].g_cost = g_cost_new;
                    openList[i-1][j].h_cost = h_cost_new;
                    openList[i-1][j].parent_i = i;
                    openList[i-1][j].parent_j = j;
                }
            }
        }
    }
    cout << fileName << ": There is no valid route to the destination" << endl;
}

Grid fileToMap(const string& fileName) {
    ifstream file(fileName);
    string line;
    int a_x, a_y, b_x, b_y;
    int y_index = 0;

    // Find origin and destination
    while (!file.eof()) {
        file >> line;

        if (line.find('A') != string::npos) {
            a_x = line.find('A');
            a_y = y_index;
        } if (line.find('B') != string::npos) {
            b_x = line.find('B');
            b_y = y_index;
        }
        y_index++;
    }

    ROW = y_index;
    COLUMN = line.length();
    ORIGIN = make_pair(a_y, a_x);
    DESTINATION = make_pair(b_y, b_x);

    file.close();
    file.open(fileName);

    Grid grid = new char*[ROW];

    for (int i=0; i<ROW; i++)
        grid[i] = new char[COLUMN];

    for (int i=0; i<ROW; i++)
        for (int j=0; j<COLUMN; j++)
            file >> grid[i][j];

    return grid;
}

int main() {
    // Map 1
    string file1 = "quickest_route_1.txt";
    Grid grid1 = fileToMap(file1);
    aStarPath(grid1, ORIGIN, DESTINATION, file1);

    // Map 2
    string file2 = "quickest_route_2.txt";
    Grid grid2 = fileToMap(file2);
    aStarPath(grid2, ORIGIN, DESTINATION, file2);

    // Map 3
    string file3 = "quickest_route_3.txt";
    Grid grid3 = fileToMap(file3);
    aStarPath(grid3, ORIGIN, DESTINATION, file3);

    // Map 4
    string file4 = "quickest_route_4.txt";
    Grid grid4 = fileToMap(file4);
    aStarPath(grid4, ORIGIN, DESTINATION, file4);

    cout << endl;

    system("pause");

    return 0;
}