#ifndef PATTERN_TABLE_H
#define PATTERN_TABLE_H

#include "solution_pair.h"
#include "array_solution.h"
#include <string>
#include <vector>

using namespace std;

class Pattern_Table
{
//    int points_size;
//    int routes_size;

    vector<Solution_Pair> points;
    vector<int>           routes;

public:
    Pattern_Table();
    ~Pattern_Table();

    //START: get/set

    int get_routes_size();
    int get_points_size();

    Solution_Pair get_point(int pos);
    int           get_route(int pos);

    //END: get/set

    void add_point    (int x, int y);
    void add_point    (int x, int y, int v);
    void add_point    (Solution_Pair p);
    void add_route    (int route);
    void print        ();
    void print_points ();
    void resize_routes(int s);    
    void resize_points(int s);
    void copy         (Pattern_Table other);
    void insert_union (Pattern_Table table1, Pattern_Table table2);

    bool can_join          (Pattern_Table other);
    bool can_join_sequence (Pattern_Table other);
    bool equals_but        (Pattern_Table other, int index);
    bool is_subsequence    (Pattern_Table other);

    string to_string();
};




#endif // PATTERN_TABLE_H
