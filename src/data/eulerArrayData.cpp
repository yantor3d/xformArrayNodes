/**
    Copyright (c) 2017 Ryan Porter    
    You may use, distribute, or modify this code under the terms of the MIT license.
*/

/**
eulerArray data
    Custom data type for a variable length array of euler rotations. 
    This array is contiguous, unlike a multi-attribute, which may be sparse.
*/

#include "eulerArrayData.h"
#include "arrayData.h"

#include <stdio.h>
#include <stdlib.h>

#include <algorithm>
#include <istream>
#include <ostream>
#include <vector>

#include <maya/MAngle.h>
#include <maya/MArgList.h>
#include <maya/MEulerRotation.h>
#include <maya/MGlobal.h>
#include <maya/MPxData.h>
#include <maya/MObject.h>
#include <maya/MTypeId.h>
#include <maya/MString.h>
#include <maya/MStringArray.h>
#include <maya/MStatus.h>
#include <maya/MVector.h>

EulerArrayData::EulerArrayData() {}
EulerArrayData::~EulerArrayData() {}

void* EulerArrayData::creator()
{
    return new EulerArrayData;
}


unsigned int EulerArrayData::length()
{
    return (unsigned int) this->data.size();
}


MStatus EulerArrayData::readASCII(const MArgList& args, unsigned int &end)
{
    MStatus status;

    std::vector<double> values = readASCIIData(3, args, end, &status);
    if (status) { this->setValues(values); }

    return status;   
}


MStatus EulerArrayData::writeASCII(std::ostream &out)
{
    MStatus status;

    std::vector<double> values = this->getValues();
    status = writeASCIIData(values, this->length(), out);

    return status; 
}


MStatus EulerArrayData::readBinary(std::istream &in, unsigned int length)
{
    MStatus status; 

    std::vector<double> values = readBinaryData(3, in, length, &status);
    if (status) { this->setValues(values); }

    return status; 
}


MStatus EulerArrayData::writeBinary(std::ostream &out)
{
    MStatus status;

    std::vector<double> values = this->getValues();
    status = writeBinaryData(values, this->length(), out);

    return status; 
}


std::vector<MEulerRotation> EulerArrayData::getArray()
{
    return std::vector<MEulerRotation>(this->data);
}


void EulerArrayData::setArray(std::vector<MEulerRotation> array)
{
    this->data.resize(array.size());
    std::copy(
        array.begin(), 
        array.end(),
        this->data.begin()
    );
}


void EulerArrayData::setValues(std::vector<double> values)
{
    size_t numberOfValues = values.size();
    size_t numberOfItems = numberOfValues / 3;
    this->data.resize(numberOfItems);

    MAngle::Unit unit = MAngle::uiUnit();

    for (size_t i = 0; i < numberOfItems; i++)
    {
        this->data[i] = MEulerRotation(
            MAngle(values[(i*3)+0], unit).asRadians(),
            MAngle(values[(i*3)+1], unit).asRadians(),
            MAngle(values[(i*3)+2], unit).asRadians(),
            MEulerRotation::RotationOrder::kXYZ
        );
    }
}


std::vector<double> EulerArrayData::getValues()
{
    std::vector<double> values;
    size_t numberOfItems = this->data.size();
    values.resize(numberOfItems * 3);

    for (size_t i = 0; i < numberOfItems; i++)
    {
        MEulerRotation &rot = this->data[i];
        values[(i*3)+0] = rot.x;
        values[(i*3)+1] = rot.y;
        values[(i*3)+2] = rot.z;
    }

    return values;
}

void EulerArrayData::copy(const MPxData& other)
{
    if (this->typeId() == other.typeId())
    {
        const EulerArrayData &otherData = (const EulerArrayData &) other;
        this->data.resize(otherData.data.size());
        std::copy(
            otherData.data.begin(), 
            otherData.data.end(),
            this->data.begin()
        );
    }
}

MTypeId EulerArrayData::typeId() const
{
    return EulerArrayData::TYPE_ID;
}


MString EulerArrayData::name() const
{ 
    return EulerArrayData::TYPE_NAME; 
}