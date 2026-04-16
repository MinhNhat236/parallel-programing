#include <mpi.h>
#include <iostream>
#include "Matrix.h"
#include <chrono>
#include <fstream>


template<typename T>
Matrix<T> parallel_multiply(const Matrix<T>& m1, const Matrix<T>& m2) {
    int rank, size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (m1.columns() != m2.rows()) {
        throw "Матрицы не совместны";
    }

    size_t rows = m1.rows();
    size_t columns = m2.columns();

    Matrix<T> result;
    if (rank == 0) {
        result = Matrix<T>(rows, columns);
    }

    T* local_result = new T[rows * columns]{ 0 }; // Инициализация динамического массива нулями
    //for (size_t i = 0; i < rows * columns; i++) {
    //local_result[i] = 0;
    //}

    size_t work = (m1.columns() / size);
    size_t local_start = rank * work;

    size_t local_end;
    local_end = local_start + work;

    if (rank == size - 1) {
        local_end = m1.columns();
    }
    //std::cout << rank << ": [" << local_start << ", " << local_end << "]\n";

    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < columns; j++) {
            /* */
            for (size_t r = local_start; r < local_end; r++) {
                local_result[i * m1.rows() + j] += m1(i, r) * m2(r, j);
            }
        }
    }
    MPI_Reduce(local_result, result.data(), m1.rows() * m2.columns(), MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    delete[] local_result;

    return result;
}


int main(int argc, char** argv) {
#ifdef _WIN32
    system("chcp 65001 > nul");
#endif
    std::ios::sync_with_stdio(false);
    // std::cout << std::setprecision(std::numeric_limits<double>::max_digits10);

    if (MPI_Init(&argc, &argv) != MPI_SUCCESS) {
        std::cerr << "Не удалось инициализировать MPI!\n";
        return -1;
    }

    int rank, size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);


    Matrix<double> matrix;
    int n, m;
    if (rank == 0) {
        std::cin >> n >> m;
        matrix = Matrix<double>(n, m);
        for (size_t i = 0; i < n; i++) {
            for (size_t j = 0; j < m; j++) {
                std::cin >> matrix(i, j);
            }
        }
    }
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&m, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank != 0)
        matrix = Matrix<double>(n, m);
    MPI_Bcast(matrix.data(), n * m, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        std::cout << "threads: " << size << "\n";
        std::cout << "parallel_multiply() [size:" << matrix.rows() << "x" << matrix.rows() << "]\n";
    }

    auto start = std::chrono::steady_clock::now();
    Matrix<double> result = parallel_multiply(matrix, matrix);
    auto end = std::chrono::steady_clock::now();

    if (rank == 0) {
        auto elapsed_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
        double elapsed_ms = (double)elapsed_ns / 1000000;
        std::cout << "time elapsed: " << elapsed_ms << "ms\n";
        std::cout << "result: " << result << "\n";
    }

    MPI_Finalize();
    return 0;
}