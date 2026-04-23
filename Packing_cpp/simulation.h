#ifndef SIMULATION_H
#define SIMULATION_H

#include "Item.h"
#include "test.h"
#include "GreedyPacker.h"
#include "ShelfPacker.h"
#include "io.h"
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <sstream>


void simul(Packer* packer, size_t chainIndex, size_t items[], size_t n, std::string outfile);
void greedy(size_t chainIndex, size_t items[], size_t n, int policy, std::string outfile);

#endif