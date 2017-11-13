/**
    Copyright (c) 2017 Ryan Porter
    You may use, distribute, or modify this code under the terms of the MIT license.
*/

/**
vectorToPointArray node
This node converts vectors to points.

    inputVector (iv1) vectorArray
        Array of vectors.

    outputPoint (op) pointArray
        Array of points.
        Array of vectors calculated by this node.

*/

#include "../nodeData.h"
#include "vectorToPointArrayNode.h"

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


MObject VectorToPointArrayNode::inputVectorAttr;

MObject VectorToPointArrayNode::outputPointAttr;


void* VectorToPointArrayNode::creator()
{
    return new VectorToPointArrayNode();
}


MStatus VectorToPointArrayNode::initialize()
{
    MStatus status;

    MFnTypedAttribute T;

    inputVectorAttr = T.create("inputVector", "iv", MFnData::kVectorArray, MObject::kNullObj, &status);

    addAttribute(inputVectorAttr);

    outputPointAttr = T.create("outputPoint", "op", MFnData::kPointArray, MObject::kNullObj, &status);
    T.setStorable(false);

    addAttribute(outputPointAttr);

    attributeAffects(inputVectorAttr, outputPointAttr);

    return MStatus::kSuccess;
}


MStatus VectorToPointArrayNode::compute(const MPlug& plug, MDataBlock& data)
{
    MStatus status;

    if (plug != outputPointAttr)
    {
        return MStatus::kUnknownParameter;
    }

    MDataHandle inputHandle = data.inputValue(inputVectorAttr);
    MDataHandle outputHandle = data.outputValue(outputPointAttr);

    std::vector<MVector> input = getMayaArray<MVector, MFnVectorArrayData>(inputHandle);
    size_t numberOfValues = input.size();
    std::vector<MPoint> output(numberOfValues);

    for (size_t i = 0; i < numberOfValues; i++)
    {
        output[i] = MPoint(input[i]);
    }

    setMayaArray<MPoint, MPointArray, MFnPointArrayData>(outputHandle, output);

    return MStatus::kSuccess;   
}