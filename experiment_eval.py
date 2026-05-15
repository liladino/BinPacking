from pathlib import Path

import pandas as pd
import io

PROJECT_ROOT = Path(__file__).resolve().parent
DATA = PROJECT_ROOT / "results" / "experiment_results.csv"


def process_strategy_matrix(input_csv_path, output_csv_path):
    df = pd.read_csv(input_csv_path)

    # minimum cost for each (sample_id, algo) group
    df['min_cost_in_group'] = df.groupby(['sample_id', 'algo'])['cost'].transform('min')

    # which rows are lowest cost equals
    # tie: both strategies get a point
    winners = df[df['cost'] == df['min_cost_in_group']]

    # rows: strategy, columns: algo, value: count of wins
    matrix = pd.crosstab(winners['strategy'], winners['algo'])

    all_strategies = df['strategy'].unique()
    all_algos = df['algo'].unique()
    matrix = matrix.reindex(index=all_strategies, columns=all_algos, fill_value=0)

    matrix.to_csv(output_csv_path)
    print(f"Success! Matrix exported to {output_csv_path}")
    
    return matrix

if __name__ == "__main__":
    process_strategy_matrix(str(DATA), 'strategy_win_matrix.csv')
    pass