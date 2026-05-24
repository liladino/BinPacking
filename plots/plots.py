import csv
import matplotlib.pyplot as plt
import numpy as np
from pathlib import Path

PLOTS_FOLDER = Path(__file__).resolve().parent

algos = ["mohó (nem forgat)", "mohó (legnagyobb oldal fenn)", "mohó (min. slack összeg)", "mohó (első alkalmas irány)", "shelf", "extrém pontok"]

def time_plot(file = 'time_raklap', title = ' '):
    data = []
    input = PLOTS_FOLDER / (file + '.csv')
    output = PLOTS_FOLDER / (file + '.png')
    
    with open(input) as file:
        reader = csv.reader(file)
        i = 0
        for row in reader:
            data.append([])
            for col in row:
                data[i].append(float(col))
            i = i+1

    # print(data)

    x = np.arange(len(algos)) 
    width = 0.35

    plt.figure(figsize=(8, 6))

    plt.bar(x - width/2, data[0], width, label='First fit', color='blue')
    plt.bar(x + width/2, data[1], width, label='Iteratív', color='orange')

    plt.title(title)
    plt.ylabel("Átlagos futásidő (másodperc)")
    plt.legend()
        
    plt.xticks(x, algos, rotation=45, ha='right')
    plt.tight_layout()
    
    plt.grid(axis='y', linestyle='--', alpha=0.7)

    plt.savefig(output)
    plt.show()

def win_loss(file = 'winloss_random', title = ' '):
    data = []
    input = PLOTS_FOLDER / (file + '.csv')
    output = PLOTS_FOLDER / (file + '.png')
    
    with open(input) as file:
        reader = csv.reader(file)
        i = 0
        for row in reader:
            data.append([])
            j = 0
            for col in row:
                data[i].append(int(col))
            i = i+1

    data = [list(row) for row in zip(*data)]
    # print(data)
    
    ties = np.array(data[2]) / (10000 / 100)
    it_wins = np.array(data[1]) / (10000 / 100)
    ff_wins = np.array(data[0]) / (10000 / 100)

    plt.figure(figsize=(10, 4))
    plt.barh(algos, ties, color='lightgray', label='Azonos hatékonyság')
    plt.barh(algos, it_wins, left=ties, color='orange', label='Iteratív dominál')
    plt.barh(algos, ff_wins, left=(ties + it_wins), color='blue', label='First fit dominál')

    plt.title(title)
    plt.xlabel("Szimulációk (%)")
    plt.xlim(0, 100) 
    plt.legend(loc='upper left', bbox_to_anchor=(1, 1))
    plt.tight_layout()

    plt.savefig(output)
    plt.show()

def average_packed(file = 'winloss_random', title = ' '):
    data = []
    input = PLOTS_FOLDER / (file + '.csv')
    output = PLOTS_FOLDER / (file + '.png')
    
    with open(input) as file:
        reader = csv.reader(file)
        i = 0
        for row in reader:
            data.append([])
            j = 0
            for col in row:
                data[i].append(float(col))
            i = i+1

    data = [list(row) for row in zip(*data)]
    # print(data)
    
    x = np.arange(len(algos))
    width = 0.35 

    plt.figure(figsize=(10, 6))

    plt.bar(x - width/2, data[1], width, label='Csökkenő térfogat', color='#2ca02c') 
    plt.bar(x + width/2, data[0], width, label='Random', color='#d62728') 

    # Formatting
    plt.title(title)
    plt.ylabel("Átlag pakolt elemek sorrend alapján")
    plt.xticks(x, algos, rotation=45)
    plt.legend()
    plt.grid(axis='y', linestyle='--', alpha=0.7)
    plt.tight_layout()

    plt.savefig(output)
    plt.show()

# time_plot('time_raklap', "Futásidő\nRaklap")
# time_plot('time_doboz', "Futásidő\nPosta doboz")
# win_loss('winloss_random', "Hatékonyság\nRandom sorrend")
# win_loss('winloss_csokkeno', "Hatékonyság\nTérfogat szerint csökkenő sorrend")
average_packed('pakolt_random_vs_csokkeno', "Átlagosan bepakolt elemek")
