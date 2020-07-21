#ifndef INT_MATRIX_H
#define INT_MATRIX_H

using namespace std;

class Int_Matrix
{
    int rows, cols, size;
    int *data;

public:
    Int_Matrix();
    ~Int_Matrix();
    Int_Matrix(int r, int c);
    Int_Matrix(int s);

    //GET e SET
    int  get_rows();
    void set_rows(int r);

    int  get_cols();
    void set_cols(int c);

    int  get_size();

    int get_cell(int x, int y);
    void set_cell(int x, int y, int value);

    int get_cell(int pos);
    void set_cell(int pos, int value);

    /*int  get_element(int r, int c);
    void set_element(int r, int c, int value);

    int  get_element(int pos);
    void set_element(int pos, int value);*/

    int  matrix_to_array(int r, int c);
    int array_to_matrix_x(int pos);
    int array_to_matrix_y(int pos);

    void initialize();

    void print();

    Int_Matrix * copy();
};


#endif // INT_MATRIX_H
