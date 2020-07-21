#ifndef ARRAY_TOOLS_H
#define ARRAY_TOOLS_H

#include "array_solution.h"
#include "solution_pair.h"
#include "matrix.h"
#include "uav.h"

using namespace std;

class Array_Tools
{
public:
    Array_Tools();
    ~Array_Tools();

//    static int			  sizeofarray(int** array_1);
    static int*			    concatarrays   (int a1[], int a2[]);
    static int*			    concatarrayint (int a1[], int x);

    static int**		    intersection (int** array_1, int** array_2, int size_1, int size_2);

    static Array_Solution     adjacents (int reference_x, int reference_y, int number_lines, int number_columns, bool diagonal_movement, bool west, bool east, bool north, bool south); // pointer
    static Array_Solution     adjacents (int reference_x, int reference_y, int number_lines, int number_columns, bool diagonal_movement); // pointer
    static vector<Stop_Point> adjacents (Stop_Point position, Matrix map, bool diagonal_movement);

    static Array_Solution   from_file_solution (string file_name); // pointer
    static Route            from_file_route    (string file_name); // pointer

    static void             UAV_from_file_uav      (UAV * route, string file_name)        ;
    static void             UAV_from_file_solution (Solution * solution, string file_name);

    static bool			    arraycontainspoint (int reference_x, int reference_y, int** points, int array_size);
    static bool			    are_adjacents      (int x_1, int y_1, int x_2, int y_2);
    static bool			    are_adjacents      (Solution_Pair p_1, Solution_Pair p_2);

    static int              distance (int x_1, int y_1, int x_2, int y_2);
    static int              distance (Solution_Pair p_1, Solution_Pair p_2);
};



#endif // ARRAY_TOOLS_H
