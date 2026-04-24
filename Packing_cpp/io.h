#ifndef IO_H
#define IO_H

#include "Packer.h"
#include <string>
#include <sstream>
#include <fstream>

std::string jsonData(std::string name, size_t val);
void exportPackingToJSON(Packer* packer, std::string outfile);
size_t importItems(const std::string& infile, std::vector<size_t>& items);

#endif