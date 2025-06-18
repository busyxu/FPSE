import subprocess
import os

# Set the directory path where your C programs are located
c_programs_dir = '/home/aaa/gsl/drives/bench_complexNum/complexNum'
output_dir = '/home/aaa/gsl/drives/bench_complexNum/complexNum/temp'

# Get a list of all the C program files in the directory
c_programs = [file for file in os.listdir(c_programs_dir) if file.endswith('.c')]

cnt = 0
# Compile and run each C program
for program in c_programs:
    print(program)
    # Build the compilation command
    compilation_command1 = f'gcc -Wall -I/home/aaa/gsl/install/include -c {os.path.join(c_programs_dir, program)} -o {os.path.join(output_dir, program[:-2]+".o")}'
    compilation_command2 = f'gcc -L/home/aaa/gsl/install/lib {os.path.join(output_dir, program[:-2]+ ".o")} -lgsl -lgslcblas -lm -o {os.path.join(output_dir, program[:-2]+".out")}'
    # Compile the C program
    subprocess.run(compilation_command1, shell=True, check=True)
    subprocess.run(compilation_command2, shell=True, check=True)

    # Run the compiled program
    execution_command = os.path.join(output_dir, program[:-2]+".out")
    subprocess.run(execution_command, shell=True)
    cnt+=1

print('total:', cnt)


