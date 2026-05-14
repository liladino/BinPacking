from pathlib import Path

PROJECT_ROOT = Path(__file__).resolve().parent.parent
DATASET = PROJECT_ROOT / "data" / "generated_items.csv"
PACKER_EXECUTABLE = PROJECT_ROOT / "Packing_cpp" / "packer.exe"
RESULTS_DIR = PROJECT_ROOT / "results"

BOX_PRICES = {
    "xs": 3285,
    "s": 3685,
    "m": 4205,
    "l": 5315,
    "xl": 8350
}

# Map strings to your C++ algorithm IDs
ALGORITHMS = {
    "greedy_0": 0,
    "greedy_1": 1,
    "greedy_2": 2,
    "greedy_3": 3,
    "shelf": 4
}