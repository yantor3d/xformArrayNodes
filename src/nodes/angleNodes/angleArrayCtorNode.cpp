/**
    Copyright (c) 2017 Ryan Porter
    You may use, distribute, or modify this code under the terms of the MIT license.
*/

/**
angleArrayCtor node
This node outputs a contiguous array of doubleAngle values.

    input (i) doubleAngle[]
        An array of doubleAngle values.

    fillValue (fv) doubleAngle3
        Default value. Indices that are missing from the input array will be filled with this value.

    size (s) int 
        Size of the output array.

    output (o) arrayAngle
        An array of doubleAngle values.
*/

#include <vector>

#include "../nodeData.h"
#include "../../data/angleArrayData.h"
#include "angleArrayCtorNode.h"

#include <maya/MAngle.h>
#include <maya/MDataBlock.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnNumericData.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MPlug.h>
#include <maya/MPxNode.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>


MObject AngleArrayCtorNode::inputAttr;
MObject AngleArrayCtorNode::fillValueAttr;
MObject AngleArrayCtorNode::sizeAttr;

MObject AngleArrayCtorNode::outputAttr;


void* AngleArrayCtorNode::creator()
{
    return new AngleArrayCtorNode();
}


MStatus AngleArrayCtorNode::initialize()
{
    MStatus status;

    MFnNumericAttribute N;
    MFnTypedAttribute T;
    MFnUnitAttribute U;

    inputAttr = U.create("input", "i", MFnUnitAttribute::kAngle, 0, &status);
    U.setArray(true);

    fillValueAttr = U.create("fillValue", "fv", MFnUnitAttribute::kAngle, 0, &status);
    U.setChannelBox(true);
    U.setKeyable(true);

    sizeAttr = N.create("size", "s", MFnNumericData::kInt, 8, &status);
    N.setMin(0);
    N.setChannelBox(true);
    N.setKeyable(true);
    
    addAttribute(inputAttr);
    addAttribute(fillValueAttr);
    addAttribute(sizeAttr);

    outputAttr = T.create("output", "o", AngleArrayData::TYPE_ID, MObject::kNullObj, &status);
    T.setStorable(false);

    addAttribute(outputAttr);

    attributeAffects(inputAttr, outputAttr);
    attributeAffects(fillValueAttr, outputAttr);
    attributeAffects(sizeAttr, outputAttr);

    return MStatus::kSuccess;
}


MStatus AngleArrayCtorNode::compute(const MPlug& plug, MDataBlock& data)
{
    MStatus status;

    if (plug != outputAttr)
    {
        return MStatus::kInvalidParameter;
    }

    MArrayDataHandle inputArrayHandle = data.inputArrayValue(inputAttr);
    MAngle fillValue = data.inputValue(fillValueAttr).asAngle();
    unsigned size = (unsigned) data.inputValue(sizeAttr).asInt();

    std::vector<MAngle> values = getArrayElements(
        inputArrayHandle,
        &AngleArrayCtorNode::getElement,
        size,
        fillValue
    );

    MDataHandle outputHandle = data.outputValue(outputAttr);

    setUserArray<MAngle, AngleArrayData>(outputHandle, values);

    return MStatus::kSuccess;   
}


MAngle AngleArrayCtorNode::getElement(MDataHandle &elementHandle)
{
    return elementHandle.asAngle();
}