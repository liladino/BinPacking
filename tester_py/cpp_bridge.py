import subprocess
import json
import tempfile
import os
from config import PACKER_EXECUTABLE

def run_packer(items, algo_id, is_first_fit=False):
    # items: [(x, y, z), ...]
    inp = "\n".join([f"{x} {y} {z}" for x, y, z in items]) + "\n"
    
    args = [str(PACKER_EXECUTABLE), "--algorithm", str(algo_id)]
    
    if is_first_fit:
        args.append("--firstFit")

    try:
        result = subprocess.run(args, input=inp, capture_output=True, text=True, check=True)
        output_json = json.loads(result.stdout)
        return output_json
    except subprocess.CalledProcessError as e:
        print(f"Error while running the packer: {e.stderr}")
        raise ValueError({e.stderr})
    except json.JSONDecodeError:
        print(f"Non standard JSON. Raw output:\n{result.stdout}")
        return None