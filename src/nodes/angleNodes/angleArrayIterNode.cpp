/**
    Copyright (c) 2017 Ryan Porter
    You may use, distribute, or modify this code under the terms of the MIT license.
*/

/**
angleArrayIter node
This node outputs the elements of a contiguous doubleAngle array.

    input (i) angleArray
        An array of doubleAngle values.

    output (o) doubleAngle[]
        An array of doubleAngle values.

*/

#include "../../data/angleArrayData.h"
#include "../nodeData.h"
#include "angleArrayIterNode.h"

#include <vector>

#include <maya/MAngle.h>
#include <maya/MArrayDataBuilder.h>
#include <maya/MArrayDataHandle.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MGlobal.h>
#include <maya/MPlug.h>
#include <maya/MPxNode.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>


MObject AngleArrayIterNode::inputAttr;

MObject AngleArrayIterNode::outputAttr;


void* AngleArrayIterNode::creator()
{
    return new AngleArrayIterNode();
}


MStatus AngleArrayIterNode::initialize()
{
    MStatus status;

    MFnTypedAttribute T;
    MFnUnitAttribute U;

    inputAttr = T.create("input", "i", AngleArrayData::TYPE_ID, MObject::kNullObj, &status);

    addAttribute(inputAttr);

    outputAttr = U.create("output", "o", MFnUnitAttribute::kAngle, 0, &status);
    U.setArray(true);
    U.setStorable(false);
    U.setUsesArrayDataBuilder(true);

    addAttribute(outputAttr);

    attributeAffects(inputAttr, outputAttr);

    return MStatus::kSuccess;
}


MStatus AngleArrayIterNode::compute(const MPlug& plug, MDataBlock& data)
{
    MStatus status;

    if (plug != outputAttr)
    {
        return MStatus::kInvalidParameter;
    }

    MDataHandle inputHandle = data.inputValue(inputAttr);
    std::vector<MAngle> values = getUserArray<MAngle, AngleArrayData>(inputHandle);

    MArrayDataHandle outputArrayHandle = data.outputArrayValue(outputAttr, &status);
    status = setArrayElements<MAngle>(outputArrayHandle, values, &AngleArrayIterNode::setElement);

    return status;   
}

MStatus AngleArrayIterNode::setElement(MDataHandle &elementHandle, MAngle value)
{
    elementHandle.setMAngle(value);
    return MStatus::kSuccess;
}