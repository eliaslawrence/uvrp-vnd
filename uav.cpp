#include "uav.h"

/* INIT */
void UAV_init_pool(Pool *p){
    p->size = 0;
    p->capacity = POOL_INIT_CAPACITY;
    p->solutions = (Solution**) malloc(p->capacity*sizeof(Solution*));
}

void UAV_init_solution(Solution *s){

    s->size           = 0;
    s->capacity       = SOLUTION_INIT_CAPACITY;
    s->iteration      = 0;
    s->num_objectives = NUM_OBJECTIVES;
    s->valid          = true;
    s->complete       = false;

    s->objectives     = (double*) malloc(s->num_objectives*sizeof(double));

    int i;
    for (i = 0; i < s->num_objectives; i++) {
        s->objectives[i] = 0;
    }

    s->neighborhoods_executed = (bool*) malloc(NUM_NEIGHBORHOODS*sizeof(bool));

    for (i = 0; i < NUM_NEIGHBORHOODS; i++) {
        s->neighborhoods_executed[i] = false;
    }

    s->uavs           = (UAV**) malloc(s->capacity*sizeof(UAV*));
}

void UAV_init_uav(UAV *u){

    u->size           = 0                ;
    u->capacity       = UAV_INIT_CAPACITY;
    u->num_objectives = NUM_OBJECTIVES   ;
    u->valid          = true             ;
    u->complete       = false            ;

    u->objectives     = (double*) malloc(u->num_objectives*sizeof(double));

    int i;
    for (i = 0; i < u->num_objectives; i++) {
        u->objectives[i] = 0;
    }

    u->route          = NULL;//(Movement*) malloc(u->capacity*sizeof(Movement));

}

/* COPY */
void UAV_copy_uav(UAV * original, UAV * copy){

    copy->size           = 0                      ;
    copy->capacity       = original->capacity       ;
    copy->num_objectives = original->num_objectives ;
    copy->valid          = original->valid          ;
    copy->complete       = original->complete       ;

    // Objectives
    int i;
    for (i = 0; i < copy->num_objectives; i++) {
        copy->objectives[i] = original->objectives[i];
    }

    // Route
    Movement * current = original->route;
    while(current != NULL){
        UAV_push_movement(current, copy);
        current = current->next;
    }

}

void UAV_copy_solution(Solution * original, Solution * copy){

    copy->size           = 0                      ;
    copy->capacity       = original->capacity       ;
    copy->iteration      = original->iteration      ;
    copy->num_objectives = original->num_objectives ;
    copy->valid          = original->valid          ;
    copy->complete       = original->complete       ;

    // Objectives
    int i;
    for (i = 0; i < copy->num_objectives; i++) {
        copy->objectives[i] = original->objectives[i];
    }

    for (i = 0; i < NUM_NEIGHBORHOODS; i++) {
        copy->neighborhoods_executed[i] = original->neighborhoods_executed[i];
    }

    // UAVs
    for (i = 0; i < original->size; i++) {
        UAV * current = new UAV;
        UAV_init_uav(current);
        UAV_copy_uav(original->uavs[i], current);

        UAV_add_uav(copy, current);
    }

}

/* ADD */

#ifndef MONO
bool UAV_add_solution(Pool *p, Solution *s){

    for (int i = 0; i < p->size; i++)
    {
        if(s->equals(p->solutions[i])){
            return false;
        }else if(s->dominates(p->solutions[i])){
            UAV_remove_solution(p, i);
            i--;
        }else if(p->solutions[i]->dominates(s)){
            return false;
        }
    }

    if(p->size < p->capacity){
        p->solutions[p->size] = new Solution;
        UAV_init_solution(p->solutions[p->size]);

        UAV_copy_solution(s, p->solutions[p->size]);

        p->size++;

        return true;
    }

    return false;

}
#endif

#ifdef MONO
bool UAV_add_solution(Pool *p, Solution *s){

    for (int i = 0; i < p->size; i++)
    {
        if(s->equals(p->solutions[i])){
            return false;
        }else if(s->dominates(p->solutions[i])){
            int end = p->size - 1;
            if(p->size < p->capacity){
                p->solutions[p->size] = new Solution;
                UAV_init_solution(p->solutions[p->size]);
                end = p->size;
                p->size++;
            }

            for (int j = end; j > i + 1; --j) {
                p->solutions[j] = p->solutions[j - 1];
            }

            UAV_copy_solution(s, p->solutions[i]);

            return true;
        }
    }

    if(p->size < p->capacity){
        p->solutions[p->size] = new Solution;
        UAV_init_solution(p->solutions[p->size]);

        UAV_copy_solution(s, p->solutions[p->size]);

        p->size++;

        return true;
    }

    return false;

}
#endif



void UAV_add_uav(Solution *s, UAV *u){
    if(s->capacity != s->size){
        s->uavs[s->size++] = u;
    }
}

void UAV_push_movement(Movement * m, UAV *u){
    UAV_push_movement(m->x, m->y, m->vel, m->charge_rate, m->battery_charge, u);
}

void UAV_push_movement(int x, int y, UAV *u){
    UAV_push_movement(x, y, 10, 100, 100.0, u);
}

void UAV_push_movement(int x, int y, double vel, int charge_rate, double battery_charge, UAV *u){

    if(u->route == NULL){
        u->route = (Movement*) malloc(sizeof(Movement));

        u->route->x              = x;
        u->route->y              = y;
        u->route->vel            = vel;
        u->route->charge_rate    = charge_rate;
        u->route->battery_charge = battery_charge;
        u->route->next           = NULL;
        u->route->prev           = NULL;
    } else {
        Movement* current_movement = u->route;

        while(current_movement->next != NULL){
            current_movement = current_movement->next;
        }

        current_movement->next = (Movement*) malloc(sizeof(Movement));

        current_movement->next->x              = x;
        current_movement->next->y              = y;
        current_movement->next->vel            = vel;
        current_movement->next->charge_rate    = charge_rate;
        current_movement->next->battery_charge = battery_charge;
        current_movement->next->next           = NULL;
        current_movement->next->prev           = current_movement;
    }

    u->size++;

}

void UAV_insert_movement(UAV *u, Movement * m, int pos){
    UAV_insert_movement(u, m->x, m->y, m->vel, m->charge_rate, m->battery_charge, pos);
}

void UAV_insert_movement(UAV *u, int x, int y, int pos){
    UAV_insert_movement(u, x, y, 10, 100, 100.0, pos);
}

void UAV_insert_movement(UAV *u, int x, int y, double vel, int charge_rate, double battery_charge, int pos){

    if(pos > u->size){
        return;
    }

    Movement* current_movement    = u->route;
    Movement* temp_movement       = NULL;

    int i;

    if(pos == 0){

        u->route = (Movement*) malloc(sizeof(Movement));

        u->route->x              = x;
        u->route->y              = y;
        u->route->vel            = vel;
        u->route->charge_rate    = charge_rate;
        u->route->battery_charge = battery_charge;
        u->route->prev           = NULL;
        u->route->next           = current_movement;
        u->route->next->prev     = u->route;

    } else {
        for (i = 0; i < pos - 1; i++) {
            if(current_movement->next == NULL){
                return;
            }

            current_movement = current_movement->next;
        }

        temp_movement                          = current_movement->next;
        current_movement->next                 = (Movement*) malloc(sizeof(Movement));
        current_movement->next->x              = x;
        current_movement->next->y              = y;
        current_movement->next->vel            = vel;
        current_movement->next->charge_rate    = charge_rate;
        current_movement->next->battery_charge = battery_charge;

        current_movement->next->prev           = current_movement;
        current_movement->next->next           = temp_movement;

        if(current_movement->next->next != NULL){            
            current_movement->next->next->prev = current_movement->next;
        }
    }

    u->size++;

}

void UAV_insert_subpath(UAV *u, UAV *u2, int pos){

    if(pos > u->size){
        return;
    }

    if(u2->size == 0){
        return;
    }

    Movement* current_movement    = u->route;
    Movement* u2_current_movement = u2->route;
    Movement* temp_movement       = NULL;

    int i;

    if(pos == 0){

        u->route = u2->route;

        for (i = 0; i < u2->size - 1; i++) {
            if(u2_current_movement->next == NULL){
                return;
            }

            u2_current_movement = u2_current_movement->next;
        }

        u2_current_movement->next = current_movement;
        current_movement->prev    = u2_current_movement;

    } else {
        for (i = 0; i < pos - 1; i++) {
            if(current_movement->next == NULL){
                return;
            }

            current_movement = current_movement->next;
        }

        temp_movement                = current_movement->next;
        current_movement->next       = u2->route;
        current_movement->next->prev = current_movement;

        for (i = 0; i < u2->size - 1; i++) {
            if(u2_current_movement->next == NULL){
                return;
            }

            u2_current_movement = u2_current_movement->next;
        }

        u2_current_movement->next       = temp_movement;
        u2_current_movement->next->prev = u2_current_movement;
    }

    u->size = u->size + u2->size;

}

void UAV_merge_uav_without_start_end(UAV *u, UAV *u2, int pos){

    // Remove first movement
    UAV_pop_movement(u2);
    // Remove last movement
    UAV_remove_last_movement(u2);

    UAV_insert_subpath(u, u2, pos);
}

/* REMOVE */
void UAV_pop_movement(UAV *u){

    if(u->route == NULL){
        return;
    }

    Movement* next_movement = u->route->next;
    free(u->route);

    u->route       = next_movement;

    if(u->route != NULL){
        u->route->prev = NULL;
    }

    u->size--;
}

void UAV_remove_last_movement(UAV *u){

    UAV_remove_movement(u, u->size - 1);

}

void UAV_remove_movement(UAV *u, int pos){

    if(pos == 0){
        return UAV_pop_movement(u);
    }

    Movement* current_movement = u->route;
    Movement* temp_movement    = NULL;

    int i;
    for (i = 0; i < pos - 1; i++) {
        if(current_movement->next == NULL){
            return;
        }

        current_movement = current_movement->next;
    }

    temp_movement                = current_movement->next;
    current_movement->next       = temp_movement->next;

    if(current_movement->next != NULL){
        current_movement->next->prev = current_movement;
    }

    free(temp_movement);

    u->size--;

}

void UAV_remove_subpath(UAV *u, int start, int end){

    if(start < 0 || start > u->size || end > u->size || end < start){
        return;
    }

    int i;
    if(start == 0){
        for (i = 0; i < end + 1; i++) {
            UAV_pop_movement(u);
        }
        return;
    }

    Movement* current_movement = u->route;
    Movement* temp_movement    = NULL;

    for (i = 0; i < start - 1; i++) {
        if(current_movement->next == NULL){
            return;
        }

        current_movement = current_movement->next;
    }

    for (i; i < end; i++) {
        if(current_movement->next == NULL){
            return;
        }

        temp_movement                = current_movement->next;
        current_movement->next       = temp_movement->next;
        free(temp_movement);

        u->size--;
    }

    if(current_movement->next != NULL){
        current_movement->next->prev = current_movement;
    }

}

void UAV_delete_uav(UAV* u){
    while(u->route != NULL){
        UAV_pop_movement(u);
    }
    free(u->objectives);
    free(u);
}

void UAV_remove_uav(Solution *s, int pos){

    if(pos < 0 || pos > s->size){
        return;
    }

    UAV_delete_uav(s->uavs[pos]);
    s->uavs[pos] = NULL;

    int i;
    for (i = pos; i < s->size - 1; i++) {
        s->uavs[i] = s->uavs[i + 1];
        s->uavs[i + 1] = NULL;
    }

    s->size--;
}

void UAV_delete_solution(Solution *s){
    int i;
    for (i = 0; i < s->size; i++) {
        UAV_delete_uav(s->uavs[i]);
    }

    free(s->uavs);
    free(s->objectives);
    free(s->neighborhoods_executed);
    free(s);
}

void UAV_remove_solution(Pool *p, int pos){

    if(pos < 0 || pos > p->size){
        return;
    }

    UAV_delete_solution(p->solutions[pos]);
    p->solutions[pos] = NULL;

    int i;
    for (i = pos; i < p->size - 1; i++) {
        p->solutions[i] = p->solutions[i + 1];
//        UAV_delete_solution(p->solutions[i + 1]);
        p->solutions[i + 1] = NULL;
    }

    p->size--;
}

void UAV_delete_pool(Pool *p){
    int i;
    for (i = 0; i < p->size; i++) {
        UAV_delete_solution(p->solutions[i]);
    }

    free(p->solutions);
    free(p);
}


/* FILE */

void UAV_to_file_uav(UAV* u, string file_name)
{
    ofstream array_file;
    array_file.open (file_name);

    array_file << u->size << "\n";
    array_file << u->num_objectives << "\n";

    array_file << u->valid << "\n";

    int i;
    for (i = 0; i < u->num_objectives; i++) {
        array_file << u->objectives[i] << " ";
    }
    array_file << "\n";

    Movement * current = u->route;
    while(current != NULL){
        array_file << current->x              << " " <<
                      current->y              << " " <<
                      current->vel            << " " <<
                      current->charge_rate    << " " <<
                      current->battery_charge << "\n";

        current = current->next;
    }

    array_file.close();
}

void UAV_to_file_solution(Solution * s, string file_name)
{
    ofstream array_file;
    array_file.open (file_name);

    array_file << s->size           << "\n";
    array_file << s->num_objectives << "\n";
    array_file << s->iteration      << "\n";
    array_file << s->valid          << "\n";

    int i;
    for (i = 0; i < s->num_objectives; i++) {
        array_file << s->objectives[i] << " ";
    }
    array_file << "\n";

    array_file.close();

    size_t found = file_name.find_last_of("/");
    string solution_name = file_name.substr(found, file_name.size() - 1);
    file_name.erase (found, file_name.size() - 1);
    string routes_file_name = file_name;
    routes_file_name.append("/routes");

    for (int pos_route = 0; pos_route < s->get_size(); pos_route++) {
        string route_file_name = routes_file_name + solution_name + "_route_" + std::to_string(pos_route);
        UAV_to_file_uav(s->uavs[pos_route], route_file_name);
    }
}

/* OTHER */
void UAV_invert_uav(UAV * uav)
{
    int size = uav->size;

    // Route
    int i;
    for (i = size - 1; i >= 0; i--) {
        UAV_push_movement(uav->get_movement(i), uav);
    }

    for (i = 0; i < size; i++) {
        UAV_pop_movement(uav);
    }

}

/* TESTS */

void UAV_test_movement(Movement *m){
    int i = 0;
}

void UAV_test_objective(double o){
    int i = 0;
}

void UAV_test_uav(UAV *u){
    Movement* current_movement = u->route;

    while(current_movement != NULL){
        UAV_test_movement(current_movement);
        current_movement = current_movement->next;
    }

    int i = 0;
    for (i; i < u->num_objectives; i++) {
        UAV_test_objective(u->objectives[i]);
    }
}

bool UAV_test_continuity_uav(UAV *u){
    Movement* current_movement = u->route;

    int i = 0;
    while(current_movement != NULL){
        current_movement = current_movement->next;
        i++;
    }

    return i == u->size;
}

void UAV_test_solution(Solution *s){
    int i = 0;
    for (i; i < s->size; i++) {
        UAV_test_uav(s->uavs[i]);
    }
}

void UAV_test_pool(Pool *p){
    int i = 0;
    for (i; i < p->size; i++) {
        UAV_test_solution(p->solutions[i]);
    }
}
