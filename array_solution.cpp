#include "array_solution.h"
#include "core.h"
#include <iostream>
#include <limits>

using namespace fixedValues;

Array_Solution::Array_Solution()
{
    this->size = 0;
    this->points         = 0;
    this->qty_points[0]  = 0;
    this->qty_points[1]  = 0;
    this->qty_points[2]  = 0;
    this->qty_points[3]  = 0;
    this->num_objectives = 4;

    neighbor_visited.assign (9, 0);

    initialize_z(this->num_objectives);

    this->routes.assign(3, Route());
}

Array_Solution::~Array_Solution()
{
//    vector<Route>().swap(routes);
//    v.swap(routes);

//    routes.clear();

//    data1.clear();
//    delete [] z;

//    for (int var = 0; var < size; var++) {
//        delete this->routes[var];
//    }
}

//Array_Solution::Array_Solution(Array_Solution copy) // pointer
//{
//    this->size           = copy.size;
//    this->points         = copy.points;
//    this->qty_points[0]  = copy.qty_points[0];
//    this->qty_points[1]  = copy.qty_points[1];
//    this->qty_points[2]  = copy.qty_points[2];
//    this->qty_points[3]  = copy.qty_points[3];
//    this->num_objectives = copy.num_objectives;

//    neighbor_visited.assign (9, 0);

//    initialize_z(this->num_objectives);

//    int i;
//    for (i = 0; i < copy.get_size(); i++) {
//        this->add_route(copy.get_routes()[i]);
//    }

//    for (i = 0; i < copy.num_objectives; i++) {
//        this->set_z(i, copy.z[i]);
//    }

//    for (i = 0; i < copy.neighbor_visited.size(); i++) {
//        this->neighbor_visited[i] = copy.neighbor_visited[i];
//    }

//    this->set_iteration(copy.iteration);
//    this->set_valid(copy.valid);

//    this->set_qty_charge(copy.get_qty_charge());
//    this->set_qty_collect(copy.get_qty_collect());
//    this->set_qty_deliver(copy.get_qty_deliver());

//}

Array_Solution::Array_Solution(int s, int n_obj)
{
    this->size           = s;
    this->points         = 0;
    this->qty_points[0]  = 0;
    this->qty_points[1]  = 0;
    this->qty_points[2]  = 0;
    this->qty_points[3]  = 0;
    this->num_objectives = n_obj;

    neighbor_visited.assign (9, 0);


    initialize_z(this->num_objectives);

//    data = new Solution_Pair[s];
    this->routes.assign(3, Route());
}

Array_Solution::Array_Solution(int s)
{
    this->size           = s;
    this->points         = 0;
    this->qty_points[0]  = 0;
    this->qty_points[1]  = 0;
    this->qty_points[2]  = 0;
    this->qty_points[3]  = 0;
    this->num_objectives = 4;

    neighbor_visited.assign (9, 0);

    initialize_z(this->num_objectives);

//    data = new Solution_Pair[s];
    this->routes.assign(3, Route());
}

Array_Solution::Array_Solution(bool n)
{
    this->null = n;
    this->routes.assign(3, Route());
}

bool Array_Solution::is_null()
{
    return this->null;
}

void Array_Solution::initialize_z(int n_obj) // pointer
{
//    z = new double[n_obj];

    for (int i = 0; i < this->num_objectives; i++) {
        z.push_back(0);
    }
}

vector<Route> Array_Solution::get_routes(){ // pointer
    return this->routes;
}

void Array_Solution::set_routes(vector<Route> routes){ // pointer
    this->routes = routes;
}

//vector<Solution_Pair> Array_Solution::get_data()
//{
//    return data1;
//}

//Solution_Pair Array_Solution::get_solution_pair(int pos)
//{
//    return data1[pos];//return data[pos];
//}

//void Array_Solution::remove_solution_pair(int pos)
//{
//    data1.erase(data1.begin() + pos);
//}

//void Array_Solution::add_solution_pair(Solution_Pair e)
//{
//    data1.push_back(e.copy());
//}

//void Array_Solution::add_solution_pair(int x, int y)
//{
//    Solution_Pair s_p(x, y);
//    data1.push_back(s_p);
//}

//void Array_Solution::set_solution_pair(Solution_Pair e, int pos)
//{
//    data1[pos] = e.copy();//    data[pos] = e.copy();
//}

//void Array_Solution::set_solution_pair(int x, int y, int pos)
//{
//    Solution_Pair s_p(x, y);
//    data1[pos] = s_p;//data[pos] = s_p;
//}

//void Array_Solution::insert_solution_pair(Solution_Pair e, int pos)
//{
//    this->data1.insert (this->data1.begin() + pos + 1, e);
//}

//void Array_Solution::insert_solution_pair(int x, int y, int pos)
//{
//    Solution_Pair e(x, y);
//    this->data1.insert (this->data1.begin() + pos + 1, e);
//}

int Array_Solution::get_size() {
    return size;//routes.size();//return size;
}

void Array_Solution::set_size(int s) {
    this->size = s;
}

int Array_Solution::get_time() {
    return time;
}

void Array_Solution::set_time(int t) {
    this->time = t;
}

int Array_Solution::get_qty_deliver(){
    return this->qty_points[STOPS::DELIVER];
}

void Array_Solution::set_qty_deliver(int q_d){
    this->qty_points[STOPS::DELIVER] = q_d;
}

int Array_Solution::get_qty_collect(){
    return this->qty_points[STOPS::COLLECT];
}

void Array_Solution::set_qty_collect(int q_c){
    this->qty_points[STOPS::COLLECT] = q_c;
}

int Array_Solution::get_qty_charge(){
    return this->qty_points[STOPS::CHARGE];
}

void Array_Solution::set_qty_charge(int q_c){
    this->qty_points[STOPS::CHARGE] = q_c;
}

double Array_Solution::get_points(){
//    double points = 1;
//    for (int i = 0; i < this->num_objectives; i++) {
//        points *= this->z[i];
//    }
//    return this->z[0] == 0 ? std::numeric_limits<int>::min() : (int) (this->z[3] / this->z[0]);// points;
    return (-1) * this->get_size() / this->z[0];//this->z[3] / this->z[0];// points;
}

vector<double> Array_Solution::get_z(){
    return this->z;
}

void Array_Solution::set_z(int pos, double value){
    this->z[pos] = value;
}

int Array_Solution::get_num_objectives(){
    return this->num_objectives;
}

int  Array_Solution::get_iteration(){
    return this->iteration;
}

void Array_Solution::set_iteration(int i){
    this->iteration = i;
}

bool Array_Solution::is_valid(){ // pointer
    for (int pos_route = 0; pos_route < this->routes.size(); pos_route++) {
        if(!this->routes[pos_route].is_valid()){
            return false;
        }
    }

    return true;
}

void Array_Solution::set_valid(bool v){
    this->valid = v;
}

bool Array_Solution::is_complete(){
    return this->complete;
}

void Array_Solution::set_complete(bool c){
    this->complete = c;
}

string Array_Solution::get_validation(){
    return this->valid ? "true" : "false";
}

void Array_Solution::set_neighborhood_visitation(int pos, int visitation)
{
    neighbor_visited[pos] = visitation;
}

int Array_Solution::is_neighborhood_visited(int pos)
{
    return neighbor_visited[pos];
}

void Array_Solution::print() /* prints solution */ // pointer
{    
    for (int i = 0; i < this->get_size(); i++) {
        routes[i].print(); // prints route of certain vehicle
        cout << "\n";
    }
}

string Array_Solution::to_string() /* generates a string with all routes */ // pointer
{    
    string solution;

    for (int i = 0; i < this->get_size(); i++) {
        solution.append(routes[i].to_string()); // generates a string with route of certain vehicle
        solution.append("\n");
    }

    return solution;
}

bool Array_Solution::contains(Solution_Pair e) /* verify if solution contains certain point */ // pointer
{
    // Run through all routes
    for (int i = 0; i < this->get_size(); i++)
    {        
        // Verify if route contains the point
        if (routes[i].contains(e)) {
            return true;
        }
    }
    return false;
}

//Array_Solution * Array_Solution::intersection(Array_Solution * a_2)
//{
//    int biggest_size = this->get_size() > a_2->get_size() ? this->get_size() : a_2->get_size();

//    Array_Solution * a_intersection = new Array_Solution(biggest_size);

//    Solution_Pair reference_element;

//    int count = 0;

//    for (int i = 0; i < a_2->get_size(); i++)
//    {
//        reference_element = a_2->get_solution_pair(i);

//        for (int j = 0; j < this->get_size(); j++)
//        {
//            if (reference_element.equals(this->get_solution_pair(j))) {
//                a_intersection->add_solution_pair(reference_element);

//                count++;
//                break;
//            }
//        }
//    }

//    a_intersection->set_size(count);

//    return a_intersection;
//}

void Array_Solution::to_file(string file_name) // pointer
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

    array_file.close();

    size_t found = file_name.find_last_of("/");
    string solution_name = file_name.substr(found, file_name.size() - 1);
    file_name.erase (found, file_name.size() - 1);
    string routes_file_name = file_name;
    routes_file_name.append("/routes");

    for (int pos_route = 0; pos_route < this->get_size(); pos_route++) {
        string route_file_name = routes_file_name + solution_name + "_route_" + std::to_string(pos_route);
        this->get_routes()[pos_route].to_file(route_file_name);
    }
}

Array_Solution Array_Solution::copy()
{
    Array_Solution copy = Array_Solution();
    Solution_Pair temp;

    int i;
    for (i = 0; i < this->get_size(); i++) {
        copy.add_route(get_routes()[i].full_copy());
//        temp = this->get_solution_pair(i);
//        copy->add_solution_pair(temp);//copy->set_solution_pair(temp, i);
    }

    for (i = 0; i < this->num_objectives; i++) {
        copy.set_z(i, this->z[i]);
    }

//    for (i = 0; i < this->neighbor_visited.size(); i++) {
//        copy->neighbor_visited[i] = this->neighbor_visited[i];
//    }

    copy.set_iteration(this->iteration);
    copy.set_valid(this->valid);

    copy.set_qty_charge(this->get_qty_charge());
    copy.set_qty_collect(this->get_qty_collect());
    copy.set_qty_deliver(this->get_qty_deliver());

    return copy;
}

Array_Solution Array_Solution::invert() // pointer
{
    Array_Solution copy(this->get_size(), this->num_objectives);
    Solution_Pair temp;

    int i;
    for (i = this->get_size() - 1; i >= 0; i--) {
        copy.add_route(Route(get_routes()[i]));
//        temp = this->get_solution_pair(i);
//        copy->add_solution_pair(temp);//copy->set_solution_pair(temp, i);
    }

    for (i = 0; i < this->num_objectives; i++) {
        copy.set_z(i, this->z[i]);
    }

    for (i = 0; i < this->neighbor_visited.size(); i++) {
        copy.neighbor_visited[i] = this->neighbor_visited[i];
    }

    copy.set_iteration(this->iteration);
    copy.set_valid(this->valid);

    copy.set_qty_charge(this->get_qty_charge());
    copy.set_qty_collect(this->get_qty_collect());
    copy.set_qty_deliver(this->get_qty_deliver());

    return copy;
}

Array_Solution Array_Solution::full_copy()
{
    Array_Solution copy = Array_Solution();
    Solution_Pair temp;

    int i;
    for (i = 0; i < this->get_size(); i++) {
        copy.add_route(get_routes()[i].full_copy());
    }

    for (i = 0; i < this->num_objectives; i++) {
        copy.set_z(i, this->z[i]);
    }

    for (i = 0; i < this->neighbor_visited.size(); i++) {
        copy.neighbor_visited[i] = this->neighbor_visited[i];
    }

    copy.set_iteration(this->iteration);
    copy.set_valid(this->valid);

    copy.set_qty_charge(this->get_qty_charge());
    copy.set_qty_collect(this->get_qty_collect());
    copy.set_qty_deliver(this->get_qty_deliver());

    copy.set_dna(this->dna);

    return copy;
}

bool Array_Solution::dominates(Array_Solution a_2) /* verifica se solucao domina outra solucao */ // pointer
{
    if(!this->is_valid() && a_2.is_valid()){
        return false;
    }

    if(this->is_valid() && !a_2.is_valid()){
        return true;
    }

    bool greaterThan = false;

    // Percorre todo array
    for (int i = 0; i < num_objectives; i++)
    {
        // Compara o ponto de referencia com os pares ordenados do array
        if (this->get_z()[i] < a_2.get_z()[i]) {
            return false;
        }else if(this->get_z()[i] > a_2.get_z()[i]){
            greaterThan = true;
        }
    }
//    return greaterThan ? greaterThan : this->size < a_2->size;
    return greaterThan;
}


bool Array_Solution::weakly_dominates(Array_Solution a_2) /* verifica se solucao domina outra solucao */ // pointer
{
    if(!this->is_valid() && a_2.is_valid()){
        return false;
    }

    if(this->is_valid() && !a_2.is_valid()){
        return true;
    }


    // Percorre todo array
    for (int i = 0; i < num_objectives; i++)
    {
        // Compara o ponto de referencia com os pares ordenados do array
        if (this->get_z()[i] < a_2.get_z()[i]) {
            return false;
        }
    }

    return true;
}

bool Array_Solution::equals(Array_Solution a_2) /* verifica se solucao domina outra solucao */ // pointer
{
    if(this->get_size() != a_2.size){
        return false;
    }

    // Percorre todo array
    for (int i = 0; i < this->get_size(); i++)
    {
        if (!this->get_routes()[i].equals(a_2.get_routes()[i])) {
            return false;
        }

//        // Compara o ponto dos dois arrays
//        if (!this->get_solution_pair(i).equals(a_2->get_solution_pair(i))) {
//            return false;
//        }
    }
    return true;
}

//void Array_Solution::merge_without_start_and_end(Array_Solution * a_2, int start) /* verifica se solucao domina outra solucao */
//{
//    Array_Solution * copy = a_2->copy();
//    copy->remove_solution_pair(copy->get_size() - 1);
//    copy->remove_solution_pair(0);

//    this->data1.insert (this->data1.begin() + start + 1, copy->data1.begin(), copy->data1.end());
//}

//void Array_Solution::merge(Array_Solution * a_2, int start) /* verifica se solucao domina outra solucao */
//{
//    this->data1.insert (this->data1.begin() + start + 1, a_2->data1.begin(), a_2->data1.end());
//}

void Array_Solution::resize(int s)
{
//    this->size = s;
//    Solution_Pair *new_data = new Solution_Pair[s];

//    for (int i = 0; i < s; ++i) {
//        new_data[i] = data[i].copy();
//    }

//    delete [] data;

//    data = new_data;
}

void Array_Solution::add_route(Route r) // Add route to solution // pointer
{
//   routes.push_back(r->full_copy());   
//   routes.push_back(Route(r)); //memcheck
   routes[size] = Route(r); //memcheck
   this->size++;
}

void Array_Solution::remove_route(int pos) // Remove route from solution // pointer
{
//   delete this->routes[pos];
   routes.erase(routes.begin() + pos);

   this->size--;
}

void Array_Solution::update_values() /* update objective values according to its routes */ // pointer
{

    int    qty_clients  = 0;
    double final_charge = 0,
           consumption  = 0;

    for (int pos_route = 0; pos_route < this->get_size(); pos_route++) {
         qty_clients  += this->routes[pos_route].get_z()[0]; // Add the counting of clients attended by each route
         final_charge += this->routes[pos_route].get_z()[1]; // Add the charge at the final of each route
         consumption  += this->routes[pos_route].get_z()[3]; // Add the consumption of each route
    }

    this->set_z(0, qty_clients);
    this->set_z(1, final_charge);
    this->set_z(3, consumption);

    // Get the maximum time spent by one vehicle

    int max_time  = 0,
        temp_time = 0;    

    for (int pos_route = 0; pos_route < this->get_size(); pos_route++) {
         temp_time = (-1) * this->routes[pos_route].get_z()[2];

         if(temp_time > max_time){
             max_time = temp_time;
         }
    }

    // The max_time is set as the final time of the solution
    max_time = max_time * (-1);

    this->set_z(2, max_time);
}

// Neural Network
void Array_Solution::set_dna(Neural_Network n_n){
    dna.clear();

    // For each layer
    for(int i = 0; i < n_n.hidden_layers.size(); i++){
        // For each neuron
        for(int j = 0; j < n_n.hidden_layers[i].get_size(); j++){
            // For each weight
            for(int k = 0; k < n_n.hidden_layers[i].neurons[k].get_size(); k++){
                dna.push_back(n_n.hidden_layers[i].neurons[j].weights[k]);
            }

            dna.push_back(n_n.hidden_layers[i].neurons[j].get_bias());
        }
    }

    // For each neuron
    for(int j = 0; j < n_n.get_output_layer().get_size(); j++){
        // For each weight
        for(int k = 0; k < n_n.get_output_layer().get_neurons()[k].get_size(); k++){
            dna.push_back(n_n.get_output_layer().get_neurons()[j].weights[k]);
        }

        dna.push_back(n_n.get_output_layer().get_neurons()[j].get_bias());
    }
}

void Array_Solution::set_dna(vector<double> dna){
    this->dna = dna;
}

vector<double> Array_Solution::get_dna(){
    return dna;
}
