#ifndef IO_H
#define IO_H

#include "Packer.h"
#include <string>
#include <sstream>
#include <fstream>

template<typename T>
std::string jsonData(const std::string& name, T val);

template<>
std::string jsonData<const std::string&>(const std::string& name, const std::string& val);

void exportPackingToJSON(Packer* packer, const std::string& outfile);
std::string metaDataToJSON(const std::string& neededBin, size_t allItems, Packer* packer);
size_t importItems(const std::string& infile, std::vector<size_t>& items);

#endif