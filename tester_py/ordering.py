from pathlib import Path
from input_gen import constrained_random_sample 
from input_gen import rows_into_tuples 

def calculate_cost(json_output):
    prices = {"xs": 3285, "s": 3685, "m": 4205, "l": 5315, "xl": 8350}
    
    base_price = prices[json_output["bin_needed"]]
    vol_ratio = json_output["bounding_box_volume"]
    max_slack = json_output["max_leftover_slack"]
    
	#constants
    c_vol_ratio = 100
    c_max_slack = 0
        
    cost = base_price -\
           c_vol_ratio * vol_ratio + \
           c_max_slack * max_slack
     
    return cost

PROJECT_ROOT = Path(__file__).resolve().parent.parent
DATASET = PROJECT_ROOT / "data" / "generated_items.csv"
TARGETFOLDER = PROJECT_ROOT / "data"
PACKER = PROJECT_ROOT / "Packing_cpp" / "packer.exe"
RESULTS = PROJECT_ROOT / "results"

rows = constrained_random_sample(str(DATASET), 15, 43500)
tuples = rows_into_tuples(rows) # [(x, y z), (x, y z), ...]