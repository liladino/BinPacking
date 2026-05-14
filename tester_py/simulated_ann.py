import math
import random
from config import BOX_PRICES
from cpp_bridge import run_packer

def calculate_cost(json_data):
    base_price = BOX_PRICES[json_data["bin_needed"]]
    vol_ratio = json_data["bounding_box_volume"]
    max_slack = json_data["max_leftover_slack"]
    
	#constants
    c_vol_ratio = 100
    c_max_slack = 0.1
        
    cost = base_price - \
           c_vol_ratio * vol_ratio + \
           c_max_slack * max_slack
     
    return cost

def get_neighbor(items):
    # Swaps two random items -> neighbour state
    new_items = items.copy()
    idx1, idx2 = random.sample(range(len(new_items)), 2)
    new_items[idx1], new_items[idx2] = new_items[idx2], new_items[idx1]
    return new_items

def run_sa(initial_items, algo_id, max_iters=500, temp=1000.0, cooling_rate=0.95):
    current_items = initial_items
    current_json = run_packer(current_items, algo_id)
    current_cost = calculate_cost(current_json)
    
    best_items, best_cost, best_json = current_items, current_cost, current_json

    for i in range(max_iters):
        neighbor = get_neighbor(current_items)
        neighbor_json = run_packer(neighbor, algo_id)
        neighbor_cost = calculate_cost(neighbor_json)
        
        cost_diff = neighbor_cost - current_cost
        
        # If better, or if worse but accepted by probability
        if cost_diff < 0 or random.random() < math.exp(-cost_diff / temp):
            current_items = neighbor
            current_cost = neighbor_cost
            current_json = neighbor_json
            
            if current_cost < best_cost:
                best_items = current_items
                best_cost = current_cost
                best_json = current_json
                
        temp *= cooling_rate

    return best_items, best_cost, best_json