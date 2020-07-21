#include "matrix.h"
#include <iostream>
#include <fstream> //FILE
#include "core.h"

using namespace fixedValues;

Matrix::Matrix()
{
    this->rows = 0;
    this->cols = 0;

    this->size = 0;
}

Matrix::~Matrix()
{
    delete [] data;
}

Matrix::Matrix(int r, int c)
{
    this->rows = r;
    this->cols = c;
    this->size = rows * cols;

    data = new Stop_Point[size];
}

Matrix::Matrix(int s)
{
    data = new Stop_Point[s];
}

int Matrix::get_rows()
{
    return rows;
}

void Matrix::set_rows(int r)
{
    this->rows = r;
    this->size = rows * cols;
}

int Matrix::get_cols()
{
    return cols;
}

void Matrix::set_cols(int c)
{
    this->cols = c;
    this->size = rows * cols;
}

int Matrix::get_size() {
    return size;
}

int Matrix::matrix_to_array(int x, int y)
{
    return y*cols + x;
}

int Matrix::array_to_matrix_x(int pos)
{
    return pos % this->cols;
}

int Matrix::array_to_matrix_y(int pos)
{
    int temp = pos - pos % this->cols;
    return temp / this->cols;
}

Stop_Point Matrix::get_stop_point(Solution_Pair p)
{
    return data[matrix_to_array(p.get_x(),p.get_y())];
}

Stop_Point Matrix::get_stop_point(int x, int y)
{
    return data[matrix_to_array(x,y)];
}

Stop_Point Matrix::get_stop_point(int pos)
{
    return data[pos];
}

void Matrix::set_stop_point(int x, int y, int type)
{
    Stop_Point s_p(x, y, type);
    data[matrix_to_array(x, y)] = s_p;
}

void Matrix::set_stop_point(int pos, int type)
{
    int x = array_to_matrix_x(pos);
    int y = array_to_matrix_y(pos);
    Stop_Point s_p(x, y, type);

    data[pos] = s_p;
}

void Matrix::print()
{
    std::vector<int> v_x;
    std::vector<int> v_y;
    cout << "\n";

    for (int j = 0; j < rows; j++) {
        for (int i = 0; i < cols; i++) {
            get_stop_point(i, j).print();

            if(get_stop_point(i, j).get_type() == STOPS::DELIVER){
                v_x.emplace_back(i);
                v_y.emplace_back(j);
            }
        }
        cout << "\n";
    }

    cout << "\n";

//   for (int i = 0; i < v_x.size(); i++) {
//       cout << v_x[i] << " " << v_y[i] << "\n";
//   }

//    cout << "\n";
}

void Matrix::save_to_file(string file_name)
{
    //START: Info file
    ofstream matrix_file;
    matrix_file.open (file_name);

    for (int j = 0; j < rows; j++) {
        for (int i = 0; i < cols; i++) {
            matrix_file << get_stop_point(i, j).get_type() << " ";
        }
        matrix_file << "\n";
    }

    matrix_file.close();
}

void Matrix::initialize()
{
    for (int i = 0; i < size; i++) {
        set_stop_point(i, 0);
    }
}

bool Matrix::contains(int reference_type) /* verifica se mapa possui determinado ponto de parada */
{
    // Percorre todo array
    for (int i = 0; i < this->size; i++)
    {
        // Compara o ponto de referencia com os pares ordenados do array
        if (reference_type == get_stop_point(i).get_type()) {
            return true;
        }
    }
    return false;
}

Matrix * Matrix::copy()
{
    Matrix * copy = new Matrix(this->rows, this->cols);
    for (int i = 0; i < size; i++) {
        copy->set_stop_point(i, this->get_stop_point(i).get_type());
    }

    return copy;
}

bool Matrix::is_inside_perimeter(int x, int y)
{
    return x >= 0 && x < this->get_cols() && y >= 0 && y < this->get_rows();
}

bool Matrix::is_prohibited(int x, int y)
{
    return get_stop_point(x,y).get_type() == STOPS::PROHIBITED;
}
