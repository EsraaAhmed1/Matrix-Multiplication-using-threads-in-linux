#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

// this struct made to sent the data to the thread i made it general and used it in both variations
typedef struct 
{
    int** matrix1;
    int** matrix2;
    int** result;
    int rows1;
    int cols1;
    int rows2;
    int cols2;
    int row;
    int column;
} thread_data;

// this function is for variation 1 at which each element in the result matrix calculated by thread we just send to it which row in matrix 1 will be used in dot product and which column in matrix 2 
void* calculate_element(void *args) 
{
    thread_data *data = (thread_data *)args;
    int element = 0;
    // loop over each element in the sent row and column
    for (int i = 0; i < data->cols1; i++) 
    {
        element += data->matrix1[data->row][i] * data->matrix2[i][data->column];
    }
    // the element is directly stored in the result matrix
    data->result[data->row][data->column] = element;
    // free your data
    free(data); 
    return NULL;
}

// this function is used in variation 2 at which each row in the result matrix calculated by thread we just send to it only which row in matrix we are computing
void* calculate_ByRow(void *args)
{
    thread_data *data = (thread_data *)args;
    // loop over each column in matrix 2
    for(int i=0;i<data->cols2;i++)
    {
        int sum=0;
        //loop over each column in matrix 1 to get every element in the given row
        for(int j=0;j<data->cols1;j++)
        {
            sum=sum+data->matrix1[data->row][j]*data->matrix2[j][i];
        }
        data->result[data->row][i]=sum;
    }
    // free your data
    free(data);
}

int main() {
    clock_t start1,end1;
    double elapsed_time1;
    
    clock_t start2,end2;
    double elapsed_time2;

    //open the file 
    FILE* file = fopen("in", "r");
    if (!file) 
    {
        printf("Error opening the file.\n");
        return 1;
    }

    int rows1, cols1, rows2, cols2;
    
    // read from the file the dimensions of matrix 1 it ensures that there are 2 int or it returns error
    if (fscanf(file, "%d %d", &rows1, &cols1) != 2) 
    {
        printf("Error reading rows and columns of the first matrix.\n");
        fclose(file);
        return 1;
    }
    // initialize matrix1 by initializing pointer to the row pointers 
    int** matrix1 = (int**)malloc(sizeof(int*) * rows1);
    //initialize the columns and book them for matrix 1
    for (int i = 0; i < rows1; i++) 
    {
        matrix1[i] = (int*)malloc(sizeof(int) * cols1);
    }
    // read every element for matrix 1 by the dimensions we read before
    for (int i = 0; i < rows1; i++) 
    {
        for (int j = 0; j < cols1; j++) 
        {
            if (fscanf(file, "%d", &matrix1[i][j]) != 1) 
            {
                printf("Error reading elements of the first matrix.\n");
                fclose(file);
                return 1;
            }
        }
    }
    // print first matrix
    printf("First Matrix (%dx%d):\n", rows1, cols1);
    for (int i = 0; i < rows1; i++) 
    {
        for (int j = 0; j < cols1; j++) 
        {
            printf("%d ", matrix1[i][j]);
        }
        printf("\n");
    }
    // read from the file the dimensions of matrix 2 it ensures that there are 2 int or it returns error
    if (fscanf(file, "%d %d", &rows2, &cols2) != 2) 
    {
        printf("Error in reading rows and columns of matrix 2.\n");
        fclose(file);
        return 1;
    }
    // initialize matrix2 by initializing pointer to the row pointers
    int** matrix2 = (int**)malloc(sizeof(int*) * rows2);
    for (int i = 0; i < rows2; i++) 
    {
        //initialize the columns and book them for matrix 2
        matrix2[i] = (int*)malloc(sizeof(int) * cols2);
    }
    // read every element for matrix 2 by the dimensions we read before
    for (int i = 0; i < rows2; i++) 
    {
        for (int j = 0; j < cols2; j++) 
        {
            if (fscanf(file, "%d", &matrix2[i][j]) != 1) 
            {
                printf("Error in reading elements of matrix2\n");
                fclose(file);
                return 1;
            }
        }
    }
    // print second matrix
    printf("Second Matrix (%dx%d):\n", rows2, cols2);
    for (int i = 0; i < rows2; i++) 
    {
        for (int j = 0; j < cols2; j++) 
        {
            printf("%d ", matrix2[i][j]);
        }
        printf("\n");
    }
    // check if the multiplication is valid or not
    if (cols1 != rows2) 
    {
        printf("Can't multiply these matrices together\n");
        return 1;
    }
    
    
    // start variation 1 code
    // initialize the result matrix for variation 1
    int** result1 = (int**)malloc(sizeof(int*) * rows1);
    for (int i = 0; i < rows1; i++) 
    {
        result1[i] = (int*)malloc(sizeof(int) * cols2);
    }

    pthread_t threads[rows1 * cols2];
    int count = 0;
    // start calculating time
    start1=clock();
    // loop over the size of the result matrix and start creating thread at which each thread calculate single element in this array
    for (int i = 0; i < rows1; i++) 
    {
        for (int j = 0; j < cols2; j++) 
        {
            //make variable from the struct and initialize each variable in this struct 
            thread_data* data = (thread_data*)malloc(sizeof(thread_data));
            data->matrix1 = matrix1;
            data->matrix2 = matrix2;
            data->result = result1;
            data->rows1 = rows1;
            data->cols1 = cols1;
            data->rows2 = rows2;
            data->cols2 = cols2;
            data->row = i;
            data->column = j;
            // create thread 
            pthread_create(&threads[count++], NULL, calculate_element, (void*)data);
        }
    }

    for (int i = 0; i < count; i++) 
    {
        pthread_join(threads[i], NULL);
    }
    // stop the time here 
    end1=clock();
    
    // print result matrix
    printf("Result Matrix using variation 1 (%dx%d):\n", rows1, cols2);
    for (int i = 0; i < rows1; i++) 
    {
        for (int j = 0; j < cols2; j++) 
        {
            printf("%d ", result1[i][j]);
        }
        printf("\n");
    }
    // calculate the time variation1 took
    elapsed_time1=((double)(end1-start1)) / CLOCKS_PER_SEC;
    printf("Elapsed time: %f seconds\n",elapsed_time1);
    
    // end variation 1 code
    
    //start variation 2 code
    // initialize the result matrix for variation 2
    int** result2 = (int**)malloc(sizeof(int*) * rows1);
    for (int i = 0; i < rows1; i++) 
    {
        result2[i] = (int*)malloc(sizeof(int) * cols2);
    }

    pthread_t thread[rows1 * cols2];
    int count2 = 0;
    // start calculating time
    start2=clock();
    // loop over the size of the result matrix and start creating thread at which each thread calculate single element in this array
    for (int i = 0; i < rows1; i++) 
    {
        for (int j = 0; j < cols2; j++) 
        {
            //make variable from the struct and initialize each variable in this struct 
            thread_data* data = (thread_data*)malloc(sizeof(thread_data));
            data->matrix1 = matrix1;
            data->matrix2 = matrix2;
            data->result = result2;
            data->rows1 = rows1;
            data->cols1 = cols1;
            data->rows2 = rows2;
            data->cols2 = cols2;
            data->row = i;
            data->column = j;
            // create thread
            pthread_create(&thread[count2++], NULL, calculate_ByRow, (void*)data);
        }
    }

    for (int i = 0; i < count2; i++) 
    {
        pthread_join(thread[i], NULL);
    }
    // stop the time here
    end2=clock();
    // print the result matrix for variation 2
    printf("Result Matrix using variation 2 (%dx%d):\n", rows1, cols2);
    for (int i = 0; i < rows1; i++) 
    {
        for (int j = 0; j < cols2; j++) 
        {
            printf("%d ", result2[i][j]);
        }
        printf("\n");
    }
    // calculate the time variation 2 took
    elapsed_time2=((double)(end2-start2)) / CLOCKS_PER_SEC;
    printf("Elapsed time: %f seconds\n",elapsed_time2);
    
    
    
    
    // end variation 2 code

    // free the booked space for my data
    for (int i = 0; i < rows1; i++) 
    {
        free(matrix1[i]);
    }
    free(matrix1);

    for (int i = 0; i < rows2; i++) 
    {
        free(matrix2[i]);
    }
    free(matrix2);

    for (int i = 0; i < rows1; i++) 
    {
        free(result1[i]);
    }
    free(result1);
    
    for (int i = 0; i < rows1; i++) 
    {
        free(result2[i]);
    }
    free(result2);

    return 0;
}

