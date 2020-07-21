#include "array_tools.h"
#include "array_solution.h"
#include "matrix.h"
#include <cmath>
#include <fstream> //FILE
#include <iostream> //cout


Array_Tools::Array_Tools()
{
}


Array_Tools::~Array_Tools()
{
}


//int Array_Tools::sizeofarray(int ** array_1)
//{
//    int count = 0;
//    int count_max = sizeof(array_1) / sizeof(*array_1);

//    //for (count = 0; count < count_max; count++)
//    while (true)
//    {
//        if (array_1[0][count] == INT_MIN)
//            return count;

//        count++;
//    }
//    return count;
//}

//Array_Solution * Array_Tools::adjacents(int reference_x, int reference_y, int number_lines, int number_columns, bool diagonal_movement, bool west, bool east, bool north, bool south) /* retorna um array com os pontos adjacentes ao ponto de referencia informado */
//{
//    // Inicializando o array final
//    Array_Solution * a_adjacents = new Array_Solution(8);

//    // Contador de pontos adjacente
//    int adj_count = 0;

//    // Variavel temporaria para guardar os pontos adjacentes
//    int adjacent_x = 0,
//        adjacent_y = 0;

//    int min_hor = -1;
//    int max_hor = 1;
//    int min_ver = -1;
//    int max_ver = 1;

//    if(!west){
//        max_hor = 0;
//    }

//    if(!east){
//        min_hor = 0;
//    }

//    if(!north){
//        max_ver = 0;
//    }

//    if(!south){
//        min_ver = 0;
//    }

//    // Percorre na vertical
//    for (int i = min_ver; i <= max_ver; i++)
//    {
//        // Percorre na horizontal
//        for (int j = min_hor; j <= max_hor; j++)
//        {
//            // Valor de referencia somado ao valor do loop
//            adjacent_x = reference_x + j;
//            adjacent_y = reference_y + i;

//            // Se está dentro dos limites da matriz e nao eh o proprio ponto de referencia
//            if (adjacent_x >= 0 && adjacent_x < number_columns && adjacent_y >= 0 && adjacent_y < number_lines
//                    && !(adjacent_x == reference_x && adjacent_y == reference_y)) {
//                // Se o movimento na diagonal nao eh permitido
//                if ((!diagonal_movement &&
//                    (((i == 0) || (j == 0)) && (i != j))) ||
//                    // Se o movimento na diagonal eh permitido
//                    (diagonal_movement)
//                    ) {

//                    // Insere o ponto na matriz final
//                    Solution_Pair e(adjacent_x, adjacent_y);
//                    a_adjacents->add_solution_pair(e);//a_adjacents->set_solution_pair(e, adj_count);

//                    // Incrementa o contador
//                    adj_count++;
//                }
//            }
//        }
//    }

//    a_adjacents->set_size(adj_count);

//    return a_adjacents;
//}

//Array_Solution * Array_Tools::adjacents(int reference_x, int reference_y, int number_lines, int number_columns, bool diagonal_movement) /* retorna um array com os pontos adjacentes ao ponto de referencia informado */
//{
//    return adjacents(reference_x, reference_y, number_lines, number_columns, diagonal_movement, true, true, true, true);
//}

vector<Stop_Point> Array_Tools::adjacents(Stop_Point position, Matrix map, bool diagonal_movement) /* retorna um array com os pontos adjacentes ao ponto de referencia informado */
{
    // Inicializando o array final
    vector<Stop_Point> adjacents;

    // Variavel temporaria para guardar os pontos adjacentes
    int adjacent_x = 0,
        adjacent_y = 0;

    int min_hor = -1;
    int max_hor = 1;
    int min_ver = -1;
    int max_ver = 1;

    int number_columns = map.get_cols();
    int number_lines   = map.get_rows();

    int reference_x = position.get_x();
    int reference_y = position.get_y();


    // Percorre na vertical
    for (int i = min_ver; i <= max_ver; i++)
    {
        // Percorre na horizontal
        for (int j = min_hor; j <= max_hor; j++)
        {
            // Valor de referencia somado ao valor do loop
            adjacent_x = position.get_x() + j;
            adjacent_y = position.get_y() + i;

            // Se está dentro dos limites da matriz e nao eh o proprio ponto de referencia
            if (adjacent_x >= 0 && adjacent_x < number_columns && adjacent_y >= 0 && adjacent_y < number_lines
                    && !(adjacent_x == reference_x && adjacent_y == reference_y)) {
                // Se o movimento na diagonal nao eh permitido
                if ((!diagonal_movement &&
                    (((i == 0) || (j == 0)) && (i != j))) ||
                    // Se o movimento na diagonal eh permitido
                    (diagonal_movement)
                    ) {

                    // Insere o ponto na matriz final
                    adjacents.push_back(map.get_stop_point(adjacent_x, adjacent_y));
                }
            }
        }
    }

    return adjacents;
}

bool Array_Tools::arraycontainspoint(int reference_x, int reference_y, int ** points, int array_size) /* verifica se array de pares ordenados possui determinado ponto */
{
    // Percorre todo array
    for (int i = 0; i < array_size; i++)
    {
        // Compara o ponto de referencia com os pares ordenados do array
        if (reference_x == points[0][i] && reference_y == points[1][i]) {
            return true;
        }
    }
    return false;
}

int ** Array_Tools::intersection(int ** array_1, int ** array_2, int size_1, int size_2)
{
    int biggest_size = size_1 > size_2 ? size_1 : size_2;

    int** intersection = new int*[2];
    intersection[0] = new int[biggest_size];
    intersection[1] = new int[biggest_size];

    int reference_x = 0,
        reference_y = 0;

    int count = 0;

    for (int i = 0; i < size_1; i++)
    {
        reference_x = array_1[0][i];
        reference_y = array_1[1][i];

        for (int j = 0; j < size_2; j++)
        {
            if (reference_x == array_2[0][j] && reference_y == array_2[1][j]) {
                intersection[0][count] = reference_x;
                intersection[1][count] = reference_y;

                count++;
                break;
            }
        }
    }

    return intersection;
}

int Array_Tools::distance(int x_1, int y_1, int x_2, int y_2)
{
    return abs(x_2 - x_1) + abs(y_2 - y_1);
}

int Array_Tools::distance(Solution_Pair p_1, Solution_Pair p_2)
{
    return abs(p_2.get_x() - p_1.get_x()) + abs(p_2.get_y() - p_1.get_y());
}

bool Array_Tools::are_adjacents(int x_1, int y_1, int x_2, int y_2)
{
    return abs(x_2 - x_1) <= 1 && abs(y_2 - y_1) <= 1;
}

bool Array_Tools::are_adjacents(Solution_Pair p_1, Solution_Pair p_2)
{
    return abs(p_2.get_x() - p_1.get_x()) <= 1 && abs(p_2.get_y() - p_1.get_y()) <= 1;
}

Array_Solution Array_Tools::from_file_solution(string file_name) /* returns an array from file */ // pointer
{

    ifstream array_file(file_name);

    if ( !array_file){
        cout << "File not found" << endl;
    }else{
        int iteration, qty_charge, qty_collect, qty_deliver, num_objectives, size, valid;
        double z;
//        string valid;

        array_file >> size;
        array_file >> num_objectives;

        // Inicializando o array final
        Array_Solution solution(size, num_objectives);

        //Iteration
        array_file >> iteration;
        solution.set_iteration(iteration);

        //Valid
        array_file >> valid;
        if(valid == 1){
            solution.set_valid(true);
        } else {
            solution.set_valid(false);
        }

        //Qty Charge
        array_file >> qty_charge;
        solution.set_qty_charge(qty_charge);

        //Qty Collect
        array_file >> qty_collect;
        solution.set_qty_collect(qty_collect);

        //Qty Deliver
        array_file >> qty_deliver;
        solution.set_qty_deliver(qty_deliver);


        //Objectives
        int i;
        for (i = 0; i < num_objectives; i++) {
            array_file >> z;
            solution.set_z(i, z);
        }


        array_file.close();

        size_t found = file_name.find_last_of("/");
        string solution_name = file_name.substr(found, file_name.size() - 1);

        if(solution_name.compare("/routes") != 0){
            file_name.erase (found, file_name.size() - 1);

            string route_file_name = file_name;
            route_file_name.append("/routes");

            //Routes
            for (int i = 0; i < size; i++) {
                string route_name_temp = route_file_name;
                route_name_temp.append(solution_name);

                route_name_temp.append("_route_");
                route_name_temp.append(std::to_string(i));

                Route route = from_file_route(route_name_temp);
                solution.add_route(route);
            }

            return solution;
        }
    }

    return Array_Solution(true);
}

Route Array_Tools::from_file_route(string file_name) /* returns an array from file */ // pointer
{

    ifstream array_file(file_name);

    if ( !array_file ){
        cout << "File not found" << endl;
    }else{
        int iteration, qty_charge, qty_collect, qty_deliver, num_objectives, size, valid;
        double z;
//        string valid;

        array_file >> size;
        array_file >> num_objectives;

        // Inicializando o array final
        Route route(num_objectives);

        //Iteration
        array_file >> iteration;
        route.set_iteration(iteration);

        //Valid
        array_file >> valid;
        if(valid == 1){
            route.set_valid(true);
        } else {
            route.set_valid(false);
        }

        //Qty Charge
        array_file >> qty_charge;
        route.set_qty_charge(qty_charge);

        //Qty Collect
        array_file >> qty_collect;
        route.set_qty_collect(qty_collect);

        //Qty Deliver
        array_file >> qty_deliver;
        route.set_qty_deliver(qty_deliver);

        //Objectives
        int i;
        for (i = 0; i < num_objectives; i++) {
            array_file >> z;
            route.set_z(i, z);
        }


        //Solution pairs
        int x, y, v, c;
        double b;
        Solution_Pair s_p;
        for (i = 0; i < size; i++) {
            array_file >> x;
            array_file >> y;
            array_file >> v;
            array_file >> c;
            array_file >> b;

            s_p = Solution_Pair(x, y, v, c, b);

            route.add_solution_pair(s_p);
        }


        array_file.close();

        return route;
    }

    return Route();
}

void Array_Tools::UAV_from_file_solution(Solution * solution, string file_name) /* returns an array from file */
{

    ifstream array_file(file_name);

    if ( !array_file){
        cout << "File not found" << endl;
    }else{
        int iteration, qty_charge, qty_collect, qty_deliver, num_objectives, size, valid;
        double z;

        array_file >> size;
        array_file >> num_objectives;
        solution->num_objectives = num_objectives;

        //Iteration
        array_file >> iteration;
        solution->iteration = iteration;

        //Valid
        array_file >> valid;
        if(valid == 1){
            solution->valid = true;
        } else {
            solution->valid = false;
        }


        //Objectives
        int i;
        for (i = 0; i < solution->num_objectives; i++) {
            array_file >> z;
            solution->objectives[i] = z;
        }


        array_file.close();

        size_t found = file_name.find_last_of("/");
        string solution_name = file_name.substr(found, file_name.size() - 1);

        if(solution_name.compare("/routes") != 0){
            file_name.erase (found, file_name.size() - 1);

            string route_file_name = file_name;
            route_file_name.append("/routes");

            //Routes
            for (int i = 0; i < size; i++) {
                string route_name_temp = route_file_name;
                route_name_temp.append(solution_name);

                route_name_temp.append("_route_");
                route_name_temp.append(std::to_string(i));

                UAV * route = new UAV;
                UAV_init_uav(route);

                UAV_from_file_uav(route, route_name_temp);

                UAV_add_uav(solution, route);
            }
        }
    }
}

void Array_Tools::UAV_from_file_uav     (UAV * route, string file_name) /* returns an array from file */ // pointer
{

    ifstream array_file(file_name);

    if ( !array_file ){
        cout << "File not found" << endl;
    }else{
        int iteration, num_objectives, size, valid;
        double z;
//        string valid;

        array_file >> size;
        array_file >> num_objectives;

        //Valid
        array_file >> valid;
        if(valid == 1){
            route->valid = true;
        } else {
            route->valid = false;
        }

        //Objectives
        int i;
        for (i = 0; i < num_objectives; i++) {
            array_file >> z;
            route->objectives[i] = z;
        }

        //Solution pairs
        int x, y, c;
        double v, b;
        for (i = 0; i < size; i++) {
            array_file >> x;
            array_file >> y;
            array_file >> v;
            array_file >> c;
            array_file >> b;

            UAV_push_movement(x, y, v, c, b, route);
        }

        array_file.close();
    }
}
