import subprocess
import json
import os
import math
import config
from input_gen import generate_random_items
from pathlib import Path

def generate_inputs(dataset, target_folder, number_of_files, max_items, max_volume):
    output_files = []
    for i in range(number_of_files):
        file = target_folder + "/items" + str(i).zfill(int(math.log10(number_of_files)+1)) + ".txt" 

        if os.path.exists(file):
            output_files.append(file)
            continue

        generate_random_items(
            dataset, 
            file, 
            max_items, 
            max_volume, 
            False,
            True)
        output_files.append(file)
    return output_files

def compare_first_fit_iterative(path_first_fit, path_iterative):
    bin_dict = {
        "xs" : 1,
        "s" : 2,
        "m" : 3,
        "l" : 4,
        "xl" : 5
    }
    # temp = input()
    # print(temp)    
    try:
        with open(path_first_fit, 'r') as f1, open(path_iterative, 'r') as f2:
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
        return
    
    if (first_fit.get("packed") > iterative.get("packed")):
        return 1
    
    if (first_fit.get("packed") < iterative.get("packed")):
        return 2

    # print(bin_dict[first_fit.get("bin_needed")])
    # print(bin_dict[iterative.get("bin_needed")])

    if (bin_dict[first_fit.get("bin_needed")] < bin_dict[iterative.get("bin_needed")]):
        return 1
    if (bin_dict[first_fit.get("bin_needed")] > bin_dict[iterative.get("bin_needed")]):
        return 2
    return 0

def runComparison(packer, algorithm, input, results, remove_input = False):
    outfile_iterative = results + "/it.json"
    outfile_first_fit = results + "/ff.json"

    if not os.path.exists(input):
        print("No input file found")
        return 0
    
    args_set_a = ["--input", input, "--algorithm", str(algorithm), "--output", outfile_iterative, "--stopIfDoesntFit"]
    args_set_b = ["--input", input, "--algorithm", str(algorithm), "--output", outfile_first_fit, "--stopIfDoesntFit", "--firstFit"]
    
    try:
        # output = 
        subprocess.run([packer] + args_set_a, capture_output=True, text=True)
        # print(output)
        # output =
        subprocess.run([packer] + args_set_b, capture_output=True, text=True)
        # print(output)

        if os.path.exists(outfile_iterative) and os.path.exists(outfile_first_fit):
            return compare_first_fit_iterative(outfile_first_fit, outfile_iterative)
        else:
            print("for input:")
            print(input)
            if os.path.exists(outfile_iterative):
                print("Error: First fit result file not found.")
            elif os.path.exists(outfile_first_fit):
                print("Error: Iterative result file not found.")
            else:
                print("Error: No result file not found.")

    except subprocess.CalledProcessError as e:
        print(f"An error occurred while running the program: {e}")
    finally:
        for f in [outfile_iterative, outfile_first_fit]:
            if os.path.exists(f):
                os.remove(f)
        if remove_input:
            if os.path.exists(input):
                os.remove(input) 

def evaluateComparison(PACKER, RESULTS, OUTPUT_JSON, inputs, tests):
    json_rows = []
    algorithms = 5
    for algo in range(algorithms):
        results = [0, 0]

        for i in range(tests):
            x = runComparison(str(PACKER), algo, inputs[i], str(RESULTS), False) # (True if algo == algorithms-1 else False))
            if x is not None:
                if x != 0:
                    results[x - 1] += 1
                # if x == 2:
                #     print(inputs[i])

        row = { "algorithm": algo, "sorted": True, "samples": tests, "first_fit": results[0], "iterative": results[1] }
        json_rows.append(row)

        print(f"Algo {algo}:\nfirst fit\t{results[0]}\niterative\t{results[1]}")

    with open(OUTPUT_JSON, "w") as file:
        json.dump(json_rows, file, indent=4)

    print(f"results written to: {OUTPUT_JSON}")

def numberOfPacked(packer, algorithm, input, results, remove_input = False):
    outfile_iterative = results + "/it.json"
    args_set = ["--input", input, "--algorithm", str(algorithm), "--output", outfile_iterative]
    try: 
        subprocess.run([packer] + args_set, capture_output=True, text=True)
        
        if os.path.exists(outfile_iterative):
            try:
                with open(outfile_iterative, 'r') as f1:
                    text1 = f1.read()
                iterative = json.loads(text1)

            except json.JSONDecodeError as e:
                print("Invalid JSON")
                print(e)
                print("found:")
                print(text1)
                return 0
            return iterative.get("packed")
        else:
            print("for input:")
            print(input)
            print("Error: Result file not found.")
            return 0

    except subprocess.CalledProcessError as e:
        print(f"An error occurred while running the program: {e}")
    finally:
        if os.path.exists(outfile_iterative):
            os.remove(outfile_iterative)
        if remove_input:
            if os.path.exists(input):
                os.remove(input)        

def evaluateNumber(PACKER, RESULTS, OUTPUT_JSON, inputs, tests):
    json_rows = []
    algorithms = 5
    for algo in range(algorithms):
        packed = 0
        for i in range(tests):
            packed += numberOfPacked(str(PACKER), algo, inputs[i], str(RESULTS)) 
        packed = packed / tests
        row = { "algorithm": algo, "samples": tests, "avg_packed": packed }
        json_rows.append(row)

        print(f"Algo {algo}: {packed}")

    with open(OUTPUT_JSON, "w") as file:
        json.dump(json_rows, file, indent=4)

    print(f"results written to: {OUTPUT_JSON}")

def main():
    TARGETFOLDER = config.PROJECT_ROOT / "data"
    OUTPUT_JSON = config.PROJECT_ROOT / "results/comparison.json"

    # print(f"Project Root: {PROJECT_ROOT}")
    # print(f"Program Path: {PACKER}")

    tests = 50
    inputs = generate_inputs(str(config.DATASET), str(TARGETFOLDER), tests, 15, 43500)

    print("inputs generated")

    # evaluateNumber(config.PACKER_EXECUTABLE, config.RESULTS_DIR, OUTPUT_JSON, inputs, tests)
    evaluateComparison(config.PACKER_EXECUTABLE, config.RESULTS_DIR, OUTPUT_JSON, inputs, tests)

if __name__ == "__main__":
    main()
