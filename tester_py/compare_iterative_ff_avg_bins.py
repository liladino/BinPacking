import random
import subprocess
import json
import os
import math
import config
from config import ALGORITHMS
from input_gen import generate_random_items
from pathlib import Path

def generate_inputs(dataset, target_folder, number_of_files, sorted_):
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
            max_items=random.randint(5, 20), 
            max_volume=1e9, 
            printToStdout=False,
            sorted=sorted_)
        
        output_files.append(file)
    return output_files

def runComparison(packer, algorithm, input_file, results):
    outfile_iterative = results + "/it.json"

    if not os.path.exists(input_file):
        print(f"No input file found: {input_file}")
        return None
    
    args_set = ["--input", input_file, "--algorithm", str(algorithm), "--output", outfile_iterative, "--shipEverything"]
    
    try:
        subprocess.run([packer] + args_set, capture_output=True, text=True)

        if os.path.exists(outfile_iterative):
            try:
                with open(outfile_iterative, 'r') as f:
                    text = f.read()
                
                it_json_data = json.loads(text)
                it_bin_needed = len(it_json_data)

            except (json.JSONDecodeError, IndexError) as e:
                print("Invalid JSON")
                print(e)
                print("found:")
                print(text)
                return None
                
            return it_bin_needed
        else:
            print(f"for input: {input_file}")
            print("Error: Iterative result file not found.")
            return None

    except subprocess.CalledProcessError as e:
        print(f"An error occurred while running the program: {e}")
        return None
    finally:
        if os.path.exists(outfile_iterative):
            os.remove(outfile_iterative)

def evaluateAlgorithmRun(PACKER, RESULTS, inputs, tests):
    algo_results = {}
    
    for name, algo in ALGORITHMS.items():
        it_total_bins_needed = 0

        for i in range(tests):
            res = runComparison(str(PACKER), algo, inputs[i], str(RESULTS))
            if res is not None:
                it_total_bins_needed += res

        it_avg_bin_needed = it_total_bins_needed / tests
        algo_results[name] = it_avg_bin_needed

        print(f"Algo {name}:\tavg bin needed: {it_avg_bin_needed:.2f}")
        
    return algo_results

def main():
    TARGETFOLDER = config.PROJECT_ROOT / "data"
    OUTPUT_JSON = config.PROJECT_ROOT / "results/comparison.json"
    tests = 10000

    print("Generating unsorted inputs...")
    inputs_unsorted = generate_inputs(str(config.DATASET), str(TARGETFOLDER), tests, sorted_=False)
    
    print("\nEvaluating unsorted inputs:")
    unsorted_results = evaluateAlgorithmRun(config.PACKER_EXECUTABLE, config.RESULTS_DIR, inputs_unsorted, tests)

    for f in inputs_unsorted:
        if os.path.exists(f):
            os.remove(f)

    print("\nGenerating sorted inputs...")
    inputs_sorted = generate_inputs(str(config.DATASET), str(TARGETFOLDER), tests, sorted_=True)
    
    print("\nEvaluating sorted inputs:")
    sorted_results = evaluateAlgorithmRun(config.PACKER_EXECUTABLE, config.RESULTS_DIR, inputs_sorted, tests)
    
    for f in inputs_sorted:
        if os.path.exists(f):
            os.remove(f)

    json_rows = []
    for name in ALGORITHMS.keys():
        row = { 
            "algorithm": name, 
            "samples": tests, 
            "unsorted": { "avg_bin_needed": unsorted_results.get(name, 0) }, 
            "sorted": { "avg_bin_needed": sorted_results.get(name, 0) } 
        }
        json_rows.append(row)

    with open(OUTPUT_JSON, "w") as file:
        json.dump(json_rows, file, indent=4)

    print(f"\nResults successfully written to: {OUTPUT_JSON}")

if __name__ == "__main__":
    main()
