import csv
from input_gen import constrained_random_sample, rows_into_tuples
from config import DATASET, ALGORITHMS, RESULTS_DIR
from cpp_bridge import run_packer
import strategies
from simulated_ann import run_sa, calculate_cost

def main():
    num_samples = 5
    all_results = []

    for i in range(num_samples):
        print(f"Processing Order {i+1}/{num_samples}...")
        
        rows = constrained_random_sample(str(DATASET), 15, 43500)
        base_items = rows_into_tuples(rows)
        
        for algo_name, algo_id in ALGORITHMS.items():
            
            # --- Evaluate Random (Baseline) ---
            json_rand = run_packer(base_items, algo_id)
            all_results.append({
                "sample_id": i, "algo": algo_name, "strategy": "random",
                "cost": calculate_cost(json_rand)
            })
            
            # --- Evaluate Volume Descending ---
            items_vol = strategies.sort_by_volume_desc(base_items)
            json_vol = run_packer(items_vol, algo_id)
            all_results.append({
                "sample_id": i, "algo": algo_name, "strategy": "volume_desc",
                "cost": calculate_cost(json_vol)
            })

            # --- Evaluate Simulated Annealing ---
            # Give SA a warm start using the volume descending sort
            _, best_sa_cost, best_sa_json = run_sa(items_vol, algo_id, max_iters=200)
            all_results.append({
                "sample_id": i, "algo": algo_name, "strategy": "sim_anneal",
                "cost": best_sa_cost
            })

    # Save to CSV
    RESULTS_DIR.mkdir(exist_ok=True)
    csv_file = RESULTS_DIR / "experiment_results.csv"
    with open(csv_file, 'w', newline='') as f:
        writer = csv.DictWriter(f, fieldnames=all_results[0].keys())
        writer.writeheader()
        writer.writerows(all_results)
        
    print(f"Results in {csv_file}")

if __name__ == "__main__":
    main()
    

	
# rows = constrained_random_sample(str(DATASET), 15, 43500)
# tuples = rows_into_tuples(rows) # [(x, y z), (x, y z), ...]