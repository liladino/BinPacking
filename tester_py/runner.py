import subprocess
import json
import os
import math
from input_gen import generate_random_items
from pathlib import Path

def generate_inputs(dataset, target_folder, number_of_files, max_items, max_volume):
    output_files = []
    for i in range(number_of_files):
        file = target_folder + "/items" + str(i).zfill(int(math.log10(number_of_files)+1)) + ".txt" 
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

    with open(path_first_fit, 'r') as f1, open(path_iterative, 'r') as f2:
        first_fit = json.load(f1)
        iterative = json.load(f2)
    
    if (first_fit.get("packed") > iterative.get("packed")):
        print("hihihiha")
        return 1
    
    if (first_fit.get("packed") < iterative.get("packed")):
        print("argghw")
        return 2

    # print(bin_dict[first_fit.get("bin_needed")])
    # print(bin_dict[iterative.get("bin_needed")])

    if (bin_dict[first_fit.get("bin_needed")] < bin_dict[iterative.get("bin_needed")]):
        return 1
    if (bin_dict[first_fit.get("bin_needed")] > bin_dict[iterative.get("bin_needed")]):
        return 2
    return 0

def runComparison(packer, input, results):
    outfile_iterative = results + "/it.json"
    outfile_first_fit = results + "/ff.json"
    
    args_set_a = ["--input", input, "--output", outfile_iterative]
    args_set_b = ["--input", input, "--output", outfile_first_fit, "--firstFit"]
    
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
            print("Error: One or both output files were not created.")

    except subprocess.CalledProcessError as e:
        print(f"An error occurred while running the program: {e}")
    finally:
       for f in [outfile_iterative, outfile_first_fit]:
           if os.path.exists(f):
              os.remove(f)


def main():
    PROJECT_ROOT = Path(__file__).resolve().parent.parent
    PACKER = PROJECT_ROOT / "Packing_cpp" / "packer"
    DATASET = PROJECT_ROOT / "data" / "generated_items.csv"
    TARGETFOLDER = PROJECT_ROOT / "data"
    PACKER = PROJECT_ROOT / "Packing_cpp" / "packer.exe"
    RESULTS = PROJECT_ROOT / "results"

    # print(f"Project Root: {PROJECT_ROOT}")
    # print(f"Program Path: {PACKER}")

    tests = 1000
    inputs = generate_inputs(str(DATASET), str(TARGETFOLDER), tests, 15, 43500)

    print("inputs generated")

    results = [0, 0]
    for i in range(tests):
        x = runComparison(str(PACKER), inputs[i], str(RESULTS))
        if None != x:
            if 0 != x:
                results[x-1] += 1
            # else:
            #     results[0] += 1
            #     results[1] += 1
                
    print(f"Results:\nfirst fit\t{results[0]}\niterative\t{results[1]}")



if __name__ == "__main__":
    main()
