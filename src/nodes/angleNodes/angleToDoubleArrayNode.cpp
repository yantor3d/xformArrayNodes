/**
    Copyright (c) 2017 Ryan Porter
    You may use, distribute, or modify this code under the terms of the MIT license.
*/

/**
angleToDoubleArray node
This node converts an array of doubleAngle values to an array of double values.

    input (i) angleArray
        An array of doubleAngle values.

    output (o) doubleArray
        An array of double values.

*/

#include "../../data/angleArrayData.h"
#include "../nodeData.h"
#include "angleToDoubleArrayNode.h"

#include <vector>

#include <maya/MAngle.h>
#include <maya/MDataBlock.h>
#include <maya/MDoubleArray.h>
#include <maya/MFnData.h>
#include <maya/MFnDoubleArrayData.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MPlug.h>
#include <maya/MPxNode.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>


MObject AngleToDoubleArrayNode::inputAttr;

MObject AngleToDoubleArrayNode::outputAttr;


void* AngleToDoubleArrayNode::creator()
{
    return new AngleToDoubleArrayNode();
}


MStatus AngleToDoubleArrayNode::initialize()
{
    MStatus status;

    MFnTypedAttribute T;

    inputAttr = T.create("input", "i", AngleArrayData::TYPE_ID, MObject::kNullObj, &status);

    addAttribute(inputAttr);

    outputAttr = T.create("output", "o", MFnData::kDoubleArray, MObject::kNullObj, &status);
    T.setStorable(false);
    
    addAttribute(outputAttr);

    attributeAffects(inputAttr, outputAttr);

    return MStatus::kSuccess;
}


MStatus AngleToDoubleArrayNode::compute(const MPlug& plug, MDataBlock& data)
{
    MStatus status;

    if (plug != outputAttr)
    {
        return MStatus::kSuccess;
    }

    MDataHandle inputHandle = data.inputValue(inputAttr);
    std::vector<MAngle> input = getUserArray<MAngle, AngleArrayData>(inputHandle);

    std::vector<double> output(input.size());

    MAngle::Unit unit = MAngle::uiUnit();

    for (size_t i = 0; i < input.size(); i++)
    {
        output[i] = input[i].as(unit);
    }

    MDataHandle outputHandle = data.outputValue(outputAttr);
    setMayaArray<double, MDoubleArray, MFnDoubleArrayData>(outputHandle, output);

    return MStatus::kSuccess;   
}