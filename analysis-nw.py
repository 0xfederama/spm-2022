# Warning
print("WARNING:")
print("This process will take approx. 15 min on an Intel(R) Xeon(R) Gold 5120 CPU.")
print("If you have a slower CPU, it may take much more time.")
print("Change the x array at line 18 if you want to change the tests with the number of workers.\n")

# Compile the program
import subprocess
subprocess.run(['make'])
print("Compilation completed.")

# Get the number of CPU cores
import os
cores = os.cpu_count()
print("Running with max", cores, "workers.\n")

# Functions to plot
x = list(range(1, cores+1))
y_seq = []
y_std = []
y_ff  = []
y_std_sp = []
y_ff_sp = []
y_std_sc = []
y_ff_sc = []

# Run the code on a matrix 1024x1024 with growing number of workers
for i in x:
    print("Running with", i, "workers")
    # Run jacobi for 10 times each iteration and get the average
    best_seq = float('inf')
    avg_std = 0
    avg_ff  = 0
    avg_std_sp = 0
    avg_ff_sp = 0
    for j in range(10):
        # Run always with the same seed to have the same matrix
        result = subprocess.getoutput(f'./jacobi 1024 1 {i}')
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
    y_std_sc.append(y_std[0] / avg_std)
    y_ff_sc.append(y_ff[0] / avg_ff)

# Print results
f = open("results.txt", "w")
print("Sequential:")
print(y_seq)
f.write("Sequential:\n")
f.write(str(y_seq))
print("STD threads:")
print(y_std)
f.write("STD threads:\n")
f.write(str(y_std))
print("FastFlow:")
print(y_ff)
f.write("\nFastFlow:\n")
f.write(str(y_ff))
print("STD parallel speedup:")
print(y_std_sp)
f.write("\nSTD parallel speedup:\n")
f.write(str(y_std_sp))
print("FastFlow speedup:")
print(y_ff_sp)
f.write("\nFastFlow speedup:\n")
f.write(str(y_ff_sp))
print("STD parallel scalability:")
print(y_std_sc)
f.write("\nSTD parallel scalability:\n")
f.write(str(y_std_sc))
print("FastFlow scalability:")
print(y_ff_sc)
f.write("\nFastFlow scalability:\n")
f.write(str(y_ff_sc))
