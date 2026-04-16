#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <cuda_runtime.h>

using namespace std;
using namespace std::chrono;

#define CHECK_CUDA(call)                                                     \
    do {                                                                     \
        cudaError_t err = (call);                                            \
        if (err != cudaSuccess) {                                            \
            cerr << "CUDA error at " << __FILE__ << ":" << __LINE__       \
                 << " -> " << cudaGetErrorString(err) << endl;              \
            exit(EXIT_FAILURE);                                              \
        }                                                                    \
    } while (0)

__global__ void multiplyMatrixGPU(const int* A, const int* B, int* C, int n) {
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int col = blockIdx.x * blockDim.x + threadIdx.x;

    if (row < n && col < n) {
        int sum = 0;
        for (int k = 0; k < n; ++k) {
            sum += A[row * n + k] * B[k * n + col];
        }
        C[row * n + col] = sum;
    }
}

void multiplyMatrixCPU(const vector<int>& A, const vector<int>& B, vector<int>& C, int n) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            int sum = 0;
            for (int k = 0; k < n; ++k) {
                sum += A[i * n + k] * B[k * n + j];
            }
            C[i * n + j] = sum;
        }
    }
}

bool compareMatrices(const vector<int>& A, const vector<int>& B) {
    if (A.size() != B.size()) return false;
    for (size_t i = 0; i < A.size(); ++i) {
        if (A[i] != B[i]) return false;
    }
    return true;
}

void fillMatrix(vector<int>& M) {
    for (size_t i = 0; i < M.size(); ++i) {
        M[i] = rand() % 10 + 1;
    }
}

int main() {
    srand(static_cast<unsigned>(time(nullptr)));

    vector<int> sizes = {200, 400, 800, 1200, 1600, 2000};
    vector<int> blockSizes = {8, 16, 32};

    cout << "CPU + CUDA MATRIX MULTIPLICATION BENCHMARK" << endl;

    cudaDeviceProp prop;
    CHECK_CUDA(cudaGetDeviceProperties(&prop, 0));

    cout << "GPU: " << prop.name << endl;
    cout << "Max threads per block: " << prop.maxThreadsPerBlock << endl;
    cout << "\n================ RESULT ================\n";

    ofstream csv("benchmark_summary.csv");
    csv << "size,method,block_size,grid_x,grid_y,time_sec,gops,correct\n";

    for (int n : sizes) {
        size_t bytes = static_cast<size_t>(n) * n * sizeof(int);

        vector<int> A(n * n);
        vector<int> B(n * n);
        vector<int> C_cpu(n * n, 0);
        vector<int> C_gpu(n * n, 0);

        fillMatrix(A);
        fillMatrix(B);

        cout << "\nMatrix size: " << n << "x" << n << endl;

        // ==============================
        // CPU benchmark
        // ==============================
        auto cpu_start = high_resolution_clock::now();
        multiplyMatrixCPU(A, B, C_cpu, n);
        auto cpu_end = high_resolution_clock::now();

        double cpu_time_sec = duration<double>(cpu_end - cpu_start).count();
        double ops = 2.0 * static_cast<double>(n) * n * n;
        double cpu_gops = ops / cpu_time_sec / 1e9;

        cout << "  CPU      | Time: " << cpu_time_sec
             << " s | GOPS: " << cpu_gops << endl;

        csv << n << ",CPU,-1,-1,-1,"
            << cpu_time_sec << ","
            << cpu_gops << ",1\n";

        // ==============================
        // GPU benchmark
        // ==============================
        int* d_A = nullptr;
        int* d_B = nullptr;
        int* d_C = nullptr;

        CHECK_CUDA(cudaMalloc(&d_A, bytes));
        CHECK_CUDA(cudaMalloc(&d_B, bytes));
        CHECK_CUDA(cudaMalloc(&d_C, bytes));

        CHECK_CUDA(cudaMemcpy(d_A, A.data(), bytes, cudaMemcpyHostToDevice));
        CHECK_CUDA(cudaMemcpy(d_B, B.data(), bytes, cudaMemcpyHostToDevice));

        for (int blockSize : blockSizes) {
            if (blockSize * blockSize > prop.maxThreadsPerBlock) {
                cout << "  GPU Block " << blockSize << "x" << blockSize
                     << " skipped (too many threads per block)" << endl;
                continue;
            }

            CHECK_CUDA(cudaMemset(d_C, 0, bytes));

            dim3 threadsPerBlock(blockSize, blockSize);
            dim3 blocksPerGrid((n + blockSize - 1) / blockSize,
                               (n + blockSize - 1) / blockSize);

            cudaEvent_t start, stop;
            CHECK_CUDA(cudaEventCreate(&start));
            CHECK_CUDA(cudaEventCreate(&stop));

            CHECK_CUDA(cudaEventRecord(start));
            multiplyMatrixGPU<<<blocksPerGrid, threadsPerBlock>>>(d_A, d_B, d_C, n);
            CHECK_CUDA(cudaGetLastError());
            CHECK_CUDA(cudaEventRecord(stop));
            CHECK_CUDA(cudaEventSynchronize(stop));

            float time_ms = 0.0f;
            CHECK_CUDA(cudaEventElapsedTime(&time_ms, start, stop));
            CHECK_CUDA(cudaMemcpy(C_gpu.data(), d_C, bytes, cudaMemcpyDeviceToHost));

            double gpu_time_sec = time_ms / 1000.0;
            double gpu_gops = ops / gpu_time_sec / 1e9;
            bool correct = compareMatrices(C_cpu, C_gpu);

            cout << "  GPU " << blockSize << "x" << blockSize
                 << " | Grid: " << blocksPerGrid.x << "x" << blocksPerGrid.y
                 << " | Time: " << gpu_time_sec << " s"
                 << " | GOPS: " << gpu_gops
                 << " | Correct: " << (correct ? "YES" : "NO") << endl;

            csv << n << ",GPU,"
                << blockSize << ","
                << blocksPerGrid.x << ","
                << blocksPerGrid.y << ","
                << gpu_time_sec << ","
                << gpu_gops << ","
                << (correct ? 1 : 0) << "\n";

            CHECK_CUDA(cudaEventDestroy(start));
            CHECK_CUDA(cudaEventDestroy(stop));
        }

        CHECK_CUDA(cudaFree(d_A));
        CHECK_CUDA(cudaFree(d_B));
        CHECK_CUDA(cudaFree(d_C));
    }

    csv.close();

    cout << "\nSaved summary to benchmark_summary.csv" << endl;
    cout << "DONE!" << endl;
    return 0;
}
