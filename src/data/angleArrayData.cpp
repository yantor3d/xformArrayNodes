/**
    Copyright (c) 2017 Ryan Porter    
    You may use, distribute, or modify this code under the terms of the MIT license.
*/

/**
angleArray data
    Custom data type for a variable length array of doubleAngle values. 
    This array is contiguous, unlike a multi-attribute, which may be sparse.
*/

#include "angleArrayData.h"
#include "arrayData.h"

#include <algorithm>
#include <istream>
#include <ostream>
#include <vector>

#include <maya/MAngle.h>
#include <maya/MArgList.h>
#include <maya/MPxData.h>
#include <maya/MObject.h>
#include <maya/MTypeId.h>
#include <maya/MString.h>
#include <maya/MStatus.h>

AngleArrayData::AngleArrayData() {}

AngleArrayData::~AngleArrayData() {}

void* AngleArrayData::creator()
{
    return new AngleArrayData;
}


unsigned int AngleArrayData::length()
{
    return (unsigned int) this->data.size();
}


MStatus AngleArrayData::readASCII(const MArgList& args, unsigned int &end)
{
    MStatus status;

    std::vector<double> values = readASCIIData(1, args, end, &status);
    if (status) { this->setValues(values); }

    return status;   
}


MStatus AngleArrayData::writeASCII(std::ostream &out)
{
    MStatus status;

    std::vector<double> values = this->getValues();
    status = writeASCIIData(values, this->length(), out);

    return status; 
}


MStatus AngleArrayData::readBinary(std::istream &in, unsigned int length)
{
    MStatus status; 

    std::vector<double> values = readBinaryData(1, in, length, &status);
    if (status) { this->setValues(values); }

    return status; 
}


MStatus AngleArrayData::writeBinary(std::ostream &out)
{
    MStatus status;

    std::vector<double> values = this->getValues();
    status = writeBinaryData(values, this->length(), out);

    return status; 
}


std::vector<MAngle> AngleArrayData::getArray()
{
    return std::vector<MAngle>(this->data);
}


void AngleArrayData::setArray(std::vector<MAngle> array)
{
    this->data.resize(array.size());
    std::copy(
        array.begin(), 
        array.end(),
        this->data.begin()
    );
}


void AngleArrayData::setValues(std::vector<double> values)
{
    MAngle::Unit u = MAngle::uiUnit();
    size_t numberOfItems = values.size();
    this->data.resize(numberOfItems);

    for (size_t i = 0; i < numberOfItems; i++)
    {
        this->data[i] = MAngle(values[i], u);
    }
}


std::vector<double> AngleArrayData::getValues()
{
    std::vector<double> values;
    size_t numberOfItems = data.size();
    values.resize(numberOfItems);

    for (size_t i = 0; i < numberOfItems; i++)
    {
        values[i] = data[i].value();
    }

    return values;
}


void AngleArrayData::copy(const MPxData& other)
{
    if (this->typeId() == other.typeId())
    {
        const AngleArrayData &otherData = (const AngleArrayData &) other;
        this->data.resize(otherData.data.size());
        std::copy(
            otherData.data.begin(), 
            otherData.data.end(),
            this->data.begin()
        );
    }
}

MTypeId AngleArrayData::typeId() const
{
    return AngleArrayData::TYPE_ID;
}


MString AngleArrayData::name() const
{ 
    return AngleArrayData::TYPE_NAME; 
}