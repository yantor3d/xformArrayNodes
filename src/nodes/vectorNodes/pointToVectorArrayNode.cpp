/**
    Copyright (c) 2017 Ryan Porter
    You may use, distribute, or modify this code under the terms of the MIT license.
*/

/**
pointToVectorArray node
This node converts points to vectors.

    inputPoint (ip) pointArray
        Array of points.

    outputVector (ov) vectorArray
        Array of vectors.

*/

#include "../nodeData.h"
#include "pointToVectorArrayNode.h"

#include <vector>

#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MFnData.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnPointArrayData.h>
#include <maya/MFnVectorArrayData.h>
#include <maya/MPlug.h>
#include <maya/MPoint.h>
#include <maya/MPointArray.h>
#include <maya/MPxNode.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>
#include <maya/MVector.h>
#include <maya/MVectorArray.h>


MObject PointToVectorArrayNode::inputPointAttr;

MObject PointToVectorArrayNode::outputVectorAttr;


void* PointToVectorArrayNode::creator()
{
    return new PointToVectorArrayNode();
}


MStatus PointToVectorArrayNode::initialize()
{
    MStatus status;

    MFnTypedAttribute T;

    inputPointAttr = T.create("inputPoint", "ip", MFnData::kPointArray, MObject::kNullObj, &status);

    addAttribute(inputPointAttr);

    outputVectorAttr = T.create("outputVector", "ov", MFnData::kVectorArray, MObject::kNullObj, &status);
    T.setStorable(false);

    addAttribute(outputVectorAttr);

    attributeAffects(inputPointAttr, outputVectorAttr);

    return MStatus::kSuccess;
}


MStatus PointToVectorArrayNode::compute(const MPlug& plug, MDataBlock& data)
{
    MStatus status;

    if (plug != outputVectorAttr)
    {
        return MStatus::kInvalidParameter;
    }

    MDataHandle inputHandle = data.inputValue(inputPointAttr);
    MDataHandle outputHandle = data.outputValue(outputVectorAttr);

    std::vector<MPoint> input = getMayaArray<MPoint, MFnPointArrayData>(inputHandle);
    size_t numberOfValues = input.size();
    std::vector<MVector> output(numberOfValues);

    for (size_t i = 0; i < numberOfValues; i++)
    {
        output[i] = MVector(input[i]);
    }

    setMayaArray<MVector, MVectorArray, MFnVectorArrayData>(outputHandle, output);

    return MStatus::kSuccess;   
}