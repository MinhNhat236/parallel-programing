#include <mpi.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <algorithm>

using namespace std;

// Generate square matrix with random integers in flat form
vector<int> generateMatrixFlat(int size)
{
    vector<int> matrix(size * size);
    for (int i = 0; i < size * size; ++i)
    {
        matrix[i] = rand() % 10;
    }
    return matrix;
}

// Save flat matrix to file
void saveMatrixToFile(const string &filename, const vector<int> &matrix, int rows, int cols)
{
    ofstream file(filename);
    if (!file)
    {
        cerr << "Error: Unable to open file " << filename << endl;
        return;
    }

    file << rows << " " << cols << '\n';
    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < cols; ++j)
        {
            file << matrix[i * cols + j] << ' ';
        }
        file << '\n';
    }
}

// Save timing results
void saveExecutionTime(const string &filename, int size, int num_procs, double timeElapsed)
{
    ofstream file(filename, ios::app);
    if (!file)
    {
        cerr << "Error: Cannot open timing file " << filename << endl;
        return;
    }
    file << size << ' ' << num_procs << ' ' << timeElapsed << '\n';
}

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);

    int rank = 0;
    int num_procs = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    srand(static_cast<unsigned>(time(nullptr)) + rank);

    // Sizes required by the assignment
    vector<int> sizes = {200, 400, 800, 1200, 1600, 2000};

    if (rank == 0)
    {
        ofstream clearFile("mpi_execution_times.txt");
        clearFile << "MatrixSize Processes TimeSeconds\n";
    }

    for (int size : sizes)
    {
        vector<int> A;
        vector<int> B(size * size);
        vector<int> C;

        if (rank == 0)
        {
            A = generateMatrixFlat(size);
            B = generateMatrixFlat(size);
            C.resize(size * size);
        }

        // Broadcast full matrix B to all processes
        MPI_Bcast(B.data(), size * size, MPI_INT, 0, MPI_COMM_WORLD);

        // Distribute rows as evenly as possible
        vector<int> sendCounts(num_procs);
        vector<int> displs(num_procs);

        int baseRows = size / num_procs;
        int remainder = size % num_procs;

        int offset = 0;
        for (int p = 0; p < num_procs; ++p)
        {
            int rowsForProc = baseRows + (p < remainder ? 1 : 0);
            sendCounts[p] = rowsForProc * size;
            displs[p] = offset;
            offset += sendCounts[p];
        }

        int localElements = sendCounts[rank];
        int localRows = localElements / size;

        vector<int> localA(localElements);
        vector<int> localC(localElements, 0);

        MPI_Scatterv(
            rank == 0 ? A.data() : nullptr,
            sendCounts.data(),
            displs.data(),
            MPI_INT,
            localA.data(),
            localElements,
            MPI_INT,
            0,
            MPI_COMM_WORLD);

        MPI_Barrier(MPI_COMM_WORLD);
        double start = MPI_Wtime();

        // Local matrix multiplication
        for (int i = 0; i < localRows; ++i)
        {
            for (int j = 0; j < size; ++j)
            {
                int sum = 0;
                for (int k = 0; k < size; ++k)
                {
                    sum += localA[i * size + k] * B[k * size + j];
                }
                localC[i * size + j] = sum;
            }
        }

        MPI_Barrier(MPI_COMM_WORLD);
        double end = MPI_Wtime();
        double localTime = end - start;
        double maxTime = 0.0;

        // We take the maximum time across all processes as the real parallel time
        MPI_Reduce(&localTime, &maxTime, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

        MPI_Gatherv(
            localC.data(),
            localElements,
            MPI_INT,
            rank == 0 ? C.data() : nullptr,
            sendCounts.data(),
            displs.data(),
            MPI_INT,
            0,
            MPI_COMM_WORLD);

        if (rank == 0)
        {
            string resultFile = "matrix_result_mpi_" + to_string(size) + ".txt";
            saveMatrixToFile(resultFile, C, size, size);
            saveExecutionTime("mpi_execution_times.txt", size, num_procs, maxTime);

            cout << "Matrix size: " << size << "x" << size
                 << ", Processes: " << num_procs
                 << ", Time: " << maxTime << " seconds" << endl;
        }
    }

    MPI_Finalize();
    return 0;
}
