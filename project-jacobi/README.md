# Jacobi parallel mathod

Compile with `g++ -O3 -pthread -g -std=c++20 jacobi.cpp -o jacobi`.
Execution:
- `./jacobi <matrix-size> <nw>` to create a random matrix with elements in $[-128,128]$ and `max_iterations=1024`
- `./jacobi inputfile <nw>` to execute it on the `input.txt` file with user-defined inputs. In the input file, the order of the arguments is: size of the matrix, matrix A, matrix B and the maximum number of iterations.