import numpy as np
import pandas as pd
import random

# mean length, mean width, mean height (cm), deviation fraction
CATEGORIES = {
    "tiny":        (5, 5, 4, 0.25),    # cables, adapters
    "small_box":   (12, 10, 8, 0.20),  # cosmetics, small electronics
    "flat":        (25, 20, 5, 0.20),  # t-shirts, books
    "medium_box":  (30, 20, 12, 0.25), # shoes, appliances
    "large":       (50, 40, 20, 0.30), # bulkier items
}

# Probability 
CATEGORY_WEIGHTS = {
    "tiny": 0.25,
    "small_box": 0.30,
    "flat": 0.20,
    "medium_box": 0.20,
    "large": 0.05,
}

N_ITEMS = 1000
OUTPUT_FILE = "generated_items.csv"

# -----------------------------

def sample_dimension(mean, std_fraction):
    std = mean * std_fraction
    value = np.random.normal(mean, std)

    return max(1.0, value) # at least 1cm 


def generate_item(category_name, item_id):
    mean_l, mean_w, mean_h, std_frac = CATEGORIES[category_name]

    l = sample_dimension(mean_l, std_frac)
    w = sample_dimension(mean_w, std_frac)
    h = sample_dimension(mean_h, std_frac)

    dims = [l, w, h]
    # random.shuffle(dims)

    return {
        "item_id": item_id,
        "category": category_name,
        "length_mm": round(dims[0]*10),
        "width_mm": round(dims[1]*10),
        "height_mm": round(dims[2]*10),
        "volume_cm3": round(dims[0] * dims[1] * dims[2], 2),
    }


def generate_dataset(n_items):
    categories = list(CATEGORY_WEIGHTS.keys())
    weights = list(CATEGORY_WEIGHTS.values())

    items = []

    for i in range(n_items):
        cat = random.choices(categories, weights=weights, k=1)[0]
        item = generate_item(cat, i)
        items.append(item)

    return pd.DataFrame(items)

# -----------------------------

df = generate_dataset(N_ITEMS)

df.to_csv(OUTPUT_FILE, index=False)
print(f"Generated {len(df)} items -> {OUTPUT_FILE}")
