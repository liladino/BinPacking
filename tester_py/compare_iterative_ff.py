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
            max_items=15,
            max_volume=43500, 
            # min_items=5, 
            # max_items=random.randint(5, 20), 
            # max_volume=1e9, 
            printToStdout=False,
            sorted=False)
        
        output_files.append(file)
    return output_files

def compare_first_fit_iterative(first_fit, iterative):
    bin_dict = {
        "xs" : 1,
        "s" : 2,
        "m" : 3,
        "l" : 4,
        "xl" : 5
    }
    
    # the one that packed more wins
    if (first_fit.get("packed") > iterative.get("packed")):
        return 1
    
    if (first_fit.get("packed") < iterative.get("packed")):
        return 2

    # the one that used a smaller container wins
    if (bin_dict[first_fit.get("bin_needed")] < bin_dict[iterative.get("bin_needed")]):
        return 1
    if (bin_dict[first_fit.get("bin_needed")] > bin_dict[iterative.get("bin_needed")]):
        return 2
    
    # the one with the smaller bounding box wins (ratio = volume/bounding_box)
    # if (first_fit.get("bounding_box_volume_ratio") > iterative.get("bounding_box_volume_ratio")):
    #     return 1
    # if (first_fit.get("bounding_box_volume_ratio") < iterative.get("bounding_box_volume_ratio")):
    #     return 2
    
    return 0

def runComparison(packer, algorithm, input_file, results, remove_input = False):
    outfile_iterative = results + "/it.json"
    outfile_first_fit = results + "/ff.json"

    if not os.path.exists(input_file):
        print("No input file found")
        return None
    
    args_set_a = ["--input", input_file, "--algorithm", str(algorithm), "--output", outfile_iterative, "--skipIfDoesntFit"]
    args_set_b = ["--input", input_file, "--algorithm", str(algorithm), "--output", outfile_first_fit, "--skipIfDoesntFit", "--firstFit"]
    
    try:
        # output = 
        subprocess.run([packer] + args_set_a, capture_output=True, text=True)
        # print(output)
        # output =
        subprocess.run([packer] + args_set_b, capture_output=True, text=True)
        # print(output)

        if os.path.exists(outfile_iterative) and os.path.exists(outfile_first_fit):
            try:
                with open(outfile_first_fit, 'r') as f1, open(outfile_iterative, 'r') as f2:
                    text1 = f1.read()
                    text2 = f2.read()
                first_fit = json.loads(text1)[0]
                iterative = json.loads(text2)[0]
            except json.JSONDecodeError as e:
                print("Invalid JSON")
                print(e)
                print("found:")
                print(text1)
                print(text2)
                return None

            winner = compare_first_fit_iterative(first_fit, iterative)
            return winner, first_fit.get("packed", 0), iterative.get("packed", 0)
        else:
            print(f"for input: {input_file}")
            if os.path.exists(outfile_iterative):
                print("Error: First fit result file not found.")
            elif os.path.exists(outfile_first_fit):
                print("Error: Iterative result file not found.")
            else:
                print("Error: No result file found.")
            return None

    except subprocess.CalledProcessError as e:
        print(f"An error occurred while running the program: {e}")
        return None
    finally:
        for f in [outfile_iterative, outfile_first_fit]:
            if os.path.exists(f):
                os.remove(f)
        if remove_input and os.path.exists(input_file):
            os.remove(input_file) 

def evaluateComparison(PACKER, RESULTS, OUTPUT_JSON, inputs, tests):
    json_rows = []
    for name, algo in ALGORITHMS.items():
        ff_dominant = 0
        it_dominant = 0
        ff_total_packed = 0
        it_total_packed = 0

        for i in range(tests):
            res = runComparison(str(PACKER), algo, inputs[i], str(RESULTS), False)
            if res is not None:
                winner, ff_packed, it_packed = res
                ff_total_packed += ff_packed
                it_total_packed += it_packed
                
                if winner == 1:
                    ff_dominant += 1
                elif winner == 2:
                    it_dominant += 1

        ff_avg = ff_total_packed / tests
        it_avg = it_total_packed / tests

        row = { 
            "algorithm": name, 
            "sorted": True, 
            "samples": tests, 
            "first_fit": [{ "dominant": ff_dominant, "avg_packed": ff_avg }], 
            "iterative": [{ "dominant": it_dominant, "avg_packed": it_avg }] 
        }
        json_rows.append(row)

        print(f"Algo {name}:\nfirst fit\tdominant: {ff_dominant}\tavg packed: {ff_avg:.2f}\niterative\tdominant: {it_dominant}\tavg packed: {it_avg:.2f}")

    with open(OUTPUT_JSON, "w") as file:
        json.dump(json_rows, file, indent=4)

    print(f"results written to: {OUTPUT_JSON}")

def main():
    TARGETFOLDER = config.PROJECT_ROOT / "data"
    OUTPUT_JSON = config.PROJECT_ROOT / "results/comparison.json"

    tests = 99
    inputs = generate_inputs(str(config.DATASET), str(TARGETFOLDER), tests)

    print("inputs generated")
    evaluateComparison(config.PACKER_EXECUTABLE, config.RESULTS_DIR, OUTPUT_JSON, inputs, tests)

if __name__ == "__main__":
    main()
