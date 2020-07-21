#include "core.h"
#include "matrix.h"
#include "array_solution.h"
#include "array_tools.h"
#include "mainwindow.h"
#include "dirent.h"
#include "array_objective_solution.h"
#include <iostream>
#include <ctime> //random
#include <chrono>
#include "hypervolume.h"
#include "neural_network.h"

// MKDIR
#include <bits/stdc++.h>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>

// Verify if directory exists
#include <sys/types.h>
#include <sys/stat.h>


//namespace fixedValues {
//          enum STOPS { NOTHING = 0, DELIVER = 1, COLLECT = 2, CHARGE = 3 }; // tipos de parada no mapa
//    const int  POINTS_BY_STOP_POINT[] = { 0, 10, 10, 2 }; // qtd de pontos de parada para cada tipo: 0 - sem valor; 1 - DELIVER; 2 - COLLECT; 3 - CHARGE
//}

Core::Core()
{    
//     a_solutions       = new Array_Solution *[NUMBER_ITERATIONS];
//    a_solutions.resize(NUMBER_ITERATIONS);
    pool_of_solutions = Pool_of_Solutions(20);
    p_temp            = Pool_of_Solutions(NUMBER_ITERATIONS);

    NEIGHBORHOOD_QTY_VEC.assign(NEIGHBORHOOD_QTY, 0);
    NEIGHBORHOOD_TIME.assign   (NEIGHBORHOOD_QTY, 0);
    NEIGHBORHOOD_ADDED.assign  (NEIGHBORHOOD_QTY, 0);
}

using namespace std;
using namespace fixedValues;

void Core::set_max_time			   (int m_t){
    MAX_TIME			   = m_t;
}

void Core::set_time_interval		   (int t_i){
    TIME_INTERVAL		   = t_i;
}

void Core::set_matrix_number_columns (int n_c){
    MATRIX_NUMBER_COLUMNS  = n_c;
}

void Core::set_matrix_number_lines   (int n_l){
    MATRIX_NUMBER_LINES    = n_l;
}

void Core::set_battery_recharge      (int b_r){
    BATTERY_RECHARGE       = b_r;
}

void Core::set_max_battery_charge    (int m_b){
    MAX_BATTERY_CHARGE     = m_b;
}

void Core::set_initial_battery_charge(int i_b){
    initial_battery_charge = i_b;
}

void Core::set_consumption_battery   (int c_b){
    CONSUMPTION_BATTERY    = c_b;
}

void Core::set_qty_points            (int q_p){
//    QTY_POINTS[]           = q_p;
}

void Core::set_number_iterations     (int n_i){
    NUMBER_ITERATIONS      = n_i;
}

int Core::get_number_iterations     (){
    return NUMBER_ITERATIONS;
}

vector<Array_Solution> Core::get_solutions(){
    return a_solutions;
}

Pool_of_Solutions Core::get_pool_of_solutions(){
    return pool_of_solutions;
}

Matrix * Core::get_map(){
    return m_map;
}

Matrix * Core::initialize_mtrx(Matrix * m) /* generates a matrix with random positions for the stopping points */
{
    // Initializing the array that represents the map
    m = new Matrix(MATRIX_NUMBER_LINES, MATRIX_NUMBER_COLUMNS);

    // Fills the entire array with '0'
    m->initialize();

    int qty_deliver    = 0, // 1 - Variables that will be incremented as the stopping points are instantiated
        qty_collect    = 0,
        qty_charge     = 0,
        qty_prohibited = 0,

        random_number = 0, // 2 - Temporary variable that receives a random value along the loop

        total_points = m->get_size(), // 3 - Qty of points in the map

        parameter_deliver    = QTY_POINTS[STOPS::DELIVER], // 4 - Percentage relative to stop points qty referenced within entire map
        parameter_collect    = QTY_POINTS[STOPS::COLLECT],
        parameter_charge     = QTY_POINTS[STOPS::CHARGE],
        parameter_prohibited = QTY_POINTS[STOPS::PROHIBITED];

    // While all stop points are not instantiated
    while (qty_deliver    != QTY_POINTS[STOPS::DELIVER] ||
           qty_collect    != QTY_POINTS[STOPS::COLLECT] ||
           qty_charge     != QTY_POINTS[STOPS::CHARGE]  ||
           qty_prohibited != QTY_POINTS[STOPS::PROHIBITED]) {

        // Run through the whole grid
        for (int i = 0; i < total_points; i++) {
            if (m->get_stop_point(i).equals(STOPS::NOTHING)) {
                random_number = rand() % total_points; // Gera um valor aleatorio entre 0 e 100

                // If the number is less than the parameter of the DELIVERY point and has not yet put on the map the total qty of these points
                if (random_number < parameter_deliver &&
                    (qty_deliver != QTY_POINTS[STOPS::DELIVER])) {
                    m->set_stop_point(i,STOPS::DELIVER);
                    qty_deliver++;

                    Solution_Pair * s = new Solution_Pair(m->get_stop_point(i).get_x(), m->get_stop_point(i).get_y());
                    client_points.push_back(s->copy());
                    delete s;
                }
                // If the number is less than the parameter of the COLLECT point and has not yet put on the map the total qty of these points
                else if (random_number < parameter_deliver + parameter_collect &&
                    (qty_collect != QTY_POINTS[STOPS::COLLECT]))
                {
                    m->set_stop_point(i,STOPS::COLLECT);
                    qty_collect++;

                    Solution_Pair * s = new Solution_Pair(m->get_stop_point(i).get_x(), m->get_stop_point(i).get_y());
                    client_points.push_back(s->copy());
                    delete s;
                }
                // If the number is less than the parameter of the RECHARGE point and has not yet put on the map the total qty of these points
                else if (random_number < parameter_deliver + parameter_collect + parameter_charge &&
                    (qty_charge != QTY_POINTS[STOPS::CHARGE]))
                {
                    m->set_stop_point(i,STOPS::CHARGE);
                    qty_charge++;

                    Solution_Pair * s = new Solution_Pair(m->get_stop_point(i).get_x(), m->get_stop_point(i).get_y());
                    recharge_points.push_back(s->copy());
                    delete s;
                }
                // If the number is less than the parameter of the PROHIBITED point and has not yet put on the map the total qty of these points
                else if (random_number < parameter_deliver + parameter_collect + parameter_charge + parameter_prohibited &&
                    (qty_prohibited != QTY_POINTS[STOPS::PROHIBITED]) &&
                    // Not origin
                    (m->get_stop_point(i).get_x() != origin_x && m->get_stop_point(i).get_y() != origin_y))
                {
                    m->set_stop_point(i,STOPS::PROHIBITED);
                    qty_prohibited++;
                }
            }
        }
    }

    return m;
}

Matrix * Core::initialize_mtrx2(Matrix * m, string instanceName, string clientsVisitedFileName) /* generates a matrix with random positions for the stopping points */
{    
    ifstream inTSP1(instanceName.c_str());

    if ( !inTSP1 ){
      cout << "File not found" << endl;
      exit(1);
    }

    string temp;
    std::getline(inTSP1, temp); // Line
    temp.erase (std::remove(temp.begin(), temp.end(), ' '), temp.end());
    MATRIX_NUMBER_COLUMNS = temp.length();
    MATRIX_NUMBER_LINES = 0;

    do{
        MATRIX_NUMBER_LINES++;
    } while(std::getline(inTSP1, temp));

    inTSP1.close();

    ifstream inTSP(instanceName.c_str());

    string file;
    int x = 0, d = 0, co = 0, ch = 0, p = 0;

    // Initializing the array that represents the map
    m = new Matrix(MATRIX_NUMBER_LINES, MATRIX_NUMBER_COLUMNS);

    // Fills the entire array with '0'
    m->initialize();

    // Initializing vectors
    client_points.clear();
    recharge_points.clear();
    clients_visited.clear();

    // Inicializa Matriz
    for ( int i = 0; i < m->get_size(); i++ ) {
        inTSP >> x;

        m->set_stop_point(i, x);

        if(x == STOPS::DELIVER){
            Solution_Pair * s = new Solution_Pair(m->get_stop_point(i).get_x(), m->get_stop_point(i).get_y());
            client_points.push_back(s->copy());
            d++;
            delete s;
        }else if(x == STOPS::COLLECT){
            Solution_Pair * s = new Solution_Pair(m->get_stop_point(i).get_x(), m->get_stop_point(i).get_y());
            client_points.push_back(s->copy());
            co++;
            delete s;
        } else if(x == STOPS::CHARGE){
            Solution_Pair * s = new Solution_Pair(m->get_stop_point(i).get_x(), m->get_stop_point(i).get_y());
            recharge_points.push_back(s->copy());
            ch++;
            delete s;
        } else if(x == STOPS::PROHIBITED){
            p++;
        }
    }

    ifstream clientsVisitedFile(clientsVisitedFileName.c_str());

    if ( !clientsVisitedFile ){
        int qtd_clients = (int) d * pct_clients_visited / 100;
        int client_visited;

        while(clients_visited.size() < qtd_clients){
            client_visited = rand() % client_points.size();
            vector<int> client = {client_points[client_visited].get_x(), client_points[client_visited].get_y()};
            clients_visited.push_back(client);

            client_points.erase(client_points.begin() + client_visited);
            d--;

            m->set_stop_point(client[0], client[1], 0);
        }

        //Write clients positions on file
        ofstream out_file;
        out_file.open (clientsVisitedFileName);

        //Write the number of clients visited
        out_file << clients_visited.size() << "\n";

        for (int i = 0; i < clients_visited.size(); i++) {
            out_file << clients_visited[i][0] << " " << clients_visited[i][1] << "\n";
        }

        out_file.close();
    } else {
        int s, x, y;
        clientsVisitedFile >> s; //Dimension

        for ( int i = 0; i < s; i++ ) {
            clientsVisitedFile >> x;
            clientsVisitedFile >> y;

            vector<int> client = {x, y};
            clients_visited.push_back(client);

            m->set_stop_point(client[0], client[1], 0);

            for (int j = 0; j < client_points.size(); j++) {
                if(client[0] == client_points[j].get_x() && client[1] == client_points[j].get_y()){
                    client_points.erase(client_points.begin() + j);
                    d--;
                    break;
                }
            }
        }

    }

    clientsVisitedFile.close();

    QTY_POINTS[STOPS::DELIVER] = d;
    QTY_POINTS[STOPS::COLLECT] = co;
    QTY_POINTS[STOPS::CHARGE] = ch;
    QTY_POINTS[STOPS::PROHIBITED] = p;

    inTSP.close();

    return m;
}

void Core::set_origin_point(Matrix * m, string originFileName) /* set origin point of the drones */
{
    ifstream originFile(originFileName.c_str());

    if ( originFile ){

        originFile >> origin_x;
        originFile >> origin_y;

        originFile.close();

    } else {

        Stop_Point origin_point;
        int x;

        do{
            x = rand() % m->get_size();
            origin_point = m->get_stop_point(x);
        }while(origin_point.get_type() == STOPS::PROHIBITED);

        origin_x = origin_point.get_x();
        origin_y = origin_point.get_y();

        //Write origin position on file
        ofstream out_file;
        out_file.open (originFileName);

        out_file << origin_x << "\n";
        out_file << origin_y << "\n";

        out_file.close();
    }
}

void Core::get_clients_visited(string instanceName, string clientsVisitedFileName) /* generates a matrix with random positions for the stopping points */
{

//   ifstream clientsVisitedFile(instanceName.c_str());

//   if ( !clientsVisitedFile ){
//     cout << "File not found" << endl;
//     exit(1);
//   }

//   string temp;
//   std::getline(clientsVisitedFile, temp); // Names
//   std::getline(clientsVisitedFile, temp); // Comment
//   std::getline(clientsVisitedFile, temp); // Type

//   clientsVisitedFile >> temp; //Dimension
//   clientsVisitedFile >> temp; // :

//   int dim, x, y;
//   clientsVisitedFile >> dim; //Dimension

//   std::getline(clientsVisitedFile, temp);
//   std::getline(clientsVisitedFile, temp); // Edge Type
//   std::getline(clientsVisitedFile, temp); // Name Coord

//   vector<int> clients;

//   vector<int> clients_x;
//   vector<int> clients_y;

//   // Inicializa Matriz
//   for ( int i = 0; i < dim; i++ ) {
//       clientsVisitedFile >> x;
//       clients.push_back(x);

//       clientsVisitedFile >> x;
//       clients_x.push_back(x);

//       clientsVisitedFile >> y;
//       clients_y.push_back(y);
//   }

//   int qtd_clients = (int) dim * pct_clients_visited / 100;
//   int client_visited;

//   while(clients_visited.size() < qtd_clients){
//       client_visited = rand() % clients.size();
//       clients_visited.push_back(clients[client_visited]);

//       clients.erase(clients.begin() + client_visited);
//   }

//   clientsVisitedFile.close();


//   //Write clients positions on file
//   ofstream out_file;
//   out_file.open (clientsVisitedFileName);

//   //Write the number of clients visited
//   out_file << clients_visited.size() << "\n";

//   for (int i = 0; i < clients_visited.size(); i++) {
//       out_file << clients_visited[i] << " " << clients_x[clients_visited[i]] << " " << clients_y[clients_visited[i]] << "\n";
//   }

//   out_file.close();

}

Matrix * Core::initialize_mtrx_load_test_files(Matrix * m, string instanceName, int pct_prohibited = 0) /* generates a matrix with random positions for the stopping points */
{
    ifstream gridFile(instanceName.c_str());

    if ( !gridFile ){
      cout << "File not found" << endl;
      exit(1);
    }

    string temp;
    std::getline(gridFile, temp); // Names
    std::getline(gridFile, temp); // Comment
    std::getline(gridFile, temp); // Type

    gridFile >> temp; //Dimension
    gridFile >> temp; // :

    int dim, x, y, maxX = INT_MIN, maxY = INT_MIN, minX = INT_MAX, minY = INT_MAX;
    gridFile >> dim; //Dimension

    std::getline(gridFile, temp);
    std::getline(gridFile, temp); // Edge Type
    std::getline(gridFile, temp); // Name Coord

    vector<int> vectorX;
    vector<int> vectorY;

    // Inicializa Matriz
    for ( int i = 0; i < dim; i++ ) {
        gridFile >> x;        

        gridFile >> x;
        gridFile >> y;

        // Insert coordinates from the client in the vectors
        vectorX.push_back(x);
        vectorY.push_back(y);

        if(x > maxX){
            maxX = x;
        }

        if(x < minX){
            minX = x;
        }

        if(y > maxY){
            maxY = y;
        }

        if(y < minY){
            minY = y;
        }
    }    

    MATRIX_NUMBER_COLUMNS = maxX - minX + 1;
    MATRIX_NUMBER_LINES   = maxY - minY + 1;

    // Initializing the array that represents the map
    m = new Matrix(MATRIX_NUMBER_LINES, MATRIX_NUMBER_COLUMNS);

    for ( int i = 0; i < vectorX.size(); i++ ) {
        vectorX[i] -= minX;
        vectorY[i] -= minY;
    }

    int qtd_clients = (int) dim * pct_clients_visited / 100;
    int client_visited;

    client_points.clear();
    recharge_points.clear();
    clients_visited.clear();

    while(clients_visited.size() < qtd_clients){
        client_visited = rand() % vectorX.size();
        vector<int> client = {vectorX[client_visited], vectorY[client_visited]};
        clients_visited.push_back(client);

        vectorX.erase(vectorX.begin() + client_visited);
        vectorY.erase(vectorY.begin() + client_visited);
    }

    // Fills the entire array with '0'
    m->initialize();

    // Inicializa Matriz
    for ( int i = 0; i < vectorX.size(); i++ ) {
        m->set_stop_point(vectorX[i], vectorY[i], STOPS::DELIVER);

        Solution_Pair * s = new Solution_Pair(vectorX[i], vectorY[i]);
        client_points.push_back(s->copy());
        delete s;
    }

    QTY_POINTS[STOPS::DELIVER] = client_points.size();
    QTY_POINTS[STOPS::COLLECT] = 0;
    PCT_POINTS[STOPS::PROHIBITED] = pct_prohibited;

    gridFile.close();

    return fill_mtrx(m);
}

Matrix * Core::fill_mtrx(Matrix * m) /* fill matrix with random positions for the recharge and prohibited points */
{
    int qty_charge     = 0,// 1 - Variables that will be incremented as the stopping points are instantiated
        qty_prohibited = 0,

        random_number = 0, // 2 - Temporary variable that receives a random value along the loop

        random_x = 0, // 2 - Temporary variable that receives a random value along the loop
        random_y = 0, // 2 - Temporary variable that receives a random value along the loop

        total_points = m->get_size() - client_points.size(), // 3 - Qty of points in the map

        parameter_charge     = PCT_POINTS[STOPS::CHARGE],  // 4 - Percentage relative to stop points qty referenced within entire map
        parameter_prohibited = PCT_POINTS[STOPS::PROHIBITED];

    QTY_POINTS[STOPS::CHARGE]     = (int) (PCT_POINTS[STOPS::CHARGE] * total_points / 100);
    QTY_POINTS[STOPS::PROHIBITED] = (int) (PCT_POINTS[STOPS::PROHIBITED] * total_points / 100);

    // While all stop points are not instantiated
//    while (qty_charge     != QTY_POINTS[STOPS::CHARGE]  ||
//           qty_prohibited != QTY_POINTS[STOPS::PROHIBITED]) {

        while (qty_charge != QTY_POINTS[STOPS::CHARGE]){
            random_number = rand() % m->get_size();

            if (m->get_stop_point(random_number).equals(STOPS::NOTHING)) {
                    m->set_stop_point(random_number,STOPS::CHARGE);
                    qty_charge++;

                    Solution_Pair * s = new Solution_Pair(m->get_stop_point(random_number).get_x(), m->get_stop_point(random_number).get_y());
                    recharge_points.push_back(s->copy());
                    delete s;
            }
        }

        while (qty_prohibited != QTY_POINTS[STOPS::PROHIBITED]){
            random_number = rand() % m->get_size();

            // Not origin
            if (m->get_stop_point(random_number).equals(STOPS::NOTHING) && random_number != 0) {
                    m->set_stop_point(random_number,STOPS::PROHIBITED);
                    qty_prohibited++;
            }
        }

        // Run through the whole grid
//        for (int i = 0; i < total_points; i++) {
//            if (m->get_stop_point(i).equals(STOPS::NOTHING)) {
//                random_number = rand() % 100;//total_points; // Gera um valor aleatorio entre 0 e 100

//                // If the number is less than the parameter of the RECHARGE point and has not yet put on the map the total qty of these points
//                if (random_number < parameter_charge &&
//                    (qty_charge != QTY_POINTS[STOPS::CHARGE]))
//                {
//                    m->set_stop_point(i,STOPS::CHARGE);
//                    qty_charge++;

//                    Solution_Pair * s = new Solution_Pair(m->get_stop_point(i).get_x(), m->get_stop_point(i).get_y());
//                    recharge_points.push_back(s->copy());
//                    delete s;
//                }
//                // If the number is less than the parameter of the PROHIBITED point and has not yet put on the map the total qty of these points
//                else if (random_number < parameter_charge + parameter_prohibited &&
//                    (qty_prohibited != QTY_POINTS[STOPS::PROHIBITED]) &&
//                         i != 0)
////                    (m->get_stop_point(i).get_x() != 0 && m->get_stop_point(i).get_y() != 0))
//                {
//                    m->set_stop_point(i,STOPS::PROHIBITED);
//                    qty_prohibited++;
//                }
//            }
//        }
//    }

    return m;
}

void Core::generate_neighbors(int neighbors_qty) /* get the closest neighbors */
{    
    int distance_temp;
//    vector<Solution_Pair> clients_temp = client_points;

    neighbors.clear();

    for (int i = 0; i < client_points.size(); i++) {
        vector<int> min_distances (neighbors_qty, INT_MAX);
        vector<int> min_positions (neighbors_qty, 0);

        //Comparing the distance between all points
        for (int position = 0; position < client_points.size(); position++) {
            if(position != i){
                distance_temp = Array_Tools::distance(client_points[i], client_points[position]);

                for (int min_position = 0; min_position < min_distances.size(); min_position++) {
                    if(distance_temp < min_distances[min_position]){
                        for(int k = min_distances.size() - 1; k > min_position; k--) {
                          min_positions[k] = min_positions[k-1];
                          min_distances[k] = min_distances[k-1];
                        }
                        min_distances[min_position] = distance_temp;
                        min_positions[min_position] = position;

                        break;
                    }
                }
            }
        }

//        vector<int> neighbors_temp = min_positions;

        neighbors.push_back(min_positions);

        min_distances.clear();
        min_positions.clear();
    }
}

void Core::generate_further_neighbors(int neighbors_qty) /* get the furthest neighbors */
{
    int distance_temp;
//    vector<Solution_Pair> clients_temp = client_points;

    further_neighbors.clear();

    for (int i = 0; i < client_points.size(); i++) {
        vector<int> max_distances (neighbors_qty, INT_MIN);
        vector<int> max_positions (neighbors_qty, 0);

        //Comparing the distance between all points
        for (int position = 0; position < client_points.size(); position++) {
            if(position != i){
                distance_temp = Array_Tools::distance(client_points[i], client_points[position]);

                for (int max_position = 0; max_position < max_distances.size(); max_position++) {
                    if(distance_temp > max_distances[max_position]){
                        for(int k = max_distances.size() - 1; k > max_position; k--) {
                          max_positions[k] = max_positions[k-1];
                          max_distances[k] = max_distances[k-1];
                        }
                        max_distances[max_position] = distance_temp;
                        max_positions[max_position] = position;

                        break;
                    }
                }
            }
        }

//        vector<int> neighbors_temp = min_positions;

        further_neighbors.push_back(max_positions);

        max_distances.clear();
        max_positions.clear();
    }
}

void Core::print_neighbors() /* gera array de pares ordenados indicando um caminho */
{
    for (int i = 0; i < neighbors.size(); i++) {
        vector<int> temp = neighbors[i];

        for (int j = 0; j < temp.size(); j++) {
            cout << temp[j] << ", ";
        }

        cout << endl;
    }
}

Array_Solution Core::initial_solution2(int num_lines, int num_columns, Array_Solution solution) /* constructor */
{
//    vector<Route> routes = solution.routes;

    // Add origin
    for (int j = 0; j < solution.get_size(); j++) {
         solution.routes[j].add_solution_pair(origin_x, origin_y);
    }

    int pos_min, distance_temp, distance_min;

    //Copy clients vector
    vector<Solution_Pair> clients_temp = client_points;

    // If origin is a client, does not need to pass through it again during the route
    for (int i = 0; i < clients_temp.size(); i++) {
        // Equals origin
        if(clients_temp[i].get_x() == origin_x && clients_temp[i].get_y() == origin_y){
            clients_temp.erase(clients_temp.begin() + i);
            break;
        }
    }// END FOR 'clients_temp'

    int num_clients = clients_temp.size();

//    Route route;

    //Number of clients that will be inserted into the routes
    for (int i = 0; i < num_clients; i++) {

        //For each route
        for (int j = 0; j < solution.get_size() &&
                        i * solution.get_size() + j < num_clients; // Number of clients inserted is smaller than the amount of clients on map
                 j++) {

//            route =  solution.routes[j];

    //        vector<int> min_distances ((int) GRASP_FACTOR * clients_temp.size() / 10 + 1, INT_MAX);
    //        vector<int> min_positions ((int) GRASP_FACTOR * clients_temp.size() / 10 + 1, 0);

            distance_min = INT_MAX;
            vector<int> min_distances ((int) GRASP_FACTOR, INT_MAX);
            vector<int> min_positions ((int) GRASP_FACTOR, 0);

            for (int position = 0; position < clients_temp.size(); position++) {
                //Distance between new client and the last one inserted into the solution
                distance_temp = Array_Tools::distance(clients_temp[position], solution.routes[j].get_solution_pair(i));

    //            int greater_pos = -1;
    //            bool smaller = false;

                //Run through current closest neighbors
                for (int min_position = 0; min_position < min_distances.size(); min_position++) {

                    //If new distance is smaller
                    if(distance_temp < min_distances[min_position]){

                        //Update vector of closest neighbors
                        for(int k = min_distances.size() - 1; k > min_position; k--) {
                          min_positions[k] = min_positions[k-1];
                          min_distances[k] = min_distances[k-1];
                        }
                        min_distances[min_position] = distance_temp;
                        min_positions[min_position] = position;

                        break;
    //                    smaller = true;

    //                    if(greater_pos == -1 || min_distances[min_position] > min_distances[greater_pos]){
    //                        greater_pos = min_position;
    //                    }
                    }
                }// END FOR 'min_position'

    //            if(smaller){
    //                min_distances[greater_pos] = distance_temp;
    //                min_positions[greater_pos] = position;
    //            }
            }// END FOR 'position'

            //Get random position from the vector
            pos_min = rand() % (min_positions.size());
            pos_min = min_positions[pos_min];

            //Push client to the solution
            solution.routes[j].add_solution_pair(clients_temp[pos_min].get_x(), clients_temp[pos_min].get_y());
    //        sub_path2(solution, solution->get_size() - 2, solution->get_size() - 1, num_lines, num_columns);

            //Link the new client inserted and the last one from the solution
            solution.routes[j] = sub_path3(solution.routes[j], solution.routes[j].get_size() - 2, solution.routes[j].get_size() - 1);

            clients_temp.erase(clients_temp.begin() + pos_min);
//            i--;

            min_distances.clear();
            min_positions.clear();

//            routes[j] = route;
        }// END FOR 'solution'

    }// END FOR 'clients_temp'

//    solution.set_routes(routes);

    return solution;
}

//Array_Solution * Core::fill_with_clients() /* gera array de pares ordenados indicando um caminho */
//{
//    for (int count = 0; count < pattern.size(); count++) {
//        // Equals origin
//        if(pattern[count].get_x() == origin_x && pattern[count].get_x() == origin_y){
//            pattern.erase(pattern.begin() + count);
//            break;
//        }
//    }

//    Solution_Pair pair(origin_x, origin_y);
//    pattern.insert(pattern.begin(), pair);

//    int pos_min, distance_temp, distance_min, type;
//    vector<Solution_Pair> clients_temp = client_points; // Clients that last to be inserted

//    for (int client_pos = 0; client_pos < clients_temp.size(); client_pos++) {
////        type = m_map->get_stop_point(pattern[count]).get_type();
////        if(type == STOPS::DELIVER || type == STOPS::COLLECT){
//            for (int count = 0; count < pattern.size(); count++) {
//                if(pattern[count].is_at_same_position(clients_temp[client_pos])){
//                    clients_temp.erase(clients_temp.begin() + client_pos);
//                    client_pos--;
//                    break;
//                }
//            }
////        }
//    }

//    for (int client_pos = 0; client_pos < clients_temp.size(); client_pos++) {
//        distance_min = INT_MAX;
//        vector<int> min_distances ((int) GRASP_FACTOR * clients_temp.size() / 10 + 1, INT_MAX);
//        vector<int> min_positions ((int) GRASP_FACTOR * clients_temp.size() / 10 + 1, 0);

//        int greater_pos = -1;
//        bool smaller = false;

//        for (int position = 0; position < pattern.size() - 1; position++) {
//            distance_temp = Array_Tools::distance(pattern[position], clients_temp[client_pos]) + Array_Tools::distance(clients_temp[client_pos], pattern[position + 1]);

//            greater_pos = -1;
//            smaller = false;

//            for (int min_position = 0; min_position < min_distances.size(); min_position++) {
//                if(distance_temp < min_distances[min_position]){
//                    smaller = true;

//                    if(greater_pos == -1 || min_distances[min_position] > min_distances[greater_pos]){
//                        greater_pos = min_position;
//                    }
//                }
//            }

//            if(smaller){
//                min_distances[greater_pos] = distance_temp;
//                min_positions[greater_pos] = position + 1;
//            }
//        }

//        distance_temp = Array_Tools::distance(pattern[pattern.size() - 1], clients_temp[client_pos]);

//        greater_pos = -1;
//        smaller = false;

//        for (int min_position = 0; min_position < min_distances.size(); min_position++) {
//            if(distance_temp < min_distances[min_position]){
//                smaller = true;

//                if(greater_pos == -1 || min_distances[min_position] > min_distances[greater_pos]){
//                    greater_pos = min_position;
//                }
//            }
//        }

//        if(smaller){
//            min_distances[greater_pos] = distance_temp;
//            min_positions[greater_pos] = pattern.size();
//        }

//        pos_min = rand() % (min_positions.size());
//        pos_min = min_positions[pos_min];

//        pattern.insert(pattern.begin() + pos_min, clients_temp[client_pos]);
//        clients_temp.erase(clients_temp.begin() + client_pos);
//        client_pos--;

//        min_distances.clear();
//        min_positions.clear();
//    }

//    Array_Solution * copy_solution = new Array_Solution(MAX_TIME / TIME_INTERVAL + 1);

//    for (int count = 0; count < pattern.size(); count++) {
//        copy_solution->add_solution_pair(pattern[count]);
//    }

//    for (int i = copy_solution->get_size() - 1; i > 0; i--) {
////        sub_path2(copy_solution, i-1, i, MATRIX_NUMBER_LINES, MATRIX_NUMBER_COLUMNS);
//        sub_path3(copy_solution, i-1, i);
//    }

//    return evaluate(copy_solution, m_map);
//}

//Array_Solution * Core::fill_with_recharge_points(Array_Solution * solution, int average_distance) /* gera array de pares ordenados indicando um caminho */
//{
//    int distance_temp, type;
//    vector<Solution_Pair> recharge_temp = recharge_points;
//    Array_Solution * copy_solution = solution->copy();

//    for (int i = copy_solution->get_size() - 2; i >= 0; i--) {
//        type = m_map->get_stop_point(copy_solution->get_solution_pair(i).get_x(), copy_solution->get_solution_pair(i).get_y()).get_type();
//        if(i == 0
//           || type == STOPS::DELIVER
//           || type == STOPS::COLLECT
//                ){

//            for (int j = i+1; j < copy_solution->get_size(); j++) {
//                type = m_map->get_stop_point(copy_solution->get_solution_pair(j).get_x(), copy_solution->get_solution_pair(j).get_y()).get_type();
//                if(type == STOPS::DELIVER
//                   || type == STOPS::COLLECT
//                                        ){
//                    break;
//                }else{
//                    copy_solution->remove_solution_pair(j);
//                    j--;
//                }
//            }

////            if(i + 2 == copy_solution->get_size()) reached_end = true;

//            int min_distance = INT_MAX;
//            int pos_min = 0;

//            for (int position = 0; position < recharge_temp.size(); position++) {
//                distance_temp = Array_Tools::distance(recharge_temp[position], copy_solution->get_solution_pair(i)) + average_distance * Array_Tools::distance(recharge_temp[position], copy_solution->get_solution_pair(i+1));
//                if(distance_temp < min_distance){
//                    min_distance = distance_temp;
//                    pos_min = position;
//                }
//            }

//            if(Array_Tools::distance(copy_solution->get_solution_pair(i), recharge_temp[pos_min]) < Array_Tools::distance(copy_solution->get_solution_pair(i), copy_solution->get_solution_pair(i + 1))
//                    || copy_solution->get_solution_pair(i + 1).get_battery_charge() < 10){
//                copy_solution->insert_solution_pair(recharge_temp[pos_min].get_x(), recharge_temp[pos_min].get_y(), i);
////                sub_path2(copy_solution, i+1, i+2, MATRIX_NUMBER_LINES, MATRIX_NUMBER_COLUMNS);
////                sub_path2(copy_solution, i, i+1, MATRIX_NUMBER_LINES, MATRIX_NUMBER_COLUMNS);
//                sub_path3(copy_solution, i+1, i+2);
//                sub_path3(copy_solution, i, i+1);
//            }else{
////                sub_path2(copy_solution, i, i+1, MATRIX_NUMBER_LINES, MATRIX_NUMBER_COLUMNS);
//                sub_path3(copy_solution, i, i+1);
//            }


//        }
//    }

//    return evaluate(copy_solution, m_map);
//}

//Array_Solution * Core::validate(Array_Solution * a_solution) {/* run through all the solution evaluating it*/

//    int distance_temp, type;
//    vector<Solution_Pair> recharge_temp = recharge_points;

//    for (int i = 1; i < a_solution->get_size(); i++) {
//        if (a_solution->get_solution_pair(i).get_battery_charge() < 0) {

//            Array_Solution * copy_solution = a_solution->copy();
//            for (int k = i; k < copy_solution->get_size(); k++) {
//                type = m_map->get_stop_point(copy_solution->get_solution_pair(k).get_x(), copy_solution->get_solution_pair(k).get_y()).get_type();
//                if(type != STOPS::NOTHING){
//                    break;
//                }
//                copy_solution->remove_solution_pair(k);
//                k--;
//            }

//            int new_pos = i-1;

//            for (int k = i-1; k >= 0; k--) {
//                type = m_map->get_stop_point(copy_solution->get_solution_pair(k).get_x(), copy_solution->get_solution_pair(k).get_y()).get_type();
//                if(type != STOPS::NOTHING || k == 0){
//                    new_pos = k;
//                    break;
//                }
//                copy_solution->remove_solution_pair(k);
//            }

//            int min_distance = INT_MAX;
//            int pos_min = 0;

//            for (int position = 0; position < recharge_temp.size(); position++) {
//                if(!recharge_temp[position].equals(copy_solution->get_solution_pair(i))){
//                    distance_temp = Array_Tools::distance(recharge_temp[position], copy_solution->get_solution_pair(new_pos)) + Array_Tools::distance(recharge_temp[position], copy_solution->get_solution_pair(new_pos+1));
//                    if(distance_temp < min_distance){
//                        min_distance = distance_temp;
//                        pos_min = position;
//                    }
//                }
//            }

//            copy_solution->insert_solution_pair(recharge_temp[pos_min].get_x(), recharge_temp[pos_min].get_y(), new_pos);
////            sub_path2(copy_solution, new_pos+1, new_pos+2, MATRIX_NUMBER_LINES, MATRIX_NUMBER_COLUMNS);
////            sub_path2(copy_solution, new_pos, new_pos+1, MATRIX_NUMBER_LINES, MATRIX_NUMBER_COLUMNS);
//            sub_path3(copy_solution, new_pos+1, new_pos+2);
//            sub_path3(copy_solution, new_pos, new_pos+1);
//            copy_solution = evaluate(copy_solution, m_map);

//            return copy_solution;
//        }
//    }

//    return NULL;
//}

Array_Solution Core::remove_recharge(int pos, vector<Array_Solution> solutions) /* gera array de pares ordenados indicando um caminho */
{
    int distance_temp, type;
    bool reached_end;
    Array_Solution copy_solution;
//    vector<Solution_Pair> recharge_temp = recharge_points;
//    vector<Array_Solution *> resp;

    // Run through all routes of the solution
    for (int pos_route = 0; pos_route < solutions[pos].get_size(); pos_route++) {
        reached_end = false;

        // Run through the route
        for (int i = 0; i < solutions[pos].get_routes()[pos_route].get_size() && !reached_end; i++) {

            // Get type of the point
            type = m_map->get_stop_point(solutions[pos].get_routes()[pos_route].get_solution_pair(i).get_x(),
                                         solutions[pos].get_routes()[pos_route].get_solution_pair(i).get_y()).get_type();

            // Point is the origin or client
            if(i == 0 ||
               type == STOPS::DELIVER ||
               type == STOPS::COLLECT){


//                Array_Solution * copy_solution = solutions[pos]->copy();
                copy_solution = solutions[pos].copy(); //memcheck

                // Go forward and eliminates the points until finds an inspection point (client)
                for (int j = i+1; j < copy_solution.get_routes()[pos_route].get_size(); j++) {
                    type = m_map->get_stop_point(copy_solution.get_routes()[pos_route].get_solution_pair(j).get_x(),
                                                 copy_solution.get_routes()[pos_route].get_solution_pair(j).get_y()).get_type();
                    if(type == STOPS::DELIVER ||
                       type == STOPS::COLLECT){
                        break;
                    }else{
                        copy_solution.routes[pos_route].remove_solution_pair(j);
                        j--;
                    }
                }

                // Reached last recharge point
                if(i + 2 == copy_solution.get_routes()[pos_route].get_size()) reached_end = true;

    //            sub_path2(copy_solution, i, i+1, MATRIX_NUMBER_LINES, MATRIX_NUMBER_COLUMNS);
                copy_solution.routes[pos_route] = sub_path3(copy_solution.get_routes()[pos_route], i, i+1);
                copy_solution = evaluate(copy_solution, m_map);

                bool dominated_or_equal = false;
                if(!solutions[pos].weakly_dominates(copy_solution)){//if(copy_solution->dominates(solutions[pos])){//copy_solution->is_valid()
                    for (int k = 0; k < solutions.size(); k++) {
                        if(solutions[k].equals(copy_solution)){
                            dominated_or_equal = true;
                            break;
                        }
                        if(solutions[k].dominates(copy_solution)){
                            dominated_or_equal = true;
                            break;
                        }
                    }

                    if(!dominated_or_equal){
                        return copy_solution;
                    }
                }

//                delete copy_solution;
            }
        }
    }

    return Array_Solution(true);
}

Array_Solution Core::closest_recharge(int pos, vector<Array_Solution> solutions, int average_distance) /* insert recharge point between inspect points */
{    
    int distance_temp, type;
//    bool reached_end = false;
    vector<Solution_Pair> recharge_temp = recharge_points;
    Array_Solution copy_solution;
//    vector<Array_Solution *> resp;

    for (int pos_route = 0; pos_route < solutions[pos].get_size(); pos_route++) {

        // Run through the solution
        for (int i = 0; i < solutions[pos].get_routes()[pos_route].get_size() - 1; i++){
    //         && !reached_end; i++) {
            type = m_map->get_stop_point(solutions[pos].get_routes()[pos_route].get_solution_pair(i).get_x(),
                                         solutions[pos].get_routes()[pos_route].get_solution_pair(i).get_y()).get_type(); // Get type of the current point

            // Point is the origin or other important point (client/recharge)
            if(i == 0 ||
               type != STOPS::NOTHING){


//                Array_Solution * copy_solution = solutions[pos]->copy();
                copy_solution = solutions[pos].copy(); //memcheck

                // Remove points between important points
                for (int j = i+1; j < copy_solution.get_routes()[pos_route].get_size(); j++) {
                    type = m_map->get_stop_point(copy_solution.get_routes()[pos_route].get_solution_pair(j).get_x(),
                                                 copy_solution.get_routes()[pos_route].get_solution_pair(j).get_y()).get_type();
                    if(type != STOPS::NOTHING){
                        break;
                    }else{
                        copy_solution.routes[pos_route].remove_solution_pair(j);
                        j--;
                    }
                }

    //            if(i + 2 == copy_solution->get_size()) {
    //                reached_end = true;
    //            } else {
                    int min_distance = INT_MAX;
                    int pos_min = 0;

                    // Get the closest recharge point
                    for (int position = 0; position < recharge_temp.size(); position++) {
                        if(!recharge_temp[position].equals(copy_solution.get_routes()[pos_route].get_solution_pair(i))){
                            distance_temp = Array_Tools::distance(recharge_temp[position],
                                                                  copy_solution.get_routes()[pos_route].get_solution_pair(i)) + average_distance * Array_Tools::distance(recharge_temp[position],
                                                                                                                                                                           copy_solution.get_routes()[pos_route].get_solution_pair(i+1));
                            if(distance_temp < min_distance){
                                min_distance = distance_temp;
                                pos_min = position;
                            }
                        }
                    }

                    copy_solution.routes[pos_route].insert_solution_pair(recharge_temp[pos_min].get_x(), recharge_temp[pos_min].get_y(), i);
        //            sub_path2(copy_solution, i+1, i+2, MATRIX_NUMBER_LINES, MATRIX_NUMBER_COLUMNS);
        //            sub_path2(copy_solution, i, i+1, MATRIX_NUMBER_LINES, MATRIX_NUMBER_COLUMNS);

                    // Small path between recharge point and next important point
                    copy_solution.routes[pos_route] = sub_path3(copy_solution.get_routes()[pos_route], i+1, i+2);

                    // Small path between important point and new recharge point
                    copy_solution.routes[pos_route] = sub_path3(copy_solution.get_routes()[pos_route], i, i+1);
    //            }

                // Evaluate solution
                copy_solution = evaluate(copy_solution, m_map);

    //            if(!copy_solution->get_solution_pair(copy_solution->get_size() - 1).is_neighbor_from(copy_solution->get_solution_pair(copy_solution->get_size() - 2))){
    //                cout << "";
    //            }else{
    //                cout << "";
    //            }

                bool dominated_or_equal = false;
                if(!solutions[pos].weakly_dominates(copy_solution)){//copy_solution->dominates(solutions[pos])){//copy_solution->is_valid() &&
                    for (int k = 0; k < solutions.size(); k++) {
                        if(solutions[k].equals(copy_solution)){
                            dominated_or_equal = true;
                            break;
                        }
                        if(solutions[k].dominates(copy_solution)){
                            dominated_or_equal = true;
                            break;
                        }
                    }

                    if(!dominated_or_equal){
                        return copy_solution;
                    }
                }


    //            resp.push_back(evaluate(copy_solution->copy(), m_map));
//                delete copy_solution;
            }
        }
    }

    return Array_Solution(true);
}

//Array_Solution * Core::remove_client(int pos, vector<Array_Solution *> solutions) /* remove clients from the route */
//{
//    int type;
//    for (int i = 1; i < solutions[pos]->get_size(); i++) {
//        type = m_map->get_stop_point(solutions[pos]->get_solution_pair(i).get_x(), solutions[pos]->get_solution_pair(i).get_y()).get_type();
//        if(i != 0 && (type == STOPS::DELIVER || type == STOPS::COLLECT)){


//            Array_Solution * copy_solution = solutions[pos]->copy();
//            for (int k = i + 1; k < copy_solution->get_size(); k++) {
//                type = m_map->get_stop_point(copy_solution->get_solution_pair(k).get_x(), copy_solution->get_solution_pair(k).get_y()).get_type();
//                if(type == STOPS::DELIVER || type == STOPS::COLLECT){
//                    break;
//                }
//                copy_solution->remove_solution_pair(k);
//                k--;
//            }

//            copy_solution->remove_solution_pair(i);
//            int new_pos = i;

//            for (int k = i-1; k >= 0; k--) {
//                type = m_map->get_stop_point(copy_solution->get_solution_pair(k).get_x(), copy_solution->get_solution_pair(k).get_y()).get_type();
//                if(type == STOPS::DELIVER || type == STOPS::COLLECT || k == 0){
//                    new_pos = k;
//                    break;
//                }
//                copy_solution->remove_solution_pair(k);
//            }

////            sub_path2(copy_solution, new_pos, new_pos+1, MATRIX_NUMBER_LINES, MATRIX_NUMBER_COLUMNS);
//            sub_path3(copy_solution, new_pos, new_pos+1);

//            copy_solution = evaluate(copy_solution, m_map);

//            bool dominated_or_equal = false;
////            if(copy_solution->dominates(solutions[pos])){//copy_solution2->is_valid() &&
//                for (int k = 0; k < solutions.size(); k++) {
//                    if(solutions[k]->equals(copy_solution)){
//                        dominated_or_equal = true;
//                        break;
//                    }
//                    if(solutions[k]->dominates(copy_solution)){
//                        dominated_or_equal = true;
//                        break;
//                    }
//                }

//                if(!dominated_or_equal){
//                    return copy_solution;
//                }
////            }

//            delete copy_solution;
//        }
//    }

//    return NULL;
//}

Array_Solution Core::remove_repeated(int pos, vector<Array_Solution> solutions) /* remove clients already visited through the route */
{

    int type;
    Array_Solution copy_solution;
    Solution_Pair temp;

    for (int pos_route = 0; pos_route < solutions[pos].get_size(); pos_route++) {

        // Runs through route
        for (int i = 0; i < solutions[pos].get_routes()[pos_route].get_size(); i++) {
            type = m_map->get_stop_point(solutions[pos].get_routes()[pos_route].get_solution_pair(i).get_x(),
                                         solutions[pos].get_routes()[pos_route].get_solution_pair(i).get_y()).get_type();

            // Finds client
            if(type == STOPS::DELIVER || type == STOPS::COLLECT){


//                Array_Solution * copy_solution = solutions[pos]->copy();
                copy_solution = solutions[pos].copy(); //memcheck
                temp = copy_solution.get_routes()[pos_route].get_solution_pair(i).copy();


                for (int pos_route_comp = 0; pos_route_comp < solutions[pos].get_size(); pos_route_comp++) {

                    // Runs through route backwards
                    for (int j = copy_solution.get_routes()[pos_route_comp].get_size() - 1; j > 0; j--) {

                        if(pos_route != pos_route_comp || // Not same route
                                j != i){ // Same route but position of client different

                            // Finds repeated point
                            if(copy_solution.get_routes()[pos_route_comp].get_solution_pair(j).equals(temp)){

                                // Remove path after it
                                for (int k = j + 1; k < copy_solution.get_routes()[pos_route_comp].get_size(); k++) {
                                    type = m_map->get_stop_point(copy_solution.get_routes()[pos_route_comp].get_solution_pair(k).get_x(),
                                                                 copy_solution.get_routes()[pos_route_comp].get_solution_pair(k).get_y()).get_type();
                                    if(type == STOPS::DELIVER || type == STOPS::COLLECT){
                                        break;
                                    }
                                    copy_solution.routes[pos_route_comp].remove_solution_pair(k);
                                    k--;
                                }

                                // Remove it
                                copy_solution.routes[pos_route_comp].remove_solution_pair(j);

                                // Remove path before it
                                for (int k = j-1; k >= 0; k--) {
                                    type = m_map->get_stop_point(copy_solution.get_routes()[pos_route_comp].get_solution_pair(k).get_x(),
                                                                 copy_solution.get_routes()[pos_route_comp].get_solution_pair(k).get_y()).get_type();
                                    if(type == STOPS::DELIVER || type == STOPS::COLLECT || k == 0){
                                        j = k;
                                        break;
                                    }
                                    copy_solution.routes[pos_route_comp].remove_solution_pair(k);
                                }

        //                        sub_path2(copy_solution, j, j+1, MATRIX_NUMBER_LINES, MATRIX_NUMBER_COLUMNS);
                                if(j != copy_solution.get_routes()[pos_route_comp].get_size() - 1){
                                    copy_solution.routes[pos_route] = sub_path3(copy_solution.get_routes()[pos_route_comp], j, j+1);
                                }

                            }
                        }
                    }
                }

                copy_solution = evaluate(copy_solution, m_map);

                bool dominated_or_equal = false;
                if(copy_solution.dominates(solutions[pos])){//copy_solution2->is_valid() &&
                    for (int k = 0; k < solutions.size(); k++) {
                        if(solutions[k].equals(copy_solution)){
                            dominated_or_equal = true;
                            break;
                        }
                        if(solutions[k].dominates(copy_solution)){
                            dominated_or_equal = true;
                            break;
                        }
                    }

                    if(!dominated_or_equal){
                        return copy_solution;
                    }
                }

//                delete copy_solution;
            }
        }
    }

    return Array_Solution(true);
}

//Array_Solution * Core::swap3(int pos, vector<Array_Solution *> solutions) /* gera array de pares ordenados indicando um caminho */
//{

//    int type;
//    for (int i = 1; i < solutions[pos]->get_size() - 1; i++) { // Run through all the solution except last point
//        type = m_map->get_stop_point(solutions[pos]->get_solution_pair(i).get_x(), solutions[pos]->get_solution_pair(i).get_y()).get_type();

//        if(type == STOPS::DELIVER || type == STOPS::COLLECT){ // If it is an inspection point (client)
//            Array_Solution * copy_solution = solutions[pos]->copy();
//            Solution_Pair temp = copy_solution->get_solution_pair(i).copy();
//            int pos1 = 0; // position of the first point that will swap

//            for (int k = i + 1; k < copy_solution->get_size(); k++) { // Go forward and eliminates the points until finds an inspection point (client)
//                type = m_map->get_stop_point(copy_solution->get_solution_pair(k).get_x(), copy_solution->get_solution_pair(k).get_y()).get_type();
//                if(type == STOPS::DELIVER || type == STOPS::COLLECT){
//                    break;
//                }
//                copy_solution->remove_solution_pair(k);
//                k--;
//            }

//            copy_solution->remove_solution_pair(i);

//            for (int k = i-1; k >= 0; k--) { // Go backward and eliminates the points until finds an inspection point (client)
//                type = m_map->get_stop_point(copy_solution->get_solution_pair(k).get_x(), copy_solution->get_solution_pair(k).get_y()).get_type();
//                if(type == STOPS::DELIVER || type == STOPS::COLLECT || k == 0){
//                    pos1 = k;
//                    break;
//                }
//                copy_solution->remove_solution_pair(k);
//            }

//            for (int j = pos1+1; j < copy_solution->get_size(); j++) {// Finds other inspection point (client)
//                 type = m_map->get_stop_point(copy_solution->get_solution_pair(j).get_x(), copy_solution->get_solution_pair(j).get_y()).get_type();
//                 if(!copy_solution->get_solution_pair(j).equals(temp) && (type == STOPS::DELIVER || type == STOPS::COLLECT)){
//                     Array_Solution * copy_solution2 = copy_solution->copy();
//                     for (int k = j + 1; k < copy_solution2->get_size(); k++) { // Go forward and eliminates the points until finds an inspection point (client)
//                         type = m_map->get_stop_point(copy_solution2->get_solution_pair(k).get_x(), copy_solution2->get_solution_pair(k).get_y()).get_type();
//                         if(type == STOPS::DELIVER || type == STOPS::COLLECT){
//                             break;
//                         }
//                         copy_solution2->remove_solution_pair(k);
//                         k--;
//                     }

//                     Solution_Pair temp2 = copy_solution2->get_solution_pair(j).copy();
//                     copy_solution2->remove_solution_pair(j);
//                     int pos2 = j - 1; // position of the second point that will swap

//                     for (int k = pos2; k >= pos1; k--) { // Go backward and eliminates the points until finds an inspection point (client)
//                         type = m_map->get_stop_point(copy_solution2->get_solution_pair(k).get_x(), copy_solution2->get_solution_pair(k).get_y()).get_type();
//                         if(type == STOPS::DELIVER || type == STOPS::COLLECT || k == pos1){
//                             pos2 = k;
//                             break;
//                         }
//                         copy_solution2->remove_solution_pair(k);
//                     }

//                     copy_solution2->insert_solution_pair(temp, pos2); // insert the first point at the second position

//                     if(pos2 + 2 != copy_solution2->get_size()){
////                        sub_path2(copy_solution2, pos2+1, pos2+2, MATRIX_NUMBER_LINES, MATRIX_NUMBER_COLUMNS);
//                         sub_path3(copy_solution2, pos2+1, pos2+2);
//                     }

//                     if(pos2 != pos1){
////                        sub_path2(copy_solution2, pos2, pos2+1, MATRIX_NUMBER_LINES, MATRIX_NUMBER_COLUMNS);
//                         sub_path3(copy_solution2, pos2, pos2+1);
//                     }

//                     copy_solution2->insert_solution_pair(temp2, pos1); // insert the second point at the first position

////                     sub_path2(copy_solution2, pos1+1, pos1+2, MATRIX_NUMBER_LINES, MATRIX_NUMBER_COLUMNS);
////                     sub_path2(copy_solution2, pos1, pos1+1, MATRIX_NUMBER_LINES, MATRIX_NUMBER_COLUMNS);
//                     sub_path3(copy_solution2, pos1+1, pos1+2);
//                     sub_path3(copy_solution2, pos1, pos1+1);

//                     copy_solution2 = evaluate(copy_solution2, m_map);

//                     // New solution is not weakly dominated by the old solution
//                     bool dominated_or_equal = false;
//                     if(!solutions[pos]->weakly_dominates(copy_solution2)){//if(copy_solution2->dominates(solutions[pos])){//copy_solution2->is_valid() &&
//                         for (int k = 0; k < solutions.size(); k++) {
//                             if(solutions[k]->equals(copy_solution2)){
//                                 dominated_or_equal = true;
//                                 break;
//                             }
//                             if(solutions[k]->dominates(copy_solution2)){
//                                 dominated_or_equal = true;
//                                 break;
//                             }
//                         }

//                         if(!dominated_or_equal){
//                             return copy_solution2;
//                         }
//                     }

//                     delete copy_solution2;
//                 }
//            }

//            delete copy_solution;
//        }
//    }

//    return NULL;
//}

Array_Solution Core::swap4_multi(int pos, vector<Array_Solution> solutions) /* gera array de pares ordenados indicando um caminho */
{
    Route route;
    Array_Solution copy_solution, copy_solution2;
    Solution_Pair temp, temp2;

    for (int pos_route = 0; pos_route < solutions[pos].get_size(); pos_route++) {
        route = solutions[pos].get_routes()[pos_route];

        int type;
        for (int i = 1; i < route.get_size() - 1; i++) { // Run through all the solution except last point
            type = m_map->get_stop_point(route.get_solution_pair(i).get_x(),
                                         route.get_solution_pair(i).get_y()
                                         ).get_type();

            if(type == STOPS::DELIVER || type == STOPS::COLLECT){ // If it is an inspection point (client)
//                Array_Solution * copy_solution = solutions[pos]->copy();
                copy_solution = solutions[pos].copy(); //memcheck

                temp = copy_solution.get_routes()[pos_route].get_solution_pair(i).copy();
                int pos1 = 0; // position of the first point that will swap

                for (int k = i + 1; k < copy_solution.get_routes()[pos_route].get_size(); k++) { // Go forward and eliminates the points until finds an inspection point (client)
                    type = m_map->get_stop_point(copy_solution.get_routes()[pos_route].get_solution_pair(k).get_x(),
                                                 copy_solution.get_routes()[pos_route].get_solution_pair(k).get_y()
                                                 ).get_type();
                    if(type == STOPS::DELIVER || type == STOPS::COLLECT){
                        break;
                    }
                    copy_solution.routes[pos_route].remove_solution_pair(k);
                    k--;
                }

                copy_solution.routes[pos_route].remove_solution_pair(i);

                for (int k = i-1; k >= 0; k--) { // Go backward and eliminates the points until finds an inspection point (client)
                    type = m_map->get_stop_point(copy_solution.get_routes()[pos_route].get_solution_pair(k).get_x(),
                                                 copy_solution.get_routes()[pos_route].get_solution_pair(k).get_y()
                                                 ).get_type();

                    if(type == STOPS::DELIVER || type == STOPS::COLLECT || k == 0){
                        pos1 = k;
                        break;
                    }
                    copy_solution.routes[pos_route].remove_solution_pair(k);
                }

                int client_pos;
                for (int j = 0; j < client_points.size(); j++) {
                    if(client_points[j].is_at_same_position(temp)){
                        client_pos = j;
                        break;
                    }
                }


                for (int l = 0; l < neighbors[client_pos].size(); l++) {

                    for (int j = pos1+1; j < copy_solution.get_routes()[pos_route].get_size(); j++) {
                        if(copy_solution.get_routes()[pos_route].get_solution_pair(j).is_at_same_position(client_points[neighbors[client_pos][l]])){
//                             Array_Solution * copy_solution2 = copy_solution->copy();
                            copy_solution2 = copy_solution.copy(); // memcheck

                             for (int k = j + 1; k < copy_solution2.get_routes()[pos_route].get_size(); k++) { // Go forward and eliminates the points until finds an inspection point (client)
                                 type = m_map->get_stop_point(copy_solution2.get_routes()[pos_route].get_solution_pair(k).get_x(),
                                                              copy_solution2.get_routes()[pos_route].get_solution_pair(k).get_y()
                                                              ).get_type();

                                 if(type == STOPS::DELIVER || type == STOPS::COLLECT){
                                     break;
                                 }
                                 copy_solution2.routes[pos_route].remove_solution_pair(k);
                                 k--;
                             }

                             temp2 = copy_solution2.get_routes()[pos_route].get_solution_pair(j).copy();
                             copy_solution2.routes[pos_route].remove_solution_pair(j);
                             int pos2 = j - 1; // position of the second point that will swap

                             for (int k = pos2; k >= pos1; k--) { // Go backward and eliminates the points until finds an inspection point (client)
                                 type = m_map->get_stop_point(copy_solution2.get_routes()[pos_route].get_solution_pair(k).get_x(),
                                                              copy_solution2.get_routes()[pos_route].get_solution_pair(k).get_y()
                                                              ).get_type();

                                 if(type == STOPS::DELIVER || type == STOPS::COLLECT || k == pos1){
                                     pos2 = k;
                                     break;
                                 }
                                 copy_solution2.routes[pos_route].remove_solution_pair(k);
                             }

                             copy_solution2.routes[pos_route].insert_solution_pair(temp, pos2); // insert the first point at the second position

                             if(pos2 + 2 != copy_solution2.get_routes()[pos_route].get_size()){
                                  copy_solution2.routes[pos_route] = sub_path3(copy_solution2.get_routes()[pos_route], pos2+1, pos2+2);
                             }

                             if(pos2 != pos1){
                                 copy_solution2.routes[pos_route] = sub_path3(copy_solution2.get_routes()[pos_route], pos2, pos2+1);
                             }

                             copy_solution2.routes[pos_route].insert_solution_pair(temp2, pos1); // insert the second point at the first position

                             copy_solution2.routes[pos_route] = sub_path3(copy_solution2.get_routes()[pos_route], pos1+1, pos1+2);
                             copy_solution2.routes[pos_route] = sub_path3(copy_solution2.get_routes()[pos_route], pos1, pos1+1);


                             copy_solution2 = evaluate(copy_solution2, m_map);

                             bool dominated_or_equal = false;
                             if(!solutions[pos].weakly_dominates(copy_solution2)){
                                 for (int k = 0; k < solutions.size(); k++) {
                                     if(solutions[k].equals(copy_solution2)){
                                         dominated_or_equal = true;
                                         break;
                                     }
                                     if(solutions[k].dominates(copy_solution2)){
                                         dominated_or_equal = true;
                                         break;
                                     }
                                 }

                                 if(!dominated_or_equal){
                                     return copy_solution2;
                                 }
                             }

//                             delete copy_solution2;
                        }
                    }
                }

//                delete copy_solution;
            }
        }
    }

    return Array_Solution(true);
}

//Array_Solution * Core::swap4(int pos, vector<Array_Solution *> solutions) /* gera array de pares ordenados indicando um caminho */
//{

//    int type;
//    for (int i = 1; i < solutions[pos]->get_size() - 1; i++) { // Run through all the solution except last point
//        type = m_map->get_stop_point(solutions[pos]->get_solution_pair(i).get_x(), solutions[pos]->get_solution_pair(i).get_y()).get_type();

//        if(type == STOPS::DELIVER || type == STOPS::COLLECT){ // If it is an inspection point (client)
//            Array_Solution * copy_solution = solutions[pos]->copy();
//            Solution_Pair temp = copy_solution->get_solution_pair(i).copy();
//            int pos1 = 0; // position of the first point that will swap

//            for (int k = i + 1; k < copy_solution->get_size(); k++) { // Go forward and eliminates the points until finds an inspection point (client)
//                type = m_map->get_stop_point(copy_solution->get_solution_pair(k).get_x(), copy_solution->get_solution_pair(k).get_y()).get_type();
//                if(type == STOPS::DELIVER || type == STOPS::COLLECT){
//                    break;
//                }
//                copy_solution->remove_solution_pair(k);
//                k--;
//            }

//            copy_solution->remove_solution_pair(i);

//            for (int k = i-1; k >= 0; k--) { // Go backward and eliminates the points until finds an inspection point (client)
//                type = m_map->get_stop_point(copy_solution->get_solution_pair(k).get_x(), copy_solution->get_solution_pair(k).get_y()).get_type();
//                if(type == STOPS::DELIVER || type == STOPS::COLLECT || k == 0){
//                    pos1 = k;
//                    break;
//                }
//                copy_solution->remove_solution_pair(k);
//            }

//            int client_pos;
//            for (int j = 0; j < client_points.size(); j++) {
//                if(client_points[j].is_at_same_position(temp)){
//                    client_pos = j;
//                    break;
//                }
//            }

////            cout << client_pos << endl;


//            for (int l = 0; l < neighbors[client_pos].size(); l++) {

//            for (int j = pos1+1; j < copy_solution->get_size(); j++) {
////            for (int j = pos1+1; j < copy_solution->get_size(); j++) {
////                 type = m_map->get_stop_point(copy_solution->get_solution_pair(j).get_x(), copy_solution->get_solution_pair(j).get_y()).get_type();
////                 if(!copy_solution->get_solution_pair(j).equals(temp) && (type == STOPS::DELIVER || type == STOPS::COLLECT)){
//                if(copy_solution->get_solution_pair(j).is_at_same_position(client_points[neighbors[client_pos][l]])){
//                     Array_Solution * copy_solution2 = copy_solution->copy();
//                     for (int k = j + 1; k < copy_solution2->get_size(); k++) { // Go forward and eliminates the points until finds an inspection point (client)
//                         type = m_map->get_stop_point(copy_solution2->get_solution_pair(k).get_x(), copy_solution2->get_solution_pair(k).get_y()).get_type();
//                         if(type == STOPS::DELIVER || type == STOPS::COLLECT){
//                             break;
//                         }
//                         copy_solution2->remove_solution_pair(k);
//                         k--;
//                     }

//                     Solution_Pair temp2 = copy_solution2->get_solution_pair(j).copy();
//                     copy_solution2->remove_solution_pair(j);
//                     int pos2 = j - 1; // position of the second point that will swap

//                     for (int k = pos2; k >= pos1; k--) { // Go backward and eliminates the points until finds an inspection point (client)
//                         type = m_map->get_stop_point(copy_solution2->get_solution_pair(k).get_x(), copy_solution2->get_solution_pair(k).get_y()).get_type();
//                         if(type == STOPS::DELIVER || type == STOPS::COLLECT || k == pos1){
//                             pos2 = k;
//                             break;
//                         }
//                         copy_solution2->remove_solution_pair(k);
//                     }

//                     copy_solution2->insert_solution_pair(temp, pos2); // insert the first point at the second position

//                     if(pos2 + 2 != copy_solution2->get_size()){
////                        sub_path2(copy_solution2, pos2+1, pos2+2, MATRIX_NUMBER_LINES, MATRIX_NUMBER_COLUMNS);
//                         sub_path3(copy_solution2, pos2+1, pos2+2);
//                     }

//                     if(pos2 != pos1){
////                        sub_path2(copy_solution2, pos2, pos2+1, MATRIX_NUMBER_LINES, MATRIX_NUMBER_COLUMNS);
//                        sub_path3(copy_solution2, pos2, pos2+1);
//                     }

//                     copy_solution2->insert_solution_pair(temp2, pos1); // insert the second point at the first position

////                     sub_path2(copy_solution2, pos1+1, pos1+2, MATRIX_NUMBER_LINES, MATRIX_NUMBER_COLUMNS);
////                     sub_path2(copy_solution2, pos1, pos1+1, MATRIX_NUMBER_LINES, MATRIX_NUMBER_COLUMNS);
//                     sub_path3(copy_solution2, pos1+1, pos1+2);
//                     sub_path3(copy_solution2, pos1, pos1+1);


//                     copy_solution2 = evaluate(copy_solution2, m_map);

//                     bool dominated_or_equal = false;
//                     if(!solutions[pos]->weakly_dominates(copy_solution2)){//if(copy_solution2->dominates(solutions[pos])){//copy_solution2->is_valid() &&
////                     if(copy_solution2->is_valid()){
//                         for (int k = 0; k < solutions.size(); k++) {
//                             if(solutions[k]->equals(copy_solution2)){
//                                 dominated_or_equal = true;
//                                 break;
//                             }
//                             if(solutions[k]->dominates(copy_solution2)){
//                                 dominated_or_equal = true;
//                                 break;
//                             }
//                         }

//                         if(!dominated_or_equal){
//                             return copy_solution2;
//                         }
//                     }

//                     delete copy_solution2;
//                 }
//            }
//            }

//            delete copy_solution;
//        }
//    }

//    return NULL;
//}

//Array_Solution * Core::swap5(int pos, vector<Array_Solution *> solutions) /* gera array de pares ordenados indicando um caminho */
//{

//    int type;
//    for (int i = 1; i < solutions[pos]->get_size() - 1; i++) { // Run through all the solution except last point
//        type = m_map->get_stop_point(solutions[pos]->get_solution_pair(i).get_x(), solutions[pos]->get_solution_pair(i).get_y()).get_type();

//        if(type == STOPS::DELIVER || type == STOPS::COLLECT){ // If it is an inspection point (client)
//            Array_Solution * copy_solution = solutions[pos]->copy();
//            Solution_Pair temp = copy_solution->get_solution_pair(i).copy();
//            int pos1 = 0; // position of the first point that will swap

//            for (int k = i + 1; k < copy_solution->get_size(); k++) { // Go forward and eliminates the points until finds an inspection point (client)
//                type = m_map->get_stop_point(copy_solution->get_solution_pair(k).get_x(), copy_solution->get_solution_pair(k).get_y()).get_type();
//                if(type == STOPS::DELIVER || type == STOPS::COLLECT){
//                    break;
//                }
//                copy_solution->remove_solution_pair(k);
//                k--;
//            }

//            copy_solution->remove_solution_pair(i);

//            for (int k = i-1; k >= 0; k--) { // Go backward and eliminates the points until finds an inspection point (client)
//                type = m_map->get_stop_point(copy_solution->get_solution_pair(k).get_x(), copy_solution->get_solution_pair(k).get_y()).get_type();
//                if(type == STOPS::DELIVER || type == STOPS::COLLECT || k == 0){
//                    pos1 = k;
//                    break;
//                }
//                copy_solution->remove_solution_pair(k);
//            }

//            int client_pos;
//            for (int j = 0; j < client_points.size(); j++) {
//                if(client_points[j].is_at_same_position(temp)){
//                    client_pos = j;
//                    break;
//                }
//            }

////            cout << client_pos << endl;


//            for (int l = 0; l < further_neighbors[client_pos].size(); l++) {

//            for (int j = pos1+1; j < copy_solution->get_size(); j++) {
////            for (int j = pos1+1; j < copy_solution->get_size(); j++) {
////                 type = m_map->get_stop_point(copy_solution->get_solution_pair(j).get_x(), copy_solution->get_solution_pair(j).get_y()).get_type();
////                 if(!copy_solution->get_solution_pair(j).equals(temp) && (type == STOPS::DELIVER || type == STOPS::COLLECT)){
//                if(copy_solution->get_solution_pair(j).is_at_same_position(client_points[further_neighbors[client_pos][l]])){
//                     Array_Solution * copy_solution2 = copy_solution->copy();
//                     for (int k = j + 1; k < copy_solution2->get_size(); k++) { // Go forward and eliminates the points until finds an inspection point (client)
//                         type = m_map->get_stop_point(copy_solution2->get_solution_pair(k).get_x(), copy_solution2->get_solution_pair(k).get_y()).get_type();
//                         if(type == STOPS::DELIVER || type == STOPS::COLLECT){
//                             break;
//                         }
//                         copy_solution2->remove_solution_pair(k);
//                         k--;
//                     }

//                     Solution_Pair temp2 = copy_solution2->get_solution_pair(j).copy();
//                     copy_solution2->remove_solution_pair(j);
//                     int pos2 = j - 1; // position of the second point that will swap

//                     for (int k = pos2; k >= pos1; k--) { // Go backward and eliminates the points until finds an inspection point (client)
//                         type = m_map->get_stop_point(copy_solution2->get_solution_pair(k).get_x(), copy_solution2->get_solution_pair(k).get_y()).get_type();
//                         if(type == STOPS::DELIVER || type == STOPS::COLLECT || k == pos1){
//                             pos2 = k;
//                             break;
//                         }
//                         copy_solution2->remove_solution_pair(k);
//                     }

//                     copy_solution2->insert_solution_pair(temp, pos2); // insert the first point at the second position

//                     if(pos2 + 2 != copy_solution2->get_size()){
////                        sub_path2(copy_solution2, pos2+1, pos2+2, MATRIX_NUMBER_LINES, MATRIX_NUMBER_COLUMNS);
//                         sub_path3(copy_solution2, pos2 + 1, pos2+2);
//                     }

//                     if(pos2 != pos1){
////                        sub_path2(copy_solution2, pos2, pos2+1, MATRIX_NUMBER_LINES, MATRIX_NUMBER_COLUMNS);
//                         sub_path3(copy_solution2, pos2, pos2+1);
//                     }

//                     copy_solution2->insert_solution_pair(temp2, pos1); // insert the second point at the first position

////                     sub_path2(copy_solution2, pos1+1, pos1+2, MATRIX_NUMBER_LINES, MATRIX_NUMBER_COLUMNS);
////                     sub_path2(copy_solution2, pos1, pos1+1, MATRIX_NUMBER_LINES, MATRIX_NUMBER_COLUMNS);
//                     sub_path3(copy_solution2, pos1+1, pos1+2);
//                     sub_path3(copy_solution2, pos1, pos1+1);

//                     copy_solution2 = evaluate(copy_solution2, m_map);

//                     bool dominated_or_equal = false;
//                     if(!solutions[pos]->weakly_dominates(copy_solution2)){//if(copy_solution2->dominates(solutions[pos])){//copy_solution2->is_valid() &&
////                     if(copy_solution2->is_valid()){
//                         for (int k = 0; k < solutions.size(); k++) {
//                             if(solutions[k]->equals(copy_solution2)){
//                                 dominated_or_equal = true;
//                                 break;
//                             }
//                             if(solutions[k]->dominates(copy_solution2)){
//                                 dominated_or_equal = true;
//                                 break;
//                             }
//                         }

//                         if(!dominated_or_equal){
//                             return copy_solution2;
//                         }
//                     }

//                     delete copy_solution2;
//                 }
//            }
//            }

//            delete copy_solution;
//        }
//    }

//    return NULL;
//}

//Array_Solution * Core::swap6(int pos, vector<Array_Solution *> solutions) /* gera array de pares ordenados indicando um caminho */
//{
//    int type;
//    for (int i = 1; i < solutions[pos]->get_size(); i++) { // Run through all the solution except last point
//        type = m_map->get_stop_point(solutions[pos]->get_solution_pair(i).get_x(), solutions[pos]->get_solution_pair(i).get_y()).get_type(); // Type of the current point

//        if(type == STOPS::DELIVER || type == STOPS::COLLECT){ // If it is an inspection point (client)
//            Array_Solution * copy_solution = solutions[pos]->copy(); // Copy the current route

////            if(!copy_solution->get_solution_pair(copy_solution->get_size() - 1).is_neighbor_from(copy_solution->get_solution_pair(copy_solution->get_size() - 2))){
////                cout << "";
////            }else{
////                cout << "";
////            }

//            Solution_Pair temp = copy_solution->get_solution_pair(i).copy(); // Copy current point
//            int pos1 = 0; // position of the first point that will swap

//            for (int k = i + 1; k < copy_solution->get_size(); k++) { // Go forward and eliminates the points until finds an inspection point (client)
//                type = m_map->get_stop_point(copy_solution->get_solution_pair(k).get_x(), copy_solution->get_solution_pair(k).get_y()).get_type();
//                if(type == STOPS::DELIVER || type == STOPS::COLLECT){
//                    break;
//                }
//                copy_solution->remove_solution_pair(k);
//                k--;
//            }

//            copy_solution->remove_solution_pair(i); // Remove current inspection point

//            for (int k = i-1; k >= 0; k--) { // Go backward and eliminates the points until finds an inspection point (client)
//                type = m_map->get_stop_point(copy_solution->get_solution_pair(k).get_x(), copy_solution->get_solution_pair(k).get_y()).get_type();
//                if(type == STOPS::DELIVER || type == STOPS::COLLECT || k == 0){
//                    pos1 = k;
//                    break;
//                }
//                copy_solution->remove_solution_pair(k);
//            }

////            int client_pos;
////            for (int j = 0; j < client_points.size(); j++) {
////                if(client_points[j].is_at_same_position(temp)){
////                    client_pos = j;
////                    break;
////                }
////            }

////            cout << client_pos << endl;

//            vector<Solution_Pair> clients_temp = client_points; // Copy vector of inspection points (clients)
//            vector<int> random_neighbors;
//            int random_pos;
//            for (int n_count = 0; n_count < neighbors_qty; n_count++) { // Get 'neighbors_qty' clients to swap with the current one
//                do{
//                   random_pos = rand()%(clients_temp.size());
//                }while(clients_temp[random_pos].is_at_same_position(temp)); // While client is different from the current one

//                clients_temp.erase(clients_temp.begin() + random_pos); // Delete the client chosen from the copy vector
//                random_neighbors.push_back(random_pos);
//            }


//            for (int l = 0; l < random_neighbors.size(); l++) { // Run throught the vector of random chosen clients

//            for (int j = pos1+1; j < copy_solution->get_size(); j++) { // Run throught the current route forward the current client
////            for (int j = pos1+1; j < copy_solution->get_size(); j++) {
////                 type = m_map->get_stop_point(copy_solution->get_solution_pair(j).get_x(), copy_solution->get_solution_pair(j).get_y()).get_type();
////                 if(!copy_solution->get_solution_pair(j).equals(temp) && (type == STOPS::DELIVER || type == STOPS::COLLECT)){
//                if(copy_solution->get_solution_pair(j).is_at_same_position(client_points[random_neighbors[l]])){ // If the current point is any client from the chosen ones
//                     Array_Solution * copy_solution2 = copy_solution->copy(); // Copy the new route
//                     for (int k = j + 1; k < copy_solution2->get_size(); k++) { // Go forward and eliminates the points until finds an inspection point (client)
//                         type = m_map->get_stop_point(copy_solution2->get_solution_pair(k).get_x(), copy_solution2->get_solution_pair(k).get_y()).get_type(); // Get the type of current point
//                         if(type == STOPS::DELIVER || type == STOPS::COLLECT){
//                             break;
//                         }
//                         copy_solution2->remove_solution_pair(k);
//                         k--;
//                     }

//                     Solution_Pair temp2 = copy_solution2->get_solution_pair(j).copy(); // Copy the client2
//                     copy_solution2->remove_solution_pair(j);
//                     int pos2 = j - 1; // position of the second point that will swap

//                     for (int k = pos2; k >= pos1; k--) { // Go backward and eliminates the points until finds an inspection point (client)
//                         type = m_map->get_stop_point(copy_solution2->get_solution_pair(k).get_x(), copy_solution2->get_solution_pair(k).get_y()).get_type();
//                         if(type == STOPS::DELIVER || type == STOPS::COLLECT || k == pos1){
//                             pos2 = k;
//                             break;
//                         }
//                         copy_solution2->remove_solution_pair(k);
//                     }

//                     copy_solution2->insert_solution_pair(temp, pos2); // insert the first point at the second position

//                     if(pos2 + 2 != copy_solution2->get_size()){ // If client2 is not the last point from the route
////                        sub_path2(copy_solution2, pos2+1, pos2+2, MATRIX_NUMBER_LINES, MATRIX_NUMBER_COLUMNS);
//                         sub_path3(copy_solution2, pos2 + 1, pos2+2);
//                     }

//                     if(pos2 != pos1){ // If clients were not adjacents
////                        sub_path2(copy_solution2, pos2, pos2+1, MATRIX_NUMBER_LINES, MATRIX_NUMBER_COLUMNS);
//                         sub_path3(copy_solution2, pos2, pos2+1);

////                         if(!copy_solution2->get_solution_pair(copy_solution2->get_size() - 1).is_neighbor_from(copy_solution2->get_solution_pair(copy_solution2->get_size() - 2))){
////                             cout << "";
////                         }else{
////                             cout << "";
////                         }
//                     }

//                     copy_solution2->insert_solution_pair(temp2, pos1); // insert the second point at the first position

////                     sub_path2(copy_solution2, pos1+1, pos1+2, MATRIX_NUMBER_LINES, MATRIX_NUMBER_COLUMNS);
////                     sub_path2(copy_solution2, pos1, pos1+1, MATRIX_NUMBER_LINES, MATRIX_NUMBER_COLUMNS);
//                     sub_path3(copy_solution2, pos1+1, pos1+2);
//                     sub_path3(copy_solution2,   pos1, pos1+1);

////                     if(!copy_solution2->get_solution_pair(copy_solution2->get_size() - 1).is_neighbor_from(copy_solution2->get_solution_pair(copy_solution2->get_size() - 2))){
////                         cout << "";
////                     }else{
////                         cout << "";
////                     }

//                     copy_solution2 = evaluate(copy_solution2, m_map);

////                     if(!copy_solution2->get_solution_pair(copy_solution2->get_size() - 1).is_neighbor_from(copy_solution2->get_solution_pair(copy_solution2->get_size() - 2))){
////                         cout << "";
////                     }else{
////                         cout << "";
////                     }

//                     // New solution not dominated by old one
//                     bool dominated_or_equal = false;
//                     if(!solutions[pos]->weakly_dominates(copy_solution2)){//if(copy_solution2->dominates(solutions[pos])){//copy_solution2->is_valid() &&
////                     if(copy_solution2->is_valid()){
//                         for (int k = 0; k < solutions.size(); k++) {
//                             if(solutions[k]->equals(copy_solution2)){
//                                 dominated_or_equal = true;
//                                 break;
//                             }
//                             if(solutions[k]->dominates(copy_solution2)){
//                                 dominated_or_equal = true;
//                                 break;
//                             }
//                         }

//                         if(!dominated_or_equal){
//                             return copy_solution2;
//                         }
//                     }

//                     delete copy_solution2;
//                 }
//            }
//            }

//            random_neighbors.clear();
//            delete copy_solution;
//        }
//    }

//    return NULL;
//}

Array_Solution Core::swap6_multi(int pos, vector<Array_Solution> solutions) /* gera array de pares ordenados indicando um caminho */
{
    Route route;
    Array_Solution copy_solution, copy_solution2;
    Solution_Pair temp, temp2;
    vector<Solution_Pair> clients_temp;

    for (int pos_route = 0; pos_route < solutions[pos].get_size(); pos_route++) {
        route = solutions[pos].get_routes()[pos_route];

        int type;
        for (int i = 1; i < route.get_size(); i++) { // Run through all the solution except last point
            type = m_map->get_stop_point(route.get_solution_pair(i).get_x(),
                                         route.get_solution_pair(i).get_y()).get_type(); // Type of the current point

            if(type == STOPS::DELIVER || type == STOPS::COLLECT){ // If it is an inspection point (client)
//                Array_Solution * copy_solution = solutions[pos]->copy(); // Copy the current route
                copy_solution = solutions[pos].copy(); // memcheck

                temp = copy_solution.get_routes()[pos_route].get_solution_pair(i).copy(); // Copy current point
                int pos1 = 0; // position of the first point that will swap

                for (int k = i + 1; k < copy_solution.get_routes()[pos_route].get_size(); k++) { // Go forward and eliminates the points until finds an inspection point (client)
                    type = m_map->get_stop_point(copy_solution.get_routes()[pos_route].get_solution_pair(k).get_x(),
                                                 copy_solution.get_routes()[pos_route].get_solution_pair(k).get_y()).get_type();
                    if(type == STOPS::DELIVER || type == STOPS::COLLECT){
                        break;
                    }
                    copy_solution.routes[pos_route].remove_solution_pair(k);
                    k--;
                }

                copy_solution.routes[pos_route].remove_solution_pair(i); // Remove current inspection point

                for (int k = i-1; k >= 0; k--) { // Go backward and eliminates the points until finds an inspection point (client)
                    type = m_map->get_stop_point(copy_solution.get_routes()[pos_route].get_solution_pair(k).get_x(),
                                                 copy_solution.get_routes()[pos_route].get_solution_pair(k).get_y()).get_type();
                    if(type == STOPS::DELIVER || type == STOPS::COLLECT || k == 0){
                        pos1 = k;
                        break;
                    }
                    copy_solution.routes[pos_route].remove_solution_pair(k);
                }

                clients_temp = client_points; // Copy vector of inspection points (clients)
                vector<int> random_neighbors;
                int random_pos;
                for (int n_count = 0; n_count < neighbors_qty; n_count++) { // Get 'neighbors_qty' clients to swap with the current one
                    do{
                       random_pos = rand()%(clients_temp.size());
                    }while(clients_temp[random_pos].is_at_same_position(temp)); // While client is different from the current one

                    clients_temp.erase(clients_temp.begin() + random_pos); // Delete the client chosen from the copy vector
                    random_neighbors.push_back(random_pos);
                }


                for (int l = 0; l < random_neighbors.size(); l++) { // Run throught the vector of random chosen clients

                    for (int j = pos1+1; j < copy_solution.get_routes()[pos_route].get_size(); j++) { // Run throught the current route forward the current client
                        if(copy_solution.get_routes()[pos_route].get_solution_pair(j).is_at_same_position(client_points[random_neighbors[l]])){ // If the current point is any client from the chosen ones
//                            Array_Solution * copy_solution2 = copy_solution->copy(); // Copy the new route
                            copy_solution2 = copy_solution.copy(); //memcheck

                            for (int k = j + 1; k < copy_solution2.get_routes()[pos_route].get_size(); k++) { // Go forward and eliminates the points until finds an inspection point (client)
                                type = m_map->get_stop_point(copy_solution2.get_routes()[pos_route].get_solution_pair(k).get_x(),
                                                             copy_solution2.get_routes()[pos_route].get_solution_pair(k).get_y()).get_type(); // Get the type of current point
                                if(type == STOPS::DELIVER || type == STOPS::COLLECT){
                                    break;
                                }
                                copy_solution2.routes[pos_route].remove_solution_pair(k);
                                k--;
                            }

                            temp2 = copy_solution2.get_routes()[pos_route].get_solution_pair(j).copy(); // Copy the client2
                            copy_solution2.routes[pos_route].remove_solution_pair(j);
                            int pos2 = j - 1; // position of the second point that will swap

                            for (int k = pos2; k >= pos1; k--) { // Go backward and eliminates the points until finds an inspection point (client)
                                type = m_map->get_stop_point(copy_solution2.get_routes()[pos_route].get_solution_pair(k).get_x(),
                                                             copy_solution2.get_routes()[pos_route].get_solution_pair(k).get_y()).get_type();
                                if(type == STOPS::DELIVER || type == STOPS::COLLECT || k == pos1){
                                    pos2 = k;
                                    break;
                                }
                                copy_solution2.routes[pos_route].remove_solution_pair(k);
                            }

                            copy_solution2.routes[pos_route].insert_solution_pair(temp, pos2); // insert the first point at the second position

                            if(pos2 + 2 != copy_solution2.get_routes()[pos_route].get_size()){ // If client2 is not the last point from the route
                                copy_solution2.routes[pos_route] = sub_path3(copy_solution2.get_routes()[pos_route], pos2 + 1, pos2+2);
                            }

                            if(pos2 != pos1){ // If clients were not adjacents
                                copy_solution2.routes[pos_route] = sub_path3(copy_solution2.get_routes()[pos_route], pos2, pos2+1);
                            }

                            copy_solution2.routes[pos_route].insert_solution_pair(temp2, pos1); // insert the second point at the first position

                            copy_solution2.routes[pos_route] = sub_path3(copy_solution2.get_routes()[pos_route], pos1+1, pos1+2);
                            copy_solution2.routes[pos_route] = sub_path3(copy_solution2.get_routes()[pos_route],   pos1, pos1+1);

                            copy_solution2 = evaluate(copy_solution2, m_map);

                            // New solution not dominated by old one
                            bool dominated_or_equal = false;
                            if(!solutions[pos].weakly_dominates(copy_solution2)){
                                for (int k = 0; k < solutions.size(); k++) {
                                    if(solutions[k].equals(copy_solution2)){
                                        dominated_or_equal = true;
                                        break;
                                    }
                                    if(solutions[k].dominates(copy_solution2)){
                                        dominated_or_equal = true;
                                        break;
                                    }
                                }

                                if(!dominated_or_equal){
                                    return copy_solution2;
                                }
                            }

//                            delete copy_solution2;
                        }
                    }
                }

                random_neighbors.clear();
//                delete copy_solution;
            }

        } //END FOR 'route'

    } // END FOR 'routes'

    return Array_Solution(true);
}


//Array_Solution * Core::initial_solution(int num_lines, int num_columns, Array_Solution * solution) /* gera array de pares ordenados indicando um caminho */
//{
//    // Insere na matriz de solucao a posicao gerada
////    Solution_Pair e(0, 0);
////    solution->set_solution_pair(e, 0);
//    solution->set_solution_pair(origin_x, origin_y, 0);

//    // Proximo passo
//    return initial_solution_loop(1, origin_x, origin_y, num_lines, num_columns, solution);
//}

//Array_Solution * Core::initial_solution_loop(int current_time, int x, int y, int num_lines, int num_columns, Array_Solution * solution) /* gera array de pares ordenados indicando um caminho */
//{
//    // Se ja chegou ao tempo limite, retorna
//    if (current_time == MAX_TIME / TIME_INTERVAL + 1)
//        return solution;

//    int random_x = 0,
//        random_y = 0;

//    do {
//        // Gera posicoes adjacentes a atual randomicamente
//        random_x = rand() % 3 - 1 + x;
//        random_y = rand() % 3 - 1 + y;
//    } while (random_x < 0 || random_x >= num_columns || random_y < 0 || random_y >= num_lines || (random_x == x && random_y == y)); // Enquanto está dentro do perimetro do mapa e andou para um ponto diferente do atual

//    // Insere na matriz de solucao a posicao gerada
////    Solution_Pair e(random_x, random_y);
////    solution->set_solution_pair(e, current_time);
//    solution->set_solution_pair(random_x, random_y, current_time);

//    // Proximo passo
//    return initial_solution_loop(current_time + 1, random_x, random_y, num_lines, num_columns, solution);
//}

//Array_Solution * Core::perturb_loop(int current_pos, int max_pos, Solution_Pair e_old, int num_lines, int num_columns, Array_Solution * solution) /* gera array de pares ordenados indicando um caminho */
//Array_Solution * Core::perturb_loop(int current_pos, int max_pos, int e_old_x, int e_old_y, int num_lines, int num_columns, Array_Solution * solution) /* gera array de pares ordenados indicando um caminho */
//{
//    // Se ja chegou ao tempo limite, retorna
//    if (current_pos >= max_pos)
//        return evaluate(solution, m_map);

//    int random_x = 0,
//        random_y = 0;

//    int x = e_old_x,
//        y = e_old_y;

//    do {
//        // Gera posicoes adjacentes a atual randomicamente
//        random_x = rand() % 3 - 1 + x;
//        random_y = rand() % 3 - 1 + y;
//    } while (random_x < 0 || random_x >= num_columns || random_y < 0 || random_y >= num_lines || (random_x == x && random_y == y)); // Enquanto está dentro do perimetro do mapa e andou para um ponto diferente do atual

//                                                                                                                                  // Insere na matriz de solucao a posicao gerada
////    Solution_Pair e_new(random_x, random_y);
////    solution->set_solution_pair(e_new, current_pos);
//    solution->set_solution_pair(random_x, random_y, current_pos);

//    // Proximo passo
//    return perturb_loop(current_pos + 1, max_pos, random_x, random_y, num_lines, num_columns, solution);
//}

//Array_Solution * Core::perturb(Array_Solution * solution, int num_lines, int num_columns, int new_size) /* gera array de pares ordenados indicando um caminho */
//{
//    int max_size = solution->get_size() > new_size ? new_size : solution->get_size();
//    int random_pos = max_size < 3 ? max_size : (rand() % (max_size - 2)) + 1;

//    Array_Solution * new_solution = new Array_Solution(new_size);

//    for (int count = 0; count < max_size; ++count) {
//        new_solution->add_solution_pair(solution->get_solution_pair(count));
//    }

////    delete solution;

//    return perturb_loop(random_pos + 1, new_size, new_solution->get_solution_pair(random_pos).get_x(), new_solution->get_solution_pair(random_pos).get_y(), num_lines, num_columns, new_solution);
//}

//Array_Solution * Core::increment(Array_Solution * solution, int num_lines, int num_columns, int new_size) /* gera array de pares ordenados indicando um caminho */
//{
//    int old_size = solution->get_size();

//    Array_Solution * new_solution = new Array_Solution(new_size);

//    for (int count = 0; count < old_size; ++count) {
//        new_solution->set_solution_pair(solution->get_solution_pair(count), count);
//    }

////    delete solution;

//    return perturb_loop(old_size, new_size, new_solution->get_solution_pair(old_size - 1).get_x(), new_solution->get_solution_pair(old_size - 1).get_y(), num_lines, num_columns, new_solution);
//}

//Array_Solution * Core::change_final(Array_Solution * solution, int num_lines, int num_columns) /* gera array de pares ordenados indicando um caminho */
//{
//    int old_size = (rand() % (solution->get_size() - 2)) + 1;
//    int new_size = solution->get_size();

//    Array_Solution * new_solution = new Array_Solution(new_size);

//    for (int count = 0; count < old_size; ++count) {
//        new_solution->set_solution_pair(solution->get_solution_pair(count), count);
//    }

////    delete solution;

//    return perturb_loop(old_size, new_size, new_solution->get_solution_pair(old_size - 1).get_x(), new_solution->get_solution_pair(old_size - 1).get_y(), num_lines, num_columns, new_solution);
//}

//Array_Solution * Core::swap2(Array_Solution * a_solution, int number_lines, int number_columns) /* change a random stretch for   */
//{
//    int random_pos = -1;
//    Array_Solution * a_adjacents_1, * a_adjacents_2, * a_intersection;
//    do {
//        if(random_pos != -1){
//            delete a_intersection;
//        }

//        random_pos = (rand() % (a_solution->get_size() - 2)) + 1;

//        a_adjacents_1 = Array_Tools::adjacents(a_solution->get_solution_pair(random_pos).get_x()    , a_solution->get_solution_pair(random_pos).get_y()    , number_lines, number_columns, true);

//        a_adjacents_2 = Array_Tools::adjacents(a_solution->get_solution_pair(random_pos + 1).get_x(), a_solution->get_solution_pair(random_pos + 1).get_y(), number_lines, number_columns, true);

//        a_intersection = a_adjacents_1->intersection(a_adjacents_2);

//        delete a_adjacents_1;
//        delete a_adjacents_2;
//    } while (a_intersection->get_size() == 0);

//    Array_Solution * a_new_solution = a_solution->copy();
//    a_new_solution->merge(a_intersection, random_pos + 1);

//    return a_new_solution;
//}

//Array_Solution * Core::swap(Array_Solution * a_solution, int number_lines, int number_columns) /* change a random stretch for   */
//{
//    int random_pos = -1;
//    Array_Solution * a_adjacents_1, * a_adjacents_2, * a_intersection;
//    do {
//        if(random_pos != -1){
//            delete a_intersection;
//        }

//        random_pos = (rand() % (a_solution->get_size() - 2)) + 1;

//        a_adjacents_1 = Array_Tools::adjacents(a_solution->get_solution_pair(random_pos).get_x()    , a_solution->get_solution_pair(random_pos).get_y()    , number_lines, number_columns, true);

//        a_adjacents_2 = Array_Tools::adjacents(a_solution->get_solution_pair(random_pos + 1).get_x(), a_solution->get_solution_pair(random_pos + 1).get_y(), number_lines, number_columns, true);

//        a_intersection = a_adjacents_1->intersection(a_adjacents_2);

//        delete a_adjacents_1;
//        delete a_adjacents_2;
//    } while (a_intersection->get_size() == 0);

//    int pos_intersection = rand() % a_intersection->get_size();

//    Array_Solution * a_new_solution = new Array_Solution(a_solution->get_size());

//    int new_sol_count = 0,
//        old_sol_count = 0;

//    /*cout << "\nPARES ORDENADOS: ";
//    a_solution.get_solution_pair(random_pos).print();
//    cout << " ";
//    a_solution.get_solution_pair(random_pos + 1).print();
//    cout << endl;

//    cout << "\nADJACENTES 1: \n";
//    a_adjacents_1.print();
//    cout << "\nADJACENTES 2: \n";
//    a_adjacents_2.print();
//    cout << "\nINTERSECAO: \n";
//    a_intersection.print();*/

//    while (new_sol_count < a_new_solution->get_size())
//    {
//        a_new_solution->set_solution_pair(a_solution->get_solution_pair(old_sol_count), new_sol_count);

//        new_sol_count++;

//        if (old_sol_count == random_pos) {
//            a_new_solution->set_solution_pair(a_intersection->get_solution_pair(pos_intersection), new_sol_count);

//            new_sol_count++;
//        }

//        old_sol_count++;
//    }

//    a_new_solution = evaluate(a_new_solution, m_map);

//    delete a_intersection;
////    delete a_solution;

//    return a_new_solution;
//}

//Array_Path * Core::get_paths(string directory_name)/* return the route with a sub path from START point to END modified */
//{
//    Array_Solution * a_s_path;
//    Path * path;
//    Array_Path * a_path = new Array_Path();

//    string paths_directory_name = directory_name + "/paths";
//    string d_name;

//    DIR * dir;
//    struct dirent * ent;
//    if((dir = opendir(paths_directory_name.c_str())) == NULL){
//        cout << "Directory does not exist" << endl;
//        return a_path;
//    } else {
//        while((ent = readdir(dir)) != NULL){
//            d_name = ent->d_name;
//            if(d_name.compare(".") != 0 && d_name.compare("..") != 0){
//                a_s_path = Array_Tools::from_file_solution(paths_directory_name + "/" + d_name);

//                path = new Path (a_s_path->get_solution_pair(0).get_position(), a_s_path->get_solution_pair(a_s_path->get_size() - 1).get_position(), a_s_path->copy());
//                a_path->add_path(path);
//                delete path;

//                delete a_s_path;
//            }

//        }

//        closedir(dir);
//    }

//    return a_path;
//}

void Core::set_paths(Matrix * map, vector<Solution_Pair> client_points, vector<Solution_Pair> recharge_points, string directory_name) /* return the route with a sub path from START point to END modified */
{
    string paths_directory_name = directory_name + "/paths";
    mkdir(paths_directory_name.c_str(), 0777);

    Route path;

    // Instantiate origin
    Stop_Point origin = map->get_stop_point(origin_x,origin_y);

    // If origin is not client neither recharge point
    if(origin.get_type() == STOPS::NOTHING){
        // Distance between origin and recharge points
        foreach (Solution_Pair recharge, recharge_points) {
            ifstream path_file(directory_name + "/paths/" +
                                to_string(origin_x) + "_" + to_string(origin_y) + "_" + //"0_0_" +
                                to_string(recharge.get_x()) + "_" + to_string(recharge.get_y()) + ".txt");

            if ( !path_file ){
                path = a_star(map, origin, map->get_stop_point(recharge));
                path.to_file(directory_name + "/paths/" +
                              to_string(origin_x) + "_" + to_string(origin_y) + "_" + //"0_0_" +
                              to_string(recharge.get_x()) + "_" + to_string(recharge.get_y()) + ".txt");

                // Reverse order
                path = path.invert();
                path.to_file(directory_name + "/paths/" +
                              to_string(recharge.get_x()) + "_" + to_string(recharge.get_y()) + "_" +
                              to_string(origin_x) + "_" + to_string(origin_y) + ".txt");//            +"0_0.txt");

//                delete path;
            }
        }

        // Distance between origin and clients
        for (int index = 0; index < client_points.size(); index++) {
            ifstream path_file(directory_name + "/paths/" +
                               to_string(origin_x) + "_" + to_string(origin_y) + "_" + //"0_0_" +
                               to_string(client_points[index].get_x()) + "_" + to_string(client_points[index].get_y()) +".txt");

            if ( !path_file ){
                path = a_star(map, origin, map->get_stop_point(client_points[index]));
                path.to_file(directory_name + "/paths/" +
                              to_string(origin_x) + "_" + to_string(origin_y) + "_" + //"0_0_" +
                              to_string(client_points[index].get_x()) + "_" + to_string(client_points[index].get_y()) +".txt");

                // Reverse order
                path = path.invert();
                path.to_file(directory_name + "/paths/" +
                              to_string(client_points[index].get_x()) + "_" + to_string(client_points[index].get_y()) + "_" +
                              to_string(origin_x) + "_" + to_string(origin_y) + ".txt");//            +"0_0.txt");

//                delete path;
            }
        }
    }

    // Distance between clients and recharge points
    foreach (Solution_Pair client, client_points) {
        foreach (Solution_Pair recharge, recharge_points) {
            ifstream path_file(directory_name + "/paths/" +
                               to_string(client.get_x())   + "_" + to_string(client.get_y())   + "_" +
                               to_string(recharge.get_x()) + "_" + to_string(recharge.get_y()) +".txt");

            if ( !path_file ){
                path = a_star(map, map->get_stop_point(client), map->get_stop_point(recharge));
                path.to_file(directory_name + "/paths/" +
                              to_string(client.get_x())   + "_" + to_string(client.get_y())   + "_" +
                              to_string(recharge.get_x()) + "_" + to_string(recharge.get_y()) +".txt");

                // Reverse order
                path = path.invert();
                path.to_file(directory_name + "/paths/" +
                              to_string(recharge.get_x()) + "_" + to_string(recharge.get_y()) + "_" +
                              to_string(client.get_x())   + "_" + to_string(client.get_y())   + ".txt");

//                delete path;
            }
        }
    }

    // Distance between recharge points
    for (int index_1 = 0; index_1 < recharge_points.size() - 1; index_1++) {
        for (int index_2 = index_1 + 1; index_2 < recharge_points.size(); index_2++) {
            ifstream path_file(directory_name + "/paths/" +
                               to_string(recharge_points[index_1].get_x()) + "_" + to_string(recharge_points[index_1].get_y()) + "_" +
                               to_string(recharge_points[index_2].get_x()) + "_" + to_string(recharge_points[index_2].get_y()) +".txt");

            if ( !path_file ){
                path = a_star(map, map->get_stop_point(recharge_points[index_1]), map->get_stop_point(recharge_points[index_2]));
                path.to_file(directory_name + "/paths/" +
                              to_string(recharge_points[index_1].get_x()) + "_" + to_string(recharge_points[index_1].get_y()) + "_" +
                              to_string(recharge_points[index_2].get_x()) + "_" + to_string(recharge_points[index_2].get_y()) +".txt");

                // Reverse order
                path = path.invert();
                path.to_file(directory_name + "/paths/" +
                              to_string(recharge_points[index_2].get_x()) + "_" + to_string(recharge_points[index_2].get_y()) + "_" +
                              to_string(recharge_points[index_1].get_x()) + "_" + to_string(recharge_points[index_1].get_y()) +".txt");

//                delete path;
            }
        }
    }

    // Distance between clients
    for (int index_1 = 0; index_1 < client_points.size() - 1; index_1++) {
        for (int index_2 = index_1 + 1; index_2 < client_points.size(); index_2++) {
            ifstream path_file(directory_name + "/paths/" +
                               to_string(client_points[index_1].get_x()) + "_" + to_string(client_points[index_1].get_y()) + "_" +
                               to_string(client_points[index_2].get_x()) + "_" + to_string(client_points[index_2].get_y()) +".txt");

            if ( !path_file ){
                path = a_star(map, map->get_stop_point(client_points[index_1]), map->get_stop_point(client_points[index_2]));
                path.to_file(directory_name + "/paths/" +
                              to_string(client_points[index_1].get_x()) + "_" + to_string(client_points[index_1].get_y()) + "_" +
                              to_string(client_points[index_2].get_x()) + "_" + to_string(client_points[index_2].get_y()) +".txt");

                // Reverse order
                path = path.invert();
                path.to_file(directory_name + "/paths/" +
                              to_string(client_points[index_2].get_x()) + "_" + to_string(client_points[index_2].get_y()) + "_" +
                              to_string(client_points[index_1].get_x()) + "_" + to_string(client_points[index_1].get_y()) +".txt");

//                delete path;
            }
        }
    }

}

Route Core::a_star(Matrix * map, Stop_Point start, Stop_Point end) /* return the route with a sub path from START point to END modified */
{

    // Create start and end node
    A_Star_Node * start_node = new A_Star_Node(NULL, start);
    A_Star_Node * end_node   = new A_Star_Node(NULL, end);

    // Initialize both open and closed list
    vector<A_Star_Node *> open_list;
    vector<A_Star_Node *> closed_list;

    // Add the start node
    open_list.push_back(start_node);

    // Current node
    A_Star_Node * current_node;
    int current_index;


//    vector<A_Star_Node *> children;
//    A_Star_Node * child;
//    A_Star_Node * closed_child;
//    A_Star_Node * open_node;

    // Loop until you find the end
    while(!open_list.empty()){

        // Get the current node of smaller value of f
        current_node = open_list[0];
        current_index = 0;

        for (int index = 0; index < open_list.size(); index++) {
            if(open_list[index]->get_f() < current_node->get_f()){
                current_node = open_list[index];
                current_index = index;
            }
        }

        // Pop current node off open list and add it to the closed list
        open_list.erase(open_list.begin() + current_index);
        closed_list.push_back(current_node);

        // Found the goal
        if(current_node->equals(end_node)){
            vector<Stop_Point> temp;
            Route path(temp.size());
            A_Star_Node * current = current_node;

            while (current != NULL) {
                temp.push_back(current->get_position());
                current = current->get_parent();
            }

            // Reverse the path
            for (int i = temp.size() - 1; i >= 0; i--) {
                Solution_Pair p = Solution_Pair(temp[i]);
                path.add_solution_pair(p);
            }

            // Delete from memory
//            delete current;
//            delete current_node;
//            delete start_node;
//            delete end_node;
//            delete open_node;
//            delete child;
//            delete closed_child;

            for (int i = 0; i < open_list.size(); i++) {
                delete open_list[i];
            }

            for (int i = 0; i < closed_list.size(); i++) {
                delete closed_list[i];
            }

            temp.clear();

            // Return
            return path;
        }

        // Generate children
        vector<A_Star_Node *> children = current_node->adjacents(map, true);

        // Loop through children        
        foreach (A_Star_Node * child, children) {

            // Child is on the closed list
            foreach (A_Star_Node * closed_child, closed_list) {
                if(child->equals(closed_child)){
//                    continue;
                    goto next_iteration;
                }
            }

            // Set f, g and h values
            child->set_g(current_node->get_g() + 1);
            child->set_h(pow(child->get_position().get_x() - end_node->get_position().get_x(), 2) + pow(child->get_position().get_y() - end_node->get_position().get_y(), 2));
            child->set_f(child->get_g() + child->get_h());

            // Child is already on the open list and better path
            foreach (A_Star_Node * open_node, open_list) {
                if(child->equals(open_node) && child->get_g() > open_node->get_g()){
//                    continue;
                    goto next_iteration;
                }
            }

            // Add the child to the open list
            open_list.push_back(child);

            next_iteration:;
        }
    }
}

Route Core::sub_path3(Route route, int start, int end) /* return the route with a sub path from START point to END */
{
    auto start_time = std::chrono::system_clock::now();

    // Get extreme points
    Solution_Pair start_pair  = route.get_solution_pair(start);
    Solution_Pair end_pair    = route.get_solution_pair(end);

    if(start_pair.is_at_same_position(end_pair)){
        route.remove_solution_pair(end);
        return route;
    }

    // Get path between start and end points
    Route sub_path_route;
    if(pre_processing){
        // Get path from struct pre-generated
        sub_path_route = Array_Tools::from_file_route(global_file_name + "/paths/" +
                                                                        to_string(start_pair.get_x()) + "_" + to_string(start_pair.get_y()) + "_" +
                                                                        to_string(end_pair.get_x()) + "_" + to_string(end_pair.get_y()) +".txt");
    } else {
        // Generate path with A* heuristic
        sub_path_route = a_star(m_map, m_map->get_stop_point(start_pair), m_map->get_stop_point(end_pair));
    }

//    Path * path = best_paths->get_path(start_pair.get_position(), end_pair.get_position());
//    Array_Solution * sub_path_solution = path->get_path()->copy();

    // Merge paths
    route.merge_without_start_and_end(sub_path_route, start);

//    delete sub_path_route;

    auto end_time = std::chrono::system_clock::now();
    std::chrono::duration<double> diff_time = end_time - start_time;
    sub_path3_qty++;
    sub_path3_time += diff_time.count();

    return route;
}

//void Core::sub_path2(Array_Solution * a_solution, int start, int end, int number_lines, int number_columns) /* return the route with a sub path from START point to END modified */
//{
//    int random_pos;
//    Array_Solution * a_adjacents;

//    Array_Solution * sub_path_solution = new Array_Solution(a_solution->get_size());
//    Solution_Pair start_pair  = a_solution->get_solution_pair(start);
//    Solution_Pair end_pair    = a_solution->get_solution_pair(end);
//    Solution_Pair actual_pair = a_solution->get_solution_pair(start);
//    Solution_Pair temp_pair;

//    while(!Array_Tools::are_adjacents(actual_pair, end_pair)){ // !actual_pair.is_at_same_position(end_pair)){
//        a_adjacents = Array_Tools::adjacents(actual_pair.get_x(), actual_pair.get_y(), number_lines, number_columns, true, actual_pair.is_west_from(end_pair), actual_pair.is_east_from(end_pair), actual_pair.is_north_from(end_pair), actual_pair.is_south_from(end_pair));
//        int i = 0;

//        // Avoiding prohibited points
//        do{
//            random_pos = a_adjacents->get_size() == 1 ? 0 : (rand() % (a_adjacents->get_size() - 1));
//            temp_pair = a_adjacents->get_solution_pair(random_pos).copy();

//            if(i == a_adjacents->get_size()){
//                delete a_adjacents;
//                a_adjacents = Array_Tools::adjacents(actual_pair.get_x(), actual_pair.get_y(), number_lines, number_columns, true, 1, 1, 1, 1);
//            }

//            i++;
//        } while (m_map->get_stop_point(temp_pair.get_x(), temp_pair.get_y()).get_type() == STOPS::PROHIBITED);

//        actual_pair = temp_pair.copy();

//        sub_path_solution->add_solution_pair(actual_pair);

//        delete a_adjacents;
//    }

//    a_solution->merge(sub_path_solution, start);

//    delete sub_path_solution;
//}

//Array_Solution * Core::sub_path(Array_Solution * a_solution, int start, int end, int number_lines, int number_columns) /* return the route with a sub path from START point to END modified */
//{
//    int random_pos;
//    Array_Solution * a_adjacents;

//    int sub_path_count = 0;
//    Array_Solution * sub_path_solution = new Array_Solution(a_solution->get_size());
//    Solution_Pair start_pair  = a_solution->get_solution_pair(start);
//    Solution_Pair end_pair    = a_solution->get_solution_pair(end);
//    Solution_Pair actual_pair = a_solution->get_solution_pair(start);

//    while(!actual_pair.is_at_same_position(end_pair)){
//        a_adjacents = Array_Tools::adjacents(actual_pair.get_x(), actual_pair.get_y(), number_lines, number_columns, true, actual_pair.is_west_from(end_pair), actual_pair.is_east_from(end_pair), actual_pair.is_north_from(end_pair), actual_pair.is_south_from(end_pair));

//        random_pos = a_adjacents->get_size() == 1 ? 0 : (rand() % (a_adjacents->get_size() - 1));

//        actual_pair = a_adjacents->get_solution_pair(random_pos).copy();

//        sub_path_solution->set_solution_pair(actual_pair, sub_path_count);
//        sub_path_count++;

//        delete a_adjacents;
//    }

//    int new_sol_count = 0,
//        old_sol_count = 0;

//    Array_Solution * a_new_solution = new Array_Solution(a_solution->get_size() + sub_path_count);

//    while (old_sol_count < a_solution->get_size())
//    {
//        if (old_sol_count == start + 1) {
//            for (int count = 0; count < sub_path_count; count++) {
//                a_new_solution->set_solution_pair(sub_path_solution->get_solution_pair(count), new_sol_count);
//                new_sol_count++;
//            }
//            old_sol_count = end + 1;
//        }else{
//            a_new_solution->set_solution_pair(a_solution->get_solution_pair(old_sol_count), new_sol_count);
//            new_sol_count++;
//            old_sol_count++;
//        }
//    }

//    a_new_solution->resize(new_sol_count);
//    a_new_solution = evaluate(a_new_solution, m_map);

//    delete sub_path_solution;
////    delete a_solution;

//    return a_new_solution;
//}

//Array_Solution * Core::random_sub_path(Array_Solution * a_solution, int number_lines, int number_columns) /* return the route with a random sub path modified */
//{
//    int start = (rand() % (a_solution->get_size() - 2)) + 1;
//    int end   = (rand() % (a_solution->get_size() - start - 1)) + start + 1;

//    return sub_path(a_solution, start, end, number_lines, number_columns);
//}

//Array_Solution * Core::random_sub_path(Array_Solution * a_solution, int number_lines, int number_columns) /* return the route with a random sub path modified */
//{
//    bool flag = false;

//    Array_Solution * a_new_solution = a_solution->copy();

//    int start, end, x, y, type, count = 0, count_max = (int) a_new_solution->get_size() * 0.1;

////    do{

//    do{
//        start = (rand() % (a_new_solution->get_size() - 2)) + 1;

//        x = a_new_solution->get_solution_pair(start).get_x();
//        y = a_new_solution->get_solution_pair(start).get_y();

//        type = m_map->get_stop_point(x, y).get_type();

//    }while(type != STOPS::NOTHING);

////        end   = (rand() % (a_new_solution->get_size() - start - 1)) + start + 1;
//        end = a_solution->get_size();
//        count_max = a_solution->get_size();

//        for (int current_pos = start; current_pos < count_max; current_pos++) {
//            x = a_new_solution->get_solution_pair(current_pos).get_x();
//            y = a_new_solution->get_solution_pair(current_pos).get_y();

//            type = m_map->get_stop_point(x, y).get_type();

//            if(type != STOPS::NOTHING){
//                end = current_pos;
//                flag = true;
//                break;
//            }
//        }

//        if(!flag || end - start <= 1){
//            return a_new_solution;
//        }

//        return sub_path(a_solution, start, end, number_lines, number_columns);
//}

bool Core::can_change_charge_rate(Route route, int pos, int value) { /* verify if the new recharge rate is valid */
    int new_value = route.get_solution_pair(pos).get_charge_rate() + value;

    return (new_value <= MAX_CHARGE_RATE) && (new_value >= MIN_CHARGE_RATE);
}

Array_Solution Core::change_charge_rate2(int pos, vector<Array_Solution> solutions, int value) /* get a random sub path from the route and change the recharge rate from of each stretch */
{
    int type;
    Array_Solution copy_solution;

    // Run through all routes
    for (int pos_route = 0; pos_route < solutions[pos].get_size(); pos_route++) {

        // Run through the route
        for (int i = 0; i < solutions[pos].get_routes()[pos_route].get_size(); i++) {
            type = m_map->get_stop_point(solutions[pos].get_routes()[pos_route].get_solution_pair(i).get_x(),
                                         solutions[pos].get_routes()[pos_route].get_solution_pair(i).get_y()).get_type();
            if(type == STOPS::CHARGE && can_change_charge_rate(solutions[pos].get_routes()[pos_route], i, value)){


//                Array_Solution * copy_solution = solutions[pos]->copy();
                copy_solution = solutions[pos].copy(); //memcheck

                Solution_Pair pair(copy_solution.get_routes()[pos_route].get_solution_pair(i).get_x(),
                                   copy_solution.get_routes()[pos_route].get_solution_pair(i).get_y(),
                                   copy_solution.get_routes()[pos_route].get_solution_pair(i).get_vel(),
                                   copy_solution.get_routes()[pos_route].get_solution_pair(i).get_charge_rate() + value);
                copy_solution.routes[pos_route].set_solution_pair(pair, i);

//                copy_solution.routes[pos_route].get_solution_pair(i).add_charge_rate(value);


                copy_solution = evaluate(copy_solution, m_map);

                bool dominated_or_equal = false;
    //            if(copy_solution->is_valid() && copy_solution->dominates(solutions[pos])){
                if(!solutions[pos].weakly_dominates(copy_solution)){//
                    for (int k = 0; k < solutions.size(); k++) {
                        if(solutions[k].equals(copy_solution)){
                            dominated_or_equal = true;
                            break;
                        }
                        if(solutions[k].dominates(copy_solution)){
                            dominated_or_equal = true;
                            break;
                        }
                    }

                    if(!dominated_or_equal){
                        return copy_solution;
                    }
                }//

//                delete copy_solution;
            }
        }
    }

    return Array_Solution(true);
}

//Array_Solution * Core::change_vel2(int pos, vector<Array_Solution *> solutions, int value) /* get a random sub path from the route and change the recharge rate from of each stretch */
//{
//    int type;
//    for (int i = 0; i < solutions[pos]->get_size(); i++) {
//        if(can_change_vel(solutions[pos], i, value)){


//            Array_Solution * copy_solution = solutions[pos]->copy();
//            Solution_Pair pair(copy_solution->get_solution_pair(i).get_x(),
//                               copy_solution->get_solution_pair(i).get_y(),
//                               copy_solution->get_solution_pair(i).get_vel() + value,
//                               copy_solution->get_solution_pair(i).get_charge_rate());
//            copy_solution->set_solution_pair(pair, i);


//            copy_solution = evaluate(copy_solution, m_map);

//            bool dominated_or_equal = false;
//            if(solutions[pos]->weakly_dominates(copy_solution)){//if(copy_solution->is_valid() && copy_solution->dominates(solutions[pos])) {
//                for (int k = 0; k < solutions.size(); k++) {
//                    if(solutions[k]->equals(copy_solution)){
//                        dominated_or_equal = true;
//                        break;
//                    }
//                    if(solutions[k]->dominates(copy_solution)){
//                        dominated_or_equal = true;
//                        break;
//                    }
//                }

//                if(!dominated_or_equal){
//                    return copy_solution;
//                }
//            }

//            delete copy_solution;
//        }
//    }

//    return NULL;
//}

//Array_Solution * Core::change_charge_rate(Array_Solution * a_solution, int value) /* get a random sub path from the route and change the recharge rate from of each stretch */
//{
////    int best = -1;
//    int x, y, type;

//    Array_Solution * new_solution = a_solution->copy();

////    for (int current_pos = 1; current_pos < a_solution->get_size(); current_pos++) {
////        x = a_solution->get_solution_pair(current_pos).get_x();
////        y = a_solution->get_solution_pair(current_pos).get_y();

////        type = m_map->get_stop_point(x, y).get_type();

////        if(type == STOPS::CHARGE && can_change_charge_rate(a_solution, current_pos, value)){
////            a_solution->get_solution_pair(current_pos).set_charge_rate(a_solution->get_solution_pair(current_pos).get_charge_rate() + value);
////            a_solution = evaluate(a_solution, m_map);

////            if(!best_solution->dominates(a_solution) && a_solution->is_valid()){
////                delete best_solution;
////                best_solution = a_solution->copy();

////                best = current_pos;
////            }

////            a_solution->get_solution_pair(current_pos).set_charge_rate(a_solution->get_solution_pair(current_pos).get_charge_rate() - value);
////        }
////    }

//    int start = (rand() % (new_solution->get_size() - 2)) + 1;
//    int end   = (rand() % (new_solution->get_size() - start - 1)) + start + 1;


//    for (int current_pos = start; current_pos < end; current_pos++) {
//        x = a_solution->get_solution_pair(current_pos).get_x();
//        y = a_solution->get_solution_pair(current_pos).get_y();

//        type = m_map->get_stop_point(x, y).get_type();

//        if(type == STOPS::CHARGE && can_change_charge_rate(new_solution, current_pos, value)){
//            Solution_Pair pair(new_solution->get_solution_pair(current_pos).get_x(),
//                               new_solution->get_solution_pair(current_pos).get_y(),
//                               new_solution->get_solution_pair(current_pos).get_vel(),
//                               new_solution->get_solution_pair(current_pos).get_charge_rate() + value,
//                               new_solution->get_solution_pair(current_pos).get_battery_charge());
//            new_solution->set_solution_pair(pair, current_pos);

////            new_solution->get_solution_pair(current_pos).set_charge_rate(new_solution->get_solution_pair(current_pos).get_charge_rate() + value);
//        }
//    }

////    int random_pos = -1;
////    int count = 0;
////    do {
////        random_pos = (rand() % (a_solution->get_size() - 2)) + 1;

////        if(type == STOPS::CHARGE && can_change_charge_rate(a_solution, random_pos, value)){
////            best_solution->get_solution_pair(random_pos).set_charge_rate(best_solution->get_solution_pair(random_pos).get_charge_rate() + value);
////            return evaluate(best_solution, m_map);
////        }

////        count++;
////    } while (count < a_solution->get_size()/2);

////    delete best_solution;

////    best_solution = a_solution->copy();

////    if(best == -1){
////        return evaluate(best_solution, m_map);
////    }

////    best_solution->get_solution_pair(best).set_charge_rate(best_solution->get_solution_pair(best).get_charge_rate() + value);

//    return evaluate(new_solution, m_map);
//}

bool Core::can_change_vel(Route route, int pos, int value) { /* verify if the new velocity is valid */
    int new_value = route.get_solution_pair(pos).get_vel() + value;

    return (new_value <= MAX_VEL) && (new_value >= MIN_VEL);
}

Array_Solution Core::change_vel(int pos, vector<Array_Solution> solutions, int value) /* get a random sub path from the route and change the velocity from of each stretch */
{
//    int best = -1;
    int x, y;
//    Array_Solution * new_solution = solutions[pos]->copy();//a_solution->copy();
//    Array_Solution new_solution(solutions[pos]); //memcheck
    Array_Solution new_solution = solutions[pos].copy();

//    for (int count = 1; count < a_solution->get_size(); count++) {
//        //Pode alterar velocidade
//        if(can_change_vel(a_solution, count, value)){
//            a_solution->get_solution_pair(count).set_vel(a_solution->get_solution_pair(count).get_vel() + value);
//            a_solution = evaluate(a_solution, m_map);

//            if(!best_solution->dominates(a_solution) && a_solution->is_valid()){
//                delete best_solution;
//                best_solution = a_solution->copy();

//                best = count;
//            }

//            a_solution->get_solution_pair(count).set_vel(a_solution->get_solution_pair(count).get_vel() - value);
//        }
//    }

    int pos_route = (rand() % new_solution.get_size());
    int start     = (rand() % (new_solution.get_routes()[pos_route].get_size() - 2)) + 1;
    int end       = (rand() % (new_solution.get_routes()[pos_route].get_size() - start - 1)) + start + 1;
//    Solution_Pair pair;


    for (int current_pos = start; current_pos < end; current_pos++) {
        if(can_change_vel(new_solution.get_routes()[pos_route], current_pos, value)){
//            vel = new_solution->get_solution_pair(current_pos).get_vel() + value;
//            new_solution->get_solution_pair(current_pos).set_vel(vel);
            Solution_Pair pair(new_solution.get_routes()[pos_route].get_solution_pair(current_pos).get_x(),
                               new_solution.get_routes()[pos_route].get_solution_pair(current_pos).get_y(),
                               new_solution.get_routes()[pos_route].get_solution_pair(current_pos).get_vel() + value,
                               new_solution.get_routes()[pos_route].get_solution_pair(current_pos).get_charge_rate(),
                               new_solution.get_routes()[pos_route].get_solution_pair(current_pos).get_battery_charge());
            new_solution.routes[pos_route].set_solution_pair(pair, current_pos);

//            new_solution.routes[pos_route].get_solution_pair(current_pos).add_vel(value);
        }
    }


//    int random_pos = -1;
//    int count = 0;
//    do {
//        random_pos = (rand() % (a_solution->get_size() - 2)) + 1;

//        if(can_change_vel(a_solution, random_pos, value)){
//            new_solution->get_solution_pair(random_pos).set_vel(new_solution->get_solution_pair(random_pos).get_vel() + value);
//            return evaluate(new_solution, m_map);
//        }

//        count++;
//    } while (count < a_solution->get_size()/2);



//    delete best_solution;

//    best_solution = a_solution->copy();

//    if(best == -1){
//        return evaluate(best_solution, m_map);
//    }

//    best_solution->get_solution_pair(best).set_vel(best_solution->get_solution_pair(best).get_vel() + value);


//    if(solutions[pos].weakly_dominates(new_solution)){
//        return Array_Solution(true);
//    }

    return new_solution;// evaluate(new_solution, m_map);
}

Array_Solution Core::change_vel_3(int pos, vector<Array_Solution> solutions, int value) /* gera array de pares ordenados indicando um caminho */
{

    int type;
//    Array_Solution copy_solution;
//    Solution_Pair temp;
    // Run through the routes
    for (int pos_route = 0; pos_route < solutions[pos].get_size(); pos_route++) {

        // Run through all the route
        for (int i = 0; i < solutions[pos].get_routes()[pos_route].get_size(); i++) {
            type = m_map->get_stop_point(solutions[pos].get_routes()[pos_route].get_solution_pair(i).get_x(),
                                         solutions[pos].get_routes()[pos_route].get_solution_pair(i).get_y()).get_type();

            // If it is a recharge point or origin
            if(i == 0 || type == STOPS::CHARGE){
    //        if(i == 0 || type == STOPS::DELIVER || type == STOPS::COLLECT){
//                Array_Solution * copy_solution = solutions[pos]->copy();
//                copy_solution = Array_Solution(solutions[pos]); //memcheck
                Array_Solution copy_solution = solutions[pos].copy();

                Solution_Pair temp = copy_solution.get_routes()[pos_route].get_solution_pair(i).copy();
                int pos1 = 0; // position of the first point that will swap

                for (int k = i + 1; k < copy_solution.get_routes()[pos_route].get_size(); k++) { // Go forward and eliminates the points until finds an inspection point (client)
                    type = m_map->get_stop_point(copy_solution.get_routes()[pos_route].get_solution_pair(k).get_x(),
                                                 copy_solution.get_routes()[pos_route].get_solution_pair(k).get_y()).get_type();
                    if(type == STOPS::CHARGE){
    //                                if(type == STOPS::DELIVER || type == STOPS::COLLECT){
                        break;
                    }

                    if(can_change_vel(copy_solution.get_routes()[pos_route], k, value)){
                        Solution_Pair pair(copy_solution.get_routes()[pos_route].get_solution_pair(k).get_x(),
                                           copy_solution.get_routes()[pos_route].get_solution_pair(k).get_y(),
                                           copy_solution.get_routes()[pos_route].get_solution_pair(k).get_vel() + value,
                                           copy_solution.get_routes()[pos_route].get_solution_pair(k).get_charge_rate(),
                                           copy_solution.get_routes()[pos_route].get_solution_pair(k).get_battery_charge());
                        copy_solution.routes[pos_route].set_solution_pair(pair, k);

//                        copy_solution.routes[pos_route].get_solution_pair(k).add_vel(value);
                    }
                }

               copy_solution = evaluate(copy_solution, m_map);

               bool dominated_or_equal = false;
               if(!solutions[pos].weakly_dominates(copy_solution)){//if(copy_solution2->dominates(solutions[pos])){//copy_solution2->is_valid() &&
                   for (int k = 0; k < solutions.size(); k++) {
                       if(solutions[k].equals(copy_solution)){
                           dominated_or_equal = true;
                           break;
                       }
                       if(solutions[k].dominates(copy_solution)){
                           dominated_or_equal = true;
                           break;
                       }
                   }

                   if(!dominated_or_equal){
                       return copy_solution;
                   }
               }
//               delete copy_solution;
            }
        }
    }

    return Array_Solution(true);
}

Array_Solution Core::neighbor(int pos, vector<Array_Solution> solutions, int type) /* directs to the desirable neighborhood function */
{
    switch (type) {
    case 0:
        if(!swap_test || (swap_test && swap_3_available)){
//            return swap3(pos, solutions);
        }else if(swap_test && swap_4_available){
            return swap4_multi(pos, solutions);//closest_recharge(pos, solutions, 0);
        }
        break;
    case 1:
        if(swap_test && swap_5_available){
//            return swap5(pos, solutions);
        }else if(swap_test && swap_6_available){
//            return swap6_multi(pos, solutions);//closest_recharge(pos, solutions, 0);
        }
        break;
    case 2:
        return remove_recharge(pos, solutions);//closest_recharge(pos, solutions, 0);
        break;
    case 3:
        return closest_recharge(pos, solutions, 0);
        break;
    case 4:
        return remove_repeated(pos, solutions);
        break;
    case 5:
//        if(solutions[pos]->is_valid()){
            return change_vel_3(pos, solutions, -1);//change_vel(solutions[pos], 1);
//        }
        break;
    case 6:
//        if(solutions[pos]->is_valid()){
            return change_vel_3(pos, solutions, 1);
//        }
        break;
    case 7:
//        if(solutions[pos]->is_valid()){
            return change_charge_rate2(pos, solutions, -5);
//        }
        break;
    case 8:
//        if(solutions[pos]->is_valid()){
            return change_charge_rate2(pos, solutions, 5);
//        }
        break;
    case 9:
//        if(solutions[pos]->is_valid()){
            return change_vel(pos, solutions, -1);
//        }
        break;
    case 10:
//        if(solutions[pos]->is_valid()){
            return change_vel(pos, solutions, 1);
//        }
        break;


//    case 2:
//        if(!solutions[pos]->is_valid()){
//            return remove_client(pos, solutions);//closest_recharge(pos, solutions, 0);
//        }
//        break;
//    case 3:
//        return change_vel(a_solution, -1);
//        break;
//    case 4:
//        return change_vel(a_solution, 1);
//        break;
//    case 5:
//        return change_charge_rate(a_solution, -1);
//        break;
//    case 6:
//        return change_charge_rate(a_solution, 1);
//        break;


    }

    return Array_Solution(true);
}

//Array_Solution * Core::neighbor(Array_Solution * a_solution, int number_lines, int number_columns, int type) /* directs to the desirable neighborhood function */
//{
//    switch (type) {
//    case 0:
//        return swap2(a_solution, number_lines, number_columns);
//        break;
//    case 1:
//        return random_sub_path(a_solution, number_lines, number_columns);
//        break;
//    case 2:
//        return change_final(a_solution, number_lines, number_columns);
//        break;
//    case 3:
//        return change_vel(a_solution, -1);
//        break;
//    case 4:
//        return change_vel(a_solution, 1);
//        break;
//    case 5:
//        return change_charge_rate(a_solution, -1);
//        break;
//    case 6:
//        return change_charge_rate(a_solution, 1);
//        break;
//    }

//}

//multidrone
//bool Core::completed_path(Array_Solution * a_solution){ /* verify if the current solution reached all points of collect and deliver */
//    return a_solution->get_qty_collect() >= QTY_POINTS[STOPS::COLLECT] && a_solution->get_qty_deliver() >= QTY_POINTS[STOPS::DELIVER];
//}

bool Core::completed_path(Array_Solution a_solution){ /* verify if the current solution reached all points of collect and deliver */
    vector<Route> routes = a_solution.get_routes();
    Route route;

    int qty_collect = 0, qty_deliver = 0;

    foreach (route, routes) {
        qty_collect += route.get_qty_collect();
        qty_deliver += route.get_qty_deliver();
    }
    return qty_collect >= QTY_POINTS[STOPS::COLLECT] && qty_deliver >= QTY_POINTS[STOPS::DELIVER];
}

bool Core::completed_path(Solution * a_solution){ /* verify if the current solution reached all points of collect and deliver */


    int qty_clients = 0;

    for (int pos_route = 0; pos_route < a_solution->size; pos_route++) {
         qty_clients  += a_solution->uavs[pos_route]->objectives[0]; // Add the counting of clients attended by each route
    }

    return qty_clients >= QTY_POINTS[STOPS::COLLECT] + QTY_POINTS[STOPS::DELIVER];
}

Route Core::evaluate_loop(Route route, Matrix * m_map, double current_time, int current_pos, double battery_charge, int points) {/* run through all the solution evaluating it*/

//    if (battery_charge < 0) {
//        //cout << "\nPontos: " << points << " - Battery Charge: " << battery_charge;
//        points -= POINTS_BY_STOP_POINT[STOPS::CHARGE];
//    }        

    //Consumo conforme velocidade
    double vel = route.get_solution_pair(current_pos).get_vel();
    double variable_consumption = (vel * VARIABLE_BATTERY_CONSUMPTION);
    battery_charge -= variable_consumption;

    //Consumo fixo
    battery_charge -= FIXED_BATTERY_CONSUMPTION;

    //Insert into the array of objectives of the solution
    //NEGATIVE: we want to minimize
    route.z[3] += (-1) * (variable_consumption + FIXED_BATTERY_CONSUMPTION);

    //Replace the current point with an updated one
    Solution_Pair pair(route.get_solution_pair(current_pos).get_x(),
                       route.get_solution_pair(current_pos).get_y(),
                       route.get_solution_pair(current_pos).get_vel(),
                       route.get_solution_pair(current_pos).get_charge_rate(),
                       battery_charge);      
    route.set_solution_pair(pair, current_pos);

    if (battery_charge < 0) {
        route.set_valid(false);
    }

    //Tempo para percorrer o trecho    
    current_time += (DISCRETIZATION / vel);

    int x = route.get_solution_pair(current_pos).get_x();
    int y = route.get_solution_pair(current_pos).get_y();

    int type = m_map->get_stop_point(x, y).get_type();

//    cout << "\n-Current time: " << current_time << " - DELIVER: ";
//    cout << "\n(" << x << "," << y << "): " << type << endl;

    switch (type) {
    case STOPS::NOTHING: break;

    case STOPS::PROHIBITED:
        route.set_valid(false);
        break;

    case STOPS::DELIVER:
//        cout << "\nPontos: " << points << " - DELIVER: ";
//        cout << "(" << x << "," << y << ")";

        points += POINTS_BY_STOP_POINT[STOPS::DELIVER];
        m_map->set_stop_point(x,y, STOPS::NOTHING);

        route.set_qty_deliver(route.get_qty_deliver() + 1);

        route.z[0] += 1;
        break;
    case STOPS::COLLECT:
//        cout << "\nPontos: " << points << " - COLLECT: ";
//        cout << "(" << x << "," << y << ")";

        points += POINTS_BY_STOP_POINT[STOPS::COLLECT];
        m_map->set_stop_point(x, y, STOPS::NOTHING);

        route.set_qty_collect(route.get_qty_collect() + 1);

        route.z[0] += 1;
        break;
    case STOPS::CHARGE:        
        double recharge = MAX_BATTERY_CHARGE - battery_charge;
        recharge = (recharge >  BATTERY_RECHARGE) ? BATTERY_RECHARGE : recharge;

        double new_battery_charge = battery_charge + (recharge * route.get_solution_pair(current_pos).get_charge_rate() / 100);

        if (new_battery_charge > MAX_BATTERY_CHARGE)
            new_battery_charge = MAX_BATTERY_CHARGE;

//        cout << "\nBATTERY CHARGE: " << battery_charge;
//        cout << " (" << x << "," << y << ")";

        //Tempo gasto carregando
        current_time += TIME_BATTERY_RECHARGE * route.get_solution_pair(current_pos).get_charge_rate() / 100;//(new_battery_charge - battery_charge)/MAX_BATTERY_CHARGE;
        battery_charge = new_battery_charge;

        //Qtd de pontos de recarga pelos quais o UAV passou
        route.set_qty_charge(route.get_qty_charge() + 1);
        break;    
    }

    current_pos++;

//    Multidrone:

//    if (completed_path(a_solution)) {
//        points += NUMBER_ITERATIONS - current_pos;
//        a_solution->resize(current_pos);

//        if(a_solution->is_valid()){
//            a_solution->set_complete(true);
//            completed = true;
//        }
//    }

    if (current_pos >= route.get_size()) {
//        points += battery_charge;
//        a_solution->set_points(points);

        //Setando o valor dos objetivos
        route.z[1] = battery_charge;
        route.z[2] = (-1) * current_time;

//        cout << "\nPontuacao EVALUATE: " << a_solution.get_points() << endl;
//        cout << "\nPontuacao EVALUATE: z0 = " << a_solution.get_z()[0] << " z1 = " << a_solution.get_z()[1] << endl;

//        delete m_map;

        return route;
    }

    return evaluate_loop(route, m_map, current_time, current_pos, battery_charge, points);
}

Array_Solution Core::evaluate(Array_Solution a_solution, Matrix * m_map) { /* set the value of the objective functions of the current solution */

    Matrix * m_map_copy = m_map->copy(); // Copy of the map that is going to be used by every route

    for (int pos_route = 0; pos_route < a_solution.get_size(); pos_route++) {// Run through all routes

        for (int i = 0; i < a_solution.get_num_objectives(); i++) {
            a_solution.routes[pos_route].get_z()[i] = 0;
        }

        a_solution.routes[pos_route].set_qty_deliver(0);
        a_solution.routes[pos_route].set_qty_collect(0);
        a_solution.routes[pos_route].set_qty_charge (0);
        a_solution.routes[pos_route].set_valid(true);

        //Verify origin point
        int x = a_solution.get_routes()[pos_route].get_solution_pair(0).get_x();
        int y = a_solution.get_routes()[pos_route].get_solution_pair(0).get_y();

        int type = m_map_copy->get_stop_point(x, y).get_type();

        switch (type) {
        case STOPS::DELIVER:
            m_map_copy->set_stop_point(x,y, STOPS::NOTHING);

            a_solution.routes[pos_route].set_qty_deliver(a_solution.get_routes()[pos_route].get_qty_deliver() + 1);

            a_solution.routes[pos_route].z[0] += 1;
            break;
        case STOPS::COLLECT:
            m_map_copy->set_stop_point(x, y, STOPS::NOTHING);

            a_solution.routes[pos_route].set_qty_collect(a_solution.get_routes()[pos_route].get_qty_collect() + 1);

            a_solution.routes[pos_route].z[0] += 1;
            break;
        }
        //

        a_solution.routes[pos_route] = evaluate_loop(a_solution.get_routes()[pos_route], m_map_copy, 0, 1, initial_battery_charge, 0);
    }

    if (completed_path(a_solution)) {
        if(a_solution.is_valid()){
            a_solution.set_complete(true);
            completed = true;
        }
    }

    a_solution.update_values(); // Update objective values of the current solution

    delete m_map_copy;

    return a_solution;
}

//bool Core::is_stable(int acceptable, int a_max[]) { /* verify if ANY neighborhood has reached the maximum number of iterations without inserting new solutions */
//    for (int i = 0; i < NEIGHBORHOOD.size(); i++) {
//        if(a_max[i] >= acceptable){
//            return false;
//        }
//    }
//    return true;
//}

//bool Core::is_stable(bool n_stable[]) { /* verify if ALL neighborhood has reached the maximum number of iterations without inserting new solutions */
//    for (int i = 0; i < NEIGHBORHOOD.size(); i++) {
//        if(!n_stable[i]){
//            return false;
//        }
//    }
//    return true;
//}

//void Core::clean_neighborhood_qty() { /* set every neighborhood qty of iterations stable to 0 */
//    for (int i = 0; i < NEIGHBORHOOD.size(); i++) {
//        NEIGHBORHOOD_QTY[i] = 0;
//        NEIGHBORHOOD_STABLE[i] = false;
//    }
//}

//vector<Array_Solution *> Core::generate_solutions(int type, string file_name)
//{
//    // Ensures that the number will be randomly generated
//    srand(time(0));

//    // Initializing the array that represents the map
////    m_map = new Matrix(MATRIX_NUMBER_LINES, MATRIX_NUMBER_COLUMNS);

//    // Generates the map
//    if(type == 0){
//        m_map = initialize_mtrx(m_map);
//    } else if (type == 1){
//        m_map = initialize_mtrx2(m_map, file_name, "/visited.txt");//"/home/elias/Documents/Mestrado/uav_routing_qt/UAV_Routing_Qt_1/mtrx.txt");//
//    } else if (type == 2){
//        m_map = initialize_mtrx_load_test_files(m_map, "/home/elias/Documents/Mestrado/uav_routing_qt/UAV_Routing_Qt_1/test.lib");
//    }

//    // Print the map
//    m_map->print();

//    cout << ">> Lines: " << MATRIX_NUMBER_LINES << endl;
//    cout << ">> Columns: " << MATRIX_NUMBER_COLUMNS << endl;

//    cout << "\n>> Deliver: " << QTY_POINTS[STOPS::DELIVER] << endl;

//    generate_neighbors(neighbors_qty);
//    generate_further_neighbors(neighbors_qty);


//    /* Initializes the initial solution containing:
//    - 2 rows: (x, y)
//    - Number of columns equal to the number of time slots
//    */
////    Array_Solution * a_initial_solution = new Array_Solution(MAX_TIME / TIME_INTERVAL + 1);

//    // Generates initial solution
////    a_initial_solution = initial_solution2(MATRIX_NUMBER_LINES, MATRIX_NUMBER_COLUMNS, a_initial_solution);
////    a_initial_solution->set_iteration(0);

//    // Insert the initial solution directly to the final set of solutions, just to compare to the solutions generated in the loop
////    a_solutions.push_back(a_initial_solution->copy());//    a_solutions[0] = a_initial_solution->copy();
////    a_solutions[0] = evaluate(a_solutions[0], m_map);
//    //

//    // Perturbs initial solution
////    Array_Solution * a_perturb_solution = perturb(a_initial_solution, MATRIX_NUMBER_LINES, MATRIX_NUMBER_COLUMNS, MAX_TIME / TIME_INTERVAL + 1);

//    // Apply SWAP to the perturbed solution
////    Array_Solution * a_swap_solution = swap2(a_perturb_solution, MATRIX_NUMBER_LINES, MATRIX_NUMBER_COLUMNS);//Array_Solution * a_swap_solution = swap(a_perturb_solution, MATRIX_NUMBER_LINES, MATRIX_NUMBER_COLUMNS);
////    a_swap_solution = evaluate(a_swap_solution, m_map);
////    a_initial_solution = evaluate(a_initial_solution, m_map);;
////    a_initial_solution = a_swap_solution;
//    //

//    // Insert the SWAPED solution to the pool of solutions
////    a_swap_solution->set_iteration(0);
////    pool_of_solutions.add(a_initial_solution);

//    // Initialize variables that will be used in the loop
//    Array_Solution * a_neighbor_solution;
//    bool added = false;
//    Pool_of_Solutions * p_temp = new Pool_of_Solutions(NUMBER_ITERATIONS);
//    int actual_max_size = MAX_TIME / TIME_INTERVAL;
//    int i = 0;
//    int best_iteration = 0;
//    //
//    auto start = std::chrono::system_clock::now();
//    double diff_count = 0;

//    cout << "\nSTART: " << endl;
//    do
////    for (i = 1; (i < MAX_ITERATIONS && !completed) || (i < NUMBER_ITERATIONS); i++)
//    {
//        i++;
//        cout << "\nITERATION: " << i << endl;
//        Array_Solution * a_initial_solution = new Array_Solution(MAX_TIME / TIME_INTERVAL + 1);
//        a_initial_solution = initial_solution2(MATRIX_NUMBER_LINES, MATRIX_NUMBER_COLUMNS, a_initial_solution);

//        a_initial_solution->set_iteration(-i);
//        a_initial_solution = evaluate(a_initial_solution, m_map);


////        while(!a_initial_solution->is_valid()){
////            a_initial_solution = fill_with_recharge_points(a_initial_solution, 1);
////            a_initial_solution = validate(a_initial_solution);
////            cout << "\nvalidou" << endl;
////        }


////            if(!a_initial_solution->is_valid()){
////                a_initial_solution = fill_with_recharge_points(a_initial_solution, 1);
//    //            a_initial_solution->set_iteration(-i);
//    //            a_initial_solution = evaluate(a_initial_solution, m_map);
//    //            p_temp->add(a_initial_solution);
//    //            pool_of_solutions.add_solution(a_initial_solution);
//    //            a_solutions.push_back(a_initial_solution->copy());
////            }

//        if(pattern_vel.size() > 0){
//            for (int pos = 0; pos < a_initial_solution->get_size(); pos++) {
//                for (int count = 0; count < pattern_vel.size(); count++) {
//                    if(a_initial_solution->get_solution_pair(pos).is_at_same_position(pattern_vel[count])){
//                        a_initial_solution->set_solution_pair(pattern_vel[count], pos);
//                        break;
//                    }
//                }
//            }
//        }

//        a_initial_solution = evaluate(a_initial_solution, m_map);
//        p_temp->add(a_initial_solution);
//        pool_of_solutions.add_solution(a_initial_solution);
//        a_solutions.push_back(a_initial_solution->copy());

//        delete a_initial_solution;


////        for (int k = 0; k < pool_of_solutions.get_size(); k++) {
////            p_temp->add(pool_of_solutions.get_solutions()[k]);
////        }

////        // Clean temporary pool of solutions
////        delete p_temp;
////        p_temp = new Pool_of_Solutions(NUMBER_ITERATIONS);

//        // If it is not adding new solutions to the pool for a certain time
////        if(!completed && !is_stable(STABLE_ITERATIONS_ACCEPTABLE, NEIGHBORHOOD_QTY)){
////        if(!completed && !is_stable(NEIGHBORHOOD_STABLE)){
////            actual_max_size += 10;
////            iteration_completed = i;
////            clean_neighborhood_qty();

////            for (int k = 0; k < pool_of_solutions.get_size(); k++) {
////                // Increment the size of the solution
////                a_perturb_solution = increment(pool_of_solutions.get_solutions()[k], MATRIX_NUMBER_LINES, MATRIX_NUMBER_COLUMNS, actual_max_size);
////                a_perturb_solution->set_iteration(i);

////                p_temp->add(a_perturb_solution);
////                pool_of_solutions.add_solution(a_perturb_solution);

////                delete a_perturb_solution;
////            }
////        }else{
////            // Perturb all routes in the pool
////            for (int k = 0; k < pool_of_solutions.get_size(); k++) {
////                a_perturb_solution = perturb(pool_of_solutions.get_solutions()[k], MATRIX_NUMBER_LINES, MATRIX_NUMBER_COLUMNS, actual_max_size);
////                a_perturb_solution->set_iteration(i);

////                p_temp->add(a_perturb_solution);
////                pool_of_solutions.add_solution(a_perturb_solution);

////                delete a_perturb_solution;
////            }
////        }

//        cout << ">> Local Search (VNS)" << endl;
//        // Local Search (VNS)
//        bool loop_added = true;

//        while(loop_added){
//            loop_added = false;
//        for (int n_count = 0; n_count < NEIGHBORHOOD_QTY  && diff_count < MAX_TIME_ALG; n_count++) {
//        // Run through all routes perturbed
////        for (int j = 0; j < p_temp->get_size() && diff_count < MAX_TIME_ALG; j++) {

////            while(loop_added){
////                loop_added = false;
//            // Run through all neighborhoods
////            for (int n_count = 0; n_count < NEIGHBORHOOD.size(); n_count++) {
////                if(!NEIGHBORHOOD_STABLE[n_count]){
//                    // Apply local search

////            if(p_temp->get_solutions()[j]->is_valid()){

////            for (int n_count = 0; n_count < NEIGHBORHOOD_QTY && diff_count < MAX_TIME_ALG; n_count++) {
//            for (int j = 0; j < p_temp->get_size() && diff_count < MAX_TIME_ALG; j++) {

//                if(p_temp->get_solutions()[j]->is_neighborhood_visited(n_count) == 0){
//                    added = true;
//                    p_temp->get_solutions()[j]->set_neighborhood_visitation(n_count, 1);

//                    while(added
//                          && diff_count < MAX_TIME_ALG){
//                        added = false;
//                        auto start_neighbor = std::chrono::system_clock::now();
//                        a_neighbor_solution = neighbor(j, p_temp->get_solutions(), n_count);//neighbor(p_temp->get_solutions()[j], MATRIX_NUMBER_LINES, MATRIX_NUMBER_COLUMNS, NEIGHBORHOOD[n_count]);

//                        auto end_neighbor = std::chrono::system_clock::now();
//                        std::chrono::duration<double> diff_neighbor = end_neighbor - start_neighbor;
//                        NEIGHBORHOOD_QTY_VEC[n_count]++;
//                        NEIGHBORHOOD_TIME[n_count] += diff_neighbor.count();

//                        if(a_neighbor_solution){
//                            a_neighbor_solution->set_iteration(i);

//                            added = pool_of_solutions.add_solution(a_neighbor_solution);

//                            if(added){
//                                NEIGHBORHOOD_ADDED[n_count]++;
//                                loop_added = true;
//                            }

//                            for (int k = 0; k < p_temp->get_size(); k++) {
//                                if(k != j){
//                                    if(a_neighbor_solution->dominates(p_temp->get_solutions()[k])){
//                                        p_temp->remove_solution(k);
//                                        k--;
//                                        if(k < j){
//                                            j--;
//                                        }
//                                    }
//                                }
//                            }

//                            p_temp->add(a_neighbor_solution);

////                                if(added){
//        //                            break;
//        //    //                          NEIGHBORHOOD_BOOL[n_count] = true;
////                                }


//                            delete a_neighbor_solution;
//                        }
//        //                }
//        //            }
//                        auto end = std::chrono::system_clock::now();
//                        std::chrono::duration<double> diff = end-start;
//                        diff_count = diff.count();
//                    }
//            }
////                auto end = std::chrono::system_clock::now();
////                std::chrono::duration<double> diff = end-start;
////                diff_count = diff.count();
//                }

////            if(n_count == 6){
////                if(pool_of_solutions.get_size() > 1){
//////                            cout << ">> Matriz de padroes:" << endl;
////                    get_pattern2(p_temp);
//////                            cout << ">> FIM Matriz" << endl;
////                }
////            }
////            }
//        }
//        }

////        point_qty.clear();
////        table.clear();

//        // Update the number of iterations without inserting new routes in the pool
////        for (int n_count = 0; n_count < NEIGHBORHOOD.size(); n_count++) {
////            if(!NEIGHBORHOOD_STABLE[n_count]){
////                if(NEIGHBORHOOD_BOOL[n_count]){
////                  NEIGHBORHOOD_MAX[n_count]++;
////                  NEIGHBORHOOD_BOOL[n_count] = false;
////                  NEIGHBORHOOD_QTY[n_count] = 0;
////                } else {
////                  NEIGHBORHOOD_QTY[n_count]++;

////                  if(NEIGHBORHOOD_QTY[n_count] > STABLE_ITERATIONS_ACCEPTABLE){
////                      NEIGHBORHOOD_STABLE[n_count] = true;
////                  }
////                }
////            }
////        }


//        // Clean temporary pool of solutions
//        delete p_temp;
//        p_temp = new Pool_of_Solutions(NUMBER_ITERATIONS);

////        auto end = std::chrono::system_clock::now();
////        std::chrono::duration<double> diff = end-start;
////        diff_count = diff.count();
//    } while(diff_count < MAX_TIME_ALG);

//    delete p_temp;

//    string results_file_name = "/home/elias/Documents/Mestrado/uav_routing_qt/UAV_Routing_Qt_1/results/time_420/hard/size_30/swap_04_05/vns/results_510100_swap_04_05_time_420_09.txt";
//    string info_file_name    = "/home/elias/Documents/Mestrado/uav_routing_qt/UAV_Routing_Qt_1/results/time_420/hard/size_30/swap_04_05/vns/info_510100_swap_04_05_time_420_09.txt";

////    cout << "\nMAX: " << endl;
////    cout << "\n0:" << NEIGHBORHOOD_MAX[0] <<
////            "\n1:" << NEIGHBORHOOD_MAX[1] <<
////            "\n2:" << NEIGHBORHOOD_MAX[2] <<
////            "\n3:" << NEIGHBORHOOD_MAX[3] <<
////            "\n4:" << NEIGHBORHOOD_MAX[4] <<
////            "\n5:" << NEIGHBORHOOD_MAX[5] <<
////            "\n6:" << NEIGHBORHOOD_MAX[6] << endl;
////    cout << "\nMAX SIZE: " << actual_max_size << endl;
//    cout << "\nMAX ITERATION: " << iteration_completed << endl;
//    cout << "\nLAST ITERATION: " << i << endl;

//    string completed_resp = completed ? "true" : "false";
//    cout << "\nCOMPLETED: " << completed_resp << endl;

//    auto end = std::chrono::system_clock::now();
//    std::chrono::duration<double> diff = end-start;
//    cout << "Total time: " << diff.count() << "\n" << endl;

//    //START: Info file
//    ofstream info_file;
//    info_file.open (info_file_name);

//    info_file << "INSTANCE: " << "eil51.tsp" << "\n";
//    info_file << "\nPOINTS: " << QTY_POINTS[STOPS::DELIVER] + QTY_POINTS[STOPS::COLLECT] << "\n";
//    info_file << "\nRECHARGE: " << QTY_POINTS[STOPS::CHARGE] << "\n";
//    info_file << "\nPROHIBITED: " << QTY_POINTS[STOPS::PROHIBITED] << "\n";
//    info_file << "\n------------------------------------------" << "\n";

//    info_file << "\nMETHODS: " << "swap 04 and 05" << "\n";
//    info_file << "\nSTOP CHRITERION: " << "time (420 s) - hard" << "\n";
//    info_file << "\nPOOL MAX SIZE: " << "30" << "\n";
//    info_file << "\n------------------------------------------" << "\n";

//    info_file << "\nLAST ITERATION: " << i << "\n";
//    info_file << "\n# OF SOLUTIONS: " << pool_of_solutions.get_size() << "\n";
//    info_file << "\nCOMPLETED: " << completed_resp << "\n";
//    info_file << "\nTOTAL TIME: " << diff.count() << "\n" << "\n";
//    info_file << "------------------------------------------" << "\n\n";
//    //END: Info file

////    delete p_temp;

////    a_solutions.resize(pool_of_solutions.get_size());
//    int max_final_charge = INT_MIN, min_time = INT_MAX, min_consumption = INT_MAX;
//    int total_final_charge = 0, total_time = 0, total_consumption = 0;

//    double z1, z2, z3;

//    ofstream myfile;
//    myfile.open (results_file_name);

//    for (int k = 0; k < pool_of_solutions.get_size(); k++) {

//        a_solutions.push_back(pool_of_solutions.get_solutions()[k]->copy());

//        z1 = pool_of_solutions.get_solutions()[k]->get_z()[1];
//        z2 = (-1)*pool_of_solutions.get_solutions()[k]->get_z()[2];
//        z3 = (-1)*pool_of_solutions.get_solutions()[k]->get_z()[3];

//        if(z1 > max_final_charge){
//            max_final_charge = z1;
//        }

//        if(z2 < min_time){
//            min_time = z2;
//        }

//        if(z3 < min_consumption){
//            min_consumption = z3;
//        }

//        total_final_charge += z1;
//        total_time += z2;
//        total_consumption += z3;

//        myfile << z1 << " " << -z2 << " " << -z3 << "\n";

////        cout << pool_of_solutions.get_solutions()[k]->get_z()[0] << " " <<
////                pool_of_solutions.get_solutions()[k]->get_z()[1] << " " <<
////                (-1)*pool_of_solutions.get_solutions()[k]->get_z()[2] << " " <<
////                (-1)*pool_of_solutions.get_solutions()[k]->get_z()[3] << "\n";
//    }

//    myfile.close();

//    double average_final_charge = total_final_charge/ (double) pool_of_solutions.get_size();
//    double average_time         = total_time        / (double) pool_of_solutions.get_size();
//    double average_consumption  = total_consumption / (double) pool_of_solutions.get_size();

//    double dev_final_charge = 0;
//    double dev_time         = 0;
//    double dev_consumption  = 0;

//    for (int k = 0; k < pool_of_solutions.get_size(); k++) {
//        dev_final_charge += pow(  pool_of_solutions.get_solutions()[k]->get_z()[1] - average_final_charge, 2);
//        dev_time         += pow(- pool_of_solutions.get_solutions()[k]->get_z()[2] - average_time, 2);
//        dev_consumption  += pow(- pool_of_solutions.get_solutions()[k]->get_z()[3] - average_consumption, 2);
//    }

//    dev_final_charge = sqrt(dev_final_charge);
//    dev_time         = sqrt(dev_time);
//    dev_consumption  = sqrt(dev_consumption);

//    for (int count = 0; count < NEIGHBORHOOD_QTY_VEC.size(); count++) {
//        if(NEIGHBORHOOD_QTY_VEC[count] > 0){
//            cout      << ">> Vizinhanca " << count << ": " << NEIGHBORHOOD_ADDED[count] << " added - " << NEIGHBORHOOD_QTY_VEC[count] << " (" << NEIGHBORHOOD_TIME[count] /  (double) NEIGHBORHOOD_QTY_VEC[count] << " s)" << endl;
//            info_file << ">> Vizinhanca " << count << ": " << NEIGHBORHOOD_ADDED[count] << " added - " << NEIGHBORHOOD_QTY_VEC[count] << " (" << NEIGHBORHOOD_TIME[count] /  (double) NEIGHBORHOOD_QTY_VEC[count] << " s)" << "\n";
//        }

//        // Cleaning vec
//        NEIGHBORHOOD_QTY_VEC[count] = 0;
//        NEIGHBORHOOD_ADDED[count] = 0;
//        NEIGHBORHOOD_TIME[count] = 0;

////        else{
////            cout << ">> Vizinhanca " << count << ": " << NEIGHBORHOOD_ADDED[count] << " added - " << NEIGHBORHOOD_QTY_VEC[count] << " (0 s)" << endl;
////        }
//    }

//    cout << ">> Average Final Charge: " << average_final_charge <<
//            " >> Standard Deviation Final Charge: " << dev_final_charge <<
//            " >> Best Final Charge: " << max_final_charge << endl;
//    cout << ">> Average Time: " << average_time <<
//            " >> Standard Deviation Time: " << dev_time <<
//            " >> Best Time: " << min_time << endl;
//    cout << ">> Average Consumption: " << average_consumption <<
//            " >> Standard Deviation Consumption: " << dev_consumption <<
//            " >> Best Consumption: " << min_consumption << endl;

//    //START: Info file
//    info_file << "\n------------------------------------------" << "\n";
//    info_file << "\n";

//    info_file << ">> Average Final Charge: " << average_final_charge <<
//                " >> Standard Deviation Final Charge: " << dev_final_charge <<
//                " >> Best Final Charge: " << max_final_charge << "\n";
//    info_file << ">> Average Time: " << average_time <<
//                " >> Standard Deviation Time: " << dev_time <<
//                " >> Best Time: " << min_time << "\n";
//    info_file << ">> Average Consumption: " << average_consumption <<
//                " >> Standard Deviation Consumption: " << dev_consumption <<
//                " >> Best Consumption: " << min_consumption << "\n";

//    info_file.close();
//    //END: Info file



////    delete pool_of_solutions;

//    return a_solutions;


////    #if defined(_MSC_VER)
////        // estas linhas serao executadas apenas quando o programa
////        // for compilado por alguma versao do Microsoft Visual C
////        system("pause");
////    #endif

//}

vector<Solution *> Core::test(int type, int method, string instance_file_name, string directory_name, int time_limit = 420, int num_exec = 5, int pct_prohibited = 0, bool load_file = false, string load_file_name = "")
{
    srand(time(NULL));

    global_file_name = directory_name;    

    // Matrix file
    ifstream test_file(directory_name + "/matrix.txt");

    // Get clients that were visited in an old tour
//    get_clients_visited(instance_file_name, directory_name + "/visited.txt");

    delete m_map;

    //  If does not exist a file with the map
    if ( !test_file ){
        cout << "File not found" << endl;        

        // Generates the map
        m_map = initialize_mtrx_load_test_files(m_map, instance_file_name, pct_prohibited);//"/home/elias/Documents/Mestrado/uav_routing_qt/UAV_Routing_Qt_1/test.lib", pct_prohibited);
        m_map->save_to_file(directory_name + "/matrix.txt");

        // Print the map
//        m_map->print();
    }else{
        // Generates the map
        m_map = initialize_mtrx2(m_map, directory_name + "/matrix.txt", directory_name + "/visited.txt");// initialize_mtrx_load_test_files(m_map, "/home/elias/Documents/Mestrado/uav_routing_qt/UAV_Routing_Qt_1/test.lib", pct_prohibited);

        // Print the map
//        m_map->print();
    }

    //Set the origin point of the drones
    set_origin_point(m_map, directory_name + "/origin.txt");

    test_file.close();


    // Verify if folder with paths exists
    struct stat info;

    string path_name = directory_name + "/paths";

    if( pre_processing
//            && stat(path_name.c_str(), &info) != 0
            ){

        // Generate best paths
//        set_paths(m_map, client_points, recharge_points, directory_name);
        set_paths_STRUCT(m_map, client_points, recharge_points, directory_name);
    }

    // Get paths from files to memory
//    best_paths = get_paths(directory_name);


    // Neighbor methods
    swap_test = true;

    switch (method) {
    case 0: // SWAP 3
        swap_3_available = true;
        swap_4_available = false;
        swap_5_available = false;
        swap_6_available = false;
        break;
    case 1: // SWAP 4
        swap_3_available = false;
        swap_4_available = true;
        swap_5_available = false;
        swap_6_available = false;
        break;
    case 2: // SWAP 4 and 5
        swap_3_available = false;
        swap_4_available = true;
        swap_5_available = true;
        swap_6_available = false;
        break;
    case 3: // SWAP 4 and 6
        swap_3_available = false;
        swap_4_available = true;
        swap_5_available = false;
        swap_6_available = true;
        break;
    case 4: // SWAP  6
        swap_3_available = false;
        swap_4_available = false;
        swap_5_available = false;
        swap_6_available = true;
        break;
    }

    size_t found = instance_file_name.find_last_of("/");
    instance_file_name.erase (0,found+1);

    if(type == 1){
//        return test_vns(time_limit, num_exec, instance_file_name, directory_name, pct_prohibited);
    }else{
//        return test_i_vns(time_limit, num_exec, instance_file_name, directory_name, pct_prohibited);
        return test_i_vns_STRUCT(time_limit, num_exec, instance_file_name, directory_name, pct_prohibited, load_file, load_file_name);
    }

//    vector<Array_Solution *> solutions_vec;

//    // SWAP 6
//    swap_3_available = false;
//    swap_4_available = false;
//    swap_5_available = false;
//    swap_6_available = true;
//    solutions_vec = test_vns(time_limit, num_exec, instance_file_name, directory_name, pct_prohibited);

//    for (int var = 0; var < solutions_vec.size(); var++) {
//        delete solutions_vec[var];
//    }

//    solutions_vec.clear();

//    solutions_vec = test_i_vns(time_limit, num_exec, instance_file_name, directory_name, pct_prohibited);

//    for (int var = 0; var < solutions_vec.size(); var++) {
//        delete solutions_vec[var];
//    }

//    solutions_vec.clear();


//    // SWAP 4 AND 6
//    swap_3_available = false;
//    swap_4_available = true;
//    swap_5_available = false;
//    swap_6_available = true;
//    solutions_vec = test_vns(time_limit, num_exec, instance_file_name, directory_name, pct_prohibited);

//    for (int var = 0; var < solutions_vec.size(); var++) {
//        delete solutions_vec[var];
//    }

//    solutions_vec.clear();

//    solutions_vec = test_i_vns(time_limit, num_exec, instance_file_name, directory_name, pct_prohibited);

//    return solutions_vec;
}

vector<Array_Solution> Core::test_i_vns(int time_limit, int num_exec, string instance_name, string directory_name, int pct_prohibited = 0)
{    
    string base_file_name = directory_name + "/" + "i_vns_swap_";

    if(swap_3_available){
        base_file_name += "03_";
    }
    if(swap_4_available){
        base_file_name += "04_";
    }
    if(swap_5_available){
        base_file_name += "05_";
    }
    if(swap_6_available){
        base_file_name += "06_";
    }

    //Create directory
    mkdir(base_file_name.c_str(), 0777);
    base_file_name += "/";

//    Pool_of_Solutions p_temp = Pool_of_Solutions(NUMBER_ITERATIONS);
    vector<Array_Solution> a_temp;

    //Run loop until number of executions is reached
    for (int test_count = 0; test_count < num_exec; test_count++) {
        // Ensures that the number will be randomly generated
        srand(time(0));

        // Initializing the array that represents the map

        // Generates the map
//        m_map = initialize_mtrx_load_test_files(m_map, "/home/elias/Documents/Mestrado/uav_routing_qt/UAV_Routing_Qt_1/test.lib", pct_prohibited);

        // Print the map
        m_map->print();

        cout << ">> Lines: " << MATRIX_NUMBER_LINES << endl;
        cout << ">> Columns: " << MATRIX_NUMBER_COLUMNS << endl;

        cout << "\n>> Deliver: " << QTY_POINTS[STOPS::DELIVER] << endl;

        generate_neighbors(neighbors_qty);
        generate_further_neighbors(neighbors_qty);

        // Initialize variables that will be used in the loop
        Array_Solution a_neighbor_solution, a_initial_solution;
        bool added = false;
//        Pool_of_Solutions p_temp = Pool_of_Solutions(NUMBER_ITERATIONS);
//        p_temp.clean();
        int actual_max_size = MAX_TIME / TIME_INTERVAL;
        int i = 0;
        int best_iteration = 0;
        //
        auto start = std::chrono::system_clock::now();
        double diff_count = 0;

        cout << "\nSTART: " << endl;
        do
    //    for (i = 1; (i < MAX_ITERATIONS && !completed) || (i < NUMBER_ITERATIONS); i++)
        {
//            Pool_of_Solutions p_temp(NUMBER_ITERATIONS);
            i++;
            cout << "\nITERATION: " << i << endl;
            a_initial_solution = Array_Solution();

            // Multidrone
            for (int i = 0; i < NUMBER_ROUTES; i++) {
                Route route = Route(m_map->get_size()/2);
                a_initial_solution.add_route(route);
//                delete route;
            }

            a_initial_solution = initial_solution2(MATRIX_NUMBER_LINES, MATRIX_NUMBER_COLUMNS, a_initial_solution);

            a_initial_solution.set_iteration(-i);
            a_initial_solution = evaluate(a_initial_solution, m_map);


            // Data Mining
//            if(pattern_vel.size() > 0){
//                for (int pos = 0; pos < a_initial_solution->get_size(); pos++) {
//                    for (int count = 0; count < pattern_vel.size(); count++) {
//                        if(a_initial_solution->get_solution_pair(pos).is_at_same_position(pattern_vel[count])){
//                            a_initial_solution->set_solution_pair(pattern_vel[count], pos);
//                            break;
//                        }
//                    }
//                }
//            }

//            a_initial_solution = evaluate(a_initial_solution, m_map);
//            cout << "ANTES p_temp \n";
//            p_temp.add(a_initial_solution);
            a_temp.push_back(a_initial_solution);
//            cout << "DEPOIS p_temp\n";

            cout << "p_temp size: " << p_temp.get_size() << "\n";

//            cout << "ANTES pool_of_solutions \n";
            pool_of_solutions.add_solution(a_initial_solution);
//            cout << "DEPOIS pool_of_solutions\n";

//            a_solutions.push_back(a_initial_solution->copy());

//            delete a_initial_solution;

            cout << "diff_count: " << diff_count << "\n";
            cout << "time_limit: " << time_limit << "\n";
            cout << "p_temp size: " << p_temp.get_size() << "\n";

            cout << ">> Local Search (VNS)" << endl;           

            // Local Search (VNS)
//            for (int n_count = 0; n_count < NEIGHBORHOOD_QTY  && diff_count < MAX_TIME_ALG; n_count++) {
            // Run through all routes perturbed
            for (int j = 0; j < a_temp.size() && diff_count < time_limit; j++) {
//                cout << "FOR 1 " << j << "\n";

    //            while(loop_added){
    //                loop_added = false;
                // Run through all neighborhoods
    //            for (int n_count = 0; n_count < NEIGHBORHOOD.size(); n_count++) {
    //                if(!NEIGHBORHOOD_STABLE[n_count]){
                        // Apply local search

    //            if(p_temp->get_solutions()[j]->is_valid()){

                // Run through all neighborhoods
                for (int n_count = 0; n_count < NEIGHBORHOOD_QTY && diff_count < time_limit; n_count++) {
//                    cout << "FOR 2 " << n_count << "\n";
//                for (int j = 0; j < p_temp->get_size(); j++) {

                    // If solution 'j' was not visited by neighbor 'n_count'
//                    if(p_temp.get_solutions()[j].is_neighborhood_visited(n_count) == 0){
                    if(a_temp[j].is_neighborhood_visited(n_count) == 0){
                        added = true;
//                        p_temp.get_solutions()[j].set_neighborhood_visitation(n_count, 1);

//                        p_temp.solutions[j].set_neighborhood_visitation(n_count, 1);
                        a_temp[j].set_neighborhood_visitation(n_count, 1);

                        while(added
                              && diff_count < time_limit){

                            added = false;
                            auto start_neighbor = std::chrono::system_clock::now();
//                            cout << "ANTES neighbor " << j << "\n";

//                            vector<Array_Solution> sols = p_temp.get_solutions();

                            // NEIGHBOR
//                            a_neighbor_solution = change_vel(j, a_temp, 1);
//                            a_neighbor_solution = swap4_multi(j, a_temp);
//                            a_neighbor_solution = a_temp[j].copy();
//                            int pos_route   = (rand() % a_neighbor_solution.get_size());
//                            int start_rand  = (rand() % (a_neighbor_solution.get_routes()[pos_route].get_size() - 2)) + 1;
//                            int end_rand    = (rand() % (a_neighbor_solution.get_routes()[pos_route].get_size() - start_rand - 1)) + start_rand + 1;
//                        //    Solution_Pair pair;


//                            for (int current_pos = start_rand; current_pos < end_rand; current_pos++) {
//                                if(can_change_vel(a_neighbor_solution.get_routes()[pos_route], current_pos, 1)){
//                        //            vel = new_solution->get_solution_pair(current_pos).get_vel() + value;
//                        //            new_solution->get_solution_pair(current_pos).set_vel(vel);
//                                    Solution_Pair pair(a_neighbor_solution.get_routes()[pos_route].get_solution_pair(current_pos).get_x(),
//                                                       a_neighbor_solution.get_routes()[pos_route].get_solution_pair(current_pos).get_y(),
//                                                       a_neighbor_solution.get_routes()[pos_route].get_solution_pair(current_pos).get_vel() + 1,
//                                                       a_neighbor_solution.get_routes()[pos_route].get_solution_pair(current_pos).get_charge_rate(),
//                                                       a_neighbor_solution.get_routes()[pos_route].get_solution_pair(current_pos).get_battery_charge());
//                                    a_neighbor_solution.routes[pos_route].set_solution_pair(pair, current_pos);

//                        //            new_solution.routes[pos_route].get_solution_pair(current_pos).add_vel(value);
//                                }
//                            }
                            // NEIGHBOR


                            a_neighbor_solution = neighbor(j, a_temp, n_count);//neighbor(p_temp->get_solutions()[j], MATRIX_NUMBER_LINES, MATRIX_NUMBER_COLUMNS, NEIGHBORHOOD[n_count]);
//                            cout << "DEPOIS neighbor\n";

                            auto end_neighbor = std::chrono::system_clock::now();
                            std::chrono::duration<double> diff_neighbor = end_neighbor - start_neighbor;
                            NEIGHBORHOOD_QTY_VEC[n_count]++;
                            NEIGHBORHOOD_TIME[n_count] += diff_neighbor.count();

                            if(!a_neighbor_solution.is_null()){
//                                if(!a_neighbor_solution->get_solution_pair(a_neighbor_solution->get_size() - 1).is_neighbor_from(a_neighbor_solution->get_solution_pair(a_neighbor_solution->get_size() - 2))){
//                                    cout << "";
//                                }else{
//                                    cout << "";
//                                }






                                a_neighbor_solution.set_iteration(i);

                                added = pool_of_solutions.add_solution(a_neighbor_solution);

                                if(added){
                                    NEIGHBORHOOD_ADDED[n_count]++;
                                }


                                for (int k = 0; k < a_temp.size(); k++) {
                                    if(k != j){
//                                        if(a_neighbor_solution.dominates(p_temp.get_solutions()[k])){
                                          if(a_neighbor_solution.dominates(a_temp[k])){
//                                            cout << "ANTES REMOVE \n";
//                                            p_temp.remove_solution(k);
                                              a_temp.erase(a_temp.begin() + k);

//                                            cout << "DEPOIS REMOVE \n";

                                            k--;
                                            if(k < j){
                                                j--;
                                            }
                                        }
                                    }
                                }

//                                cout << "ANTES ADD \n";
//                                p_temp.add(a_neighbor_solution);
                                if(a_temp.size() < 5){
                                    a_temp.push_back(a_neighbor_solution);
                                }


//                                p_temp.add_solution(a_neighbor_solution);
//                                cout << "DEPOIS ADD \n";






//                                delete a_neighbor_solution;
                            }
            //                }
            //            }

                            auto end = std::chrono::system_clock::now();
                            std::chrono::duration<double> diff = end-start;
                            diff_count = diff.count();
                        }
                }
//                    auto end = std::chrono::system_clock::now();
//                    std::chrono::duration<double> diff = end-start;
//                    diff_count = diff.count();
                    }

            }            


            // Clean temporary pool of solutions
//            delete p_temp;




//            cout << "ANTES \n";
//            p_temp = Pool_of_Solutions(NUMBER_ITERATIONS);

//            cout << "SIZE: " << p_temp.get_size() << "\n";
//            p_temp.clean();
            a_temp.clear();
//            p_temp = Pool_of_Solutions(NUMBER_ITERATIONS);
//            pool_of_solutions = Pool_of_Solutions(NUMBER_ITERATIONS);

//            for (int k = 0; k < p_temp.get_size(); k++) {
//                p_temp.remove_solution(k);
//                k--;
//            }
//            cout << "DEPOIS \n";
        } while(diff_count < time_limit);

//        delete p_temp;

//        string results_file_name = "/home/elias/Documents/Mestrado/uav_routing_qt/UAV_Routing_Qt_1/results/time_420/hard/size_30/swap_04_05/vns/";//results_510100_swap_04_05_time_420_10.txt";        
        string results_name = "results_";
        string file_count = to_string(test_count);
        string file_format = ".txt";

        string results_file_name = base_file_name + results_name + file_count + file_format;
//        strcat(results_file_name, results_name);
//        strcat(results_file_name, file_count);
//        strcat(results_file_name, file_format);
        cout << "Results file name: " << results_file_name << "\n" << endl;

//        string info_file_name    = "/home/elias/Documents/Mestrado/uav_routing_qt/UAV_Routing_Qt_1/results/time_420/hard/size_30/swap_04_05/vns/";//info_510100_swap_04_05_time_420_10.txt";
        string info_name = "info_";
//        strcat(results_file_name, info_name);
//        strcat(results_file_name, file_count);
//        strcat(results_file_name, file_format);

        string info_file_name = base_file_name + info_name + file_count + file_format;
        cout << "Info file name: " << info_file_name << "\n" << endl;

        cout << "\nMAX ITERATION: " << iteration_completed << endl;
        cout << "\nLAST ITERATION: " << i << endl;

        string completed_resp = completed ? "true" : "false";
        cout << "\nCOMPLETED: " << completed_resp << endl;

        auto end = std::chrono::system_clock::now();
        std::chrono::duration<double> diff = end-start;
        cout << "Total time: " << diff.count() << "\n" << endl;

        //START: Info file
        ofstream info_file;
        info_file.open (info_file_name);

        info_file << "INSTANCE: " << instance_name << "\n";
        info_file << "\nPOINTS: " << QTY_POINTS[STOPS::DELIVER] + QTY_POINTS[STOPS::COLLECT] << "\n";
        info_file << "\nRECHARGE: " << QTY_POINTS[STOPS::CHARGE] << "\n";
        info_file << "\nPROHIBITED: " << QTY_POINTS[STOPS::PROHIBITED] << "\n";
        info_file << "\n------------------------------------------" << "\n";

//        info_file << "\nMETHODS: " << "swap 04 and 05" << "\n";

        info_file << "\nMETHODS: " << "swap ";

        if(swap_3_available){
            info_file << "03 ";
        }
        if(swap_4_available){
            info_file << "04 ";
        }
        if(swap_5_available){
            info_file << "05 ";
        }
        if(swap_6_available){
            info_file << "06 ";
        }

        info_file << "\n";

        info_file << "\nSTOP CHRITERION: " << "time (" << time_limit << " s) - hard" << "\n";
        info_file << "\nPOOL MAX SIZE: " << "30" << "\n";
        info_file << "\n------------------------------------------" << "\n";

        info_file << "\nLAST ITERATION: " << i << "\n";
        info_file << "\n# OF SOLUTIONS: " << pool_of_solutions.get_size() << "\n";
        info_file << "\nCOMPLETED: " << completed_resp << "\n";
        info_file << "\nTOTAL TIME: " << diff.count() << "\n" << "\n";
        info_file << "------------------------------------------" << "\n\n";
        //END: Info file

        int max_final_charge = INT_MIN, min_time = INT_MAX, min_consumption = INT_MAX;
        int total_final_charge = 0, total_time = 0, total_consumption = 0;

        double z1, z2, z3;

        ofstream myfile;
        myfile.open (results_file_name);


        //Create directory to store solutions
        string solutions_directory_name = base_file_name + "solutions";
        mkdir(solutions_directory_name.c_str(), 0777);

        string routes_directory_name = solutions_directory_name + "/routes";
        mkdir(routes_directory_name.c_str(), 0777);

        for (int k = 0; k < pool_of_solutions.get_size(); k++) {

//            a_solutions.push_back(pool_of_solutions.get_solutions()[k]->copy());
            pool_of_solutions.get_solutions()[k].to_file(solutions_directory_name + "/solution_" + to_string(test_count) + "_" + to_string(k));

            z1 = pool_of_solutions.get_solutions()[k].get_z()[1];
            z2 = (-1)*pool_of_solutions.get_solutions()[k].get_z()[2];
            z3 = (-1)*pool_of_solutions.get_solutions()[k].get_z()[3];

            if(z1 > max_final_charge){
                max_final_charge = z1;
            }

            if(z2 < min_time){
                min_time = z2;
            }

            if(z3 < min_consumption){
                min_consumption = z3;
            }

            total_final_charge += z1;
            total_time += z2;
            total_consumption += z3;

            myfile << z1 << " " << -z2 << " " << -z3 << "\n";
        }

        myfile.close();

        double average_final_charge = total_final_charge/ (double) pool_of_solutions.get_size();
        double average_time         = total_time        / (double) pool_of_solutions.get_size();
        double average_consumption  = total_consumption / (double) pool_of_solutions.get_size();

        double dev_final_charge = 0;
        double dev_time         = 0;
        double dev_consumption  = 0;

        for (int k = 0; k < pool_of_solutions.get_size(); k++) {
            dev_final_charge += pow(  pool_of_solutions.get_solutions()[k].get_z()[1] - average_final_charge, 2);
            dev_time         += pow(- pool_of_solutions.get_solutions()[k].get_z()[2] - average_time, 2);
            dev_consumption  += pow(- pool_of_solutions.get_solutions()[k].get_z()[3] - average_consumption, 2);
        }

        dev_final_charge = sqrt(dev_final_charge);
        dev_time         = sqrt(dev_time);
        dev_consumption  = sqrt(dev_consumption);

        for (int count = 0; count < NEIGHBORHOOD_QTY_VEC.size(); count++) {
            if(NEIGHBORHOOD_QTY_VEC[count] > 0){
                cout      << ">> Vizinhanca " << count << ": " << NEIGHBORHOOD_ADDED[count] << " added - " << NEIGHBORHOOD_QTY_VEC[count] << " (" << NEIGHBORHOOD_TIME[count] /  (double) NEIGHBORHOOD_QTY_VEC[count] << " s)" << endl;
                info_file << ">> Vizinhanca " << count << ": " << NEIGHBORHOOD_ADDED[count] << " added - " << NEIGHBORHOOD_QTY_VEC[count] << " (" << NEIGHBORHOOD_TIME[count] /  (double) NEIGHBORHOOD_QTY_VEC[count] << " s)" << "\n";
            }
        }

        cout      << ">> Subpath3: " << sub_path3_qty << " (" << sub_path3_time /  (double) sub_path3_qty << " s)" << endl;
        info_file << ">> Subpath3: " << sub_path3_qty << " (" << sub_path3_time /  (double) sub_path3_qty << " s)" << "\n";

        cout << ">> Average Final Charge: " << average_final_charge <<
                " >> Standard Deviation Final Charge: " << dev_final_charge <<
                " >> Best Final Charge: " << max_final_charge << endl;
        cout << ">> Average Time: " << average_time <<
                " >> Standard Deviation Time: " << dev_time <<
                " >> Best Time: " << min_time << endl;
        cout << ">> Average Consumption: " << average_consumption <<
                " >> Standard Deviation Consumption: " << dev_consumption <<
                " >> Best Consumption: " << min_consumption << endl;

        //START: Info file
        info_file << "\n------------------------------------------" << "\n";
        info_file << "\n";

        info_file << ">> Average Final Charge: " << average_final_charge <<
                    " >> Standard Deviation Final Charge: " << dev_final_charge <<
                    " >> Best Final Charge: " << max_final_charge << "\n";
        info_file << ">> Average Time: " << average_time <<
                    " >> Standard Deviation Time: " << dev_time <<
                    " >> Best Time: " << min_time << "\n";
        info_file << ">> Average Consumption: " << average_consumption <<
                    " >> Standard Deviation Consumption: " << dev_consumption <<
                    " >> Best Consumption: " << min_consumption << "\n";

        info_file.close();
        //END: Info file

        //START: Delete variables
//        delete m_map;
//        client_points.clear();
//        recharge_points.clear();
        completed = false;

//        for (int var = 0; var < pool_of_solutions.get_size(); var++) {
//            delete pool_of_solutions.get_solutions()[var];
//        }

        pool_of_solutions = Pool_of_Solutions(NUMBER_ITERATIONS);

//        for (int count = 0; count < a_solutions.size(); count++) {
//            delete a_solutions[count];
//        }

//        a_solutions.clear();

        for (int count = 0; count < NEIGHBORHOOD_QTY_VEC.size(); count++) {
            NEIGHBORHOOD_QTY_VEC[count] = 0;
            NEIGHBORHOOD_TIME[count] = 0;
            NEIGHBORHOOD_ADDED[count] = 0;
        }

        sub_path3_qty  = 0;
        sub_path3_time = 0;
        //END: Delete
    }

    return aggregate_2(num_exec, 3, base_file_name);// a_solutions;
}

//vector<Array_Solution *> Core::test_vns(int time_limit, int num_exec, string instance_name, string directory_name, int pct_prohibited = 0)
//{
//    string base_file_name = directory_name + "/" + "vns_swap_";


//    if(swap_3_available){
//        base_file_name += "03_";
//    }
//    if(swap_4_available){
//        base_file_name += "04_";
//    }
//    if(swap_5_available){
//        base_file_name += "05_";
//    }
//    if(swap_6_available){
//        base_file_name += "06_";
//    }

//    mkdir(base_file_name.c_str(), 0777);

//    base_file_name += "/";

//    for (int test_count = 0; test_count < num_exec; test_count++) {
//        // Ensures that the number will be randomly generated
//        srand(time(0));

//        // Initializing the array that represents the map

//        // Generates the map
////        if(type == 0){
////            m_map = initialize_mtrx(m_map);
////        } else if (type == 1){
////            m_map = initialize_mtrx2(m_map, "/home/elias/Documents/Mestrado/uav_routing_qt/UAV_Routing_Qt_1/mtrx.txt");//
////        } else if (type == 2){
////            m_map = initialize_mtrx_load_test_files(m_map, "/home/elias/Documents/Mestrado/uav_routing_qt/UAV_Routing_Qt_1/test.lib", pct_prohibited);
////        }

//        // Print the map
//        m_map->print();

//        cout << ">> Lines: " << MATRIX_NUMBER_LINES << endl;
//        cout << ">> Columns: " << MATRIX_NUMBER_COLUMNS << endl;

//        cout << "\n>> Deliver: " << QTY_POINTS[STOPS::DELIVER] << endl;

//        generate_neighbors(neighbors_qty);
//        generate_further_neighbors(neighbors_qty);

//        // Initialize variables that will be used in the loop
//        Array_Solution * a_neighbor_solution;
//        bool added = false;
//        Pool_of_Solutions * p_temp = new Pool_of_Solutions(NUMBER_ITERATIONS);
//        int actual_max_size = MAX_TIME / TIME_INTERVAL;
//        int i = 0;
//        int best_iteration = 0;
//        //
//        auto start = std::chrono::system_clock::now();
//        double diff_count = 0;

//        cout << "\nSTART: " << endl;
//        do
//    //    for (i = 1; (i < MAX_ITERATIONS && !completed) || (i < NUMBER_ITERATIONS); i++)
//        {
//            i++;
//            cout << "\nITERATION: " << i << endl;
//            Array_Solution * a_initial_solution = new Array_Solution();

//            // Multidrone
//            for (int i = 0; i < NUMBER_ROUTES; i++) {
//                Route * route = new Route(MAX_TIME / TIME_INTERVAL + 1);
//                a_initial_solution->add_route(route);
//            }

//            a_initial_solution = initial_solution2(MATRIX_NUMBER_LINES, MATRIX_NUMBER_COLUMNS, a_initial_solution);

//            a_initial_solution->set_iteration(-i);
//            a_initial_solution = evaluate(a_initial_solution, m_map);


//            if(pattern_vel.size() > 0){
//                for (int pos = 0; pos < a_initial_solution->get_size(); pos++) {
//                    for (int count = 0; count < pattern_vel.size(); count++) {
//                        if(a_initial_solution->get_solution_pair(pos).is_at_same_position(pattern_vel[count])){
//                            a_initial_solution->set_solution_pair(pattern_vel[count], pos);
//                            break;
//                        }
//                    }
//                }
//            }

//            a_initial_solution = evaluate(a_initial_solution, m_map);
//            p_temp->add(a_initial_solution);
//            pool_of_solutions.add_solution(a_initial_solution);
////            a_solutions.push_back(a_initial_solution->copy());

//            delete a_initial_solution;

//            cout << ">> Local Search (VNS)" << endl;
//            // Local Search (VNS)
//            bool loop_added = true;

//            while(loop_added){
//                loop_added = false;
//            for (int n_count = 0; n_count < NEIGHBORHOOD_QTY  && diff_count < time_limit; n_count++) {
//            // Run through all routes perturbed
//    //        for (int j = 0; j < p_temp->get_size() && diff_count < MAX_TIME_ALG; j++) {

//    //            while(loop_added){
//    //                loop_added = false;
//                // Run through all neighborhoods
//    //            for (int n_count = 0; n_count < NEIGHBORHOOD.size(); n_count++) {
//    //                if(!NEIGHBORHOOD_STABLE[n_count]){
//                        // Apply local search

//    //            if(p_temp->get_solutions()[j]->is_valid()){

//    //            for (int n_count = 0; n_count < NEIGHBORHOOD_QTY && diff_count < MAX_TIME_ALG; n_count++) {
//                for (int j = 0; j < p_temp->get_size() && diff_count < time_limit; j++) {

//                    if(p_temp->get_solutions()[j]->is_neighborhood_visited(n_count) == 0){
//                        added = true;
//                        p_temp->get_solutions()[j]->set_neighborhood_visitation(n_count, 1);

//                        while(added
//                              && diff_count < time_limit){
//                            added = false;
//                            auto start_neighbor = std::chrono::system_clock::now();
//                            a_neighbor_solution = neighbor(j, p_temp->get_solutions(), n_count);//neighbor(p_temp->get_solutions()[j], MATRIX_NUMBER_LINES, MATRIX_NUMBER_COLUMNS, NEIGHBORHOOD[n_count]);

//                            auto end_neighbor = std::chrono::system_clock::now();
//                            std::chrono::duration<double> diff_neighbor = end_neighbor - start_neighbor;
//                            NEIGHBORHOOD_QTY_VEC[n_count]++;
//                            NEIGHBORHOOD_TIME[n_count] += diff_neighbor.count();

//                            if(a_neighbor_solution){
//                                a_neighbor_solution->set_iteration(i);

//                                added = pool_of_solutions.add_solution(a_neighbor_solution);

//                                if(added){
//                                    NEIGHBORHOOD_ADDED[n_count]++;
//                                    loop_added = true;
//                                }

//                                for (int k = 0; k < p_temp->get_size(); k++) {
//                                    if(k != j){
//                                        if(a_neighbor_solution->dominates(p_temp->get_solutions()[k])){
//                                            p_temp->remove_solution(k);
//                                            k--;
//                                            if(k < j){
//                                                j--;
//                                            }
//                                        }
//                                    }
//                                }

//                                p_temp->add(a_neighbor_solution);
//                                delete a_neighbor_solution;
//                            }
//            //                }
//            //            }

//                            auto end = std::chrono::system_clock::now();
//                            std::chrono::duration<double> diff = end-start;
//                            diff_count = diff.count();
//                        }
//                }
////                    auto end = std::chrono::system_clock::now();
////                    std::chrono::duration<double> diff = end-start;
////                    diff_count = diff.count();
//                    }

//            }
//            }


//            // Clean temporary pool of solutions
//            delete p_temp;
//            p_temp = new Pool_of_Solutions(NUMBER_ITERATIONS);
//        } while(diff_count < time_limit);

//        delete p_temp;

////        string results_file_name = "/home/elias/Documents/Mestrado/uav_routing_qt/UAV_Routing_Qt_1/results/time_420/hard/size_30/swap_04_05/vns/";//results_510100_swap_04_05_time_420_10.txt";
//        string results_name = "results_";
//        string file_count = to_string(test_count);
//        string file_format = ".txt";

//        string results_file_name = base_file_name + results_name + file_count + file_format;
//        cout << "Results file name: " << results_file_name << "\n" << endl;

//        string info_name = "info_";

//        string info_file_name = base_file_name + info_name + file_count + file_format;
//        cout << "Info file name: " << info_file_name << "\n" << endl;

//        cout << "\nMAX ITERATION: " << iteration_completed << endl;
//        cout << "\nLAST ITERATION: " << i << endl;

//        string completed_resp = completed ? "true" : "false";
//        cout << "\nCOMPLETED: " << completed_resp << endl;

//        auto end = std::chrono::system_clock::now();
//        std::chrono::duration<double> diff = end-start;
//        cout << "Total time: " << diff.count() << "\n" << endl;

//        //START: Info file
//        ofstream info_file;
//        info_file.open (info_file_name);

//        info_file << "INSTANCE: " << instance_name << "\n";
//        info_file << "\nPOINTS: " << QTY_POINTS[STOPS::DELIVER] + QTY_POINTS[STOPS::COLLECT] << "\n";
//        info_file << "\nRECHARGE: " << QTY_POINTS[STOPS::CHARGE] << "\n";
//        info_file << "\nPROHIBITED: " << QTY_POINTS[STOPS::PROHIBITED] << "\n";
//        info_file << "\n------------------------------------------" << "\n";

//        info_file << "\nMETHODS: " << "swap ";

//        if(swap_3_available){
//            info_file << "03 ";
//        }
//        if(swap_4_available){
//            info_file << "04 ";
//        }
//        if(swap_5_available){
//            info_file << "05 ";
//        }
//        if(swap_6_available){
//            info_file << "06 ";
//        }

//        info_file << "\n";

//        info_file << "\nSTOP CHRITERION: " << "time (" << time_limit << " s) - hard" << "\n";
//        info_file << "\nPOOL MAX SIZE: " << "30" << "\n";
//        info_file << "\n------------------------------------------" << "\n";

//        info_file << "\nLAST ITERATION: " << i << "\n";
//        info_file << "\n# OF SOLUTIONS: " << pool_of_solutions.get_size() << "\n";
//        info_file << "\nCOMPLETED: " << completed_resp << "\n";
//        info_file << "\nTOTAL TIME: " << diff.count() << "\n" << "\n";
//        info_file << "------------------------------------------" << "\n\n";
//        //END: Info file

//        int max_final_charge = INT_MIN, min_time = INT_MAX, min_consumption = INT_MAX;
//        int total_final_charge = 0, total_time = 0, total_consumption = 0;

//        double z1, z2, z3;

//        ofstream myfile;
//        myfile.open (results_file_name);

//        //Create directory to store solutions
//        string solutions_directory_name = base_file_name + "solutions";
//        mkdir(solutions_directory_name.c_str(), 0777);

//        string routes_directory_name = solutions_directory_name + "/routes";
//        mkdir(routes_directory_name.c_str(), 0777);

//        for (int k = 0; k < pool_of_solutions.get_size(); k++) {

////            a_solutions.push_back(pool_of_solutions.get_solutions()[k]->copy());
//            pool_of_solutions.get_solutions()[k]->to_file(solutions_directory_name + "/solution_" + to_string(test_count) + "_" + to_string(k));

//            z1 = pool_of_solutions.get_solutions()[k]->get_z()[1];
//            z2 = (-1)*pool_of_solutions.get_solutions()[k]->get_z()[2];
//            z3 = (-1)*pool_of_solutions.get_solutions()[k]->get_z()[3];

//            if(z1 > max_final_charge){
//                max_final_charge = z1;
//            }

//            if(z2 < min_time){
//                min_time = z2;
//            }

//            if(z3 < min_consumption){
//                min_consumption = z3;
//            }

//            total_final_charge += z1;
//            total_time += z2;
//            total_consumption += z3;

//            myfile << z1 << " " << -z2 << " " << -z3 << "\n";
//        }

//        myfile.close();

//        double average_final_charge = total_final_charge/ (double) pool_of_solutions.get_size();
//        double average_time         = total_time        / (double) pool_of_solutions.get_size();
//        double average_consumption  = total_consumption / (double) pool_of_solutions.get_size();

//        double dev_final_charge = 0;
//        double dev_time         = 0;
//        double dev_consumption  = 0;

//        for (int k = 0; k < pool_of_solutions.get_size(); k++) {
//            dev_final_charge += pow(  pool_of_solutions.get_solutions()[k]->get_z()[1] - average_final_charge, 2);
//            dev_time         += pow(- pool_of_solutions.get_solutions()[k]->get_z()[2] - average_time, 2);
//            dev_consumption  += pow(- pool_of_solutions.get_solutions()[k]->get_z()[3] - average_consumption, 2);
//        }

//        dev_final_charge = sqrt(dev_final_charge);
//        dev_time         = sqrt(dev_time);
//        dev_consumption  = sqrt(dev_consumption);

//        for (int count = 0; count < NEIGHBORHOOD_QTY_VEC.size(); count++) {
//            if(NEIGHBORHOOD_QTY_VEC[count] > 0){
//                cout      << ">> Vizinhanca " << count << ": " << NEIGHBORHOOD_ADDED[count] << " added - " << NEIGHBORHOOD_QTY_VEC[count] << " (" << NEIGHBORHOOD_TIME[count] /  (double) NEIGHBORHOOD_QTY_VEC[count] << " s)" << endl;
//                info_file << ">> Vizinhanca " << count << ": " << NEIGHBORHOOD_ADDED[count] << " added - " << NEIGHBORHOOD_QTY_VEC[count] << " (" << NEIGHBORHOOD_TIME[count] /  (double) NEIGHBORHOOD_QTY_VEC[count] << " s)" << "\n";
//            }
//        }

//        cout      << ">> Subpath3: " << sub_path3_qty << " (" << sub_path3_time /  (double) sub_path3_qty << " s)" << endl;
//        info_file << ">> Subpath3: " << sub_path3_qty << " (" << sub_path3_time /  (double) sub_path3_qty << " s)" << "\n";


//        cout << ">> Average Final Charge: " << average_final_charge <<
//                " >> Standard Deviation Final Charge: " << dev_final_charge <<
//                " >> Best Final Charge: " << max_final_charge << endl;
//        cout << ">> Average Time: " << average_time <<
//                " >> Standard Deviation Time: " << dev_time <<
//                " >> Best Time: " << min_time << endl;
//        cout << ">> Average Consumption: " << average_consumption <<
//                " >> Standard Deviation Consumption: " << dev_consumption <<
//                " >> Best Consumption: " << min_consumption << endl;

//        //START: Info file
//        info_file << "\n------------------------------------------" << "\n";
//        info_file << "\n";

//        info_file << ">> Average Final Charge: " << average_final_charge <<
//                    " >> Standard Deviation Final Charge: " << dev_final_charge <<
//                    " >> Best Final Charge: " << max_final_charge << "\n";
//        info_file << ">> Average Time: " << average_time <<
//                    " >> Standard Deviation Time: " << dev_time <<
//                    " >> Best Time: " << min_time << "\n";
//        info_file << ">> Average Consumption: " << average_consumption <<
//                    " >> Standard Deviation Consumption: " << dev_consumption <<
//                    " >> Best Consumption: " << min_consumption << "\n";

//        info_file.close();
//        //END: Info file

//        //START: Delete
////        delete m_map;
////        client_points.clear();
////        recharge_points.clear();
//        completed = false;
//        pool_of_solutions = Pool_of_Solutions(NUMBER_ITERATIONS);

////        for (int count = 0; count < a_solutions.size(); count++) {
////            delete a_solutions[count];
////        }

////        a_solutions.clear();

//        for (int count = 0; count < NEIGHBORHOOD_QTY_VEC.size(); count++) {
//            NEIGHBORHOOD_QTY_VEC[count] = 0;
//            NEIGHBORHOOD_TIME[count] = 0;
//            NEIGHBORHOOD_ADDED[count] = 0;
//        }

//        sub_path3_qty  = 0;
//        sub_path3_time = 0;

//        //END: Delete
//    }

//    return aggregate_2(num_exec, 3, base_file_name);// a_solutions;
//}

void Core::evaluate_methods(string directory_name, int num_exec = 5)
{
//    const char * directory_name = "/home/elias/Documents/Mestrado/uav_routing_qt/UAV_Routing_Qt_1/results/time_420/hard/size_30/";//"/home/elias/Documents/Mestrado/uav_routing_qt/UAV_Routing_Qt_1/results/time_420/hard/size_30/swap_04_05/vns/";
//    aggregate(5, directory_name);

    int num_obj  = 3;

    vector<double> ref_points;
    vector<string> files_names;

    Array_Objective_Solution * a_o_s;
    Array_Objective_Solution * pareto_ref = new Array_Objective_Solution();
    Objective_Solution * o_s;

    double z;

    for (int var = 0; var < num_obj; var++) {
        ref_points.push_back(INT_MAX);
    }

    string d_name;
    DIR * dir;
    struct dirent * ent;
    if((dir = opendir(directory_name.c_str())) != NULL){
        while((ent = readdir(dir)) != NULL){
//            printf("%s\n", ent->d_name);
            d_name = ent->d_name;
            if(d_name.compare(".") != 0 && d_name.compare("..") != 0 &&
                    d_name.compare("matrix.txt")     != 0 &&
                    d_name.compare("visited.txt")    != 0 &&
                    d_name.compare("origin.txt")     != 0 &&
                    d_name.compare("comparison.txt") != 0 &&
                    d_name.compare("paths") != 0){

                cout << ent->d_name << endl;
//                files_names.push_back(directory_name + "/" + d_name + "/aggregation.txt");
                files_names.push_back(d_name);                
                a_o_s = aggregate(num_exec, num_obj, directory_name + "/" + d_name + "/");
//                aggregate_2(num_exec, num_obj, directory_name + "/" + d_name + "/");
                aggregate_STRUCT(num_exec, num_obj, directory_name + "/" + d_name + "/");

                //Update the reference points to hypervolume and
                //Update the pareto reference
                for (int s = 0; s < a_o_s->size(); s++) {
                    o_s = new Objective_Solution(num_obj);

                    for (int o = 0; o < a_o_s->get(s)->get_num_objectives(); o++) {
                        z = a_o_s->get(s)->get_z()[o];
                        o_s->set_z(o, z);

                        if(z < ref_points[o]){
                            ref_points[o] = z;
                        }
                    }

                    pareto_ref->add(o_s);
                    delete o_s;
                }

                delete a_o_s;
            }

        }

        closedir(dir);

        cout << endl;
        cout << "Ref points: ";
        for (int var = 0; var < num_obj; var++) {
            ref_points[var] -= 1;
            cout << ref_points[var] << " ";
        }
        cout << endl;
        cout << endl;

        double hypervolume, coverage;
        int cardinalite;

        //START: Comparison file
        ofstream comparison_file;
        comparison_file.open (directory_name + "/comparison.txt");

        Hypervolume hyp;
        for (int var = 0; var < files_names.size(); var++) {
            a_o_s = new Array_Objective_Solution();
            a_o_s->from_file(directory_name + "/" + files_names[var] + "/aggregation.txt");

            hypervolume = hyp.hypervolumeWithExecRequestedbyFile(directory_name + "/" + files_names[var] + "/aggregation.txt", ref_points);
            coverage = hyp.setCoverage(a_o_s, pareto_ref);
            cardinalite = hyp.cardinalite(a_o_s, pareto_ref);

            cout << "Hypervolume: " << hypervolume << endl;
            cout << "Coverage: "    << coverage << endl;
            cout << "Cardinalite: " << cardinalite << endl;
            cout << endl;

            ofstream metrics_file;
            metrics_file.open (directory_name + "/" + files_names[var] + "/metrics.txt");

            metrics_file << "Hypervolume: " << hypervolume << "\n";
            metrics_file << "Coverage: " << coverage << "\n";
            metrics_file << "Cardinalite: " << cardinalite << "\n";            

            metrics_file.close();


            // Comparison
            comparison_file << "\n------------------------------------------" << "\n";
            comparison_file << "\n";
            comparison_file << files_names[var];
            comparison_file << "\n";
            comparison_file << "\nHypervolume: " << hypervolume << "\n";
            comparison_file << "Coverage: " << coverage << "\n";
            comparison_file << "Cardinalite: " << cardinalite << "\n";

            comparison_file << "\n" << hypervolume <<
                              " & " << coverage <<
                              " & " << cardinalite << "\n";

            delete a_o_s;
        }

        comparison_file << "\n------------------------------------------" << "\n";
        comparison_file << "Pareto Reference" << "\n\n";

        for (int var = 0; var < pareto_ref->size(); var++) {
            comparison_file << "(";
            for (int n_o = 0; n_o < num_obj; n_o++) {
               comparison_file << pareto_ref->get(var)->get_z()[n_o];

               if(n_o != num_obj - 1){
                comparison_file << ", ";
               }
            }
            comparison_file << ")\n";
        }

        comparison_file.close();
        //END: Comparison file

    } else {
      cout << "Directory does not exist" << endl;
      return;
    }
}

Array_Objective_Solution * Core::aggregate(int num_exec, int num_obj, string directory_name)
{    

    cout << directory_name << endl;
    Array_Objective_Solution * a_o_s = new Array_Objective_Solution();
    bool valid = false;

    for (int test_count = 0; test_count < num_exec; test_count++) {
        // Ensures that the number will be randomly generated
        srand(time(0));

        string results_file_name = directory_name;//"/home/elias/Documents/Mestrado/uav_routing_qt/UAV_Routing_Qt_1/results/time_420/hard/size_30/swap_04_05/vns/";
        string results_name = "results_";
        string file_count = to_string(test_count);
        string file_format = ".txt";

        results_file_name = results_file_name + results_name + file_count + file_format;        

        cout << results_file_name << endl;

        ifstream test_file(results_file_name);

        if ( !test_file ){
          cout << "File not found" << endl;
          break;
        }else{
          valid = true;

          int num_solutions = -1;
          string temp;

          do{
              num_solutions++;
          } while(std::getline(test_file, temp));

          test_file.close();

          ifstream test_file_1(results_file_name);

          double  z;
          Objective_Solution * o_s;

          for (int var = 0; var < num_solutions; var++) {
              o_s = new Objective_Solution(num_obj);

              for (int count = 0; count < num_obj; count++) {
                  test_file_1 >> z;
                  o_s->set_z(count, z);
              }

              a_o_s->add(o_s);
              delete o_s;
          }          

          test_file_1.close();
        }

//        string temp;
//        std::getline(test_file, temp); // Names
//        std::getline(test_file, temp); // Comment
//        std::getline(test_file, temp); // Type

//        test_file >> temp; //Dimension
//        test_file >> temp; // :

    }

    if(valid){
        cout << "FINAL:" << endl;

        a_o_s->print();
        a_o_s->to_file(directory_name + "aggregation.txt");
    }

    return a_o_s;
}

vector<Solution *> Core::show_solutions(int num_exec, int num_obj, string parent_directory_name){

    string directory_name = parent_directory_name + "/";

    size_t found = parent_directory_name.find_last_of("/");
    parent_directory_name.erase (found, parent_directory_name.size() - 1);

    // Matrix file
    ifstream test_file(parent_directory_name + "/matrix.txt");

    vector<Solution *> solutions;

    if ( !test_file ){
        cout << "File not found" << endl;

        return solutions;
    }else{
        // Generates the map
        m_map = initialize_mtrx2(m_map, parent_directory_name + "/matrix.txt", parent_directory_name + "/visited.txt");
    }    

    return aggregate_STRUCT(num_exec, num_obj, directory_name);
}

vector<Array_Solution> Core::aggregate_2(int num_exec, int num_obj, string directory_name)
{
    vector<Array_Solution> v_a_s;

    cout << directory_name << endl;
    Array_Solution a_s;
    Pool_of_Solutions p_s = Pool_of_Solutions(30);
    bool valid = false;

    string sol_directory_name = directory_name + "solutions";

    string d_name;
    DIR * dir;
    struct dirent * ent;
    if((dir = opendir(sol_directory_name.c_str())) == NULL){
        cout << "Directory does not exist" << endl;
        return v_a_s;
    } else {
        while((ent = readdir(dir)) != NULL){
//            printf("%s\n", ent->d_name);
            d_name = ent->d_name;
            if(d_name.compare(".") != 0 && d_name.compare("..") != 0){
//                cout << ent->d_name << endl;
                a_s = Array_Tools::from_file_solution(sol_directory_name + "/" + d_name);

                if(!a_s.is_null())
                    p_s.add_solution(a_s);

//                delete a_s;
            }

        }

        closedir(dir);

        double z1, z2, z3;
        int max_final_charge = INT_MIN, min_time = INT_MAX, min_consumption = INT_MAX;
        int total_final_charge = 0, total_time = 0, total_consumption = 0;

        Objective_Solution * o_s;
        Array_Objective_Solution * a_o_s = new Array_Objective_Solution();

        for (int var = 0; var < p_s.get_size(); var++) {
            v_a_s.push_back(p_s.get_solutions()[var].copy());

            o_s = new Objective_Solution(num_obj);

            for (int count = 1; count <= num_obj; count++) {
                o_s->set_z(count - 1, p_s.get_solutions()[var].get_z()[count]);
            }

            a_o_s->add(o_s->copy());
            delete o_s;

            // Average values from objectives
            z1 = p_s.get_solutions()[var].get_z()[1];
            z2 = (-1)*p_s.get_solutions()[var].get_z()[2];
            z3 = (-1)*p_s.get_solutions()[var].get_z()[3];

            if(z1 > max_final_charge){
                max_final_charge = z1;
            }

            if(z2 < min_time){
                min_time = z2;
            }

            if(z3 < min_consumption){
                min_consumption = z3;
            }

            total_final_charge += z1;
            total_time += z2;
            total_consumption += z3;
        }

        double average_final_charge = total_final_charge/ (double) p_s.get_size();
        double average_time         = total_time        / (double) p_s.get_size();
        double average_consumption  = total_consumption / (double) p_s.get_size();

        double dev_final_charge = 0;
        double dev_time         = 0;
        double dev_consumption  = 0;

        for (int k = 0; k < p_s.get_size(); k++) {
            dev_final_charge += pow(  p_s.get_solutions()[k].get_z()[1] - average_final_charge, 2);
            dev_time         += pow(- p_s.get_solutions()[k].get_z()[2] - average_time, 2);
            dev_consumption  += pow(- p_s.get_solutions()[k].get_z()[3] - average_consumption, 2);
        }

        dev_final_charge = sqrt(dev_final_charge);
        dev_time         = sqrt(dev_time);
        dev_consumption  = sqrt(dev_consumption);

        //START: Info file
        ofstream info_file;
        info_file.open (directory_name + "info_aggregation.txt");

        info_file << ">> Average Final Charge: " << average_final_charge <<
                    " >> Standard Deviation Final Charge: " << dev_final_charge <<
                    " >> Best Final Charge: " << max_final_charge << "\n";
        info_file << ">> Average Time: " << average_time <<
                    " >> Standard Deviation Time: " << dev_time <<
                    " >> Best Time: " << min_time << "\n";
        info_file << ">> Average Consumption: " << average_consumption <<
                    " >> Standard Deviation Consumption: " << dev_consumption <<
                    " >> Best Consumption: " << min_consumption << "\n";

        info_file << "\n" << average_final_charge <<
                     "\n" << dev_final_charge <<
                     "\n" << max_final_charge <<
                     "\n" << average_time <<
                     "\n" << dev_time <<
                     "\n" << min_time <<
                     "\n" << average_consumption <<
                     "\n" << dev_consumption <<
                     "\n" << min_consumption << "\n";

        info_file.close();
        //END: Info file

        a_o_s->to_file(directory_name + "aggregation.txt");
    }

    return v_a_s;
}

vector<Array_Solution> Core::neural_network(string instance_file_name, string directory_name, int time_limit = 420, int pct_prohibited = 0)
{

    global_file_name = directory_name;

    // Matrix file
    ifstream test_file(directory_name + "/matrix.txt");

    //  If does not exist a file with the map
    if ( !test_file ){
        cout << "File not found" << endl;

        // Generates the map
        m_map = initialize_mtrx_load_test_files(m_map, instance_file_name, pct_prohibited);
        m_map->save_to_file(directory_name + "/matrix.txt");

    }else{
        // Generates the map
        m_map = initialize_mtrx2(m_map, directory_name + "/matrix.txt", directory_name + "/visited.txt");
    }

    //Set the origin point of the drones
    set_origin_point(m_map, directory_name + "/origin.txt");

    test_file.close();


    size_t found = instance_file_name.find_last_of("/");
    instance_file_name.erase (0,found+1);

    string base_file_name = directory_name + "/" + "neural_network";

    //Create directory
    mkdir(base_file_name.c_str(), 0777);
    base_file_name += "/";


   // Ensures that the number will be randomly generated
   srand(time(0));

   // Print the map
   m_map->print();

   Array_Solution a_initial_solution;
   int actual_max_size = MAX_TIME / TIME_INTERVAL;
   int i = 0;
   //
   auto start = std::chrono::system_clock::now();
   double diff_count = 0;

   cout << "\nSTART: " << endl;
   i++;
   cout << "\nGENERATION: " << i << endl;


   /* Constructor */
   Neural_Network n_n;
   int sign_x, pos_x, sign_y, pos_y, vel, c_r, stop;
   int route_size;

   int population_size = 20;
   for (int num_solutions = 0; num_solutions < population_size; num_solutions++) {
       a_initial_solution = Array_Solution();

       Route route = Route(MAX_TIME / TIME_INTERVAL + 1);
       a_initial_solution.add_route(route);

       vector<Route> routes = a_initial_solution.routes;

       // Add origin
       for (int j = 0; j < a_initial_solution.get_size(); j++) {
           routes[j].add_solution_pair(origin_x, origin_y);
       }

       route_size = 1;

       n_n = Neural_Network(1, 6, 7, 7);

       int num_clients = 0;
       int type;

       do{
           type = m_map->get_stop_point(routes[0].get_solution_pair(route_size - 1)).get_type();

           if(type == STOPS::COLLECT || type == STOPS::DELIVER){
               num_clients++;
           }

           if(num_clients == client_points.size()){
               stop = 1;
           }else{
               n_n.set_entry(routes[0].get_solution_pair(route_size - 1), num_clients, type);
               n_n.calculate_output();

               sign_x = (int) n_n.get_output_layer().get_neuron(0).get_output();
               pos_x  = (int) n_n.get_output_layer().get_neuron(1).get_output();

               if(sign_x == 0){
                   pos_x = pos_x * (-1);
               }

               sign_y = (int) n_n.get_output_layer().get_neuron(2).get_output();
               pos_y  = (int) n_n.get_output_layer().get_neuron(3).get_output();

               if(sign_y == 0){
                   pos_y = pos_y * (-1);
               }

               vel    = (int) n_n.get_output_layer().get_neuron(4).get_output();
               c_r    = (int) n_n.get_output_layer().get_neuron(5).get_output();
               stop   = (int) n_n.get_output_layer().get_neuron(6).get_output();

               if(stop != 1){
                   pos_x = pos_x + routes[0].get_solution_pair(route_size - 1).get_x();
                   pos_y = pos_y + routes[0].get_solution_pair(route_size - 1).get_y();

                   if(!m_map->is_inside_perimeter(pos_x, pos_y) || routes[0].get_size() > m_map->get_size() / 2){
                       stop = 1;
                   }else{
                       Solution_Pair pair = Solution_Pair(pos_x, pos_y, vel, c_r);
                       routes[0].add_solution_pair(pair);

                       route_size++;
                   }
               }
           }

       }while(stop != 1);

       a_initial_solution.set_routes(routes);
       a_initial_solution.set_iteration(i);
       a_initial_solution = evaluate(a_initial_solution, m_map);

       a_initial_solution.set_dna(n_n);
       pool_of_solutions.add_solution(a_initial_solution);
   }

   /* --------------------------------------------------------------------- */

   vector<vector<double>> dnas;
   int dna_size = pool_of_solutions.get_solutions()[0].get_dna().size();
   int step = 5;

   do {
       //NEW GENERATION
       i++;

       /* Mutation */
       for (int var = 0; var < pool_of_solutions.get_size(); var++) {
           dnas.push_back(pool_of_solutions.get_solutions()[var].get_dna());
       }

       int num_solutions = step ? step < dnas.size() : dnas.size();
       int count_dna = 0;

       while(dnas.size() < population_size){
           vector<double> dna_copy = dnas[count_dna];
           dnas.push_back(dna_copy);
           count_dna++;
       }

       for (num_solutions; num_solutions < population_size; num_solutions++) {
           int mutations = (int)(rand() % dna_size) + 1;
           int type;

           for (int var = 0; var < mutations; var++) {
               type = (int) rand() % 3;
               int index = (int)(rand() % dna_size);

               switch (type) {
                   case 0:
                       dnas[num_solutions][index] = rand()%2000 - 1000;
                       break;
                   case 1:
                       dnas[num_solutions][index] = dnas[num_solutions][index] * ((rand() % 10001) / 10000.0 + 0.5);
                       break;
                   case 2:
                       dnas[num_solutions][index] = dnas[num_solutions][index] + (rand()%2000 - 1000) / 100.0;
                       break;
               }
           }
       }
       /* ------------------------------ */

       for (int var = 0; var < dnas.size(); var++) {
           n_n.set_weights(dnas[var]);

           a_initial_solution = Array_Solution();

           Route route = Route(MAX_TIME / TIME_INTERVAL + 1);
           a_initial_solution.add_route(route);

           vector<Route> routes = a_initial_solution.routes;

           // Add origin
           for (int j = 0; j < a_initial_solution.get_size(); j++) {
               routes[j].add_solution_pair(origin_x, origin_y);
           }

           route_size = 1;

           int num_clients = 0;
           int type;

           do{
               type = m_map->get_stop_point(routes[0].get_solution_pair(route_size - 1)).get_type();

               if(type == STOPS::COLLECT || type == STOPS::DELIVER){
                   num_clients++;
               }

               if(num_clients == client_points.size()){
                   stop = 1;
               }else{

                   n_n.set_entry(routes[0].get_solution_pair(route_size - 1), num_clients, type);
                   n_n.calculate_output();

                   sign_x = (int) n_n.get_output_layer().get_neuron(0).get_output();
                   pos_x  = (int) n_n.get_output_layer().get_neuron(1).get_output();

                   if(sign_x == 0){
                       pos_x = pos_x * (-1);
                   }

                   sign_y = (int) n_n.get_output_layer().get_neuron(2).get_output();
                   pos_y  = (int) n_n.get_output_layer().get_neuron(3).get_output();

                   if(sign_y == 0){
                       pos_y = pos_y * (-1);
                   }

                   vel    = (int) n_n.get_output_layer().get_neuron(4).get_output();
                   c_r    = (int) n_n.get_output_layer().get_neuron(5).get_output();
                   stop   = (int) n_n.get_output_layer().get_neuron(6).get_output();

                   if(stop != 1){
                       pos_x = pos_x + routes[0].get_solution_pair(route_size - 1).get_x();
                       pos_y = pos_y + routes[0].get_solution_pair(route_size - 1).get_y();

                       if(!m_map->is_inside_perimeter(pos_x, pos_y) || routes[0].get_size() > m_map->get_size() / 2){
                           stop = 1;
                       }else{
                           Solution_Pair pair = Solution_Pair(pos_x, pos_y, vel, c_r);
                           routes[0].add_solution_pair(pair);

                           route_size++;
                       }
                   }
               }
           }while(stop != 1);

           a_initial_solution.set_routes(routes);
           a_initial_solution.set_iteration(i);
           a_initial_solution = evaluate(a_initial_solution, m_map);

           a_initial_solution.set_dna(n_n);
           pool_of_solutions.add_solution(a_initial_solution);
       }

       dnas.clear();

       auto end = std::chrono::system_clock::now();
       std::chrono::duration<double> diff = end-start;
       diff_count = diff.count();
   } while(diff_count < time_limit);

    string results_name = "results";
    string file_format = ".txt";

    string results_file_name = base_file_name + results_name + file_format;
    cout << "Results file name: " << results_file_name << "\n" << endl;

    string info_name = "info_";

    string info_file_name = base_file_name + info_name + file_format;
    cout << "Info file name: " << info_file_name << "\n" << endl;

    cout << "\nLAST GENERATION: " << i << endl;

    string completed_resp = completed ? "true" : "false";
    cout << "\nCOMPLETED: " << completed_resp << endl;

    cout << "Total time: " << diff_count << "\n" << endl;

    //START: Info file
    ofstream info_file;
    info_file.open (info_file_name);

    info_file << "\nPOINTS: " << QTY_POINTS[STOPS::DELIVER] + QTY_POINTS[STOPS::COLLECT] << "\n";
    info_file << "\nRECHARGE: " << QTY_POINTS[STOPS::CHARGE] << "\n";
    info_file << "\nPROHIBITED: " << QTY_POINTS[STOPS::PROHIBITED] << "\n";
    info_file << "\n------------------------------------------" << "\n";

    info_file << "\n";

    info_file << "\nSTOP CHRITERION: " << "time (" << time_limit << " s) - hard" << "\n";
    info_file << "\nPOOL MAX SIZE: " << "30" << "\n";
    info_file << "\n------------------------------------------" << "\n";

    info_file << "\nLAST GENERATION: " << i << "\n";
    info_file << "\n# OF SOLUTIONS: " << pool_of_solutions.get_size() << "\n";
    info_file << "\nCOMPLETED: " << completed_resp << "\n";
    info_file << "\nTOTAL TIME: " << diff_count << "\n" << "\n";
    info_file << "------------------------------------------" << "\n\n";
    //END: Info file

    int max_final_charge = INT_MIN, min_time = INT_MAX, min_consumption = INT_MAX;
    int total_final_charge = 0, total_time = 0, total_consumption = 0;

    double z1, z2, z3;

    ofstream myfile;
    myfile.open (results_file_name);


    //Create directory to store solutions
    string solutions_directory_name = base_file_name + "solutions";
    mkdir(solutions_directory_name.c_str(), 0777);

    string routes_directory_name = solutions_directory_name + "/routes";
    mkdir(routes_directory_name.c_str(), 0777);

    for (int k = 0; k < pool_of_solutions.get_size(); k++) {
        pool_of_solutions.get_solutions()[k].to_file(solutions_directory_name + "/solution_" + to_string(k));

        z1 =        pool_of_solutions.get_solutions()[k].get_z()[1];
        z2 = (-1) * pool_of_solutions.get_solutions()[k].get_z()[2];
        z3 = (-1) * pool_of_solutions.get_solutions()[k].get_z()[3];

        if(z1 > max_final_charge){
            max_final_charge = z1;
        }

        if(z2 < min_time){
            min_time = z2;
        }

        if(z3 < min_consumption){
            min_consumption = z3;
        }

        total_final_charge += z1;
        total_time += z2;
        total_consumption += z3;

        myfile << z1 << " " << -z2 << " " << -z3 << "\n";
    }

    myfile.close();

    double average_final_charge = total_final_charge / (double) pool_of_solutions.get_size();
    double average_time         = total_time         / (double) pool_of_solutions.get_size();
    double average_consumption  = total_consumption  / (double) pool_of_solutions.get_size();

    double dev_final_charge = 0;
    double dev_time         = 0;
    double dev_consumption  = 0;

    for (int k = 0; k < pool_of_solutions.get_size(); k++) {
        dev_final_charge += pow(  pool_of_solutions.get_solutions()[k].get_z()[1] - average_final_charge, 2);
        dev_time         += pow(- pool_of_solutions.get_solutions()[k].get_z()[2] - average_time, 2);
        dev_consumption  += pow(- pool_of_solutions.get_solutions()[k].get_z()[3] - average_consumption, 2);
    }

    dev_final_charge = sqrt(dev_final_charge);
    dev_time         = sqrt(dev_time);
    dev_consumption  = sqrt(dev_consumption);


    cout << ">> Average Final Charge: " << average_final_charge <<
            " >> Standard Deviation Final Charge: " << dev_final_charge <<
            " >> Best Final Charge: " << max_final_charge << endl;
    cout << ">> Average Time: " << average_time <<
            " >> Standard Deviation Time: " << dev_time <<
            " >> Best Time: " << min_time << endl;
    cout << ">> Average Consumption: " << average_consumption <<
            " >> Standard Deviation Consumption: " << dev_consumption <<
            " >> Best Consumption: " << min_consumption << endl;

    //START: Info file
    info_file << "\n------------------------------------------" << "\n";
    info_file << "\n";

    info_file << ">> Average Final Charge: " << average_final_charge <<
                 " >> Standard Deviation Final Charge: " << dev_final_charge <<
                 " >> Best Final Charge: " << max_final_charge << "\n";
    info_file << ">> Average Time: " << average_time <<
                 " >> Standard Deviation Time: " << dev_time <<
                 " >> Best Time: " << min_time << "\n";
    info_file << ">> Average Consumption: " << average_consumption <<
                 " >> Standard Deviation Consumption: " << dev_consumption <<
                 " >> Best Consumption: " << min_consumption << "\n";

    info_file.close();
    //END: Info file

    return aggregate_2(1, 3, base_file_name);
}


/* NEIGHBORHOODS */
bool Core::shift_1_1_furthest(Pool * pool, Solution * neighbor_sol) /*  */
{
    Solution * current_solution = new Solution;
    UAV_init_solution(current_solution);
    UAV_copy_solution(neighbor_sol, current_solution);

    UAV * copy_uav;

    Movement * current1,
             * current2,
             * temp1,
             * temp2;

    int x1, y1,
        x2, y2,
        type,
        pos1 = 0,
        pos2 = 0,
        start1 = 0, end1 = 0,
        start2 = 0, end2 = 0;

    bool dominates;

    for (int pos_route = 0; pos_route < current_solution->get_size(); pos_route++) {

        current1 = current_solution->uavs[pos_route]->route;

        if(current1 != NULL){
            current1 = current1->next;

            start1 = 1;
            end1   = start1;

            while(current1 != NULL){

                /* First client (start) */

                while(current1 != NULL){
                    type = m_map->get_stop_point(current1->x, current1->y).get_type();

                    if(type == STOPS::DELIVER || type == STOPS::COLLECT){
                        x1 = current1->x;
                        y1 = current1->y;

                        current1 = current1->next;
                        temp1 = current1;

                        pos1 = end1;
                        end1++;

                        break;
                    }

                    current1 = current1->next;
                    end1++;
                }

                while(current1 != NULL){
                    type = m_map->get_stop_point(current1->x, current1->y).get_type();

                    if(type == STOPS::DELIVER || type == STOPS::COLLECT){
                        break;
                    }

                    current1 = current1->next;
                    end1++;
                }

                /* First client (end) */

                int current_pos;

                for (int i = 0; i < client_points.size(); i++) {

                    if(client_points[i].get_x() == x1 && client_points[i].get_y() == y1){
                        current_pos = i;
                        break;
                    }

                }

                for (int near_pos = 0; near_pos < further_neighbors[current_pos].size(); near_pos++) { // Run throught the vector of random chosen clients

                    x2 = client_points[further_neighbors[current_pos][near_pos]].get_x();
                    y2 = client_points[further_neighbors[current_pos][near_pos]].get_y();

                    /* Second client (start) */
                    start2 = pos1 + 1;
                    end2   = start2;

                    current2 = temp1;

                    while(current2 != NULL){
                        while(current2 != NULL){

                            if(current2->x == x2 && current2->y == y2){
                                temp2    = current2->next;
                                current2 = current2->prev;

                                pos2 = end2;
                                start2--;
                                end2++;

                                break;
                            }

                            current2 = current2->next;
                            end2++;
                            start2++;
                        }

                        if(current2 == NULL){
                            break;
                        }

                        // Backward
                        while(current2 != NULL){
                            type = m_map->get_stop_point(current2->x, current2->y).get_type();

                            if(type == STOPS::DELIVER || type == STOPS::COLLECT){
                                start2++;
                                break;
                            }

                            current2 = current2->prev;
                            start2--;
                        }

                        current2 = temp2;

                        // Forward
                        while(current2 != NULL){
                            type = m_map->get_stop_point(current2->x, current2->y).get_type();

                            if(type == STOPS::DELIVER || type == STOPS::COLLECT){
                                break;
                            }

                            current2 = current2->next;
                            end2++;
                        }

                        /* Second client (end) */

                        copy_uav = new UAV;
                        UAV_init_uav(copy_uav);
                        UAV_copy_uav(current_solution->uavs[pos_route], copy_uav);

                        // x------1------x------2----------x

                        UAV_remove_subpath(copy_uav, start2, end2 - 1); // x------1------x|x

                        UAV_insert_movement(copy_uav, x1, y1, start2); // insert the first point at the second position : x------1------x1x

                        if(current2 != NULL){ // If not like : x------1------x1
                             sub_path(copy_uav, start2, start2+1); // x------1------x1--------x
                        }

                        if(start2 != pos1 + 1){ // If not like : x------11--------x
                            sub_path(copy_uav, start2 - 1, start2); // x------1------x------1--------x
                            UAV_remove_subpath(copy_uav, pos1 + 1, end1 - 1); // x------1x------1--------x
                        }

                        UAV_remove_subpath(copy_uav, start1, pos1); // x|x------1--------x

                        UAV_insert_movement(copy_uav, x2, y2, start1); // insert the second point at the first position : x2x------1----------x

                        sub_path(copy_uav, start1, start1+1); // x2----------x------1----------x
                        sub_path(copy_uav, start1-1, start1); // x---2----------x------1----------x

                        UAV_delete_uav(neighbor_sol->uavs[pos_route]);
                        neighbor_sol->uavs[pos_route] = new UAV;
                        UAV_init_uav(neighbor_sol->uavs[pos_route]);

                        UAV_copy_uav(copy_uav, neighbor_sol->uavs[pos_route]);
                        UAV_delete_uav(copy_uav);

                        evaluate(neighbor_sol, m_map);

                        if(!current_solution->dominates(neighbor_sol)){
                            UAV_add_solution(pool, neighbor_sol);

                            // If new solution dominates current one
                            dominates = neighbor_sol->dominates(current_solution);

                            if(dominates){
                                UAV_delete_solution(current_solution);
                                return true;
                            }
                        }

                        start2   = pos2 + 1;
                        end2     = start2;
                        current2 = temp2;

                    } // END : while - current2 not NULL

                }

                start1   = pos1 + 1;
                end1     = start1;
                current1 = temp1;

            } // END : while - current1 not NULL
        } // END : if - current1 not NULL
    }

    UAV_delete_solution(current_solution);
    return false;
}

bool Core::shift_1_1_nearest(Pool * pool, Solution * neighbor_sol) /*  */
{
    Solution * current_solution = new Solution;
    UAV_init_solution(current_solution);
    UAV_copy_solution(neighbor_sol, current_solution);

    UAV * copy_uav;

    Movement * current1,
             * current2,
             * temp1,
             * temp2;

    int x1, y1,
        x2, y2,
        type,
        pos1 = 0,
        pos2 = 0,
        start1 = 0, end1 = 0,
        start2 = 0, end2 = 0;

    bool dominates;

    for (int pos_route = 0; pos_route < current_solution->get_size(); pos_route++) {

        current1 = current_solution->uavs[pos_route]->route;

        if(current1 != NULL){
            current1 = current1->next;

            start1 = 1;
            end1   = start1;

            while(current1 != NULL){

                /* First client (start) */

                while(current1 != NULL){
                    type = m_map->get_stop_point(current1->x, current1->y).get_type();

                    if(type == STOPS::DELIVER || type == STOPS::COLLECT){
                        x1 = current1->x;
                        y1 = current1->y;

                        current1 = current1->next;
                        temp1 = current1;

                        pos1 = end1;
                        end1++;

                        break;
                    }

                    current1 = current1->next;
                    end1++;
                }

                while(current1 != NULL){
                    type = m_map->get_stop_point(current1->x, current1->y).get_type();

                    if(type == STOPS::DELIVER || type == STOPS::COLLECT){
                        break;
                    }

                    current1 = current1->next;
                    end1++;
                }

                /* First client (end) */

                int current_pos;

                for (int i = 0; i < client_points.size(); i++) {

                    if(client_points[i].get_x() == x1 && client_points[i].get_y() == y1){
                        current_pos = i;
                        break;
                    }

                }

                for (int near_pos = 0; near_pos < neighbors[current_pos].size(); near_pos++) { // Run throught the vector of random chosen clients

                    x2 = client_points[neighbors[current_pos][near_pos]].get_x();
                    y2 = client_points[neighbors[current_pos][near_pos]].get_y();

                    /* Second client (start) */
                    start2 = pos1 + 1;
                    end2   = start2;

                    current2 = temp1;

                    while(current2 != NULL){
                        while(current2 != NULL){

                            if(current2->x == x2 && current2->y == y2){
                                temp2    = current2->next;
                                current2 = current2->prev;

                                pos2 = end2;
                                start2--;
                                end2++;

                                break;
                            }

                            current2 = current2->next;
                            end2++;
                            start2++;
                        }

                        if(current2 == NULL){
                            break;
                        }

                        // Backward
                        while(current2 != NULL){
                            type = m_map->get_stop_point(current2->x, current2->y).get_type();

                            if(type == STOPS::DELIVER || type == STOPS::COLLECT){
                                start2++;
                                break;
                            }

                            current2 = current2->prev;
                            start2--;
                        }

                        current2 = temp2;

                        // Forward
                        while(current2 != NULL){
                            type = m_map->get_stop_point(current2->x, current2->y).get_type();

                            if(type == STOPS::DELIVER || type == STOPS::COLLECT){
                                break;
                            }

                            current2 = current2->next;
                            end2++;
                        }

                        /* Second client (end) */

                        copy_uav = new UAV;
                        UAV_init_uav(copy_uav);
                        UAV_copy_uav(current_solution->uavs[pos_route], copy_uav);

                        // x------1------x------2----------x

                        UAV_remove_subpath(copy_uav, start2, end2 - 1); // x------1------x|x

                        UAV_insert_movement(copy_uav, x1, y1, start2); // insert the first point at the second position : x------1------x1x

                        if(current2 != NULL){ // If not like : x------1------x1
                             sub_path(copy_uav, start2, start2+1); // x------1------x1--------x
                        }

                        if(start2 != pos1 + 1){ // If not like : x------11--------x
                            sub_path(copy_uav, start2 - 1, start2); // x------1------x------1--------x
                            UAV_remove_subpath(copy_uav, pos1 + 1, end1 - 1); // x------1x------1--------x
                        }

                        UAV_remove_subpath(copy_uav, start1, pos1); // x|x------1--------x

                        UAV_insert_movement(copy_uav, x2, y2, start1); // insert the second point at the first position : x2x------1----------x

                        sub_path(copy_uav, start1, start1+1); // x2----------x------1----------x
                        sub_path(copy_uav, start1-1, start1); // x---2----------x------1----------x

                        UAV_delete_uav(neighbor_sol->uavs[pos_route]);
                        neighbor_sol->uavs[pos_route] = new UAV;
                        UAV_init_uav(neighbor_sol->uavs[pos_route]);

                        UAV_copy_uav(copy_uav, neighbor_sol->uavs[pos_route]);
                        UAV_delete_uav(copy_uav);

                        evaluate(neighbor_sol, m_map);

                        if(!current_solution->dominates(neighbor_sol)){
                            UAV_add_solution(pool, neighbor_sol);

                            // If new solution dominates current one
                            dominates = neighbor_sol->dominates(current_solution);

                            if(dominates){
                                UAV_delete_solution(current_solution);
                                return true;
                            }
                        }

                        start2   = pos2 + 1;
                        end2     = start2;
                        current2 = temp2;

                    } // END : while - current2 not NULL

                }

                start1   = pos1 + 1;
                end1     = start1;
                current1 = temp1;

            } // END : while - current1 not NULL
        } // END : if - current1 not NULL
    }

    UAV_delete_solution(current_solution);
    return false;
}

bool Core::shift_1_1_random(Pool * pool, Solution * neighbor_sol) /*  */
{
    Solution * current_solution = new Solution;
    UAV_init_solution(current_solution);
    UAV_copy_solution(neighbor_sol, current_solution);

    UAV * copy_uav;

    Movement * current1,
             * current2,
             * temp1,
             * temp2;

    int x1, y1,
        x2, y2,
        type,
        pos1 = 0,
        pos2 = 0,
        start1 = 0, end1 = 0,
        start2 = 0, end2 = 0;

    bool dominates;

    vector<Solution_Pair> clients_temp;

    for (int pos_route = 0; pos_route < current_solution->get_size(); pos_route++) {

        current1 = current_solution->uavs[pos_route]->route;

        if(current1 != NULL){
            current1 = current1->next;

            start1 = 1;
            end1   = start1;

            while(current1 != NULL){

                /* First client (start) */

                while(current1 != NULL){
                    type = m_map->get_stop_point(current1->x, current1->y).get_type();

                    if(type == STOPS::DELIVER || type == STOPS::COLLECT){
                        x1 = current1->x;
                        y1 = current1->y;

                        current1 = current1->next;
                        temp1 = current1;

                        pos1 = end1;
                        end1++;

                        break;
                    }

                    current1 = current1->next;
                    end1++;
                }

                while(current1 != NULL){
                    type = m_map->get_stop_point(current1->x, current1->y).get_type();

                    if(type == STOPS::DELIVER || type == STOPS::COLLECT){
                        break;
                    }

                    current1 = current1->next;
                    end1++;
                }

                /* First client (end) */

                clients_temp = client_points; // Copy vector of inspection points (clients)
                vector<int> random_neighbors;
                int random_pos;
                for (int n_count = 0; n_count < neighbors_qty; n_count++) { // Get 'neighbors_qty' clients to swap with the current one
                    do {

                       random_pos = rand()%(clients_temp.size());

                    } while(clients_temp[random_pos].get_x() == x1
                            &&
                            clients_temp[random_pos].get_y() == y1); // While client is different from the current one

                    clients_temp.erase(clients_temp.begin() + random_pos); // Delete the client chosen from the copy vector
                    random_neighbors.push_back(random_pos);
                }

                for (random_pos = 0; random_pos < random_neighbors.size(); random_pos++) { // Run throught the vector of random chosen clients                    

                    x2 = client_points[random_neighbors[random_pos]].get_x();
                    y2 = client_points[random_neighbors[random_pos]].get_y();

                    /* Second client (start) */
                    start2 = pos1 + 1;
                    end2   = start2;

                    current2 = temp1;

                    while(current2 != NULL){
                        while(current2 != NULL){

                            if(current2->x == x2 && current2->y == y2){
                                temp2    = current2->next;
                                current2 = current2->prev;

                                pos2 = end2;
                                start2--;
                                end2++;

                                break;
                            }

                            current2 = current2->next;
                            end2++;
                            start2++;
                        }

                        if(current2 == NULL){
                            break;
                        }

                        // Backward
                        while(current2 != NULL){
                            type = m_map->get_stop_point(current2->x, current2->y).get_type();

                            if(type == STOPS::DELIVER || type == STOPS::COLLECT){
                                start2++;
                                break;
                            }

                            current2 = current2->prev;
                            start2--;
                        }

                        current2 = temp2;

                        // Forward
                        while(current2 != NULL){
                            type = m_map->get_stop_point(current2->x, current2->y).get_type();

                            if(type == STOPS::DELIVER || type == STOPS::COLLECT){
                                break;
                            }

                            current2 = current2->next;
                            end2++;
                        }

                        /* Second client (end) */

                        copy_uav = new UAV;
                        UAV_init_uav(copy_uav);
                        UAV_copy_uav(current_solution->uavs[pos_route], copy_uav);

                        // x------1------x------2----------x

                        UAV_remove_subpath(copy_uav, start2, end2 - 1); // x------1------x|x

                        UAV_insert_movement(copy_uav, x1, y1, start2); // insert the first point at the second position : x------1------x1x

                        if(current2 != NULL){ // If not like : x------1------x1
                             sub_path(copy_uav, start2, start2+1); // x------1------x1--------x
                        }

                        if(start2 != pos1 + 1){ // If not like : x------11--------x
                            sub_path(copy_uav, start2 - 1, start2); // x------1------x------1--------x
                            UAV_remove_subpath(copy_uav, pos1 + 1, end1 - 1); // x------1x------1--------x
                        }

                        UAV_remove_subpath(copy_uav, start1, pos1); // x|x------1--------x

                        UAV_insert_movement(copy_uav, x2, y2, start1); // insert the second point at the first position : x2x------1----------x

                        sub_path(copy_uav, start1, start1+1); // x2----------x------1----------x
                        sub_path(copy_uav, start1-1, start1); // x---2----------x------1----------x

                        UAV_delete_uav(neighbor_sol->uavs[pos_route]);
                        neighbor_sol->uavs[pos_route] = new UAV;
                        UAV_init_uav(neighbor_sol->uavs[pos_route]);

                        UAV_copy_uav(copy_uav, neighbor_sol->uavs[pos_route]);
                        UAV_delete_uav(copy_uav);

                        evaluate(neighbor_sol, m_map);

                        if(!current_solution->dominates(neighbor_sol)){
                            UAV_add_solution(pool, neighbor_sol);

                            // If new solution dominates current one
                            dominates = neighbor_sol->dominates(current_solution);

                            if(dominates){
                                UAV_delete_solution(current_solution);
                                return true;
                            }
                        }

                        start2   = pos2 + 1;
                        end2     = start2;
                        current2 = temp2;

                    } // END : while - current2 not NULL

                }

                start1   = pos1 + 1;
                end1     = start1;
                current1 = temp1;

            } // END : while - current1 not NULL
        } // END : if - current1 not NULL
    }

    UAV_delete_solution(current_solution);
    return false;
}

bool Core::shift_1_1_complete(Pool * pool, Solution * neighbor_sol) /* swap between clients at same route */
{    
    Solution * current_solution = new Solution;
    UAV_init_solution(current_solution);
    UAV_copy_solution(neighbor_sol, current_solution);

    UAV * copy_uav;

    Movement * current1,
             * current2,
             * temp1,
             * temp2;

    int x1, y1,
        x2, y2,
        type,
        pos1 = 0,
        pos2 = 0,
        start1 = 0, end1 = 0,
        start2 = 0, end2 = 0;

    bool dominates;

    for (int pos_route = 0; pos_route < current_solution->get_size(); pos_route++) {

        current1 = current_solution->uavs[pos_route]->route;

        if(current1 != NULL){
            current1 = current1->next;

            start1 = 1;
            end1   = start1;

            while(current1 != NULL){

                /* First client (start) */

                while(current1 != NULL){
                    type = m_map->get_stop_point(current1->x, current1->y).get_type();

                    if(type == STOPS::DELIVER || type == STOPS::COLLECT){
                        x1 = current1->x;
                        y1 = current1->y;

                        current1 = current1->next;
                        temp1 = current1;

                        pos1 = end1;
                        end1++;

                        break;
                    }

                    current1 = current1->next;
                    end1++;
                }

                while(current1 != NULL){
                    type = m_map->get_stop_point(current1->x, current1->y).get_type();

                    if(type == STOPS::DELIVER || type == STOPS::COLLECT){                        
                        break;
                    }

                    current1 = current1->next;
                    end1++;
                }

                /* First client (end) */

                /* Second client (start) */
                start2 = pos1 + 1;
                end2   = start2;

                current2 = temp1;

                while(current2 != NULL){
                    while(current2 != NULL){
                        type = m_map->get_stop_point(current2->x, current2->y).get_type();

                        if(type == STOPS::DELIVER || type == STOPS::COLLECT){
                            x2 = current2->x;
                            y2 = current2->y;

                            current2 = current2->next;
                            temp2 = current2;

                            pos2 = end2;
                            end2++;

                            break;
                        }

                        current2 = current2->next;
                        end2++;
                    }

                    while(current2 != NULL){
                        type = m_map->get_stop_point(current2->x, current2->y).get_type();

                        if(type == STOPS::DELIVER || type == STOPS::COLLECT){                            
                            break;
                        }

                        current2 = current2->next;
                        end2++;
                    }

                    /* Second client (end) */

                    copy_uav = new UAV;
                    UAV_init_uav(copy_uav);
                    UAV_copy_uav(current_solution->uavs[pos_route], copy_uav);

                    // x------1------x------2----------x

                    UAV_remove_subpath(copy_uav, start2, end2 - 1); // x------1------x|x

                    UAV_insert_movement(copy_uav, x1, y1, start2); // insert the first point at the second position : x------1------x1x

                    if(current2 != NULL){ // If not like : x------1------x1
                         sub_path(copy_uav, start2, start2+1); // x------1------x1--------x
                    }

                    if(start2 != pos1 + 1){ // If not like : x------11--------x
                        sub_path(copy_uav, start2 - 1, start2); // x------1------x------1--------x
                        UAV_remove_subpath(copy_uav, pos1 + 1, end1 - 1); // x------1x------1--------x
                    }

                    UAV_remove_subpath(copy_uav, start1, pos1); // x|x------1--------x

                    UAV_insert_movement(copy_uav, x2, y2, start1); // insert the second point at the first position : x2x------1----------x

                    sub_path(copy_uav, start1, start1+1); // x2----------x------1----------x
                    sub_path(copy_uav, start1-1, start1); // x---2----------x------1----------x

                    UAV_delete_uav(neighbor_sol->uavs[pos_route]);
                    neighbor_sol->uavs[pos_route] = new UAV;
                    UAV_init_uav(neighbor_sol->uavs[pos_route]);

                    UAV_copy_uav(copy_uav, neighbor_sol->uavs[pos_route]);
                    UAV_delete_uav(copy_uav);

                    evaluate(neighbor_sol, m_map);

                    if(!current_solution->dominates(neighbor_sol)){
                        UAV_add_solution(pool, neighbor_sol);

                        // If new solution dominates current one
                        dominates = neighbor_sol->dominates(current_solution);

                        if(dominates){
                            UAV_delete_solution(current_solution);
                            return true;
                        }
                    }

                    start2   = pos2 + 1;
                    end2     = start2;
                    current2 = temp2;

                } // END : while - current2 not NULL

                start1   = pos1 + 1;
                end1     = start1;
                current1 = temp1;

            } // END : while - current1 not NULL
        } // END : if - current1 not NULL
    }

    UAV_delete_solution(current_solution);
    return false;
}

bool Core::swap_1_1(Pool * pool, Solution * neighbor_sol) /* swap between clients at different routes */
{
    Solution * current_solution = new Solution;
    UAV_init_solution(current_solution);
    UAV_copy_solution(neighbor_sol, current_solution);

    UAV * copy_uav1,
        * copy_uav2;

    Movement * current1,
             * current2,
             * temp1,
             * temp2;

    int x1, y1,
        x2, y2,
        type,
        pos1 = 0,
        pos2 = 0,
        start1 = 0, end1 = 0,
        start2 = 0, end2 = 0;

    bool dominates;

    for (int pos_route = 0; pos_route < current_solution->get_size(); pos_route++) {

        current1 = current_solution->uavs[pos_route]->route;

        if(current1 != NULL){
            current1 = current1->next;

            start1 = 1;
            end1   = start1;

            while(current1 != NULL){

                /* First client (start) */

                while(current1 != NULL){
                    type = m_map->get_stop_point(current1->x, current1->y).get_type();

                    if(type == STOPS::DELIVER || type == STOPS::COLLECT){
                        x1 = current1->x;
                        y1 = current1->y;

                        current1 = current1->next;
                        temp1 = current1;

                        pos1 = end1;
                        end1++;

                        break;
                    }

                    current1 = current1->next;
                    end1++;
                }

                while(current1 != NULL){
                    type = m_map->get_stop_point(current1->x, current1->y).get_type();

                    if(type == STOPS::DELIVER || type == STOPS::COLLECT){
                        break;
                    }

                    current1 = current1->next;
                    end1++;
                }

                /* First client (end) */

                /* Second client (start) */
                for (int pos_route2 = pos_route + 1; pos_route2 < current_solution->get_size(); pos_route2++) {
                    current2 = current_solution->uavs[pos_route2]->route;

                    if(current2 != NULL){
                        current2 = current2->next;

                        start2 = 1;
                        end2   = start2;

                        while(current2 != NULL){
                            while(current2 != NULL){
                                type = m_map->get_stop_point(current2->x, current2->y).get_type();

                                if(type == STOPS::DELIVER || type == STOPS::COLLECT){
                                    x2 = current2->x;
                                    y2 = current2->y;

                                    current2 = current2->next;
                                    temp2 = current2;

                                    pos2 = end2;
                                    end2++;

                                    break;
                                }

                                current2 = current2->next;
                                end2++;
                            }

                            while(current2 != NULL){
                                type = m_map->get_stop_point(current2->x, current2->y).get_type();

                                if(type == STOPS::DELIVER || type == STOPS::COLLECT){
                                    break;
                                }

                                current2 = current2->next;
                                end2++;
                            }

                            /* Second client (end) */

                            // First route
                            copy_uav1 = new UAV;
                            UAV_init_uav(copy_uav1);
                            UAV_copy_uav(current_solution->uavs[pos_route], copy_uav1);

                            // Second route
                            copy_uav2 = new UAV;
                            UAV_init_uav(copy_uav2);
                            UAV_copy_uav(current_solution->uavs[pos_route2], copy_uav2);

                            // x------1------x AND x------2----------x

                            UAV_remove_subpath(copy_uav2, start2, end2 - 1); // x------1------x AND x|x

                            UAV_insert_movement(copy_uav2, x1, y1, start2); // insert the first point at the second position : x------1------x AND x1x

                            if(current2 != NULL){ // If not like : x------1------x AND x1
                                sub_path(copy_uav2, start2, start2 + 1); // x------1------x AND x1--------x
                            }

                            sub_path(copy_uav2, start2 - 1, start2); // x------1------x AND x----1--------x

                            UAV_remove_subpath(copy_uav1, start1, end1 - 1); // x|x AND x----1--------x

                            UAV_insert_movement(copy_uav1, x2, y2, start1); // insert the second point at the first position : x2x AND x----1--------x

                            if(current1 != NULL){ // If not like : x2 AND x----1--------x
                                sub_path(copy_uav1, start1, start1+1); // x2---x AND x----1--------x
                            }

                            sub_path(copy_uav1, start1-1, start1); // x------2---x AND x----1--------x

                            // Route 1
                            UAV_delete_uav(neighbor_sol->uavs[pos_route]);
                            neighbor_sol->uavs[pos_route] = new UAV;
                            UAV_init_uav(neighbor_sol->uavs[pos_route]);
                            UAV_copy_uav(copy_uav1, neighbor_sol->uavs[pos_route]);
                            UAV_delete_uav(copy_uav1);

                            // Route 2
                            UAV_delete_uav(neighbor_sol->uavs[pos_route2]);
                            neighbor_sol->uavs[pos_route2] = new UAV;
                            UAV_init_uav(neighbor_sol->uavs[pos_route2]);
                            UAV_copy_uav(copy_uav2, neighbor_sol->uavs[pos_route2]);
                            UAV_delete_uav(copy_uav2);

                            evaluate(neighbor_sol, m_map);

                            if(!current_solution->dominates(neighbor_sol)){
                                UAV_add_solution(pool, neighbor_sol);

                                // If new solution dominates current one
                                dominates = neighbor_sol->dominates(current_solution);

                                if(dominates){
                                    UAV_delete_solution(current_solution);
                                    return true;
                                }
                            }

                            start2   = pos2 + 1;
                            end2     = start2;
                            current2 = temp2;

                        } // END : while - current2 not NULL
                    }
                }

                start1   = pos1 + 1;
                end1     = start1;
                current1 = temp1;

            } // END : while - current1 not NULL
        } // END : if - current1 not NULL
    }

    UAV_delete_solution(current_solution);
    return false;
}

bool Core::swap_1_0(Pool * pool, Solution * neighbor_sol) /* remove client from a route and insert into other route */
{
    Solution * current_solution = new Solution;
    UAV_init_solution(current_solution);
    UAV_copy_solution(neighbor_sol, current_solution);

    UAV * copy_uav1,
        * copy_uav2;

    Movement * current1,
             * current2,
             * temp1,
             * temp2;

    int x1, y1,
        x2, y2,
        type,
        pos1 = 0,
        pos2 = 0,
        start1 = 0, end1 = 0,
        start2 = 0, end2 = 0;

    bool dominates;

    for (int pos_route = 0; pos_route < current_solution->get_size(); pos_route++) {

        current1 = current_solution->uavs[pos_route]->route;

        if(current1 != NULL){
            current1 = current1->next;

            start1 = 1;
            end1   = start1;

            while(current1 != NULL){

                /* First client (start) */

                while(current1 != NULL){
                    type = m_map->get_stop_point(current1->x, current1->y).get_type();

                    if(type == STOPS::DELIVER || type == STOPS::COLLECT){
                        x1 = current1->x;
                        y1 = current1->y;

                        current1 = current1->next;
                        temp1 = current1;

                        pos1 = end1;
                        end1++;

                        break;
                    }

                    current1 = current1->next;
                    end1++;
                }

                while(current1 != NULL){
                    type = m_map->get_stop_point(current1->x, current1->y).get_type();

                    if(type == STOPS::DELIVER || type == STOPS::COLLECT){
                        break;
                    }

                    current1 = current1->next;
                    end1++;
                }

                /* First client (end) */

                /* Second client (start) */
                for (int pos_route2 = 0; pos_route2 < current_solution->get_size(); pos_route2++) {

                    if(pos_route != pos_route2){

                        current2 = current_solution->uavs[pos_route2]->route;

                        if(current2 != NULL){
                            current2 = current2->next;

                            start2 = 1;
                            end2   = start2;

                            while(current2 != NULL){
                                while(current2 != NULL){
                                    type = m_map->get_stop_point(current2->x, current2->y).get_type();

                                    if(type == STOPS::DELIVER || type == STOPS::COLLECT){
                                        x2 = current2->x;
                                        y2 = current2->y;

                                        current2 = current2->next;
                                        temp2 = current2;

                                        pos2 = end2;
                                        end2++;

                                        break;
                                    }

                                    current2 = current2->next;
                                    end2++;
                                }

                                while(current2 != NULL){
                                    type = m_map->get_stop_point(current2->x, current2->y).get_type();

                                    if(type == STOPS::DELIVER || type == STOPS::COLLECT){
                                        break;
                                    }

                                    current2 = current2->next;
                                    end2++;
                                }

                                /* Second client (end) */

                                // First route
                                copy_uav1 = new UAV;
                                UAV_init_uav(copy_uav1);
                                UAV_copy_uav(current_solution->uavs[pos_route], copy_uav1);

                                // Second route
                                copy_uav2 = new UAV;
                                UAV_init_uav(copy_uav2);
                                UAV_copy_uav(current_solution->uavs[pos_route2], copy_uav2);

                                // x------1------x AND x------2----------x

                                UAV_remove_subpath(copy_uav2, start2, pos2 - 1); // x------1------x AND x2----------x

                                UAV_insert_movement(copy_uav2, x1, y1, start2); // insert the first point at the second position : x------1------x AND x12----------x

                                sub_path(copy_uav2, start2, start2 + 1); // x------1------x AND x1---2----------x

                                sub_path(copy_uav2, start2 - 1, start2); // x------1------x AND x------1---2----------x

                                UAV_remove_subpath(copy_uav1, start1, end1 - 1); // x|x AND x------1---2----------x

                                if(current1 != NULL){ // If not like : x| AND x------1---2----------x
                                    sub_path(copy_uav1, start1-1, start1); // x---x AND x------1---2----------x
                                }

                                // Route 1
                                UAV_delete_uav(neighbor_sol->uavs[pos_route]);
                                neighbor_sol->uavs[pos_route] = new UAV;
                                UAV_init_uav(neighbor_sol->uavs[pos_route]);
                                UAV_copy_uav(copy_uav1, neighbor_sol->uavs[pos_route]);
                                UAV_delete_uav(copy_uav1);

                                // Route 2
                                UAV_delete_uav(neighbor_sol->uavs[pos_route2]);
                                neighbor_sol->uavs[pos_route2] = new UAV;
                                UAV_init_uav(neighbor_sol->uavs[pos_route2]);
                                UAV_copy_uav(copy_uav2, neighbor_sol->uavs[pos_route2]);
                                UAV_delete_uav(copy_uav2);

                                evaluate(neighbor_sol, m_map);

                                if(!current_solution->dominates(neighbor_sol)){
                                    UAV_add_solution(pool, neighbor_sol);

                                    // If new solution dominates current one
                                    dominates = neighbor_sol->dominates(current_solution);

                                    if(dominates){
                                        UAV_delete_solution(current_solution);
                                        return true;
                                    }
                                }

                                start2   = pos2 + 1;
                                end2     = start2;
                                current2 = temp2;

                            } // END : while - current2 not NULL
                        }
                    }
                }

                start1   = pos1 + 1;
                end1     = start1;
                current1 = temp1;

            } // END : while - current1 not NULL
        } // END : if - current1 not NULL
    }

    UAV_delete_solution(current_solution);
    return false;
}

bool Core::remove_repeated_STRUCT(Pool * pool, Solution * neighbor_sol) /* remove client from route that were already visited */
{
    Solution * current_solution = new Solution;
    UAV_init_solution(current_solution);
    UAV_copy_solution(neighbor_sol, current_solution);

    UAV * copy_uav;

    Movement * current1,
             * current2,
             * temp1,
             * temp2;

    int x1, y1,
        type,
        pos1 = 0,
        pos2 = 0,
        start1 = 0, end1 = 0,
        start2 = 0, end2 = 0;

    bool dominates;

    for (int pos_route = 0; pos_route < current_solution->get_size(); pos_route++) {

        current1 = current_solution->uavs[pos_route]->route;

        if(current1 != NULL){
            current1 = current1->next;

            start1 = 1;
            end1   = start1;

            while(current1 != NULL){

                /* First client (start) */

                while(current1 != NULL){
                    type = m_map->get_stop_point(current1->x, current1->y).get_type();

                    if(type == STOPS::DELIVER || type == STOPS::COLLECT){
                        x1 = current1->x;
                        y1 = current1->y;

                        current1 = current1->next;
                        temp1 = current1;

                        pos1 = end1;
                        end1++;

                        break;
                    }

                    current1 = current1->next;
                    end1++;
                }

                while(current1 != NULL){
                    type = m_map->get_stop_point(current1->x, current1->y).get_type();

                    if(type == STOPS::DELIVER || type == STOPS::COLLECT){
                        break;
                    }

                    current1 = current1->next;
                    end1++;
                }

                /* First client (end) */

                /* Second client (start) */
                start2 = pos1 + 1;
                end2   = start2;

                current2 = temp1;

                while(current2 != NULL){
                    while(current2 != NULL){

                        if(current2->x == x1 && current2->y == y1){
                            temp2    = current2->next;
                            current2 = current2->prev;

                            pos2 = end2;
                            start2--;
                            end2++;

                            break;
                        }

                        current2 = current2->next;
                        end2++;
                        start2++;
                    }

                    if(current2 == NULL){
                        break;
                    }

                    // Backward
                    while(current2 != NULL){
                        type = m_map->get_stop_point(current2->x, current2->y).get_type();

                        if(type == STOPS::DELIVER || type == STOPS::COLLECT){
                            start2++;
                            break;
                        }

                        current2 = current2->prev;
                        start2--;
                    }

                    current2 = temp2;

                    // Forward
                    while(current2 != NULL){
                        type = m_map->get_stop_point(current2->x, current2->y).get_type();

                        if(type == STOPS::DELIVER || type == STOPS::COLLECT){
                            break;
                        }

                        current2 = current2->next;
                        end2++;
                    }

                    /* Second client (end) */

                    copy_uav = new UAV;
                    UAV_init_uav(copy_uav);
                    UAV_copy_uav(current_solution->uavs[pos_route], copy_uav);

                    // x------1------x------1----------x

                    UAV_remove_subpath(copy_uav, start2, end2 - 1); // x------1------x|x

                    if(current2 != NULL){ // If not like : x------1------x
                        sub_path(copy_uav, start2 - 1, start2); // x------1------x--------------x
                    }

                    UAV_delete_uav(neighbor_sol->uavs[pos_route]);
                    neighbor_sol->uavs[pos_route] = new UAV;
                    UAV_init_uav(neighbor_sol->uavs[pos_route]);

                    UAV_copy_uav(copy_uav, neighbor_sol->uavs[pos_route]);
                    UAV_delete_uav(copy_uav);

                    evaluate(neighbor_sol, m_map);

                    if(!current_solution->dominates(neighbor_sol)){
                        UAV_add_solution(pool, neighbor_sol);

                        // If new solution dominates current one
                        dominates = neighbor_sol->dominates(current_solution);

                        if(dominates){
                            UAV_delete_solution(current_solution);
                            return true;
                        }
                    }

                    start2   = pos2 + 1;
                    end2     = start2;
                    current2 = temp2;

                } // END : while - current2 not NULL

                start1   = pos1 + 1;
                end1     = start1;
                current1 = temp1;

            } // END : while - current1 not NULL
        } // END : if - current1 not NULL
    }

    UAV_delete_solution(current_solution);
    return false;
}

bool Core::change_vel_random(Pool * pool, Solution * neighbor_sol, int value) /* get a random sub path from the route and change the velocity from of each stretch */
{
    Solution * current_solution = new Solution;
    UAV_init_solution(current_solution);
    UAV_copy_solution(neighbor_sol, current_solution);

    bool dominates;

    int pos_route = (rand() % neighbor_sol->get_size());
    int start     = (rand() % (neighbor_sol->uavs[pos_route]->get_size() - 2)) + 1;
    int end       = (rand() % (neighbor_sol->uavs[pos_route]->get_size() - start - 1)) + start + 1;

    Movement * current = neighbor_sol->uavs[pos_route]->get_movement(start);

    for (int current_pos = start; current_pos < end; current_pos++) {
        current->change_vel(value);
        current = current->next;
    }    

    if(!current_solution->dominates(neighbor_sol)){
        UAV_add_solution(pool, neighbor_sol);

        // If new solution dominates current one
        dominates = neighbor_sol->dominates(current_solution);

        if(dominates){
            UAV_delete_solution(current_solution);
            return true;
        }
    }

    UAV_delete_solution(current_solution);
    return false;
}

bool Core::change_vel_between_recharge(Pool * pool, Solution * neighbor_sol, int value) /* change speed in all stretches between recharge points */
{
    Solution * current_solution = new Solution;
    UAV_init_solution(current_solution);
    UAV_copy_solution(neighbor_sol, current_solution);

    UAV * copy_uav;

    Movement * current,
             * temp;

    int type,
        pos1   = 0,
        start = 0, end = 0;

    bool dominates;

    for (int pos_route = 0; pos_route < current_solution->get_size(); pos_route++) {

        current = current_solution->uavs[pos_route]->route;

        start = 0;
        end   = start;

        while(current != NULL){

            /* Subpath between recharge points (start) */
            while(current != NULL){
                type = m_map->get_stop_point(current->x, current->y).get_type();

                if(type == STOPS::CHARGE){
                    current = current->next;
                    temp = current;

                    pos1 = end;
                    end++;

                    break;
                }

                current = current->next;
                end++;
            }

            if(current == NULL){
                temp = current;
            }

            /* Subpath between recharge points (end) */

            copy_uav = new UAV;
            UAV_init_uav(copy_uav);
            UAV_copy_uav(current_solution->uavs[pos_route], copy_uav);

            current = copy_uav->get_movement(start);

            for (int current_pos = start; current_pos < end; current_pos++) {
                current->change_vel(value);
                current = current->next;
            }

            UAV_delete_uav(neighbor_sol->uavs[pos_route]);
            neighbor_sol->uavs[pos_route] = new UAV;
            UAV_init_uav(neighbor_sol->uavs[pos_route]);

            UAV_copy_uav(copy_uav, neighbor_sol->uavs[pos_route]);
            UAV_delete_uav(copy_uav);

            evaluate(neighbor_sol, m_map);

            if(!current_solution->dominates(neighbor_sol)){
                UAV_add_solution(pool, neighbor_sol);

                // If new solution dominates current one
                dominates = neighbor_sol->dominates(current_solution);

                if(dominates){
                    UAV_delete_solution(current_solution);
                    return true;
                }
            }

            start   = pos1 + 1;
            end     = start;
            current = temp;

        } // END : while - current not NULL
    }

    UAV_delete_solution(current_solution);
    return false;
}

bool Core::change_charge_rate_STRUCT(Pool * pool, Solution * neighbor_sol, int value) /* get a random sub path from the route and change the recharge rate from of each stretch */
{
    Solution * current_solution = new Solution;
    UAV_init_solution(current_solution);
    UAV_copy_solution(neighbor_sol, current_solution);

    UAV * copy_uav;

    Movement * current,
             * temp;

    int type,
        pos1   = 0;

    bool dominates;

    for (int pos_route = 0; pos_route < current_solution->get_size(); pos_route++) {

        current = current_solution->uavs[pos_route]->route;
        pos1 = 0;

        while(current != NULL){

            /* Subpath between recharge points (start) */

            while(current != NULL){
                type = m_map->get_stop_point(current->x, current->y).get_type();

                if(type == STOPS::CHARGE){                   
                    break;
                }

                current = current->next;
                pos1++;
            }

            if(current == NULL){
                break;
            }

            current = current->next;
            temp = current;

            /* Subpath between recharge points (end) */

            copy_uav = new UAV;
            UAV_init_uav(copy_uav);
            UAV_copy_uav(current_solution->uavs[pos_route], copy_uav);

            current = copy_uav->get_movement(pos1);
            current->change_charge_rate(value);

            UAV_delete_uav(neighbor_sol->uavs[pos_route]);
            neighbor_sol->uavs[pos_route] = new UAV;
            UAV_init_uav(neighbor_sol->uavs[pos_route]);

            UAV_copy_uav(copy_uav, neighbor_sol->uavs[pos_route]);
            UAV_delete_uav(copy_uav);

            evaluate(neighbor_sol, m_map);

            if(!current_solution->dominates(neighbor_sol)){
                UAV_add_solution(pool, neighbor_sol);

                // If new solution dominates current one
                dominates = neighbor_sol->dominates(current_solution);

                if(dominates){
                    UAV_delete_solution(current_solution);
                    return true;
                }
            }

            pos1++;
            current = temp;

        } // END : while - current1 not NULL
    }

    UAV_delete_solution(current_solution);
    return false;
}

bool Core::remove_recharge_STRUCT(Pool * pool, Solution * neighbor_sol) /* remove client from route that were already visited */
{
    Solution * current_solution = new Solution;
    UAV_init_solution(current_solution);
    UAV_copy_solution(neighbor_sol, current_solution);

    UAV * copy_uav;

    Movement * current1,
             * temp1;

    int type,
        pos1 = 0,
        start1 = 0, end1 = 0;

    bool dominates;

    for (int pos_route = 0; pos_route < current_solution->get_size(); pos_route++) {

        current1 = current_solution->uavs[pos_route]->route;

        if(current1 != NULL){
            current1 = current1->next;

            start1 = 1;
            end1   = start1;

            while(current1 != NULL){

                // Find recharge point
                while(current1 != NULL){
                    type = m_map->get_stop_point(current1->x, current1->y).get_type();
                    if(type == STOPS::CHARGE){
                        temp1    = current1->next;
                        current1 = current1->prev;

                        pos1 = end1;
                        start1--;
                        end1++;

                        break;
                    }

                    current1 = current1->next;
                    end1++;
                    start1++;
                }

                if(current1 == NULL){
                    break;
                }

                // Backward
                while(current1 != NULL){
                    type = m_map->get_stop_point(current1->x, current1->y).get_type();

                    if(start1 == 0 || type == STOPS::DELIVER || type == STOPS::COLLECT){
                        start1++;
                        break;
                    }

                    current1 = current1->prev;
                    start1--;
                }

                current1 = temp1;

                // Forward
                while(current1 != NULL){
                    type = m_map->get_stop_point(current1->x, current1->y).get_type();

                    if(type == STOPS::DELIVER || type == STOPS::COLLECT){
                        break;
                    }

                    current1 = current1->next;
                    end1++;
                }

                copy_uav = new UAV;
                UAV_init_uav(copy_uav);
                UAV_copy_uav(current_solution->uavs[pos_route], copy_uav);

                // x------C------x

                UAV_remove_subpath(copy_uav, start1, end1 - 1); // x|x

                if(current1 != NULL){ // If not like : x|
                    sub_path(copy_uav, start1 - 1, start1); // x------------x
                }

                UAV_delete_uav(neighbor_sol->uavs[pos_route]);
                neighbor_sol->uavs[pos_route] = new UAV;
                UAV_init_uav(neighbor_sol->uavs[pos_route]);

                UAV_copy_uav(copy_uav, neighbor_sol->uavs[pos_route]);
                UAV_delete_uav(copy_uav);

                evaluate(neighbor_sol, m_map);

                if(!current_solution->dominates(neighbor_sol)){
                    UAV_add_solution(pool, neighbor_sol);

                    // If new solution dominates current one
                    dominates = neighbor_sol->dominates(current_solution);

                    if(dominates){
                        UAV_delete_solution(current_solution);
                        return true;
                    }
                }

                start1   = pos1 + 1;
                end1     = start1;
                current1 = temp1;

            } // END : while - current1 not NULL
        } // END : if - current1 not NULL
    }

    UAV_delete_solution(current_solution);
    return false;
}

bool Core::nearest_recharge(Pool * pool, Solution * neighbor_sol) /*  */
{

    Solution * current_solution = new Solution;
    UAV_init_solution(current_solution);
    UAV_copy_solution(neighbor_sol, current_solution);

    UAV * copy_uav;

    Movement * current1,
             * temp1;

    int x1, y1,
        x2, y2,
        type,
        pos1 = 0,
        end1 = 0;

    bool dominates;

    for (int pos_route = 0; pos_route < current_solution->get_size(); pos_route++) {

        current1 = current_solution->uavs[pos_route]->route;

        pos1 = 0;
        end1 = pos1;

        while(current1 != NULL){

            /* First client (start) */

            while(current1 != NULL){
                type = m_map->get_stop_point(current1->x, current1->y).get_type();

                if(end1 == 0 || type == STOPS::DELIVER || type == STOPS::COLLECT){
                    x1 = current1->x;
                    y1 = current1->y;

                    current1 = current1->next;
                    temp1 = current1;

                    pos1 = end1;
                    end1++;

                    break;
                }

                current1 = current1->next;
                end1++;
            }

            // If not the last client
            if(current1 != NULL){
                while(current1 != NULL){
                    type = m_map->get_stop_point(current1->x, current1->y).get_type();

                    if(type == STOPS::DELIVER || type == STOPS::COLLECT){
                        break;
                    }

                    current1 = current1->next;
                    end1++;
                }

                /* First client (end) */

                int min_distance = INT_MAX;
                int pos_min = 0;

                // Get the closest recharge point
                for (int position = 0; position < recharge_points.size(); position++) {
                    int distance_temp = Array_Tools::distance(recharge_points[position].get_x(), recharge_points[position].get_y(), x1, y1);

                    if(current1 != NULL){
                        distance_temp += Array_Tools::distance(recharge_points[position].get_x(), recharge_points[position].get_y(), current1->x, current1->y);
                    }

                    if(distance_temp < min_distance){
                        min_distance = distance_temp;
                        pos_min = position;
                    }
                }

                x2 = recharge_points[pos_min].get_x();
                y2 = recharge_points[pos_min].get_y();

                /* Second client (start) */

                copy_uav = new UAV;
                UAV_init_uav(copy_uav);
                UAV_copy_uav(current_solution->uavs[pos_route], copy_uav);

                // x------1------x AND R

                UAV_remove_subpath(copy_uav, pos1 + 1, end1 - 1); // x------1|x AND R

                UAV_insert_movement(copy_uav, x2, y2, pos1 + 1); // insert the recharge point at the position : x------1Rx

                if(current1 != NULL){ // If not like : x------1R
                     sub_path(copy_uav, pos1 + 1, pos1 + 2); // x------1R--------x
                }

                sub_path(copy_uav, pos1, pos1 + 1); // x------1----R--------x



                UAV_delete_uav(neighbor_sol->uavs[pos_route]);
                neighbor_sol->uavs[pos_route] = new UAV;
                UAV_init_uav(neighbor_sol->uavs[pos_route]);

                UAV_copy_uav(copy_uav, neighbor_sol->uavs[pos_route]);
                UAV_delete_uav(copy_uav);

                evaluate(neighbor_sol, m_map);

                if(!current_solution->dominates(neighbor_sol)){
                    UAV_add_solution(pool, neighbor_sol);

                    // If new solution dominates current one
                    dominates = neighbor_sol->dominates(current_solution);

                    if(dominates){
                        UAV_delete_solution(current_solution);
                        return true;
                    }
                }

                pos1++;
                end1     = pos1;
                current1 = temp1;
            }
        } // END : while - current1 not NULL
    }

    UAV_delete_solution(current_solution);
    return false;
}

bool Core::neighbor_STRUCT(Pool * pool, Solution * neighbor_sol, int type) /* directs to the desirable neighborhood function */
{
    switch (type) {
    case 0:
        if(swap_3_available){
            return shift_1_1_complete(pool, neighbor_sol);
        } else if(swap_4_available){
            return shift_1_1_nearest(pool, neighbor_sol);
        }
        break;
    case 1:
        if(swap_5_available){
            return shift_1_1_furthest(pool, neighbor_sol);
        }else if(swap_6_available){
            return shift_1_1_random(pool, neighbor_sol);
        }
        break;
    case 2:
        return swap_1_1(pool, neighbor_sol);
        break;
    case 3:
        return swap_1_0(pool, neighbor_sol);
        break;
    case 4:
        return remove_recharge_STRUCT(pool, neighbor_sol);
        break;
    case 5:
        return nearest_recharge(pool, neighbor_sol);
        break;
    case 6:
        return remove_repeated_STRUCT(pool, neighbor_sol);
        break;
    case 7:
        return change_vel_between_recharge(pool, neighbor_sol, -1);
        break;
    case 8:
        return change_vel_between_recharge(pool, neighbor_sol, 1);
        break;
    case 9:
        return change_charge_rate_STRUCT(pool, neighbor_sol, -5);
        break;
    case 10:
        return change_charge_rate_STRUCT(pool, neighbor_sol, 5);
        break;
    case 11:
        return change_vel_random(pool, neighbor_sol, -1);
        break;
    case 12:
        return change_vel_random(pool, neighbor_sol, 1);
        break;
    }

    return false;
}

void Core::evaluate_loop(UAV * route, Matrix * m_map, double current_time, Movement * current_move, double battery_charge) {/* run through all the solution evaluating it*/

    if (current_move == NULL) {

        //Setando o valor dos objetivos
        route->objectives[1] =        battery_charge;
        route->objectives[2] = (-1) * current_time  ;

        return;
    }

    //Consumo conforme velocidade
    double vel = current_move->vel;
    double variable_consumption = (vel * VARIABLE_BATTERY_CONSUMPTION);
    battery_charge -= variable_consumption;

    //Consumo fixo
    battery_charge -= FIXED_BATTERY_CONSUMPTION;

    //Insert into the array of objectives of the solution
    //NEGATIVE: we want to minimize
    route->objectives[3] += (-1) * (variable_consumption + FIXED_BATTERY_CONSUMPTION);

    //Replace the current point with an updated one
    current_move->battery_charge = battery_charge;

    if (battery_charge < 0) {
        route->valid = false;
    }

    //Tempo para percorrer o trecho
    int prev_x = current_move->prev->get_x();
    int prev_y = current_move->prev->get_y();

    int x = current_move->get_x();
    int y = current_move->get_y();

    double distance = DISCRETIZATION * (sqrt((x - prev_x) * (x - prev_x) + (y - prev_y) * (y - prev_y)));
    current_time += (distance / vel);

    int type = m_map->get_stop_point(x, y).get_type();

    switch (type) {
    case STOPS::NOTHING: break;

    case STOPS::PROHIBITED:
        route->valid = false;
        break;

    case STOPS::DELIVER:
        m_map->set_stop_point(x,y, STOPS::NOTHING);
        route->objectives[0] += 1;
        break;
    case STOPS::COLLECT:
        m_map->set_stop_point(x, y, STOPS::NOTHING);
        route->objectives[0] += 1;
        break;
    case STOPS::CHARGE:
//        double recharge = MAX_BATTERY_CHARGE - battery_charge;
//        recharge = (recharge >  BATTERY_RECHARGE) ? BATTERY_RECHARGE : recharge;

        double new_battery_charge = battery_charge + (MAX_BATTERY_CHARGE * current_move->charge_rate / 100);

        if (new_battery_charge > MAX_BATTERY_CHARGE)
            new_battery_charge = MAX_BATTERY_CHARGE;

        //Tempo gasto carregando
        current_time += TIME_BATTERY_RECHARGE * current_move->charge_rate / 100;
        battery_charge = new_battery_charge;

        break;
    }

    return evaluate_loop(route, m_map, current_time, current_move->next, battery_charge);
}

void Core::evaluate(Solution * a_solution, Matrix * m_map) { /* set the value of the objective functions of the current solution */

    Matrix * m_map_copy = m_map->copy(); // Copy of the map that is going to be used by every route

    for (int pos_route = 0; pos_route < a_solution->get_size(); pos_route++) {// Run through all routes

        for (int i = 0; i < a_solution->num_objectives; i++) {
            a_solution->uavs[pos_route]->objectives[i] = 0;
        }

        //Verify origin point
        int x = a_solution->uavs[pos_route]->get_movement(0)->get_x();
        int y = a_solution->uavs[pos_route]->get_movement(0)->get_y();

        int type = m_map_copy->get_stop_point(x, y).get_type();

        switch (type) {
        case STOPS::DELIVER:
            m_map_copy->set_stop_point(x,y, STOPS::NOTHING);
            a_solution->uavs[pos_route]->objectives[0] += 1;
            break;
        case STOPS::COLLECT:
            m_map_copy->set_stop_point(x, y, STOPS::NOTHING);
            a_solution->uavs[pos_route]->objectives[0] += 1;
            break;
        }

        a_solution->uavs[pos_route]->complete = false;
        a_solution->uavs[pos_route]->valid    = true;
        evaluate_loop(a_solution->uavs[pos_route], m_map_copy, 0, a_solution->uavs[pos_route]->route->next, initial_battery_charge);
    }

    a_solution->update_values(); // Update objective values of the current solution

    if (completed_path(a_solution)) {
        if(a_solution->valid){
            a_solution->complete = true;
            completed = true;
            valid = true;
        }
    }

    delete m_map_copy;
}

vector<Solution *> Core::aggregate_STRUCT(int num_exec, int num_obj, string directory_name)
{
    vector<Solution *> v_a_s;

    cout << directory_name << endl;

    Pool * p_s = new Pool;
    UAV_init_pool(p_s);

    bool valid = false;

    string sol_directory_name = directory_name + "solutions";

    string d_name;
    DIR * dir;
    struct dirent * ent;
    if((dir = opendir(sol_directory_name.c_str())) == NULL){
        cout << "Directory does not exist" << endl;
        return v_a_s;
    } else {
        while((ent = readdir(dir)) != NULL){
            d_name = ent->d_name;
            if(d_name.compare(".") != 0 && d_name.compare("..") != 0 && d_name.compare("routes") != 0){
                Solution * a_s = new Solution;
                UAV_init_solution(a_s);

                Array_Tools::UAV_from_file_solution(a_s, sol_directory_name + "/" + d_name);

                UAV_add_solution(p_s, a_s);
            }

        }

        closedir(dir);

        double z1, z2, z3;
        int max_final_charge = INT_MIN, min_time = INT_MAX, min_consumption = INT_MAX;
        int total_final_charge = 0, total_time = 0, total_consumption = 0;

        Objective_Solution * o_s;
        Array_Objective_Solution * a_o_s = new Array_Objective_Solution();

        for (int var = 0; var < p_s->get_size(); var++) {
            v_a_s.push_back(p_s->solutions[var]);

            o_s = new Objective_Solution(num_obj);

            for (int count = 1; count <= num_obj; count++) {
                o_s->set_z(count - 1, p_s->solutions[var]->objectives[count]);
            }

            a_o_s->add(o_s->copy());
            delete o_s;

            // Average values from objectives
            z1 =        p_s->solutions[var]->objectives[1];
            z2 = (-1) * p_s->solutions[var]->objectives[2];
            z3 = (-1) * p_s->solutions[var]->objectives[3];

            if(z1 > max_final_charge){
                max_final_charge = z1;
            }

            if(z2 < min_time){
                min_time = z2;
            }

            if(z3 < min_consumption){
                min_consumption = z3;
            }

            total_final_charge += z1;
            total_time += z2;
            total_consumption += z3;
        }

        double average_final_charge = total_final_charge/ (double) p_s->get_size();
        double average_time         = total_time        / (double) p_s->get_size();
        double average_consumption  = total_consumption / (double) p_s->get_size();

        double dev_final_charge = 0;
        double dev_time         = 0;
        double dev_consumption  = 0;

        for (int k = 0; k < p_s->get_size(); k++) {
            dev_final_charge += pow(  p_s->solutions[k]->objectives[1] - average_final_charge, 2);
            dev_time         += pow(- p_s->solutions[k]->objectives[2] - average_time, 2);
            dev_consumption  += pow(- p_s->solutions[k]->objectives[3] - average_consumption, 2);
        }

        dev_final_charge = sqrt(dev_final_charge);
        dev_time         = sqrt(dev_time);
        dev_consumption  = sqrt(dev_consumption);

        //START: Info file
        ofstream info_file;
        info_file.open (directory_name + "info_aggregation.txt");

        info_file << ">> Average Final Charge: " << average_final_charge <<
                    " >> Standard Deviation Final Charge: " << dev_final_charge <<
                    " >> Best Final Charge: " << max_final_charge << "\n";
        info_file << ">> Average Time: " << average_time <<
                    " >> Standard Deviation Time: " << dev_time <<
                    " >> Best Time: " << min_time << "\n";
        info_file << ">> Average Consumption: " << average_consumption <<
                    " >> Standard Deviation Consumption: " << dev_consumption <<
                    " >> Best Consumption: " << min_consumption << "\n";

        info_file << "\n" << average_final_charge <<
                     "\n" << dev_final_charge <<
                     "\n" << max_final_charge <<
                     "\n" << average_time <<
                     "\n" << dev_time <<
                     "\n" << min_time <<
                     "\n" << average_consumption <<
                     "\n" << dev_consumption <<
                     "\n" << min_consumption << "\n";

        info_file.close();
        //END: Info file

        a_o_s->to_file(directory_name + "aggregation.txt");
    }

    return v_a_s;
}

void Core::set_paths_STRUCT(Matrix * map, vector<Solution_Pair> client_points, vector<Solution_Pair> recharge_points, string directory_name) /* return the route with a sub path from START point to END modified */
{
    string paths_directory_name = directory_name + "/paths";
    mkdir(paths_directory_name.c_str(), 0777);

//    Route path;
    UAV * path = new UAV;
    UAV_init_uav(path);

    // Instantiate origin
    Stop_Point origin = map->get_stop_point(origin_x,origin_y);

    // If origin is not client neither recharge point
    if(origin.get_type() == STOPS::NOTHING){
        // Distance between origin and recharge points
        foreach (Solution_Pair recharge, recharge_points) {
            ifstream path_file(directory_name + "/paths/" +
                                to_string(origin_x) + "_" + to_string(origin_y) + "_" +
                                to_string(recharge.get_x()) + "_" + to_string(recharge.get_y()) + ".txt");

            if ( !path_file ){
                a_star(path, map, origin, map->get_stop_point(recharge));
                UAV_to_file_uav(path, directory_name + "/paths/" +
                                      to_string(origin_x) + "_" + to_string(origin_y) + "_" +
                                      to_string(recharge.get_x()) + "_" + to_string(recharge.get_y()) + ".txt"
                                );

                // Reverse order
                UAV_invert_uav(path);
                UAV_to_file_uav(path, directory_name + "/paths/" +
                                      to_string(recharge.get_x()) + "_" + to_string(recharge.get_y()) + "_" +
                                      to_string(origin_x) + "_" + to_string(origin_y) + ".txt"
                                );

                UAV_delete_uav(path);
                path = new UAV;
                UAV_init_uav(path);
            }
        }

        // Distance between origin and clients
        for (int index = 0; index < client_points.size(); index++) {
            ifstream path_file(directory_name + "/paths/" +
                               to_string(origin_x) + "_" + to_string(origin_y) + "_" + //"0_0_" +
                               to_string(client_points[index].get_x()) + "_" + to_string(client_points[index].get_y()) +".txt"
                               );

            if ( !path_file ){
                a_star(path, map, origin, map->get_stop_point(client_points[index]));
                UAV_to_file_uav(path, directory_name + "/paths/" +
                                      to_string(origin_x) + "_" + to_string(origin_y) + "_" + //"0_0_" +
                                      to_string(client_points[index].get_x()) + "_" + to_string(client_points[index].get_y()) +".txt"
                                );

                // Reverse order
                UAV_invert_uav(path);
                UAV_to_file_uav(path, directory_name + "/paths/" +
                                      to_string(client_points[index].get_x()) + "_" + to_string(client_points[index].get_y()) + "_" +
                                      to_string(origin_x) + "_" + to_string(origin_y) + ".txt"
                                );

                UAV_delete_uav(path);
                path = new UAV;
                UAV_init_uav(path);
            }
        }
    }

    // Distance between clients and recharge points
    foreach (Solution_Pair client, client_points) {
        foreach (Solution_Pair recharge, recharge_points) {
            ifstream path_file(directory_name + "/paths/" +
                               to_string(client.get_x())   + "_" + to_string(client.get_y())   + "_" +
                               to_string(recharge.get_x()) + "_" + to_string(recharge.get_y()) +".txt");

            if ( !path_file ){
                a_star(path, map, map->get_stop_point(client), map->get_stop_point(recharge));
                UAV_to_file_uav(path, directory_name + "/paths/" +
                                      to_string(client.get_x())   + "_" + to_string(client.get_y())   + "_" +
                                      to_string(recharge.get_x()) + "_" + to_string(recharge.get_y()) +".txt"
                                );

                // Reverse order
                UAV_invert_uav(path);
                UAV_to_file_uav(path, directory_name + "/paths/" +
                                      to_string(recharge.get_x()) + "_" + to_string(recharge.get_y()) + "_" +
                                      to_string(client.get_x())   + "_" + to_string(client.get_y())   + ".txt"
                                );

                UAV_delete_uav(path);
                path = new UAV;
                UAV_init_uav(path);
            }
        }
    }

    // Distance between recharge points
    for (int index_1 = 0; index_1 < recharge_points.size() - 1; index_1++) {
        for (int index_2 = index_1 + 1; index_2 < recharge_points.size(); index_2++) {
            ifstream path_file(directory_name + "/paths/" +
                               to_string(recharge_points[index_1].get_x()) + "_" + to_string(recharge_points[index_1].get_y()) + "_" +
                               to_string(recharge_points[index_2].get_x()) + "_" + to_string(recharge_points[index_2].get_y()) +".txt"
                               );

            if ( !path_file ){
                a_star(path, map, map->get_stop_point(recharge_points[index_1]), map->get_stop_point(recharge_points[index_2]));
                UAV_to_file_uav(path, directory_name + "/paths/" +
                                      to_string(recharge_points[index_1].get_x()) + "_" + to_string(recharge_points[index_1].get_y()) + "_" +
                                      to_string(recharge_points[index_2].get_x()) + "_" + to_string(recharge_points[index_2].get_y()) +".txt"
                                );

                // Reverse order
                UAV_invert_uav(path);
                UAV_to_file_uav(path, directory_name + "/paths/" +
                                      to_string(recharge_points[index_2].get_x()) + "_" + to_string(recharge_points[index_2].get_y()) + "_" +
                                      to_string(recharge_points[index_1].get_x()) + "_" + to_string(recharge_points[index_1].get_y()) +".txt"
                                );

                UAV_delete_uav(path);
                path = new UAV;
                UAV_init_uav(path);
            }
        }
    }

    // Distance between clients
    for (int index_1 = 0; index_1 < client_points.size() - 1; index_1++) {
        for (int index_2 = index_1 + 1; index_2 < client_points.size(); index_2++) {
            ifstream path_file(directory_name + "/paths/" +
                               to_string(client_points[index_1].get_x()) + "_" + to_string(client_points[index_1].get_y()) + "_" +
                               to_string(client_points[index_2].get_x()) + "_" + to_string(client_points[index_2].get_y()) +".txt"
                               );

            if ( !path_file ){
                a_star(path, map, map->get_stop_point(client_points[index_1]), map->get_stop_point(client_points[index_2]));
                UAV_to_file_uav(path, directory_name + "/paths/" +
                                      to_string(client_points[index_1].get_x()) + "_" + to_string(client_points[index_1].get_y()) + "_" +
                                      to_string(client_points[index_2].get_x()) + "_" + to_string(client_points[index_2].get_y()) +".txt"
                                );

                // Reverse order
                UAV_invert_uav(path);
                UAV_to_file_uav(path, directory_name + "/paths/" +
                                      to_string(client_points[index_2].get_x()) + "_" + to_string(client_points[index_2].get_y()) + "_" +
                                      to_string(client_points[index_1].get_x()) + "_" + to_string(client_points[index_1].get_y()) +".txt"
                                );

                UAV_delete_uav(path);
                path = new UAV;
                UAV_init_uav(path);
            }
        }
    }

}

void Core::a_star(UAV * path, Matrix * map, Stop_Point start, Stop_Point end) /* return the route with a sub path from START point to END modified */
{

    // Create start and end node
    A_Star_Node * start_node = new A_Star_Node(NULL, start);
    A_Star_Node * end_node   = new A_Star_Node(NULL, end);

    // Initialize both open and closed list
    vector<A_Star_Node *> open_list;
    vector<A_Star_Node *> closed_list;

    // Add the start node
    open_list.push_back(start_node);

    // Current node
    A_Star_Node * current_node;
    int current_index;

    // Loop until you find the end
    while(!open_list.empty()){

        // Get the current node of smaller value of f
        current_node = open_list[0];
        current_index = 0;

        for (int index = 0; index < open_list.size(); index++) {
            if(open_list[index]->get_f() < current_node->get_f()){
                current_node = open_list[index];
                current_index = index;
            }
        }

        // Pop current node off open list and add it to the closed list
        open_list.erase(open_list.begin() + current_index);
        closed_list.push_back(current_node);

        // Found the goal
        if(current_node->equals(end_node)){
            vector<Stop_Point> temp;
            A_Star_Node * current = current_node;

            while (current != NULL) {
                temp.push_back(current->get_position());
                current = current->get_parent();
            }

            // Reverse the path
            for (int i = temp.size() - 1; i >= 0; i--) {
                Solution_Pair p = Solution_Pair(temp[i]);
                UAV_push_movement(temp[i].get_x(), temp[i].get_y(), path);
            }

            // Delete from memory
            for (int i = 0; i < open_list.size(); i++) {
                delete open_list[i];
            }

            for (int i = 0; i < closed_list.size(); i++) {
                delete closed_list[i];
            }

            temp.clear();

            return;
        }

        // Generate children
        vector<A_Star_Node *> children = current_node->adjacents(map, true);

        // Loop through children
        foreach (A_Star_Node * child, children) {

            // Child is on the closed list
            foreach (A_Star_Node * closed_child, closed_list) {
                if(child->equals(closed_child)){
                    goto next_iteration;
                }
            }

            // Set f, g and h values
            child->set_g(current_node->get_g() + 1);
            child->set_h(pow(child->get_position().get_x() - end_node->get_position().get_x(), 2) + pow(child->get_position().get_y() - end_node->get_position().get_y(), 2));
            child->set_f(child->get_g() + child->get_h());

            // Child is already on the open list and better path
            foreach (A_Star_Node * open_node, open_list) {
                if(child->equals(open_node) && child->get_g() > open_node->get_g()){
                    goto next_iteration;
                }
            }

            // Add the child to the open list
            open_list.push_back(child);

            next_iteration:;
        }
    }
}

void Core::sub_path(UAV *route, int start, int end) /* return the route with a sub path from START point to END */
{
    // Get extreme points
    Movement * start_movement = route->get_movement(start);
    Movement * end_movement   = route->get_movement(end);

    if(start_movement->is_at_same_position(end_movement)){
        UAV_remove_movement(route, end);
        return;
    }

    if(start_movement->x == 38 && start_movement->y == 61){
        int h = 0;
    }

    if(start_movement->x == 45 && start_movement->y == 14){
        int h = 0;
    }

    // Get path between start and end points
    UAV * sub_path_route = new UAV;
    UAV_init_uav(sub_path_route);

    if(pre_processing){
        // Get path from struct pre-generated
        Array_Tools::UAV_from_file_uav(sub_path_route, global_file_name + "/paths/" +
                                                       to_string(start_movement->get_x()) + "_" + to_string(start_movement->get_y()) + "_" +
                                                       to_string(end_movement->get_x()  ) + "_" + to_string(end_movement->get_y()  ) + ".txt"
                                       );
    } else {
        // Generate path with A* heuristic
        a_star(sub_path_route, m_map, m_map->get_stop_point(start_movement->get_x(), start_movement->get_y()),
                                      m_map->get_stop_point(end_movement->get_x()  , end_movement->get_y()  )
              );
    }

    // Merge paths
    UAV_merge_uav_without_start_end(route, sub_path_route, start + 1);
}

void Core::initial_solution(Solution * solution) /* constructor */
{
    // Add origin
    for (int j = 0; j < solution->get_size(); j++) {
        UAV_push_movement(origin_x, origin_y, solution->uavs[j]);
    }

    int pos_min, distance_temp, distance_min;

    //Copy clients vector
    vector<Solution_Pair> clients_temp = client_points;

    // If origin is a client, does not need to pass through it again during the route
    for (int i = 0; i < clients_temp.size(); i++) {
        // Equals origin
        if(clients_temp[i].get_x() == origin_x && clients_temp[i].get_y() == origin_y){
            clients_temp.erase(clients_temp.begin() + i);
            break;
        }
    }// END FOR 'clients_temp'

    int num_clients = clients_temp.size();

    //Number of clients that will be inserted into the routes
    for (int i = 0; i < num_clients; i++) {

        //For each route
        for (int j = 0; j < solution->get_size() &&
                        i * solution->get_size() + j < num_clients; // Number of clients inserted is smaller than the amount of clients on map
                 j++) {

            distance_min = INT_MAX;
            vector<int> min_distances ((int) GRASP_FACTOR, INT_MAX);
            vector<int> min_positions ((int) GRASP_FACTOR, 0);

            for (int position = 0; position < clients_temp.size(); position++) {
                //Distance between new client and the last one inserted into the solution
                distance_temp = Array_Tools::distance(clients_temp[position].get_x(),
                                                      clients_temp[position].get_y(),
                                                      solution->uavs[j]->get_movement(i)->x,
                                                      solution->uavs[j]->get_movement(i)->y);

                //Run through current closest neighbors
                for (int min_position = 0; min_position < min_distances.size(); min_position++) {

                    //If new distance is smaller
                    if(distance_temp < min_distances[min_position]){

                        //Update vector of closest neighbors
                        for(int k = min_distances.size() - 1; k > min_position; k--) {
                          min_positions[k] = min_positions[k-1];
                          min_distances[k] = min_distances[k-1];
                        }
                        min_distances[min_position] = distance_temp;
                        min_positions[min_position] = position;

                        break;
                    }
                }// END FOR 'min_position'
            }// END FOR 'position'

            //Get random position from the vector
            pos_min = rand() % (min_positions.size());
            pos_min = min_positions[pos_min];

            //Push client to the solution
            UAV_push_movement(clients_temp[pos_min].get_x(), clients_temp[pos_min].get_y(), solution->uavs[j]);

            //Link the new client inserted and the last one from the solution
            sub_path(solution->uavs[j], solution->uavs[j]->get_size() - 2, solution->uavs[j]->get_size() - 1);

            clients_temp.erase(clients_temp.begin() + pos_min);

            min_distances.clear();
            min_positions.clear();
        }// END FOR 'solution'

    }// END FOR 'clients_temp'
}

vector<Solution *> Core::test_i_vns_STRUCT(int time_limit, int num_exec, string instance_name, string directory_name, int pct_prohibited = 0, bool load_file = false, string load_file_name = "")
{
    // Ensures that the number will be randomly generated
    srand(time(NULL));

    string base_file_name = directory_name + "/" + "i_vns_STRUCT_swap_";

    if(swap_3_available){
        base_file_name += "03_";
    }
    if(swap_4_available){
        base_file_name += "04_";
    }
    if(swap_5_available){
        base_file_name += "05_";
    }
    if(swap_6_available){
        base_file_name += "06_";
    }

    //Create directory
    mkdir(base_file_name.c_str(), 0777);
    base_file_name += "/";



//    Solution * copy_sol = new Solution;
//    UAV_init_solution(copy_sol);

//    UAV_copy_solution(init_sol, copy_sol);

//    UAV_remove_uav(init_sol, 0);

//    UAV_delete_solution(init_sol);
//    UAV_test_solution(copy_sol);


    /* Operations */
//    int i;
//    for (i = 0; i < pool.capacity; i++) {
//        Solution * sol = new Solution;
//        UAV_init_solution(sol);
//        UAV_add_solution(&pool, sol);
//        sol->iteration = i + 3;
//    }
//    UAV_remove_solution(&pool, 1);
//    UAV_test_pool(&pool);
//    UAV_push_movement(4, 3, uav);
//    UAV_pop_movement (uav);
//    UAV_merge_uav_without_start_end(uav, uav2, 1);
//    UAV_insert_subpath(uav2, uav, 3);
//    UAV_test_uav(uav);
//    UAV_remove_subpath(uav, 1, 3);
//    UAV_remove_uav(init_sol, 1);
//    UAV_delete_solution(init_sol);
//    UAV_delete_uav(u1);
//    UAV_copy_uav(u1, u2);

    // Print the map
    m_map->print();

    cout << ">> Lines: "     << MATRIX_NUMBER_LINES        << endl;
    cout << ">> Columns: "   << MATRIX_NUMBER_COLUMNS      << endl;
    cout << "\n>> Deliver: " << QTY_POINTS[STOPS::DELIVER] << endl;

    generate_neighbors        (neighbors_qty);
    generate_further_neighbors(neighbors_qty);

    // Initialize variables that will be used in the loop
    bool dominates = false;
    int actual_max_size = MAX_TIME / TIME_INTERVAL;
//    int i = 0;
    int best_iteration = 0;
    //    

    int iteration_GRASP;
    Pool * pool_global,
         * pool_local;

    auto start = std::chrono::system_clock::now();
    auto end   = std::chrono::system_clock::now();
    std::chrono::duration<double> diff;
    double diff_count = 0;

    //Run loop until number of executions is reached
    for (int test_count = 0; test_count < num_exec; test_count++) {
        cout << "\nSTART: " << endl;

        pool_global = new Pool;
        UAV_init_pool(pool_global);
        iteration_GRASP = 0;

        start      = std::chrono::system_clock::now();
        diff_count = 0;

        // GRASP
        do{
            /* NEW GRASP ITERATION */
            pool_local = new Pool;
            UAV_init_pool(pool_local);

            Solution * init_sol = new Solution;
            UAV_init_solution(init_sol);

            if(load_file){
                 Array_Tools::UAV_from_file_solution(init_sol, load_file_name);
            } else {
                for (int i = 0; i < init_sol->get_capacity(); i++) {
                    UAV  *uav = new UAV;
                    UAV_init_uav(uav);
                    UAV_add_uav(init_sol, uav);
                }

                initial_solution(init_sol);
            }

            init_sol->iteration = iteration_GRASP;
            evaluate(init_sol, m_map);

            UAV_add_solution(pool_local , init_sol);
            UAV_add_solution(pool_global, init_sol);
            UAV_delete_solution(init_sol);            

            /* LOCAL SEARCH (start) */
            // Iterate over local pool
            while(pool_local->size > 0 && diff_count < time_limit)
            {
                Solution * current_sol = new Solution;
                UAV_init_solution(current_sol);

                UAV_copy_solution(pool_local->solutions[0], current_sol);
                current_sol->iteration = iteration_GRASP;

                // Remove from local pool
                UAV_remove_solution(pool_local, 0);

                // Add to global pool
                UAV_add_solution(pool_global, current_sol);

                // Run through all neighborhoods
                int n_count = 0;
                while(n_count < NEIGHBORHOOD_QTY && diff_count < time_limit)
                {

                    dominates = false;

                    auto start_neighbor = std::chrono::system_clock::now();                    

                    // EXEC neighborhood
                    dominates = neighbor_STRUCT(pool_local, current_sol, n_count);

                    auto end_neighbor = std::chrono::system_clock::now();
                    std::chrono::duration<double> diff_neighbor = end_neighbor - start_neighbor;

                    NEIGHBORHOOD_QTY_VEC[n_count]++;
                    NEIGHBORHOOD_TIME   [n_count] += diff_neighbor.count();                    

                    // If new solution dominates current one
                    if(dominates){
                        NEIGHBORHOOD_ADDED[n_count]++;

                        // Go back to first neighborhood
//                        n_count = 0;

                        // UPDATE current solution
                        UAV_delete_solution(current_sol);

                        current_sol = new Solution;
                        UAV_init_solution(current_sol);
                        UAV_copy_solution(pool_local->solutions[pool_local->size - 1], current_sol);

                        // Remove from local pool
                        UAV_remove_solution(pool_local, pool_local->size - 1);

                        // Add to global pool
                        UAV_add_solution(pool_global, current_sol);
                    }  else {
                        n_count++;
                    }

                    // TIME COUNT
                    end = std::chrono::system_clock::now();
                    diff = end - start;
                    diff_count = diff.count();
                }

                UAV_delete_solution(current_sol);
            }


            /* LOCAL SEARCH (end)   */

            UAV_delete_pool(pool_local);

            iteration_GRASP++;

        }while(diff_count < time_limit);

        /* WRITE SOLUTIONS FILE */

        string results_name = "results_";
        string file_count   = to_string(test_count);
        string file_format  = ".txt";

        string results_file_name = base_file_name + results_name + file_count + file_format;
        cout << "Results file name: " << results_file_name << "\n" << endl;

        string info_name = "info_";

        string info_file_name = base_file_name + info_name + file_count + file_format;
        cout << "Info file name: " << info_file_name << "\n" << endl;

        cout << "\nMAX ITERATION: "  << iteration_completed << endl;
        cout << "\nLAST ITERATION: " << iteration_GRASP << endl;

        string completed_resp = completed ? "true" : "false";
        cout << "\nCOMPLETED: " << completed_resp << endl;

        string valid_resp = valid ? "true" : "false";
        cout << "\nVALID: " << valid_resp << endl;

        cout << "Total time: " << diff.count() << "\n" << endl;

        //START: Info file
        ofstream info_file;
        info_file.open (info_file_name);

        info_file << "INSTANCE: "     << instance_name << "\n";
        info_file << "\nPOINTS: "     << QTY_POINTS[STOPS::DELIVER] + QTY_POINTS[STOPS::COLLECT] << "\n";
        info_file << "\nRECHARGE: "   << QTY_POINTS[STOPS::CHARGE] << "\n";
        info_file << "\nPROHIBITED: " << QTY_POINTS[STOPS::PROHIBITED] << "\n";
        info_file << "\n------------------------------------------" << "\n";

        info_file << "\nMETHODS: " << "swap ";

        if(swap_3_available){
            info_file << "03 ";
        }
        if(swap_4_available){
            info_file << "04 ";
        }
        if(swap_5_available){
            info_file << "05 ";
        }
        if(swap_6_available){
            info_file << "06 ";
        }

        info_file << "\n";

        info_file << "\nSTOP CHRITERION: " << "time (" << time_limit << " s) - hard" << "\n";
        info_file << "\nPOOL MAX SIZE: "   << "30" << "\n";
        info_file << "\n------------------------------------------" << "\n";

        info_file << "\nLAST ITERATION: " << iteration_GRASP         << "\n";
        info_file << "\n# OF SOLUTIONS: " << pool_global->get_size() << "\n";
        info_file << "\nCOMPLETED: "      << completed_resp          << "\n";
        info_file << "\nTOTAL TIME: "     << diff.count()            << "\n\n";
        info_file << "------------------------------------------"    << "\n\n";
        //END: Info file

        int max_final_charge = INT_MIN,
            min_time         = INT_MAX,
            min_consumption  = INT_MAX;

        int total_final_charge = 0,
            total_time         = 0,
            total_consumption  = 0;

        double z1, z2, z3;

        ofstream myfile;
        myfile.open (results_file_name);

        //Create directory to store solutions
        string solutions_directory_name = base_file_name + "solutions";
        mkdir(solutions_directory_name.c_str(), 0777);

        string routes_directory_name = solutions_directory_name + "/routes";
        mkdir(routes_directory_name.c_str(), 0777);

        for (int k = 0; k < pool_global->get_size(); k++) {

            UAV_to_file_solution(pool_global->solutions[k], solutions_directory_name + "/solution_" + to_string(test_count) + "_" + to_string(k));

            z1 =        pool_global->solutions[k]->objectives[1];
            z2 = (-1) * pool_global->solutions[k]->objectives[2];
            z3 = (-1) * pool_global->solutions[k]->objectives[3];

            if(z1 > max_final_charge){
                max_final_charge = z1;
            }

            if(z2 < min_time){
                min_time = z2;
            }

            if(z3 < min_consumption){
                min_consumption = z3;
            }

            total_final_charge += z1;
            total_time         += z2;
            total_consumption  += z3;

            myfile << z1 << " " << -z2 << " " << -z3 << "\n";
        }

        myfile.close();

        double average_final_charge = total_final_charge/ (double) pool_global->get_size();
        double average_time         = total_time        / (double) pool_global->get_size();
        double average_consumption  = total_consumption / (double) pool_global->get_size();

        double dev_final_charge = 0;
        double dev_time         = 0;
        double dev_consumption  = 0;

        for (int k = 0; k < pool_global->get_size(); k++) {
            dev_final_charge += pow(  pool_global->solutions[k]->objectives[1] - average_final_charge, 2);
            dev_time         += pow(- pool_global->solutions[k]->objectives[2] - average_time        , 2);
            dev_consumption  += pow(- pool_global->solutions[k]->objectives[3] - average_consumption , 2);
        }

        dev_final_charge = sqrt(dev_final_charge);
        dev_time         = sqrt(dev_time);
        dev_consumption  = sqrt(dev_consumption);

        for (int count = 0; count < NEIGHBORHOOD_QTY_VEC.size(); count++) {
            if(NEIGHBORHOOD_QTY_VEC[count] > 0){
                cout      << ">> Vizinhanca " << count << ": " << NEIGHBORHOOD_ADDED[count] << " added - " << NEIGHBORHOOD_QTY_VEC[count] << " (" << NEIGHBORHOOD_TIME[count] /  (double) NEIGHBORHOOD_QTY_VEC[count] << " s)" << endl;
                info_file << ">> Vizinhanca " << count << ": " << NEIGHBORHOOD_ADDED[count] << " added - " << NEIGHBORHOOD_QTY_VEC[count] << " (" << NEIGHBORHOOD_TIME[count] /  (double) NEIGHBORHOOD_QTY_VEC[count] << " s)" << "\n";
            }
        }

        cout      << ">> Subpath3: " << sub_path3_qty << " (" << sub_path3_time /  (double) sub_path3_qty << " s)" << endl;
        info_file << ">> Subpath3: " << sub_path3_qty << " (" << sub_path3_time /  (double) sub_path3_qty << " s)" << "\n";

        cout << ">> Average Final Charge: "             << average_final_charge <<
                " >> Standard Deviation Final Charge: " << dev_final_charge     <<
                " >> Best Final Charge: "               << max_final_charge     << endl;
        cout << ">> Average Time: "                     << average_time         <<
                " >> Standard Deviation Time: "         << dev_time             <<
                " >> Best Time: "                       << min_time             << endl;
        cout << ">> Average Consumption: "              << average_consumption  <<
                " >> Standard Deviation Consumption: "  << dev_consumption      <<
                " >> Best Consumption: "                << min_consumption     << endl;

        //START: Info file
        info_file << "\n------------------------------------------" << "\n";
        info_file << "\n";

        info_file << ">> Average Final Charge: "            << average_final_charge <<
                    " >> Standard Deviation Final Charge: " << dev_final_charge <<
                    " >> Best Final Charge: "               << max_final_charge << "\n";
        info_file << ">> Average Time: "                    << average_time <<
                    " >> Standard Deviation Time: "         << dev_time <<
                    " >> Best Time: "                       << min_time << "\n";
        info_file << ">> Average Consumption: "             << average_consumption <<
                    " >> Standard Deviation Consumption: "  << dev_consumption <<
                    " >> Best Consumption: "                << min_consumption << "\n";

        info_file.close();
        //END: Info file

        //START: Delete variables
        completed = false;
        valid     = false;

        for (int count = 0; count < NEIGHBORHOOD_QTY_VEC.size(); count++) {
            NEIGHBORHOOD_QTY_VEC[count] = 0;
            NEIGHBORHOOD_TIME   [count] = 0;
            NEIGHBORHOOD_ADDED  [count] = 0;
        }

        sub_path3_qty  = 0;
        sub_path3_time = 0;

        UAV_delete_pool(pool_global);
        //END: Delete
    }    

//    vec.push_back(init_sol);

    return aggregate_STRUCT(num_exec, 3, base_file_name);

//    return aggregate_2(num_exec, 3, base_file_name);// a_solutions;
}
