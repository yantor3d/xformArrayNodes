/**
    Copyright (c) 2017 Ryan Porter
    You may use, distribute, or modify this code under the terms of the MIT license.
*/

/**
doubleToAngleArray node
This node converts an array of double values to an array of doubleAngle values.

    input (i) doubleArray
        An array of double values.

    output (o) angleArray
        An array of doubleAngle values.

*/

#include "../../data/angleArrayData.h"
#include "../nodeData.h"
#include "doubleToAngleArrayNode.h"

#include <vector>

#include <maya/MDataBlock.h>
#include <maya/MDoubleArray.h>
#include <maya/MFnData.h>
#include <maya/MFnDoubleArrayData.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MPlug.h>
#include <maya/MPxNode.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>


MObject DoubleToAngleArrayNode::inputAttr;

MObject DoubleToAngleArrayNode::outputAttr;


void* DoubleToAngleArrayNode::creator()
{
    return new DoubleToAngleArrayNode();
}


MStatus DoubleToAngleArrayNode::initialize()
{
    MStatus status;

    MFnTypedAttribute T;

    inputAttr = T.create("input", "i", MFnData::kDoubleArray, MObject::kNullObj, &status);

    addAttribute(inputAttr);

    outputAttr = T.create("output", "o", AngleArrayData::TYPE_ID, MObject::kNullObj, &status);
    T.setStorable(false);
    
    addAttribute(outputAttr);

    attributeAffects(inputAttr, outputAttr);

    return MStatus::kSuccess;
}


MStatus DoubleToAngleArrayNode::compute(const MPlug& plug, MDataBlock& data)
{
    MStatus status;
    
    if (plug != outputAttr)
    {
        return MStatus::kInvalidParameter;
    }

    MDataHandle inputHandle = data.inputValue(inputAttr);
    std::vector<double> input = getMayaArray<double, MFnDoubleArrayData>(inputHandle);

    std::vector<MAngle> output(input.size());

    MAngle::Unit unit = MAngle::uiUnit();

    for (size_t i = 0; i < input.size(); i++)
    {
        output[i] = MAngle(input[i], unit);
    }

    MDataHandle outputHandle = data.outputValue(outputAttr);
    setUserArray<MAngle, AngleArrayData>(outputHandle, output);

    return MStatus::kSuccess;   
}