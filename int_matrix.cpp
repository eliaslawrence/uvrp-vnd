#include "int_matrix.h"
#include <iostream>

Int_Matrix::Int_Matrix()
{
    this->rows = 0;
    this->cols = 0;

    this->size = 0;
}

Int_Matrix::~Int_Matrix()
{
    delete [] data;
}

Int_Matrix::Int_Matrix(int r, int c)
{
    this->rows = r;
    this->cols = c;
    this->size = rows * cols;

    data = new int[size];
}

Int_Matrix::Int_Matrix(int s)
{
    data = new int[s];
}

int Int_Matrix::get_rows()
{
    return rows;
}

void Int_Matrix::set_rows(int r)
{
    this->rows = r;
    this->size = rows * cols;
}

int Int_Matrix::get_cols()
{
    return cols;
}

void Int_Matrix::set_cols(int c)
{
    this->cols = c;
    this->size = rows * cols;
}

int Int_Matrix::get_size() {
    return size;
}

int Int_Matrix::matrix_to_array(int x, int y)
{
    return y*cols + x;
}

int Int_Matrix::array_to_matrix_x(int pos)
{
    return pos % this->cols;
}

int Int_Matrix::array_to_matrix_y(int pos)
{
    int temp = pos - pos % this->cols;
    return temp / this->cols;
}

int Int_Matrix::get_cell(int x, int y)
{
    return data[matrix_to_array(x,y)];
}

int Int_Matrix::get_cell(int pos)
{
    return data[pos];
}

void Int_Matrix::set_cell(int x, int y, int value)
{    
    data[matrix_to_array(x, y)] = value;
}

void Int_Matrix::set_cell(int pos, int value)
{
    int x = array_to_matrix_x(pos);
    int y = array_to_matrix_y(pos);

    data[pos] = value;
}

void Int_Matrix::print()
{
    cout << "\n";

    for (int j = 0; j < rows; j++) {
        for (int i = 0; i < cols; i++) {
            cout << get_cell(i, j) << " ";
        }
        cout << "\n";
    }

    cout << "\n";
}

void Int_Matrix::initialize()
{
    for (int i = 0; i < size; i++) {
        set_cell(i, 0);
    }
}

Int_Matrix * Int_Matrix::copy()
{
    Int_Matrix * copy = new Int_Matrix(this->rows, this->cols);
    for (int i = 0; i < size; i++) {
        copy->set_cell(i, this->get_cell(i));
    }

    return copy;
}
