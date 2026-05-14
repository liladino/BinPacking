import csv
from input_gen import constrained_random_sample, rows_into_tuples
from config import DATASET, ALGORITHMS, RESULTS_DIR
from cpp_bridge import run_packer
import strategies
from simulated_ann import run_sa, calculate_cost

def main():
    num_samples = 50 # Start small for testing
    all_results = []

    for i in range(num_samples):
        print(f"Processing Order {i+1}/{num_samples}...")
        
        # 1. Generate base data
        rows = constrained_random_sample(str(DATASET), 15, 43500)
        base_items = rows_into_tuples(rows)
        
        for algo_name, algo_id in ALGORITHMS.items():
            
            # --- Evaluate Random (Baseline) ---
            json_rand = run_packer(base_items, algo_id)
            all_results.append({
                "sample_id": i, "algo": algo_name, "strategy": "random",
                "cost": calculate_cost(json_rand), "bin": json_rand["bin_needed"],
                "packed_count": json_rand["packed"]
            })
            
            # --- Evaluate Volume Descending ---
            items_vol = strategies.sort_by_volume_desc(base_items)
            json_vol = run_packer(items_vol, algo_id)
            all_results.append({
                "sample_id": i, "algo": algo_name, "strategy": "volume_desc",
                "cost": calculate_cost(json_vol), "bin": json_vol["bin_needed"],
                "packed_count": json_vol["packed"]
            })

            # --- Evaluate Simulated Annealing ---
            # Give SA a warm start using the volume descending sort
            _, best_sa_cost, best_sa_json = run_sa(items_vol, algo_id, max_iters=200)
            all_results.append({
                "sample_id": i, "algo": algo_name, "strategy": "sim_anneal",
                "cost": best_sa_cost, "bin": best_sa_json["bin_needed"],
                "packed_count": best_sa_json["packed"]
            })

    # Save to CSV
    RESULTS_DIR.mkdir(exist_ok=True)
    csv_file = RESULTS_DIR / "experiment_results.csv"
    with open(csv_file, 'w', newline='') as f:
        writer = csv.DictWriter(f, fieldnames=all_results[0].keys())
        writer.writeheader()
        writer.writerows(all_results)
        
    print(f"Done! Results saved to {csv_file}")

if __name__ == "__main__":
    main()
    

	
# rows = constrained_random_sample(str(DATASET), 15, 43500)
# tuples = rows_into_tuples(rows) # [(x, y z), (x, y z), ...]