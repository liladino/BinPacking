import argparse
import csv
import random
from input_gen import constrained_random_sample, rows_into_tuples
from config import DATASET, ALGORITHMS, RESULTS_DIR
from cpp_bridge import run_packer
import strategies
from simulated_ann import run_sa, calculate_cost

def main():
    parser = argparse.ArgumentParser(description="Run simulations.")
    parser.add_argument("-o", "--output", default=(str(RESULTS_DIR / "experiment_results.csv")), help="default: BinPacking/results/experiment_results.csv")
    args = parser.parse_args()
    
    csv_file = args.output

    num_samples = 5
    all_results = []

    for i in range(num_samples):
        print(f"Processing Order {i+1}/{num_samples}...")
        
        rows = constrained_random_sample(str(DATASET), random.randint(1, 15), 43500)
        base_items = rows_into_tuples(rows)
        
        for algo_name, algo_id in ALGORITHMS.items():
            try:
                # Random sorrend
                json_rand = run_packer(base_items, algo_id)
                all_results.append({
                    "sample_id": i, "algo": algo_name, "strategy": "random",
                    "cost": calculate_cost(json_rand)
                })
                
                # Terfogat alapjan csokkeno
                items_vol = strategies.sort_by_volume_desc(base_items)
                json_vol = run_packer(items_vol, algo_id)
                all_results.append({
                    "sample_id": i, "algo": algo_name, "strategy": "volume_desc",
                    "cost": calculate_cost(json_vol)
                })
                
                # Legnagyobb oldal
                items_vol = strategies.sort_by_largest_face_desc(base_items)
                json_vol = run_packer(items_vol, algo_id)
                all_results.append({
                    "sample_id": i, "algo": algo_name, "strategy": "largest_face_desc",
                    "cost": calculate_cost(json_vol)
                })

                # Szimulalt hules
                _, best_sa_cost, best_sa_json = run_sa(items_vol, algo_id, max_iters=10)
                all_results.append({
                    "sample_id": i, "algo": algo_name, "strategy": "sim_anneal",
                    "cost": best_sa_cost
                })
            except ValueError as e:
                print(f"{e} in {algo_name} at sample {i}")
                print(f"sample: {base_items}, selected rows: {rows}")
                break;
    
    with open(csv_file, 'w', newline='') as f:
        writer = csv.DictWriter(f, fieldnames=all_results[0].keys())
        writer.writeheader()
        writer.writerows(all_results)
        
    print(f"Results in {csv_file}")

if __name__ == "__main__":
    main()
    
