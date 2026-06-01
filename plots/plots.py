import csv
import matplotlib.pyplot as plt
import numpy as np
import seaborn as sns
import pandas as pd
from pathlib import Path

PLOTS_FOLDER = Path(__file__).resolve().parent

algos = ["mohó (nem forgat)", "mohó (legnagyobb oldal fenn)", "mohó (min. slack összeg)", "mohó (első alkalmas irány)", "shelf", "extrém pontok"]

def time_plot(file = 'time_raklap', title = ' '):
    data = []
    input = PLOTS_FOLDER / (file + '.csv')
    output = PLOTS_FOLDER / (file + '.pdf')
    
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
    output = PLOTS_FOLDER / (file + '.pdf')
    
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

def average_packed(file = 'pakolt_random_vs_csokkeno', title = ' '):
    data = []
    input = PLOTS_FOLDER / (file + '.csv')
    output = PLOTS_FOLDER / (file + '.pdf')
    
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

    plt.figure(figsize=(8, 6))

    plt.bar(x - width/2, data[0], width, label='first fit', color='blue') 
    plt.bar(x + width/2, data[1], width, label='iteratív', color='orange') 

    # Formatting
    plt.title(title)
    plt.ylabel("Átlagsan szükséges raklapok száma")
    plt.xticks(x, algos, rotation=45)
    plt.legend()
    plt.grid(axis='y', linestyle='--', alpha=0.7)
    plt.tight_layout()

    plt.savefig(output)
    plt.show()

def strategy_heatmap(file = 'strategy_mx', title = ' '):
    data = []
    input = PLOTS_FOLDER / (file + '.csv')
    output = PLOTS_FOLDER / (file + '.pdf')

    with open(input) as file:
        reader = csv.reader(file)
        i = 0
        for row in reader:
            data.append([])
            j = 0
            for col in row:
                if (0 == i or 0 == j):
                    data[i].append(col)
                else:
                    data[i].append(int(col))
                j = j+1
            i = i+1

    dic = {} 
    strategies = []
    for i in range(0, len(data[0])):
        if i == 0:
            for x in range(1, len(data)):
                strategies.append(data[x][i])
        else:    
            lista = []
            for x in range(1, len(data)):
                lista.append(data[x][i])
                
            dic[data[0][i]] = lista

    data = dic

    df = pd.DataFrame(data, index=strategies)

    plt.figure(figsize=(10, 6))

    sns.heatmap(df, annot=True, cmap="Blues", fmt="d", cbar_kws={'label': '1000-ből hány esetben állt fenn a stratégia teljes dominanciája'})

    plt.title(title, pad=20)
    plt.ylabel("Rendező stratégia")
    plt.xlabel("Pakoló algoritmus")
    plt.xticks(rotation=45, ha='right')

    plt.tight_layout()
    plt.savefig(output)
    plt.show()

time_plot('time_raklap', "Futásidő\nRaklap")
time_plot('time_doboz', "Futásidő\nPosta doboz")
win_loss('winloss_random', "Hatékonyság\nRandom sorrend")
win_loss('winloss_csokkeno', "Hatékonyság\nTérfogat szerint csökkenő sorrend")
average_packed('pakolt2', "Hatékonyság csökkenő sorrendezés mellett\nÁtlagosan felhasznált raklapok száma")
strategy_heatmap('strategy_mx', "Rendező stratégiák hatása heurisztikák sikerességére")
