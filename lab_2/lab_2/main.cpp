#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <omp.h>
#include <string>

using namespace std;
using namespace chrono;

// Function to generate a random matrix
vector<vector<int>> generateMatrix(int size, int minVal = -10, int maxVal = 10) {
    vector<vector<int>> matrix(size, vector<int>(size));
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            matrix[i][j] = minVal + rand() % (maxVal - minVal + 1);
    return matrix;
}

// Function to save a matrix to a file
void saveMatrixToFile(const string& filename, const vector<vector<int>>& matrix) {
    ofstream file(filename);
    if (!file) {
        cerr << "Error: Unable to open file " << filename << endl;
        exit(1);
    }
    int size = matrix.size();
    file << size << " " << size << endl;
    for (const auto& row : matrix) {
        for (int val : row)
            file << val << " ";
        file << endl;
    }
    file.close();
}

// Function to read a matrix from a file
vector<vector<int>> readMatrixFromFile(const string& filename, int& rows, int& cols) {
    ifstream file(filename);
    if (!file) {
        cerr << "Error: Unable to open file " << filename << endl;
        exit(1);
    }

    file >> rows >> cols;
    vector<vector<int>> matrix(rows, vector<int>(cols));

    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            file >> matrix[i][j];

    file.close();
    return matrix;
}

// Function to multiply two matrices sequentially (no threads)
vector<vector<int>> multiplyMatricesSequential(const vector<vector<int>>& A, const vector<vector<int>>& B) {
    int m = A.size(), n = A[0].size(), p = B[0].size();
    vector<vector<int>> result(m, vector<int>(p, 0));

    for (int i = 0; i < m; i++) {
        for (int j = 0; j < p; j++) {
            for (int k = 0; k < n; k++) {
                result[i][j] += A[i][k] * B[k][j];
            }
        }
    }
    return result;
}

// Function to multiply two matrices with OpenMP
vector<vector<int>> multiplyMatricesParallel(const vector<vector<int>>& A, const vector<vector<int>>& B, int num_threads) {
    int m = A.size(), n = A[0].size(), p = B[0].size();
    vector<vector<int>> result(m, vector<int>(p, 0));

#pragma omp parallel for num_threads(num_threads)
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < p; j++) {
            for (int k = 0; k < n; k++) {
                result[i][j] += A[i][k] * B[k][j];
            }
        }
    }
    return result;
}

// Function to save execution times to a single file
void saveExecutionTimes(const string& filename, int size, int num_threads, double timeElapsed) {
    ofstream file(filename, ios::app);
    if (!file) {
        cerr << "Error: Unable to open file " << filename << endl;
        exit(1);
    }
    file << size << " " << num_threads << " " << timeElapsed << endl;
    file.close();
}

int main() {
    srand(time(0));
    vector<int> sizes = { 100, 200, 400, 1000 };
    vector<int> thread_counts = { 0, 2, 4, 8 };
    string timeFile = "execution_times.txt";

    // Clear the execution times file before starting
    ofstream clearFile(timeFile, ios::trunc);
    clearFile.close();

    for (int size : sizes) {
        // Generate and save random matrices
        vector<vector<int>> A = generateMatrix(size);
        vector<vector<int>> B = generateMatrix(size);

        string fileA = "matrixA_" + to_string(size) + ".txt";
        string fileB = "matrixB_" + to_string(size) + ".txt";

        saveMatrixToFile(fileA, A);
        saveMatrixToFile(fileB, B);

        cout << "Generated " << size << "x" << size << " matrices and saved to files.\n";

        // Read matrices back from files
        int rowsA, colsA, rowsB, colsB;
        vector<vector<int>> A_read = readMatrixFromFile(fileA, rowsA, colsA);
        vector<vector<int>> B_read = readMatrixFromFile(fileB, rowsB, colsB);

        if (colsA != rowsB) {
            cerr << "Error: Cannot multiply matrices of size " << size << "!\n";
            continue;
        }

        // Test with different thread counts
        for (int num_threads : thread_counts) {
            vector<vector<int>> result;
            double elapsed_time;

            auto start = high_resolution_clock::now();
            if (num_threads == 0) {
                result = multiplyMatricesSequential(A_read, B_read);
            }
            else {
                result = multiplyMatricesParallel(A_read, B_read, num_threads);
            }
            auto stop = high_resolution_clock::now();
            elapsed_time = duration<double>(stop - start).count();

            // Save execution time to a single file
            saveExecutionTimes(timeFile, size, num_threads, elapsed_time);

            cout << "Size: " << size << "x" << size << ", Threads: " << num_threads
                << ", Execution Time: " << elapsed_time << " seconds\n";
        }
    }

    return 0;
}