#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <chrono>

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

// Function to multiply two matrices
vector<vector<int>> multiplyMatrices(const vector<vector<int>>& A, const vector<vector<int>>& B) {
    int m = A.size(), n = A[0].size(), p = B[0].size();
    vector<vector<int>> result(m, vector<int>(p, 0));

    for (int i = 0; i < m; i++)
        for (int j = 0; j < p; j++)
            for (int k = 0; k < n; k++)
                result[i][j] += A[i][k] * B[k][j];

    return result;
}

// Function to save the result matrix along with execution time and problem size
void saveResultToFile(const string& filename, const vector<vector<int>>& matrix, double timeElapsed) {
    ofstream file(filename);
    if (!file) {
        cerr << "Error: Unable to open file " << filename << endl;
        exit(1);
    }
    int rows = matrix.size(), cols = matrix[0].size();

    file << "Matrix size: " << rows << "x" << cols << endl;
    file << "Execution time: " << timeElapsed << " seconds" << endl;
    file << "Resulting matrix:" << endl;

    for (const auto& row : matrix) {
        for (int val : row)
            file << val << " ";
        file << endl;
    }

    file.close();
}

int main() {
    srand(time(0)); // Initialize random seed
    vector<int> sizes = { 100, 200, 300, 400, 500 };

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

        // Multiply matrices and measure execution time
        auto start = high_resolution_clock::now();
        vector<vector<int>> result = multiplyMatrices(A_read, B_read);
        auto stop = high_resolution_clock::now();
        duration<double> elapsed = stop - start;

        // Save the result
        string resultFile = "resultMatrix_" + to_string(size) + ".txt";
        saveResultToFile(resultFile, result, elapsed.count());

        cout << "Size: " << size << "x" << size << ", Execution Time: " << elapsed.count() << " seconds\n";
    }

    return 0;
}