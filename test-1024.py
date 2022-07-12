# Get the number of CPU cores
import os
cores = os.cpu_count()
print("Running with max", cores, "workers.\n")

# Compile the program
import subprocess
subprocess.run(['make', 'seq'])

# Functions to plot
x = list(range(1, cores+1))
y_seq = []
y_std = []
y_ff  = []
y_std_sp = []
y_ff_sp = []
y_std_sc = []
y_ff_sc = []
y_std_eff = []
y_ff_eff = []

# Run the code sequentially
best_seq = float('inf')
print("Running the sequential version\n")
for _ in range(10):
    # Run always with the same seed to have the same matrix
    result = subprocess.getoutput(f'./jacobi 1024 1 1')
    tokenized = result.split()
    best_seq = min(best_seq, float(tokenized[1]))
y_seq = [best_seq] * len(x)

# Compile the program
import subprocess
subprocess.run(['make', 'par'])

# Run the code on a matrix 1024x1024 with growing number of workers
print("Running the parallel versions")
for nw in x:
    print("Running with", nw, "workers")
    # Run jacobi for 10 times each iteration and get the average
    avg_std = 0
    avg_ff  = 0
    for _ in range(10):
        # Run always with the same seed to have the same matrix
        result = subprocess.getoutput(f'./jacobi 1024 1 {nw}')
        tokenized = result.split()
        avg_std += float(tokenized[1])
        avg_ff  += float(tokenized[4])
    avg_std /= 10
    avg_ff /= 10
    std_sp = best_seq / avg_std
    ff_sp = best_seq / avg_ff
    y_std.append(avg_std)
    y_ff.append(avg_ff)
    y_std_sp.append(std_sp)
    y_ff_sp.append(ff_sp)
    y_std_sc.append(y_std[0] / avg_std)
    y_ff_sc.append(y_ff[0] / avg_ff)
    y_std_eff.append(std_sp / nw)
    y_ff_eff.append(ff_sp / nw)

# Write results to file and read them
with open("results.txt", "w+") as f:
    f.write("Workers: ")
    f.write(str(x))
    f.write("\nSequential:\n")
    f.write(str(y_seq))
    f.write("\nSTD threads:\n")
    f.write(str(y_std))
    f.write("\nFastFlow:\n")
    f.write(str(y_ff))
    f.write("\nSTD parallel speedup:\n")
    f.write(str(y_std_sp))
    f.write("\nFastFlow speedup:\n")
    f.write(str(y_ff_sp))
    f.write("\nSTD parallel scalability:\n")
    f.write(str(y_std_sc))
    f.write("\nFastFlow scalability:\n")
    f.write(str(y_ff_sc))
    f.write("\nSTD parallel efficiency:\n")
    f.write(str(y_std_eff[1:]))
    f.write("\nFastFlow efficiency:\n")
    f.write(str(y_ff_eff[1:]))
with open("results.txt", "r") as f:
    print(f.read())
