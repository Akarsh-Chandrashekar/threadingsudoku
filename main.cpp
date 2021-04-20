#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <iostream>
#include <chrono>
#include <vector>


using namespace std;
using namespace std::chrono;
using std::vector;

const int number_of_threads=27;
const int rows=9;
const int columns=9;
int result_final[number_of_threads]={};

int sudoku[9][9]= 
            {
                {6, 2, 4, 5, 3, 9, 1, 8, 7},
                {5, 1, 9, 7, 2, 8, 6, 3, 4},
                {8, 3, 7, 6, 1, 4, 2, 9, 5},
                {1, 4, 3, 8, 6, 5, 7, 2, 9},
                {9, 5, 8, 2, 4, 7, 3, 6, 1},
                {7, 6, 2, 3, 9, 1, 4, 5, 8},
                {3, 7, 1, 9, 5, 6, 8, 4, 2},
                {4, 9, 6, 1, 8, 2, 5, 7, 3},
                {2, 8, 5, 4, 7, 3, 9, 1, 6}
        };


struct arguments
{
int row;
int column;
int (*sud_ptr)[9];
};

bool verifyGrid();
bool verifyColumn();
bool verifyRow();
bool verifySudokuBySingleThread();

void *verify_grid(void *parameter);
void *verify_row(void *params);
void *verify_col(void *params);

  
int main(void)
{
    steady_clock::time_point start_time_single_thread = steady_clock::now();
    if(verifySudokuBySingleThread())
    {
        std::cout<<"Sudoku solution is valid"<<std::endl;
    }
    else
    {
        std::cout<<"Sudoku solution is not valid"<<std::endl;
    }

    // Compute and return the elapsed time in microseconds.
    steady_clock::time_point end_time_single_thread = steady_clock::now();
    long elapsed_time_single_thread = duration_cast<microseconds>(end_time_single_thread - start_time_single_thread).count();

    cout << endl << "Total time using single thread: " << elapsed_time_single_thread << "Micro seconds" << endl << endl;

    // 27  thread approach
    
    pthread_t threads[number_of_threads];
    steady_clock::time_point starttime_threadmethod = steady_clock::now();
    int thread_index = 0;

    for (int k=0;k<9;k++)
    {
        for(int l=0;l<9;l++)
        {
            if(k%3==0 && l%3==0)
            {
                arguments *sud_grid=new arguments;
                sud_grid->row=k;
                sud_grid->column=l;
                sud_grid->sud_ptr=sudoku;
                pthread_create(&threads[thread_index++], NULL, verify_grid, sud_grid);
            }
            if(l==0)
            {
                arguments *sud_row=new arguments;
                sud_row->row=k;
                sud_row->column=l;
                sud_row->sud_ptr=sudoku;
                pthread_create(&threads[thread_index++], NULL, verify_row, sud_row);
            }
            if(k==0)
            {
                arguments *sud_col=new arguments;
                sud_col->row=k;
                sud_col->column=l;
                sud_col->sud_ptr=sudoku;
                pthread_create(&threads[thread_index++], NULL, verify_col, sud_col);
            }
        }
    }


    for(int i=0;i<27;i++)
    {
        pthread_join(threads[i], NULL);
    }

    for(int i=0;i<27;i++)
    {
        if(result_final[i]==0)
        {
            cout<<"this sudoku solution is not valid"<<endl;
            steady_clock::time_point endtime_threadmethod = steady_clock::now();
            long total_time = duration_cast<microseconds>(endtime_threadmethod - starttime_threadmethod).count();
            cout << endl << "Total time using 27 threads: " << total_time << "Micro seconds" << endl;
            return 1;

        }
    }

    cout<<"this sudoku solution is  valid"<<endl;
    steady_clock::time_point endtime_threadmethod = steady_clock::now();
    long total_time = duration_cast<microseconds>(endtime_threadmethod - starttime_threadmethod).count();
    cout << endl << "Total time using 27 threads: " << total_time << "Micro seconds" << endl;
}


bool verifyGrid(){
    for (int i = 0; i < 3; ++i){
        for (int j = 0; j < 3; ++j){
            int grid_row = 3 * i;
            int grid_column = 3 * j;
            vector<bool> numbers(rows, false);
            for(int r=grid_row; r < grid_row+3; r++){
                for(int c=grid_column; c < grid_column+3; c++){
                    numbers[sudoku[r][c]-1] = !numbers[sudoku[r][c]-1];
                }
            }
            for(auto i : numbers){
                if(!i){
                    return false;
                }
            }
        }
    }
    return true;
} 

bool verifyColumn(){
    for(int i=0; i<rows; i++){
        vector<bool> numbers(rows, false);
        for(int j=0; j<columns; j++){
            numbers[sudoku[j][i]-1] = !numbers[sudoku[j][i]-1];
        }
        for(auto i : numbers){
            if(!i){
                return false;
            }
        }
    }
    return true;
};

bool verifyRow(){
    for(int i=0; i<rows; i++){
        vector<bool> numbers(rows, false);
        for(int j=0; j<columns; j++){
                numbers[sudoku[i][j]-1] = !numbers[sudoku[i][j]-1];
        }
        for(auto i : numbers){
            if(!i){
                return false;
            }
        }
    }
    return true;
}

bool verifySudokuBySingleThread(){
    if (!verifyRow()){
        return false;
    }
    if (!verifyColumn()){
        return false;
    }
    if (!verifyGrid()) {
        return false;
    }
    return true;
}


void *verify_grid(void *parameter)
{
    arguments *sud=(arguments*) parameter;
    int row=sud->row;
    int col=sud->column;
    vector<bool> numbers(10,false);
    for (int i = row; i < row + 3; ++i)
    {
        for(int j=col;j<col+3;++j)
        {
            int ind=sud->sud_ptr[i][j];
            if (numbers[ind]==true)
            {
            pthread_exit(NULL);
            }
            else
            {
            numbers[ind]=!numbers[ind];
            }
        }
    }

    // If the execution has reached this point, then the 3x3 sub-grid is valid.
    result_final[row + col/3] = 1; // Maps the 3X3 sub-grid to an index in the first 9 indices of the result array
    pthread_exit(NULL);

}

void *verify_row(void *params)
{
    arguments *sud=(arguments*) params;
    int row=sud->row;
    vector<bool> numbers(10,false);
    for (int j=0;j<9;j++)
    {
        int ind=sud->sud_ptr[row][j];
        if (numbers[ind]==true)
        {
            pthread_exit(NULL);
        }
        else
        {
            numbers[ind]=!numbers[ind];
        }
    }

    result_final[9 + row] = 1;
    pthread_exit(NULL);
}
 
void *verify_col(void *params)
{
    arguments *sud=(arguments*) params;
    int col=sud->column;
    vector<bool> numbers(10,false);
    for (int i = 0; i < 9; i++)
    {
        int ind=sud->sud_ptr[i][col];
        if (numbers[ind]==true)
        {
            pthread_exit(NULL);
        }
        else
        {
            numbers[ind]=!numbers[ind];
        }
    }
    // If the execution has reached this point, then the column is valid.
    result_final[18 + col] = 1; // Maps the column to an index in the third set of 9 indices of the result array
    pthread_exit(NULL);

}
