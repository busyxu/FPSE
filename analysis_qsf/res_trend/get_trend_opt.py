import re

def read_filtered_benchmark_result(filename):
    names = set()
    with open(filename, 'r') as file:
        for line in file:
            parts = line.strip().split(',')
            if len(parts) > 0:
                names.add(parts[0].strip())
    return names

def filter_cov_trend_opt(input_file, output_file, names):
    with open(input_file, 'r') as infile, open(output_file, 'w') as outfile:
        keep_block = False
        for line in infile:
            if line.startswith('=== TestCase :'):
                # Extract the full name from the TestCase line
                match = re.match(r'=== TestCase : (.+?/)', line)
                if match:
                    test_case_name = match.group(1).strip()
                    # Check if the full name matches exactly with any name in the set
                    # print(test_case_name)
                    if test_case_name in names:
                        print(test_case_name)
                        keep_block = True
                        outfile.write(line)
                    else:
                        keep_block = False
            elif line.startswith('=== End'):
                if keep_block:
                    outfile.write(line)
                keep_block = False
            elif keep_block:
                outfile.write(line)

# Replace 'filtered_benchmark_result.txt' with the path to your filtered benchmark results file
names = read_filtered_benchmark_result('/home/aaa/fp-solver/analysis_opt/res_end/results_solver_3.txt')

# Replace 'cov_trend_opt.txt' with the path to your cov_trend_opt file
# Replace 'filtered_cov_trend_opt.txt' with the path to the output file
filter_cov_trend_opt('cov_trend_opt1.txt', 'trend_results_solver_3.txt', names)
