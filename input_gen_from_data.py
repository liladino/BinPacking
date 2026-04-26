import csv
import random

def constrained_random_sample(file_path, max_items, max_volume):
    items = []

    try:
        with open(file_path, mode='r', encoding='utf-8') as f:
            reader = csv.DictReader(f)
            for row in reader:
                row['volume_cm3'] = float(row['volume_cm3'])
                items.append(row)
    except FileNotFoundError:
        print(f"{file_path} not found.")
        return

    selected_items = []
    current_volume = 0.0

    while len(selected_items) < max_items:
        if not items:
            print("No items")
            break
		
        choice = random.choice(items)
        potential_volume = current_volume + choice['volume_cm3']        
        if potential_volume > max_volume:
            return selected_items
        
        selected_items.append(choice)
        current_volume = potential_volume
        # items.remove(choice)

    print(f"Selected {len(selected_items)} items.")
    return selected_items

# --- Configuration ---
N = 15             # Max items
M = 43500          # Max volume (cm3)
FILENAME = 'generated_items.csv'

sum_volume = 0
result = constrained_random_sample(FILENAME, N, M)

if result:
    with open("input.txt", "w") as f:   
        for i in result:
            f.write(f"{i['length_mm']} {i['width_mm']} {i['height_mm']} ")
            print(f"{i['length_mm']} {i['width_mm']} {i['height_mm']} ")
            sum_volume += i['volume_cm3']
    
    print(f"sum volume: {sum_volume}")
    
	
	