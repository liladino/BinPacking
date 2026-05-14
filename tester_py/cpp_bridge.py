import subprocess
import json
import tempfile
import os
from config import PACKER_EXE

def run_packer(items, algo_id, is_first_fit=False):
    """
    Passes items to C++ and returns the parsed JSON dict.
    Note: 'items' is a list of tuples: [(x, y, z), ...]
    """
    # Create a temporary input file for the C++ to read. 
    # (Even better: modify C++ to read items from stdin to avoid this file too!)
    fd, temp_input_path = tempfile.mkstemp(suffix=".txt", text=True)
    with os.fdopen(fd, 'w') as f:
        for x, y, z in items:
            f.write(f"{x} {y} {z}\n")

    args = [
        str(PACKER_EXE),
        "--input", temp_input_path,
        "--algorithm", str(algo_id),
        "--stdout" # Assume you add a flag to C++ to print JSON to terminal
    ]
    if is_first_fit:
        args.append("--firstFit")

    try:
        # capture_output=True grabs whatever C++ prints to std::cout
        result = subprocess.run(args, capture_output=True, text=True, check=True)
        
        # Parse the JSON directly from the console output
        output_json = json.loads(result.stdout)
        return output_json

    except subprocess.CalledProcessError as e:
        print(f"C++ Crash! Error: {e.stderr}")
        return None
    except json.JSONDecodeError:
        print(f"Failed to parse C++ output as JSON. Raw output:\n{result.stdout}")
        return None
    finally:
        os.remove(temp_input_path) # Clean up input file