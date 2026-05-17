import argparse
import csv
import random
from input_gen import constrained_random_sample, rows_into_tuples
from config import DATASET, ALGORITHMS, RESULTS_DIR
from cpp_bridge import run_packer
import strategies
from simulated_ann import run_sa, calculate_cost
from experiment_eval import process_strategy_matrix

def main():
    parser = argparse.ArgumentParser(description="Run simulations.")
    parser.add_argument("--result", \
                        default=(str(RESULTS_DIR / "cost_algo_strategy.csv")), \
                        help=("default: " + str(RESULTS_DIR / "cost_algo_strategy.csv")))
    parser.add_argument("--matrix", \
                        default=(str(RESULTS_DIR / "results_matrix.csv")), \
                        help=("default: " + str(RESULTS_DIR / "results_matrix.csv")))
    parser.add_argument("--samples", \
                        default=5, \
                        type=int)
    args = parser.parse_args()
    
    cost_strat_algo_table = args.result
    matrix_csv = args.matrix
    num_samples = args.samples

    all_results = []

    for i in range(num_samples):
        print(f"Processing batch {i+1}/{num_samples}...")
        
        while True:
            rows = constrained_random_sample(str(DATASET), random.randint(5, 10), 1e9)#43500)
            base_items = rows_into_tuples(rows)
            if len(base_items) > 5:
                break

        
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
                
                # Terfogat alapjan novekvo
                items_vol = strategies.sort_by_volume_asc(base_items)
                json_vol = run_packer(items_vol, algo_id)
                all_results.append({
                    "sample_id": i, "algo": algo_name, "strategy": "volume_asc",
                    "cost": calculate_cost(json_vol)
                })
                
                # Legnagyobb el csokk
                items_vol = strategies.sort_by_longest_edge_desc(base_items)
                json_vol = run_packer(items_vol, algo_id)
                all_results.append({
                    "sample_id": i, "algo": algo_name, "strategy": "largest_edge_desc",
                    "cost": calculate_cost(json_vol)
                })
                
                # Legnagyobb oldal csokk
                items_vol = strategies.sort_by_largest_face_desc(base_items)
                json_vol = run_packer(items_vol, algo_id)
                all_results.append({
                    "sample_id": i, "algo": algo_name, "strategy": "largest_face_desc",
                    "cost": calculate_cost(json_vol)
                })

                # Szimulalt hules
                _, best_sa_cost, best_sa_json = run_sa(items_vol, algo_id, max_iters=100)
                all_results.append({
                    "sample_id": i, "algo": algo_name, "strategy": "sim_anneal",
                    "cost": best_sa_cost
                })
            except ValueError as e:
                print(f"{e.stderr} in {algo_name} at sample {i}")
                print(f"sample: {base_items}, selected rows: {rows}")
                break
    
    with open(cost_strat_algo_table, 'w', newline='') as f:
        writer = csv.DictWriter(f, fieldnames=all_results[0].keys())
        writer.writeheader()
        writer.writerows(all_results)
        
    print(f"Run results in {cost_strat_algo_table}")
    process_strategy_matrix(cost_strat_algo_table, matrix_csv)
if __name__ == "__main__":
    main()
    
