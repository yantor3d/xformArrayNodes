/**
    Copyright (c) 2017 Ryan Porter
    You may use, distribute, or modify this code under the terms of the MIT license.
*/

/**
unpackQuatArray node
This node unpacks quaternion rotation components from an array.

    inputQuat (iq) quatArray
        Array of quaternion rotations to unpack.

    outputQuat (oq) compound[]
        Unpacked quaternion rotations.

    outputQuatX (oqx) double
        X component of a quaternion rotation.

    outputQuatY (oqy) double
        Y component of a quaternion rotation.

    outputQuatZ (oqz) double
        Z component of a quaternion rotation.

    outputQuatW (oqw) double
        W component of a quaternion rotation.

    outputX (ox) doubleArray
        X components of thequaternion rotations.

    outputY (oy) doubleArray
        Y components of the quaternion rotations.

    outputZ (oz) doubleArray
        Z components of the quaternion rotations.

    outputW (ow) doubleArray
        W components of the quaternion rotations.

    outputAxis (oax) vectorArray 
        Pivot axes of the quaternion rotations.

    outputAngle (oa) angleArray
        Rotations about the pivot axes of the quaternion rotations.

*/

#include "unpackQuatArrayNode.h"
#include "../nodeData.h"
#include "../../data/angleArrayData.h"
#include "../../data/quatArrayData.h"

#include <vector>

#include <maya/MAngle.h>
#include <maya/MArrayDataHandle.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
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
#include <maya/MQuaternion.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>
#include <maya/MVector.h>
#include <maya/MVectorArray.h>


MObject UnpackQuatArrayNode::inputQuatAttr;

MObject UnpackQuatArrayNode::outputQuatAttr;
MObject UnpackQuatArrayNode::outputQuatXAttr;
MObject UnpackQuatArrayNode::outputQuatYAttr;
MObject UnpackQuatArrayNode::outputQuatZAttr;
MObject UnpackQuatArrayNode::outputQuatWAttr;
MObject UnpackQuatArrayNode::outputXAttr;
MObject UnpackQuatArrayNode::outputYAttr;
MObject UnpackQuatArrayNode::outputZAttr;
MObject UnpackQuatArrayNode::outputWAttr;
MObject UnpackQuatArrayNode::outputAxisAttr;
MObject UnpackQuatArrayNode::outputAngleAttr;


void* UnpackQuatArrayNode::creator()
{
    return new UnpackQuatArrayNode();
}


MStatus UnpackQuatArrayNode::initialize()
{
    MStatus status;

    MFnCompoundAttribute C;
    MFnNumericAttribute N;
    MFnTypedAttribute T;

    inputQuatAttr = T.create("inputQuat", "iq", QuatArrayData::TYPE_ID, MObject::kNullObj, &status);

    addAttribute(inputQuatAttr);

    outputQuatXAttr = N.create("outputQuatX", "oqx", MFnNumericData::kDouble, 0, &status);
    outputQuatYAttr = N.create("outputQuatY", "oqy", MFnNumericData::kDouble, 0, &status);
    outputQuatZAttr = N.create("outputQuatZ", "oqz", MFnNumericData::kDouble, 0, &status);
    outputQuatWAttr = N.create("outputQuatW", "oqw", MFnNumericData::kDouble, 0, &status);
    outputQuatAttr  = C.create("outputQuat", "oq", &status);
    C.setArray(true);    
    C.setUsesArrayDataBuilder(true);
    C.addChild(outputQuatXAttr);
    C.addChild(outputQuatYAttr);
    C.addChild(outputQuatZAttr);
    C.addChild(outputQuatWAttr);

    outputXAttr = T.create("outputX", "ox", MFnData::kDoubleArray, MObject::kNullObj, &status);
    T.setStorable(false);
    outputYAttr = T.create("outputY", "oy", MFnData::kDoubleArray, MObject::kNullObj, &status);
    T.setStorable(false);
    outputZAttr = T.create("outputZ", "oz", MFnData::kDoubleArray, MObject::kNullObj, &status);
    T.setStorable(false);
    outputWAttr = T.create("outputW", "ow", MFnData::kDoubleArray, MObject::kNullObj, &status);
    T.setStorable(false);

    outputAxisAttr =  T.create("outputAxis", "oax", MFnData::kVectorArray, MObject::kNullObj, &status);
    T.setStorable(false);
    outputAngleAttr = T.create("outputAngle", "oa", AngleArrayData::TYPE_ID, MObject::kNullObj, &status);
    T.setStorable(false);

    addAttribute(outputQuatAttr);
    addAttribute(outputXAttr);
    addAttribute(outputYAttr);
    addAttribute(outputZAttr);
    addAttribute(outputWAttr);
    addAttribute(outputAxisAttr);
    addAttribute(outputAngleAttr);

    attributeAffects(inputQuatAttr, outputQuatAttr);
    attributeAffects(inputQuatAttr, outputXAttr);
    attributeAffects(inputQuatAttr, outputYAttr);
    attributeAffects(inputQuatAttr, outputZAttr);
    attributeAffects(inputQuatAttr, outputWAttr);
    attributeAffects(inputQuatAttr, outputAxisAttr);
    attributeAffects(inputQuatAttr, outputAngleAttr);

    return MStatus::kSuccess;
}


MStatus UnpackQuatArrayNode::compute(const MPlug& plug, MDataBlock& data)
{
    MStatus status;

    if (
        plug != outputQuatAttr && 
        plug != outputXAttr    &&
        plug != outputYAttr    &&
        plug != outputZAttr    &&
        plug != outputWAttr    &&
        plug != outputAxisAttr &&
        plug != outputAngleAttr
    ) {
        return MStatus::kUnknownParameter;
    }

    MDataHandle inputHandle = data.inputValue(inputQuatAttr);

    std::vector<MQuaternion> input = getUserArray<MQuaternion, QuatArrayData>(inputHandle);

    size_t numberOfValues = input.size();

    std::vector<double> outputX(numberOfValues);
    std::vector<double> outputY(numberOfValues);
    std::vector<double> outputZ(numberOfValues);
    std::vector<double> outputW(numberOfValues);

    std::vector<MVector> outputAxis(numberOfValues);
    std::vector<MAngle>  outputAngle(numberOfValues);

    double theta = 0.0;

    for (size_t i = 0; i < numberOfValues; i++)
    {
        MQuaternion &q = input[i];

        q.getAxisAngle(outputAxis[i], theta);
        outputAngle[i] = MAngle(theta);

        outputX[i] = q.x;
        outputY[i] = q.y;
        outputZ[i] = q.z;
        outputW[i] = q.w;
    }

    MArrayDataHandle outputArrayHandle = data.outputArrayValue(outputQuatAttr);
    setArrayElements<MQuaternion>(outputArrayHandle, input, &UnpackQuatArrayNode::setElement);

    MDataHandle outputXHandle = data.outputValue(outputXAttr);
    MDataHandle outputYHandle = data.outputValue(outputYAttr);
    MDataHandle outputZHandle = data.outputValue(outputZAttr);
    MDataHandle outputWHandle = data.outputValue(outputWAttr);

    setMayaArray<double, MDoubleArray, MFnDoubleArrayData>(outputXHandle, outputX);
    setMayaArray<double, MDoubleArray, MFnDoubleArrayData>(outputYHandle, outputY);
    setMayaArray<double, MDoubleArray, MFnDoubleArrayData>(outputZHandle, outputZ);
    setMayaArray<double, MDoubleArray, MFnDoubleArrayData>(outputWHandle, outputW);

    MDataHandle outputAxisHandle = data.outputValue(outputAxisAttr);
    MDataHandle outputAngleHandle = data.outputValue(outputAngleAttr);

    setMayaArray<MVector, MVectorArray, MFnVectorArrayData>(outputAxisHandle, outputAxis);
    setUserArray<MAngle, AngleArrayData>(outputAngleHandle, outputAngle);

    return MStatus::kSuccess;   
}

MStatus UnpackQuatArrayNode::setElement(MDataHandle &elementHandle, MQuaternion q)
{
    MDataHandle xHandle = elementHandle.child(outputQuatXAttr);
    MDataHandle yHandle = elementHandle.child(outputQuatYAttr);
    MDataHandle zHandle = elementHandle.child(outputQuatZAttr);
    MDataHandle wHandle = elementHandle.child(outputQuatWAttr);

    xHandle.setDouble(q.x);
    yHandle.setDouble(q.y);
    zHandle.setDouble(q.z);
    wHandle.setDouble(q.w);

    xHandle.setClean();
    yHandle.setClean();
    zHandle.setClean();
    wHandle.setClean();

    return MStatus::kSuccess;
}