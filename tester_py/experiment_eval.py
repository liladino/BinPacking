from pathlib import Path
import pandas as pd

def process_strategy_matrix(input_csv_path, output_csv_path):
    df = pd.read_csv(input_csv_path)

    df.columns = df.columns.str.strip()

    df['algo'] = df['algo'].astype(str).str.strip()
    df['strategy'] = df['strategy'].astype(str).str.strip()

    df['min_cost_in_group'] = df.groupby(['sample_id', 'algo'])['cost'].transform('min')

    winners = df[df['cost'] == df['min_cost_in_group']]

    winners = winners.drop_duplicates(subset=['sample_id', 'algo', 'strategy'])

    matrix = pd.crosstab(winners['strategy'], winners['algo'])

    all_strategies = sorted(df['strategy'].unique())
    all_algos = sorted(df['algo'].unique())
    matrix = matrix.reindex(index=all_strategies, columns=all_algos, fill_value=0)

    matrix.to_csv(output_csv_path)
    print(f"Result matrix in {output_csv_path}")
    
    return matrix
