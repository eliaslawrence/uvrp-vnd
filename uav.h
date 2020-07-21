#ifndef UAV_H
#define UAV_H

#include "neural_network.h"
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string>
#include <iostream>
#include <fstream>

using namespace std;

#define TAXA_APRENDIZADO        (0.1)
#define TAXA_PESO_INICIAL       (1.0)

#define SOLUTION_INIT_CAPACITY  2    // Number of drones
#define UAV_INIT_CAPACITY       2000
#define NUM_OBJECTIVES          4
#define NUM_NEIGHBORHOODS       9
#define MAX_VELOCITY            10
#define MIN_VELOCITY            1
#define MAX_CHARGE_RT           100
#define MIN_CHARGE_RT           0

//#define MONO
#define MONO_5

#ifndef MONO
#define POOL_INIT_CAPACITY      5 // multiobjective
#endif

#ifdef MONO
#define POOL_INIT_CAPACITY      1 // mono-objective
#endif

typedef struct Movement
{
    int    x, y;    
    int    charge_rate;
    double vel;
    double battery_charge;

    Movement * next;
    Movement * prev;

    int get_x(){
        return x;
    }

    int get_y(){
        return y;
    }

    void change_vel(int value){
        double new_vel = this->vel + value;

        new_vel = new_vel ? new_vel <= MAX_VELOCITY : MAX_VELOCITY;
        new_vel = new_vel ? new_vel >= MIN_VELOCITY : MIN_VELOCITY;

        this->vel = new_vel;
    }

    void change_charge_rate(int value){
        int new_charge_rate = this->charge_rate + value;

        new_charge_rate = new_charge_rate ? new_charge_rate <= MAX_CHARGE_RT : MAX_CHARGE_RT;
        new_charge_rate = new_charge_rate ? new_charge_rate >= MIN_CHARGE_RT : MIN_CHARGE_RT;

        this->charge_rate = new_charge_rate;
    }

    bool is_at_same_position(Movement *m2){
        return (m2->get_x() == this->x && m2->get_y() == this->y);
    }

    bool equals(Movement *m2)
    {
        return (m2->x == this->x && m2->y == this->y && m2->vel == this->vel && m2->charge_rate == this->charge_rate);
    }

    string to_string()
    {
        string resp;

        resp.append("(");
        resp.append(std::to_string(x));
        resp.append(",");
        resp.append(std::to_string(y));
        resp.append(") ");

        return resp;
    }

}   Movement;

typedef struct UAV
{
    int             size;
    int             capacity;
    int             num_objectives;

    bool            valid;
    bool            complete;

    double   *      objectives;

    Movement *      route;

    int get_size(){
        return size;
    }

    int get_capacity(){
        return capacity;
    }

    Movement * get_movement(int index){
        if(index > size || index < 0){
            return NULL;
        }

        Movement * current = route;

        int i;
        for (i = 0; i < index; i++) {
            current = current->next;
        }

        return current;
    }

    string to_string()
    {
        // Imprime o caminho percorrido pelo drone

        string solution;
        solution.append("{ ");

        Movement * current = route;

        while(current != NULL){
            solution.append(current->to_string());
            current = current->next;
        }

        solution.append("}");

        return solution;
    }

    bool equals(UAV * u2) /* verify if routes are the same */
    {
        if(this->size != u2->size){
            return false;
        }

        // Run through all the route
        Movement * current    = route;
        Movement * u2_current = u2->route;

        while(current != NULL){
            // Compare the point from both routes
            if(u2_current == NULL){
                return false;
            } else {
                if (!current->equals(u2_current)) {
                    return false;
                }
                current    = current->next;
                u2_current = u2_current->next;
            }

        }

        return true;
    }

}   UAV;

typedef struct Solution
{
    int             size;
    int             capacity;
    int             iteration;
    int             num_objectives;

    bool            valid;
    bool            complete;

    double *        objectives;

    bool   *        neighborhoods_executed;

    UAV    **       uavs;

    int get_size(){
        return size;
    }

    int get_capacity(){
        return capacity;
    }

    string to_string()
    {
        string solution;

        for (int i = 0; i < this->get_size(); i++) {
            solution.append(uavs[i]->to_string());
            solution.append("\n");
        }

        return solution;
    }

    void update_values() /* update objective values according to its routes */ // pointer
    {

        int    qty_clients  = 0;
        double final_charge = 0,
               consumption  = 0;

        for (int pos_route = 0; pos_route < this->size; pos_route++) {
             qty_clients  += this->uavs[pos_route]->objectives[0]; // Add the counting of clients attended by each route
//             final_charge += this->uavs[pos_route]->objectives[1]; // Add the charge at the final of each route
             consumption  += this->uavs[pos_route]->objectives[3]; // Add the consumption of each route
        }

        this->objectives[0] = qty_clients ;
//        this->objectives[1] = final_charge;
        this->objectives[3] = consumption ;

        // Get the minimum final charge by one vehicle
        int min_final_charge  = 100,
            temp_final_charge = 0;

        for (int pos_route = 0; pos_route < this->get_size(); pos_route++) {
             temp_final_charge = this->uavs[pos_route]->objectives[1];

             if(temp_final_charge < min_final_charge){
                 min_final_charge = temp_final_charge;
             }
        }

        this->objectives[1] = min_final_charge;

        // Get the maximum time spent by one vehicle
        int max_time  = 0,
            temp_time = 0;

        this->valid = true;

        for (int pos_route = 0; pos_route < this->get_size(); pos_route++) {
             temp_time = (-1) * this->uavs[pos_route]->objectives[2];

             if(temp_time > max_time){
                 max_time = temp_time;
             }                          

             if(!this->uavs[pos_route]->valid){
                this->valid = false;
             }
        }

        // The max_time is set as the final time of the solution
        max_time = max_time * (-1);

        this->objectives[2] = max_time;
    }

    bool equals(Solution * s2) /* verifica se solucao domina outra solucao */ // pointer
    {
        if(this->size != s2->size){
            return false;
        }

        // Percorre todo array
        for (int i = 0; i < this->size; i++)
        {
            if (!this->uavs[i]->equals(s2->uavs[i])) {
                return false;
            }
        }
        return true;
    }

    bool dominates(Solution * s2) /* verifica se solucao domina outra solucao */
    {
        if(!this->valid && s2->valid){
            return false;
        }

        if(this->valid && !s2->valid){
            return true;
        }

        #ifndef MONO

        bool greaterThan = false;

        // Percorre todo array
        for (int i = 0; i < num_objectives; i++)
        {
            //Compara o ponto de referencia com os pares ordenados do array
            if (this->objectives[i] < s2->objectives[i]) {
                return false;
            }else if(this->objectives[i] > s2->objectives[i]){
                greaterThan = true;
            }
        }

        return greaterThan;

        #endif

        #ifdef MONO

        double sum1 = 0;
        double sum2 = 0;

        #ifndef MONO_5
        sum1 = this->objectives[1] + this->objectives[2] + 2*this->objectives[3];
        sum2 =   s2->objectives[1] +   s2->objectives[2] +   2*s2->objectives[3];
        #endif

        #ifdef MONO_5
        sum1 = this->objectives[2] + this->objectives[3] + 5*this->objectives[1];
        sum2 =   s2->objectives[2] +   s2->objectives[3] +   5*s2->objectives[1];
        #endif

        return sum1 > sum2;

        #endif
    }

    bool weakly_dominates(Solution * s2) /* verifica se solucao domina outra solucao */ // pointer
    {
        if(!this->valid && s2->valid){
            return false;
        }

        if(this->valid && !s2->valid){
            return true;
        }


        // Percorre todo array
        for (int i = 0; i < num_objectives; i++)
        {
            // Compara o ponto de referencia com os pares ordenados do array
            if (this->objectives[i] < s2->objectives[i]) {
                return false;
            }
        }

        return true;
    }

}   Solution;

typedef struct Pool
{
    int        size;
    int        capacity;

    Solution ** solutions;

    int get_size(){
        return size;
    }

    int get_capacity(){
        return capacity;
    }

} Pool;

/* INIT */
void UAV_init_pool                  (Pool * p)    ;
void UAV_init_solution              (Solution * s);
void UAV_init_uav                   (UAV * u)     ;

/* COPY */
void UAV_copy_uav                   (UAV * original, UAV * copy)          ;
void UAV_copy_solution              (Solution * original, Solution * copy);

/* ADD */
bool UAV_add_solution               (Pool * p, Solution * s);
void UAV_add_uav                    (Solution * s, UAV * u) ;

void UAV_push_movement              (Movement * m, UAV *u);
void UAV_push_movement              (int x, int y, UAV *u);
void UAV_push_movement              (int x, int y, double vel, int charge_rate, double battery_charge, UAV *u);

void UAV_insert_movement(UAV *u, Movement * m, int pos);
void UAV_insert_movement(UAV *u, int x, int y, int pos);
void UAV_insert_movement(UAV *u, int x, int y, double vel, int charge_rate, double battery_charge, int pos);

void UAV_insert_subpath             (UAV *u,UAV *u2, int pos)                                              ;
void UAV_merge_uav_without_start_end(UAV *u, UAV *u2, int pos)                                             ;

/* REMOVE */
void UAV_pop_movement        (UAV *u)                    ;
void UAV_remove_movement     (UAV *u, int pos)           ;
void UAV_remove_last_movement(UAV *u)                    ;
void UAV_remove_subpath      (UAV *u, int start, int end);
void UAV_remove_uav          (Solution *s, int pos)      ;
void UAV_remove_solution     (Pool *p, int pos)          ;

/* DELETE */
void UAV_delete_uav      (UAV * u)     ;
void UAV_delete_solution (Solution * s);
void UAV_delete_pool     (Pool * p)    ;

/* OTHER */
void UAV_invert_uav(UAV * u);

/* FILE */
void UAV_to_file_uav     (UAV * u, string file_name)      ;
void UAV_to_file_solution(Solution * s, string file_name);

/* TESTS */
void UAV_test_solution      (Solution *s);
void UAV_test_pool          (Pool *p)    ;
void UAV_test_movement      (Movement *m);
void UAV_test_uav           (UAV *u)     ;
bool UAV_test_continuity_uav(UAV *u)     ;


#endif // UAV_H
