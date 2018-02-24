/**
    Copyright (c) 2017 Ryan Porter    
    You may use, distribute, or modify this code under the terms of the MIT license.
*/

/**
quatArray data
    Custom data type for a variable length array of quaternion rotations. 
    This array is contiguous, unlike a multi-attribute, which may be sparse.
*/

#include "quatArrayData.h"
#include "arrayData.h"

#include <istream>
#include <ostream>
#include <vector>

#include <maya/MArgList.h>
#include <maya/MPxData.h>
#include <maya/MQuaternion.h>
#include <maya/MObject.h>
#include <maya/MTypeId.h>
#include <maya/MString.h>
#include <maya/MStatus.h>

QuatArrayData::QuatArrayData() {}
QuatArrayData::~QuatArrayData() {}

void* QuatArrayData::creator()
{
    return new QuatArrayData;
}


unsigned int QuatArrayData::length()
{
    return (unsigned int) this->data.size();
}


MStatus QuatArrayData::readASCII(const MArgList& args, unsigned int &end)
{
    MStatus status;

    std::vector<double> values = readASCIIData(4, args, end, &status);
    if (status) { this->setValues(values); }

    return status;   
}


MStatus QuatArrayData::writeASCII(std::ostream &out)
{
    MStatus status;

    std::vector<double> values = this->getValues();
    status = writeASCIIData(values, this->length(), out);

    return status; 
}


MStatus QuatArrayData::readBinary(std::istream &in, unsigned int length)
{
    MStatus status; 

    std::vector<double> values = readBinaryData(4, in, length, &status);
    if (status) { this->setValues(values); }

    return status; 
}


MStatus QuatArrayData::writeBinary(std::ostream &out)
{
    MStatus status;

    std::vector<double> values = this->getValues();
    status = writeBinaryData(values, this->length(), out);

    return status; 
}


std::vector<MQuaternion> QuatArrayData::getArray()
{
    return std::vector<MQuaternion>(this->data);
}


void QuatArrayData::setArray(std::vector<MQuaternion> &array)
{
    this->data.resize(array.size());
    std::copy(
        array.begin(), 
        array.end(),
        this->data.begin()
    );
}


void QuatArrayData::setValues(std::vector<double> &values)
{
    size_t numberOfValues = values.size();
    size_t numberOfItems = numberOfValues / 4;
    this->data.resize(numberOfItems);

    for (size_t i = 0; i < numberOfItems; i++)
    {
        this->data[i] = MQuaternion(
            values[(i*4)+0], 
            values[(i*4)+1], 
            values[(i*4)+2],
            values[(i*4)+3]
        );
    }
}


std::vector<double> QuatArrayData::getValues()
{
    std::vector<double> values;
    size_t numberOfItems = this->data.size();
    values.resize(numberOfItems * 4);

    for (size_t i = 0; i < numberOfItems; i++)
    {
        MQuaternion &q = this->data[i];
        values[(i*4)+0] = q.x;
        values[(i*4)+1] = q.y;
        values[(i*4)+2] = q.z;
        values[(i*4)+3] = q.w;
    }

    return values;
}


void QuatArrayData::copy(const MPxData& other)
{
    if (this->typeId() == other.typeId())
    {
        const QuatArrayData &otherData = (const QuatArrayData &) other;
        this->data.resize(otherData.data.size());
        std::copy(
            otherData.data.begin(), 
            otherData.data.end(),
            this->data.begin()
        );
    }
}


MTypeId QuatArrayData::typeId() const
{
    return QuatArrayData::TYPE_ID;
}


MString QuatArrayData::name() const
{ 
    return QuatArrayData::TYPE_NAME; 
}