import csv
import random
import argparse
import io

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
	
def main():
    parser = argparse.ArgumentParser(description="Generate constrained random input.")
    
    parser.add_argument("-i", "--input", required=True, help="Input CSV file")
    parser.add_argument("-o", "--output", default="", help="Output file. Only stdout, if not specified.")
    parser.add_argument("--max-items", type=int, default=15, help="Maximum number of items")
    parser.add_argument("--max-volume", type=float, default=43500, help="Maximum total volume")

    args = parser.parse_args()   

    result = constrained_random_sample(
        args.input,
        args.max_items,
        args.max_volume
    )

    if not result:
        print("No result generated.")
        return

    sum_volume = 0

    output = io.StringIO()
    for i in result:
        line = f"{i['length_mm']} {i['width_mm']} {i['height_mm']}"
        print(line, file=output)
        sum_volume += i['volume_cm3'] 
    
    print(output.getvalue())   
    print(f"Sum volume: {sum_volume}")
    
    if ("" != args.output):
        with open(args.output, "w") as f:
            f.write(output.getvalue())
    
if __name__ == "__main__":
    main()