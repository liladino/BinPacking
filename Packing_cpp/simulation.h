#ifndef SIMULATION_H
#define SIMULATION_H

#include "Item.h"
#include "test.h"
#include "GreedyPacker.h"
#include "ShelfPacker.h"
#include "ExtremePoints.h"
#include "io.h"
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <sstream>

enum class ShipPolicy{
	SkipIfDoesntFit,
	StopIfDoesntFit,
	ShipEverything
};

enum class Algorithm {
	GreedyNoRotation,
	GreedyLargestFaceUp,
	GreedyMinSumLeftoverSlack,
	GreedyTryFirstFitting,
	ShelfLayer,
	ExtremePoints
};

void simulate(Algorithm algorithm, size_t items[], size_t n, std::ostream* outfile, bool firstFit, ShipPolicy shipPolicy, const std::string& visualExp = "");

#endif