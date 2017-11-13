#pragma once 

#include <istream>
#include <ostream>
#include <vector>

#include <maya/MArgList.h>
#include <maya/MStatus.h>

std::vector<double> readASCIIData(unsigned numberOfElementsPerItem, const MArgList &args, unsigned int &end, MStatus *ReturnStatus=NULL);  
std::vector<double> readBinaryData(unsigned numberOfElementsPerItem, std::istream &in, unsigned int length, MStatus *ReturnStatus=NULL);

MStatus writeASCIIData(std::vector<double> values, unsigned numberOfItems, std::ostream &out);
MStatus writeBinaryData(std::vector<double> values, unsigned numberOfItems, std::ostream &out);
