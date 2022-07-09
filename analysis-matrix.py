# Warning
print("WARNING:")
print("This process will take approx. 30m on an Intel(R) Xeon(R) Gold 5120 CPU.")
print("If you have a slower CPU with less cores, it may take much more time.")
print("Change the x array at line 19 if you want to change the dimensions of the matrix, in order to take less time.\n")

# Compile the program
import subprocess
subprocess.run(['make'])
print("Compilation completed.")

# Get the number of CPU cores
import os
cores = os.cpu_count()
print("Running on", cores, "workers.\n")

# Functions to plot
x = [64, 128, 256, 512, 768, 1024, 1536, 2048, 3072, 4096, 5120, 6144, 7168, 8192, 10240, 16384, 20480]
y_seq = []
y_std = []
y_ff  = []
y_std_sp = []
y_ff_sp = []

# Run the code on matrix of dimensions in x from 2^6=64 to 10240
for i in x:
    print("Running on matrix of dimension", i, "x", i)
    # Run jacobi for 10 times each iteration and get the average
    best_seq = float('inf')
    avg_std = 0
    avg_ff  = 0
    avg_std_sp = 0
    avg_ff_sp = 0
    for j in range(10):
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

# Print results
f = open("results.txt", "w")
print("Sequential:")
print(y_seq)
f.write("Sequential:\n")
f.write(str(y_seq))
print("STD threads:")
print(y_std)
f.write("\nSTD threads:\n")
f.write(str(y_std))
print("FastFlow:")
print(y_ff)
f.write("\nFastFlow:\n")
f.write(str(y_ff))
print("STD threads speedup:")
print(y_std_sp)
f.write("\nSTD threads speedup:\n")
f.write(str(y_std_sp))
print("FastFlow speedup:")
print(y_ff_sp)
f.write("\nFastFlow speedup:\n")
f.write(str(y_ff_sp))
