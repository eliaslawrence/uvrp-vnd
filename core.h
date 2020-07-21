#ifndef CORE_H
#define CORE_H

#include "array_solution.h"
#include "pool_of_solutions.h"
#include "int_matrix.h"
#include "matrix.h"
#include "parameters.h"
#include "pattern_table.h"
#include "solution_pair.h"
#include "array_objective_solution.h"
#include "a_star_node.h"
#include "array_path.h"
#include "uav.h"

#include <fstream> //FILE
#include <vector>

namespace fixedValues {
          enum STOPS { NOTHING = 0, DELIVER = 1, COLLECT = 2, CHARGE = 3, PROHIBITED = 4 }; // tipos de parada no mapa
    const int  POINTS_BY_STOP_POINT[] = { 0, 30, 30, 1 }; // qtd de pontos de parada para cada tipo: 0 - sem valor; 1 - DELIVER; 2 - COLLECT; 3 - CHARGE
}

class Core
{
private:    
//    std::vector< Array_Solution > a_solutions;
    vector<Array_Solution> a_solutions; // pointer

    Pool_of_Solutions pool_of_solutions;
    Pool_of_Solutions p_temp;

    Matrix * m_map;

    Parameters parameters;
    int        MAX_TIME                     = 10;
    double     MIN_VEL			            = 1;   // limite INFERIOR de velocidade
    double     MAX_VEL			            = 10;  // limite SUPERIOR de velocidade
    int        DISCRETIZATION               = 10;
    double     TIME_INTERVAL		        = 1;   // discretizacao: MAX_TIME/TIME_INTERVAL => numero de pontos de posicoes para cada coleta e entrega    

    // CHARGE settings
    int        MIN_CHARGE_RATE	            = 0;   // limite INFERIOR de CARGA
    int        MAX_CHARGE_RATE	            = 100; // limite SUPERIOR de CARGA
    int        BATTERY_RECHARGE             = 100; //10 valor da recarga da bateria do drone
    int        MAX_BATTERY_CHARGE           = 100; //20 valor da carga da bateria do drone cheia
    int        initial_battery_charge       = 100; //10 nivel de bateria do drone
    int        CONSUMPTION_BATTERY          = 1;   // consumo de bateria do drone por intervalo de tempo: a cada 1 TIME_INTERVAL o nivel de bateria cai de 1 CONSUMPTION_BATTERY
    double     FIXED_BATTERY_CONSUMPTION    = 0.1;
    double     VARIABLE_BATTERY_CONSUMPTION = 0.1;//0.05;
    int        TIME_BATTERY_RECHARGE        = 5;
    //

    // GRID Default
    int        MATRIX_NUMBER_COLUMNS        = 50;  // numero de colunas da matriz que representa o mapa
    int        MATRIX_NUMBER_LINES          = 50;  // numero de linhas da matriz que representa o mapa
    int        PCT_POINTS[5]                = { 0, 10, 10, 1, 0 };
    int        QTY_POINTS[5]                = { 0, 0, 0, 0, 0 };
//    int        QTY_POINTS[5]                = { 0, 10, 10, 10, 0 }; // qtd de pontos de parada para cada tipo: 0 - sem valor; 1 - DELIVER; 2 - COLLECT; 3 - CHARGE; 4 - PROHIBITED
    int        origin_x                     = 0;
    int        origin_y                     = 0;
    //

    // Heuristic Settings
    int         MAX_TIME_ALG                 = 420;
    int         NUMBER_ITERATIONS            = 5;
    int         MAX_ITERATIONS               = 50;
    int         STABLE_ITERATIONS_ACCEPTABLE = 20;

    int qty_clients                          = 51;

    int            NEIGHBORHOOD_QTY          = 13;
    vector<int>    NEIGHBORHOOD_QTY_VEC;
    vector<double> NEIGHBORHOOD_TIME   ;
    vector<int>    NEIGHBORHOOD_ADDED  ;

    int         GRASP_FACTOR                 = 2;
    bool        swap_test                    = false;
    bool        swap_3_available             = false;
    bool        swap_4_available             = false;
    bool        swap_5_available             = false;
    bool        swap_6_available             = false;
    bool        pre_processing               = true;

    int         NUMBER_ROUTES                = 1; // Multidrone

    // Clients already visited in an old tour
    vector<vector<int>> clients_visited;
    int         pct_clients_visited          = 0;//10;
    //

    Array_Path * best_paths;
    string global_file_name;

//    vector<int> NEIGHBORHOOD {0};
    //int    NEIGHBORHOOD[1]              = { 0 }; //, 1, 2, 3, 4, 5, 6 }; // array de vizinhanca:
                                                                    /*0 - SWAP;
                                                                    1 - RANDOM_SUB_PATH;
                                                                    2 - CHANGE_FINAL;
                                                                    3 - CHANGE_VEL(-);
                                                                    4 - CHANGE_VEL(+);
                                                                    5 - CHANGE_CHARGE_RATE(-);
                                                                    6 - CHANGE_CHARGE_RATE(+);
                                                                    2 - CROSSOVER;*/
//    vector<int> NEIGHBORHOOD_QTY     { 0, 0, 0, 0, 0, 0, 0 };// int  NEIGHBORHOOD_QTY[7]          = { 0, 0, 0, 0, 0, 0, 0 };
//    vector<int> NEIGHBORHOOD_MAX     { 0, 0, 0, 0, 0, 0, 0 };// int  NEIGHBORHOOD_MAX[7]          = { 0, 0, 0, 0, 0, 0, 0 };
//    vector<bool> NEIGHBORHOOD_BOOL   { false, false, false, false, false, false };// bool NEIGHBORHOOD_BOOL[7]         = { false, false, false, false, false, false };
//    vector<bool> NEIGHBORHOOD_STABLE { false, false, false, false, false, false };// bool NEIGHBORHOOD_STABLE[7]       = { false, false, false, false, false, false };    

    double sub_path3_time = 0;
    double sub_path3_qty  = 0;

    bool valid                        = false;
    bool completed                    = false;
    int iteration_completed           = 0;
    vector<Solution_Pair> client_points;
    vector<Solution_Pair> recharge_points;
    vector<Solution_Pair> table;
    vector<Solution_Pair> pattern;
    vector<Solution_Pair> pattern_vel;
    vector<int> point_qty;

    vector<vector<int>> neighbors;
    vector<vector<int>> further_neighbors;
    int neighbors_qty = 4;

    Array_Solution  evaluate                  (Array_Solution a_solution, Matrix * m_map);
    Route           evaluate_loop             (Route route, Matrix * m_map, double current_time, int current_pos, double battery_charge, int points);
    Array_Solution  neighbor                  (int pos, vector<Array_Solution> solutions, int type);
    Array_Solution  swap                      (Array_Solution a_solution, int number_lines, int number_columns);
    Array_Solution  change_final              (Array_Solution a_solution, int number_lines, int number_columns);
//    Array_Solution * change_vel                (Array_Solution * a_solution, int value);
    Array_Solution change_charge_rate        (Array_Solution a_solution, int value);
    Array_Solution random_sub_path           (Array_Solution a_solution, int number_lines, int number_columns);
    Array_Solution sub_path                  (Array_Solution a_solution, int start, int end, int number_lines, int number_columns);
    Array_Solution increment                 (Array_Solution a_solution, int number_lines, int number_columns, int new_size);
    Array_Solution perturb                   (Array_Solution a_solution, int number_lines, int number_columns, int new_size);
    Array_Solution perturb_loop              (int current_pos, int max_pos, int e_old_x, int e_old_y, int num_lines, int num_columns, Array_Solution solution);
    Array_Solution initial_solution          (int num_lines, int num_columns, Array_Solution solution);    
    Array_Solution initial_solution2         (int num_lines, int num_columns, Array_Solution solution);
    Array_Solution initial_solution_loop     (int current_time, int x, int y, int num_lines, int num_columns, Array_Solution solution);
    Pattern_Table  copy_pattern_table        (Pattern_Table other);
    bool           completed_path            (Array_Solution a_solution);
    bool           is_stable                 (int acceptable, int a_max[]);
    bool           is_stable                 (bool n_stable[]);
    bool           can_change_charge_rate    (Route route, int pos, int value);
    bool           can_change_vel            (Route route, int pos, int value);

    Matrix *       initialize_mtrx                 (Matrix * m);
    Matrix *       initialize_mtrx2                (Matrix * m, string instanceName, string clientsVisitedFileName);
    Matrix *       initialize_mtrx_load_test_files (Matrix * m, string instanceName, int pct_prohibited);
    Matrix *       fill_mtrx                       (Matrix * m);

    void           get_clients_visited       (string instanceName, string clientsVisitedFileName);
    void           set_origin_point          (Matrix * m, string originFileName);

    void           clean_neighborhood_qty    ();

    void           sub_path2                 (Array_Solution a_solution, int start, int end, int number_lines, int number_columns);
    Route          sub_path3                 (Route route, int start, int end);

    Array_Solution swap2                     (Array_Solution a_solution, int number_lines, int number_columns);
    Array_Solution fill_with_recharge_points (Array_Solution a_solution, int average_distance);
    Array_Solution closest_recharge          (int pos, vector<Array_Solution> solutions, int average_distance);
    Array_Solution remove_recharge           (int pos, vector<Array_Solution> solutions);
    Array_Solution remove_repeated           (int pos, vector<Array_Solution> solutions);
    Array_Solution remove_client             (int pos, vector<Array_Solution> solutions);
    Array_Solution change_charge_rate2       (int pos, vector<Array_Solution> solutions, int value);
    Array_Solution change_vel2               (int pos, vector<Array_Solution> solutions, int value);
    Array_Solution change_vel                (int pos, vector<Array_Solution> solutions, int value);
    Array_Solution change_vel_3              (int pos, vector<Array_Solution> solutions, int value);
    Array_Solution swap3                     (int pos, vector<Array_Solution> solutions);
    Array_Solution swap4                     (int pos, vector<Array_Solution> solutions);
    Array_Solution swap5                     (int pos, vector<Array_Solution> solutions);
    Array_Solution swap6                     (int pos, vector<Array_Solution> solutions);
    Array_Solution validate                  (Array_Solution a_solution);
    Array_Solution fill_with_clients         ();

    // Multidrone
    Array_Solution swap4_multi               (int pos, vector<Array_Solution> solutions);
    Array_Solution swap6_multi               (int pos, vector<Array_Solution> solutions);
    Route          a_star                    (Matrix * map, Stop_Point start, Stop_Point end);

    //STRUCT    
    void               initial_solution (Solution * solution);
    void               sub_path         (UAV* uav, int start, int end);
    void               a_star           (UAV * path, Matrix * map, Stop_Point start, Stop_Point end);
    void               evaluate_loop    (UAV * route, Matrix * m_map, double current_time, Movement * current_move, double battery_charge);
    void               evaluate         (Solution * a_solution, Matrix * m_map);
    void               set_paths_STRUCT (Matrix * map, vector<Solution_Pair> client_points, vector<Solution_Pair> recharge_points, string directory_name);


    bool               neighbor_STRUCT            (Pool * pool, Solution * neighbor_sol, int type);

    bool               completed_path             (Solution * a_solution);

    bool               shift_1_1_complete         (Pool * pool, Solution * neighbor_sol);
    bool               shift_1_1_random           (Pool * pool, Solution * neighbor_sol);
    bool               shift_1_1_nearest          (Pool * pool, Solution * neighbor_sol);
    bool               shift_1_1_furthest         (Pool * pool, Solution * neighbor_sol);
    bool               swap_1_1                   (Pool * pool, Solution * neighbor_sol);
    bool               swap_1_0                   (Pool * pool, Solution * neighbor_sol);
    bool               remove_repeated_STRUCT     (Pool * pool, Solution * neighbor_sol);
    bool               change_vel_random          (Pool * pool, Solution * neighbor_sol, int value);
    bool               change_vel_between_recharge(Pool * pool, Solution * neighbor_sol, int value);
    bool               change_charge_rate_STRUCT  (Pool * pool, Solution * neighbor_sol, int value);
    bool               remove_recharge_STRUCT     (Pool * pool, Solution * neighbor_sol);
    bool               nearest_recharge           (Pool * pool, Solution * neighbor_sol);

    void         set_paths (Matrix * map, vector<Solution_Pair> client_points, vector<Solution_Pair> recharge_points, string directory_name);
    Array_Path * get_paths (string directory_name);

    void generate_neighbors         (int neighbors_qty);
    void generate_further_neighbors (int neighbors_qty);
    void print_neighbors            ();

    Array_Objective_Solution * aggregate (int num_exec, int num_obj, string directory_name);   

    vector<Array_Solution> aggregate_2 (int num_exec, int num_obj, string directory_name);

public:
    Core();

    vector<Array_Solution> generate_solutions(int type, string file_name = "");

    vector<Array_Solution> neural_network   (string instance_file_name, string directory_name, int time_limit, int pct_prohibited);
    vector<Array_Solution> test_i_vns       (int time_limit, int num_exec, string instance_name, string directory_name, int pct_prohibited);
    vector<Array_Solution> test_vns         (int time_limit, int num_exec, string instance_name, string directory_name, int pct_prohibited);    

    //STRUCT
    vector<Solution *> test_i_vns_STRUCT(int time_limit, int num_exec, string instance_name, string directory_name, int pct_prohibited, bool load_file, string load_file_name);
    vector<Solution *> show_solutions   (int num_exec, int num_obj, string directory_name);
    vector<Solution *> test             (int type, int method, string instance_file_name, string directory_name, int time_limit, int num_exec, int pct_prohibited, bool load_file, string load_file_name);
    vector<Solution *> aggregate_STRUCT (int num_exec, int num_obj, string directory_name);

    void evaluate_methods (string directory_name, int num_exec);

    //GET e SET
    void set_rows                  (int r);
    void set_max_time			   (int m_t);
    void set_time_interval		   (int t_i);
    void set_matrix_number_columns (int n_c);
    void set_matrix_number_lines   (int n_l);
    void set_battery_recharge      (int b_r);
    void set_max_battery_charge    (int m_b);
    void set_initial_battery_charge(int i_b);
    void set_consumption_battery   (int c_b);
    void set_qty_points            (int q_p);
    void set_number_iterations     (int n_i);    
    int  get_number_iterations     ();    

    //GET
    vector<Array_Solution>  get_solutions(); //    std::vector< Array_Solution > get_solutions();
    Matrix *                  get_map();
    Pool_of_Solutions         get_pool_of_solutions();

};

#endif // CORE_H
