/**
    Copyright (c) 2017 Ryan Porter
    You may use, distribute, or modify this code under the terms of the MIT license.
*/

/**
unpackVectorArray node
This node unpacks vectors into their components.

    inputVector (iv) vectorArray
        Array of vectors to be unpacked.

    outputVector (ov) double3[]
        List of vector components.

    outputVectorX (ovx) double
        X component of a vector.

    outputVectorY (ovy) double
        Y component of a vector.

    outputVectorZ (ovz) double
        Z component of a vector.

    outputX (ox) doubleArray
        X components of the input vectors.

    outputY (oy) doubleArray
        Y components of the input vectors.

    outputZ (oz) doubleArray
        Z components of the input vectors.

*/

#include "../nodeData.h"
#include "unpackVectorArrayNode.h"

#include <vector>

#include <maya/MDataBlock.h>
#include <maya/MDoubleArray.h>
#include <maya/MFnCompoundAttribute.h>
#include <maya/MFnData.h>
#include <maya/MFnDoubleArrayData.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnNumericData.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnVectorArrayData.h>
#include <maya/MPlug.h>
#include <maya/MPxNode.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>
#include <maya/MVector.h>
#include <maya/MVectorArray.h>


MObject UnpackVectorArrayNode::inputVectorAttr;

MObject UnpackVectorArrayNode::outputVectorAttr;
MObject UnpackVectorArrayNode::outputVectorXAttr;
MObject UnpackVectorArrayNode::outputVectorYAttr;
MObject UnpackVectorArrayNode::outputVectorZAttr;
MObject UnpackVectorArrayNode::outputXAttr;
MObject UnpackVectorArrayNode::outputYAttr;
MObject UnpackVectorArrayNode::outputZAttr;


void* UnpackVectorArrayNode::creator()
{
    return new UnpackVectorArrayNode();
}


MStatus UnpackVectorArrayNode::initialize()
{
    MStatus status;

    MFnCompoundAttribute C;
    MFnNumericAttribute N;
    MFnTypedAttribute T;

    inputVectorAttr = T.create("inputVector", "iv", MFnData::kVectorArray, MObject::kNullObj, &status);

    addAttribute(inputVectorAttr);

    outputVectorXAttr = N.create("outputVectorX", "ovx", MFnNumericData::kDouble, 0, &status);
    outputVectorYAttr = N.create("outputVectorY", "ovy", MFnNumericData::kDouble, 0, &status);
    outputVectorZAttr = N.create("outputVectorZ", "ovz", MFnNumericData::kDouble, 0, &status);
    outputVectorAttr = C.create("outputVector", "ov", &status);
    C.setArray(true);
    C.setStorable(false);
    C.setUsesArrayDataBuilder(true);
    C.setStorable(false);
    C.addChild(outputVectorXAttr);
    C.addChild(outputVectorYAttr);
    C.addChild(outputVectorZAttr);

    outputXAttr = T.create("outputX", "ox", MFnData::kDoubleArray, MObject::kNullObj, &status);
    T.setStorable(false);
    outputYAttr = T.create("outputY", "oy", MFnData::kDoubleArray, MObject::kNullObj, &status);
    T.setStorable(false);
    outputZAttr = T.create("outputZ", "oz", MFnData::kDoubleArray, MObject::kNullObj, &status);
    T.setStorable(false);

    addAttribute(outputVectorAttr);
    addAttribute(outputXAttr);
    addAttribute(outputYAttr);
    addAttribute(outputZAttr);

    attributeAffects(inputVectorAttr, outputVectorAttr);
    attributeAffects(inputVectorAttr, outputXAttr);
    attributeAffects(inputVectorAttr, outputYAttr);
    attributeAffects(inputVectorAttr, outputZAttr);

    return MStatus::kSuccess;
}


MStatus UnpackVectorArrayNode::compute(const MPlug& plug, MDataBlock& data)
{
    MStatus status;

    if (
        plug != outputVectorAttr && 
        plug.parent() != outputVectorAttr &&
        plug != outputXAttr    &&
        plug != outputYAttr    &&
        plug != outputZAttr
    ) {
        return MStatus::kInvalidParameter;
    }

    MDataHandle inputHandle = data.inputValue(inputVectorAttr);

    std::vector<MVector> input = getMayaArray<MVector, MFnVectorArrayData>(inputHandle);

    size_t numberOfValues = input.size();

    std::vector<double> outputX(numberOfValues);
    std::vector<double> outputY(numberOfValues);
    std::vector<double> outputZ(numberOfValues);

    for (size_t i = 0; i < numberOfValues; i++)
    {
        MVector &v = input[i];

        outputX[i] = v.x;
        outputY[i] = v.y;
        outputZ[i] = v.z;
    }

    MArrayDataHandle outputArrayHandle = data.outputArrayValue(outputVectorAttr);
    setArrayElements<MVector>(outputArrayHandle, input, &UnpackVectorArrayNode::setElement);

    MDataHandle outputXHandle = data.outputValue(outputXAttr);
    MDataHandle outputYHandle = data.outputValue(outputYAttr);
    MDataHandle outputZHandle = data.outputValue(outputZAttr);

    setMayaArray<double, MDoubleArray, MFnDoubleArrayData>(outputXHandle, outputX);
    setMayaArray<double, MDoubleArray, MFnDoubleArrayData>(outputYHandle, outputY);
    setMayaArray<double, MDoubleArray, MFnDoubleArrayData>(outputZHandle, outputZ);
    
    return MStatus::kSuccess;   
}


MStatus UnpackVectorArrayNode::setElement(MDataHandle &elementHandle, MVector v)
{
    MDataHandle xHandle = elementHandle.child(outputVectorXAttr);
    MDataHandle yHandle = elementHandle.child(outputVectorYAttr);
    MDataHandle zHandle = elementHandle.child(outputVectorZAttr);

    xHandle.setDouble(v.x);
    yHandle.setDouble(v.y);
    zHandle.setDouble(v.z);

    xHandle.setClean();
    yHandle.setClean();
    zHandle.setClean();

    return MStatus::kSuccess;
}