import random
import subprocess
import json
import os
import math
import config
from config import ALGORITHMS
from input_gen import generate_random_items
from pathlib import Path

def generate_inputs(dataset, target_folder, number_of_files):
    output_files = []
    for i in range(number_of_files):
        file = target_folder + "/items" + str(i).zfill(int(math.log10(number_of_files)+1)) + ".txt" 

        if os.path.exists(file):
            output_files.append(file)
            continue

        generate_random_items(
            input_path=dataset, 
            output_path=file, 
            min_items=5,
            max_items=1500,#random.randint(5, 25), 
            max_volume=1e9, 
            printToStdout=False,
            sorted=True) 
        
        output_files.append(file)
    return output_files

def runComparison(packer, algorithm, input_file, results):
    outfile_iterative = results + "/it.json"
    outfile_first_fit = results + "/ff.json"

    if not os.path.exists(input_file):
        print(f"No input file found: {input_file}")
        return None
    
    args_set_a = ["--input", input_file, "--algorithm", str(algorithm), "--output", outfile_iterative, "--shipEverything"]
    args_set_b = ["--input", input_file, "--algorithm", str(algorithm), "--output", outfile_first_fit, "--shipEverything", "--firstFit"]
    
    try:
        subprocess.run([packer] + args_set_a, capture_output=True, text=True)
        subprocess.run([packer] + args_set_b, capture_output=True, text=True)

        if os.path.exists(outfile_iterative) and os.path.exists(outfile_first_fit):
            try:
                with open(outfile_first_fit, 'r') as f1, open(outfile_iterative, 'r') as f2:
                    text1 = f1.read()
                    text2 = f2.read()
                
                ff_json_data = json.loads(text1)
                it_json_data = json.loads(text2)
                # print(ff_json_data)
                # input()
                ff_bin_needed = len(ff_json_data)
                it_bin_needed = len(it_json_data)

            except (json.JSONDecodeError, IndexError) as e:
                print("Invalid JSON")
                print(e)
                print("found:")
                print(text1)
                print(text2)
                return None
                
            return ff_bin_needed, it_bin_needed
        else:
            print(f"for input: {input_file}")
            print("Error: One or both result files not found.")
            return None

    except subprocess.CalledProcessError as e:
        print(f"An error occurred while running the program: {e}")
        return None
    finally:
        for f in [outfile_iterative, outfile_first_fit]:
            if os.path.exists(f):
                os.remove(f)

def evaluateComparison(PACKER, RESULTS, OUTPUT_JSON, inputs, tests):
    json_rows = []
    
    for name, algo in ALGORITHMS.items():
        ff_total_bins_needed = 0
        it_total_bins_needed = 0

        for i in range(tests):
            res = runComparison(str(PACKER), algo, inputs[i], str(RESULTS))
            if res is not None:
                ff_bin_needed, it_bin_needed = res
                
                ff_total_bins_needed += ff_bin_needed
                it_total_bins_needed += it_bin_needed

        ff_avg_bin_needed = ff_total_bins_needed / tests if tests > 0 else 0
        it_avg_bin_needed = it_total_bins_needed / tests if tests > 0 else 0

        row = { 
            "algorithm": name, 
            "sorted": True, 
            "samples": tests, 
            "first_fit": [{ "avg_bin_needed": ff_avg_bin_needed }], 
            "iterative": [{ "avg_bin_needed": it_avg_bin_needed }] 
        }
        json_rows.append(row)

        print(f"Algo {name}:\nfirst fit\tavg bin needed: {ff_avg_bin_needed:.2f}\niterative\tavg bin needed: {it_avg_bin_needed:.2f}\n")

    with open(OUTPUT_JSON, "w") as file:
        json.dump(json_rows, file, indent=4)

    print(f"Results written to: {OUTPUT_JSON}")

def main():
    TARGETFOLDER = config.PROJECT_ROOT / "data"
    OUTPUT_JSON = config.PROJECT_ROOT / "results/comparison.json"
    tests = 100

    print("Generating sorted inputs...")
    inputs_sorted = generate_inputs(str(config.DATASET), str(TARGETFOLDER), tests)
    
    print("\nEvaluating sorted inputs...")
    evaluateComparison(config.PACKER_EXECUTABLE, config.RESULTS_DIR, OUTPUT_JSON, inputs_sorted, tests)
    
    for f in inputs_sorted:
        if os.path.exists(f):
            os.remove(f)

if __name__ == "__main__":
    main()
