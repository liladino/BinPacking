#ifndef IO_H
#define IO_H

#include "Packer.h"
#include <string>
#include <sstream>
#include <fstream>

template<typename T>
std::string jsonData(const std::string& name, T val);

std::string exportPacking(Packer* packer);
void exportPackingToJSON(Packer* packer, const std::string& outfile);
void exportPackingToJSON(const std::string& JSON, const std::string& outfile);

std::string metaDataToJSON(const std::string& neededBin, size_t allItems, Packer* packer);
void writeMetaData(std::ostream* outfile, const std::vector<std::string>& data);

size_t importItems(std::istream* is, std::vector<size_t>& items);

#endif