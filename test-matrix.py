# Compile the program
import subprocess
subprocess.run(['make', 'all'])
print("Compilation completed.")

# Get the number of CPU cores
import os
cores = os.cpu_count()
print("Running on", cores, "workers.\n")

# Functions to plot
x = [64, 128, 256, 512, 768, 1024, 1536, 2048, 3072, 4096, 5120]
y_seq = []
y_std = []
y_ff  = []
y_std_sp = []
y_ff_sp = []
y_std_eff = []
y_ff_eff = []

# Run the code on matrix of dimensions in x from 2^6=64 to 10240
for i in x:
    print("Running on matrix of dimension", i, "x", i)
    # Run jacobi for 10 times each iteration and get the average
    best_seq = float('inf')
    avg_std = 0
    avg_ff  = 0
    for _ in range(10):
        # Run always with the same seed to have the same matrix
        result = subprocess.getoutput(f'./jacobi {i} 1 {cores}')
        tokenized = result.split()
        best_seq = min(best_seq, float(tokenized[1]))
        avg_std += float(tokenized[4])
        avg_ff  += float(tokenized[7])
    avg_std /= 10
    avg_ff /= 10
    std_sp = best_seq / avg_std
    ff_sp = best_seq / avg_ff
    y_seq.append(best_seq)
    y_std.append(avg_std)
    y_ff.append(avg_ff)
    y_std_sp.append(std_sp)
    y_ff_sp.append(ff_sp)
    y_std_eff.append(std_sp / cores)
    y_ff_eff.append(ff_sp / cores)

# Write results to file and read them
with open("results.txt", "w+") as f:
    f.write("Matrices: ")
    f.write(str(x))
    f.write("\nSequential:\n")
    f.write(str(y_seq))
    f.write("\nSTD threads:\n")
    f.write(str(y_std))
    f.write("\nFastFlow:\n")
    f.write(str(y_ff))
    f.write("\nSTD threads speedup:\n")
    f.write(str(y_std_sp))
    f.write("\nFastFlow speedup:\n")
    f.write(str(y_ff_sp))
    f.write("\nSTD threads efficiency:\n")
    f.write(str(y_std_eff))
    f.write("\nFastFlow efficiency:\n")
    f.write(str(y_ff_eff))
with open("results.txt", "r") as f:
    print(f.read())
