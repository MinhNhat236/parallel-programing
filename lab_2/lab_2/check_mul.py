import numpy as np

def read_matrix_from_file(filename):
    """Read a matrix from a file saved by the C++ program."""
    with open(filename, 'r') as f:
        lines = f.readlines()
        # Read number of rows and columns from the first line
        rows, cols = map(int, lines[0].split())
        # Read matrix data
        matrix = []
        for line in lines[1:]:
            row = list(map(int, line.split()))
            matrix.append(row)
    return np.array(matrix)

def multiply_and_compare(size, result_matrix):
    """Multiply two matrices using NumPy and compare with the C++ result."""
    fileA = f"matrixA_{size}.txt"
    fileB = f"matrixB_{size}.txt"
    
    A = read_matrix_from_file(fileA)
    B = read_matrix_from_file(fileB)
    
    expected_result = np.dot(A, B)
    
    if np.array_equal(expected_result, result_matrix):
        print(f"Matrix multiplication result for {size}x{size} is CORRECT!")
    else:
        print(f"Matrix multiplication result for {size}x{size} is WRONG!")
        print("Number of differing elements:", np.sum(expected_result != result_matrix))
        print("Expected matrix (NumPy):")
        print(expected_result)
        print("Actual matrix (C++):")
        print(result_matrix)

def save_result_to_file(result, filename):
    """Save result matrix to file for later verification."""
    np.savetxt(filename, result, fmt='%d')

def main():
    sizes = [100, 200, 400, 1000]
    
    for size in sizes:
        fileA = f"matrixA_{size}.txt"
        fileB = f"matrixB_{size}.txt"
        
        A = read_matrix_from_file(fileA)
        B = read_matrix_from_file(fileB)
        
        # Compute result using NumPy
        result = np.dot(A, B)
        result_file = f"result_{size}.txt"
        save_result_to_file(result, result_file)

        multiply_and_compare(size, result) 

if __name__ == "__main__":
    main()