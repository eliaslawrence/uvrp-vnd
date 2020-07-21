#include "route.h"
#include "core.h"
#include <iostream>
#include <limits>

using namespace fixedValues;

Route::Route()
{
    this->null = true;
    this->data1.assign(3000, Solution_Pair());
}


Route::~Route()
{
//    vector<Solution_Pair>().swap(data1);
//    v.swap(data1);
//    data1.clear();

//    delete [] data;
//    delete [] z;
//    z.clear(); // pointer
}

//Route::Route(Route copy) //pointer
//{
//    this->size           = copy.get_size();
//    this->points         = copy.points;
//    this->qty_points[0]  = copy.qty_points[0];
//    this->qty_points[1]  = copy.qty_points[1];
//    this->qty_points[2]  = copy.qty_points[2];
//    this->qty_points[3]  = copy.qty_points[3];
//    this->num_objectives = copy.num_objectives;

//    initialize_z(this->num_objectives);

//    Solution_Pair temp;

//    int i;
//    for (i = 0; i < copy.get_size(); i++) {
//        temp = copy.get_solution_pair(i);
//        this->add_solution_pair(temp);
//    }

//    for (i = 0; i < copy.num_objectives; i++) {
//        this->set_z(i, copy.z[i]);
//    }

//    this->set_iteration(copy.iteration);
//    this->set_valid(copy.valid);

//    this->set_qty_charge(copy.get_qty_charge());
//    this->set_qty_collect(copy.get_qty_collect());
//    this->set_qty_deliver(copy.get_qty_deliver());
//}


Route::Route(int s, int n_obj)
{
    this->size           = s;
    this->points         = 0;
    this->qty_points[0]  = 0;
    this->qty_points[1]  = 0;
    this->qty_points[2]  = 0;
    this->qty_points[3]  = 0;
    this->num_objectives = n_obj;


    initialize_z(this->num_objectives);

//    data = new Solution_Pair[s];
//    this->data1.assign(3000, Solution_Pair());
}

Route::Route(int s)
{
    this->size           = 0;
    this->points         = 0;
    this->qty_points[0]  = 0;
    this->qty_points[1]  = 0;
    this->qty_points[2]  = 0;
    this->qty_points[3]  = 0;
    this->num_objectives = 4;

    initialize_z(this->num_objectives);

//    data = new Solution_Pair[s];
//    this->data1.assign(s, Solution_Pair());
}

bool Route::is_null(){
    return this->null;
}

void Route::initialize_z(int n_obj) // pointer
{
    for (int i = 0; i < n_obj; i++) {
        z.push_back(0);
    }
}

vector<Solution_Pair> Route::get_data()
{
    return data1;
}

Solution_Pair Route::get_solution_pair(int pos)
{
    return data1[pos];//return data[pos];
}

void Route::remove_solution_pair(int pos)
{
    data1.erase(data1.begin() + pos);
    this->size--;
}

void Route::add_solution_pair(Solution_Pair e)
{
    data1.push_back(e.copy());
//    data1[size] = e.copy();
    this->size++;
}

void Route::add_solution_pair(int x, int y)
{
    Solution_Pair s_p(x, y);
    data1.push_back(s_p);
//    data1[size] = s_p;
    this->size++;
}

void Route::set_solution_pair(Solution_Pair e, int pos)
{
    data1[pos] = e.copy();//    data[pos] = e.copy();
}

void Route::set_solution_pair(int x, int y, int pos)
{
    Solution_Pair s_p(x, y);
    data1[pos] = s_p;//data[pos] = s_p;
}

void Route::insert_solution_pair(Solution_Pair e, int pos)
{
    this->data1.insert (this->data1.begin() + pos + 1, e);
}

void Route::insert_solution_pair(int x, int y, int pos)
{
    Solution_Pair e(x, y);
    this->data1.insert (this->data1.begin() + pos + 1, e);
}

int Route::get_size() {
    return this->size;//data1.size();//return size;
}

void Route::set_size(int s) {
    this->size = s;
}

int Route::get_time() {
    return time;
}

void Route::set_time(int t) {
    this->time = t;
}

int Route::get_qty_deliver(){
    return this->qty_points[STOPS::DELIVER];
}

void Route::set_qty_deliver(int q_d){
    this->qty_points[STOPS::DELIVER] = q_d;
}

int Route::get_qty_collect(){
    return this->qty_points[STOPS::COLLECT];
}

void Route::set_qty_collect(int q_c){
    this->qty_points[STOPS::COLLECT] = q_c;
}

int Route::get_qty_charge(){
    return this->qty_points[STOPS::CHARGE];
}

void Route::set_qty_charge(int q_c){
    this->qty_points[STOPS::CHARGE] = q_c;
}

double Route::get_points(){
//    double points = 1;
//    for (int i = 0; i < this->num_objectives; i++) {
//        points *= this->z[i];
//    }
//    return this->z[0] == 0 ? std::numeric_limits<int>::min() : (int) (this->z[3] / this->z[0]);// points;
    return (-1) * this->get_size() / this->z[0];//this->z[3] / this->z[0];// points;
}

vector<double> Route::get_z(){
    return this->z;
}

void Route::set_z(int pos, double value){
    this->z[pos] = value;
}

int Route::get_num_objectives(){
    return this->num_objectives;
}

int  Route::get_iteration(){
    return this->iteration;
}

void Route::set_iteration(int i){
    this->iteration = i;
}

bool Route::is_valid(){
    return this->valid;
}

void Route::set_valid(bool v){
    this->valid = v;
}

bool Route::is_complete(){
    return this->complete;
}

void Route::set_complete(bool c){
    this->complete = c;
}

string Route::get_validation(){
    return this->valid ? "true" : "false";
}

void Route::print()
{
    // Imprime o caminho percorrido pelo drone
    cout << "{ ";

    for (int i = 0; i < this->get_size(); i++) {
        data1[i].print();//data[i].print();
    }

    cout << "}\n";
}

string Route::to_string()
{
    // Imprime o caminho percorrido pelo drone

    string solution;
    solution.append("{ ");

    for (int i = 0; i < this->get_size(); i++) {
        solution.append(data1[i].to_string());//solution.append(data[i].to_string());
    }

    solution.append("}");

    return solution;
}

bool Route::contains(Solution_Pair e) /* verifica se array de pares ordenados possui determinado ponto */
{
    // Percorre todo array
    for (int i = 0; i < this->get_size(); i++)
    {
        // Compara o ponto de referencia com os pares ordenados do array
        if (get_solution_pair(i).equals(e)) {
            return true;
        }
    }
    return false;
}

Route Route::intersection(Route a_2) // pointer
{
    int biggest_size = this->get_size() > a_2.get_size() ? this->get_size() : a_2.get_size();

    Route a_intersection(biggest_size);

    Solution_Pair reference_element;

    int count = 0;

    for (int i = 0; i < a_2.get_size(); i++)
    {
        reference_element = a_2.get_solution_pair(i);

        for (int j = 0; j < this->get_size(); j++)
        {
            if (reference_element.equals(this->get_solution_pair(j))) {
                a_intersection.add_solution_pair(reference_element);

                count++;
                break;
            }
        }
    }

    a_intersection.set_size(count);

    return a_intersection;
}

void Route::to_file(string file_name)
{
    ofstream array_file;
    array_file.open (file_name);

    array_file << this->get_size() << "\n";
    array_file << this->num_objectives << "\n";

    array_file << this->iteration << "\n";
    array_file << this->valid << "\n";

    array_file << this->get_qty_charge() << "\n";
    array_file << this->get_qty_collect() << "\n";
    array_file << this->get_qty_deliver() << "\n";

    int i;
    for (i = 0; i < this->num_objectives; i++) {
        array_file << this->z[i] << " ";
    }
    array_file << "\n";

    Solution_Pair temp;
    for (i = 0; i < this->get_size(); i++) {
        temp = get_solution_pair(i);
        array_file << temp.get_x() << " " << temp.get_y() << " " << temp.get_vel() << " " << temp.get_charge_rate() << " " << temp.get_battery_charge() << "\n";
    }

    array_file.close();
}

Route Route::copy()
{
    Route copy = Route(this->num_objectives);
    Solution_Pair temp;

    int i;
    for (i = 0; i < this->get_size(); i++) {
        temp = this->get_solution_pair(i);
        copy.add_solution_pair(temp);//copy->set_solution_pair(temp, i);
    }

    for (i = 0; i < this->num_objectives; i++) {
        copy.set_z(i, this->z[i]);
    }

    copy.set_iteration(this->iteration);
    copy.set_valid(this->valid);

    copy.set_qty_charge(this->get_qty_charge());
    copy.set_qty_collect(this->get_qty_collect());
    copy.set_qty_deliver(this->get_qty_deliver());

    return copy;
}

Route Route::invert() // pointer
{
    Route copy(this->get_size(), this->num_objectives);
    Solution_Pair temp;

    int i;
    for (i = this->get_size() - 1; i >= 0; i--) {
        temp = this->get_solution_pair(i);
        copy.add_solution_pair(temp);//copy->set_solution_pair(temp, i);
    }

    for (i = 0; i < this->num_objectives; i++) {
        copy.set_z(i, this->z[i]);
    }

    copy.set_iteration(this->iteration);
    copy.set_valid(this->valid);

    copy.set_qty_charge(this->get_qty_charge());
    copy.set_qty_collect(this->get_qty_collect());
    copy.set_qty_deliver(this->get_qty_deliver());

    return copy;
}

Route Route::full_copy()
{
    Route copy = Route(this->num_objectives);
    Solution_Pair temp;

    int i;
    for (i = 0; i < this->get_size(); i++) {
        temp = get_solution_pair(i);
        copy.add_solution_pair(temp);//copy->set_solution_pair(temp, i);
    }

    for (i = 0; i < this->num_objectives; i++) {
        copy.set_z(i, this->z[i]);
    }

    copy.set_iteration(this->iteration);
    copy.set_valid(this->valid);

    copy.set_qty_charge(this->get_qty_charge());
    copy.set_qty_collect(this->get_qty_collect());
    copy.set_qty_deliver(this->get_qty_deliver());

    return copy;
}

bool Route::equals(Route r_2) /* verify if routes are the same */ // pointer
{
    if(this->get_size() != r_2.size){
        return false;
    }

    // Run through all the route
    for (int i = 0; i < this->get_size(); i++)
    {
        // Compare the point from both routes
        if (!this->get_solution_pair(i).equals(r_2.get_solution_pair(i))) {
            return false;
        }
    }
    return true;
}

void Route::merge_without_start_and_end(Route r_2, int start) /* verifica se solucao domina outra solucao */ // pointer
{
    if(!r_2.is_null()){
        //    Route * copy = a_2->copy();
            Route copy = r_2.copy(); //memcheck
            copy.remove_solution_pair(copy.get_size() - 1);
            copy.remove_solution_pair(0);

            this->data1.insert (this->data1.begin() + start + 1, copy.data1.begin(), copy.data1.end());
            this->size = this->size + copy.size;
        //    delete copy;//memcheck
    }

}

void Route::merge(Route a_2, int start) /* verifica se solucao domina outra solucao */ // pointer
{
    this->data1.insert (this->data1.begin() + start + 1, a_2.data1.begin(), a_2.data1.end());
}

void Route::resize(int s)
{
//    this->size = s;
//    Solution_Pair *new_data = new Solution_Pair[s];

//    for (int i = 0; i < s; ++i) {
//        new_data[i] = data[i].copy();
//    }

//    delete [] data;

//    data = new_data;
}
