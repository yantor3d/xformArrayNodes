/**
    Copyright (c) 2017 Ryan Porter
    You may use, distribute, or modify this code under the terms of the MIT license.
*/

/**
rotateVectorArray node
This node computes the result of rotating vectors.

    inputVector (iv) vectorArray
        Array of vectors to rotate.

    inputQuat (iq) quatArray
        Rotations to be applied to the vectors as expressed by quaternions.

    inputRotate (ir) eulerArray 
        Rotations to be applied to the vectors as expressed by euler rotations.

    inputAxis (iax) vectorArray
        Pivot axes of the rotations to be applied to the vectors.

    inputAngle (ia) angleArray
        Rotations about an axis to be applied to the vectors.

    operation (op) enum
        Specifies how the vectors are rotated.

        No Operation      (0) does not rotate the vectors
        By Axis/Angle     (1) rotates the vectors using the "inputAxis" and "inputAngle" values.
        By Euler Rotation (2) rotates the vectors using the "inputRotate" values.
        By Quaternion     (3) rotates the vectors using the "inputQuat" values.

    outputVector (ov) vectorArray
        Array of vectors calculated by this node.

*/

#include "../../data/angleArrayData.h"
#include "../../data/eulerArrayData.h"
#include "../../data/quatArrayData.h"
#include "../nodeData.h"
#include "rotateVectorArrayNode.h"

#include <algorithm>
#include <vector>

#include <maya/MAngle.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MFnData.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnVectorArrayData.h>
#include <maya/MPlug.h>
#include <maya/MPxNode.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>
#include <maya/MVector.h>
#include <maya/MVectorArray.h>

const short AXIS_ANGLE   = 1;
const short EULER_ROTATE = 2;
const short QUATERNION   = 3;

MObject RotateVectorArrayNode::inputVectorAttr;
MObject RotateVectorArrayNode::inputQuatAttr;
MObject RotateVectorArrayNode::inputRotateAttr;
MObject RotateVectorArrayNode::inputAxisAttr;
MObject RotateVectorArrayNode::inputAngleAttr;
MObject RotateVectorArrayNode::operationAttr;

MObject RotateVectorArrayNode::outputVectorAttr;


void* RotateVectorArrayNode::creator()
{
    return new RotateVectorArrayNode();
}


MStatus RotateVectorArrayNode::initialize()
{
    MStatus status;

    MFnEnumAttribute E;
    MFnTypedAttribute T;

    inputVectorAttr = T.create("inputVector", "iv",  MFnData::kVectorArray,   MObject::kNullObj, &status);
    inputQuatAttr   = T.create("inputQuat",   "iq",  QuatArrayData::TYPE_ID,  MObject::kNullObj, &status);
    inputRotateAttr = T.create("inputRotate", "ir",  EulerArrayData::TYPE_ID, MObject::kNullObj, &status);
    inputAxisAttr   = T.create("inputAxis",   "iax", MFnData::kVectorArray,   MObject::kNullObj, &status);
    inputAngleAttr  = T.create("inputAngle",  "ia",  AngleArrayData::TYPE_ID, MObject::kNullObj,  &status);

    operationAttr = E.create("operation", "op", 1, &status);
    E.setChannelBox(true);
    E.setKeyable(true);
    E.addField("No Operation", 0);
    E.addField("Rotate by Axis/Angle",     AXIS_ANGLE);
    E.addField("Rotate by Euler Rotation", EULER_ROTATE);
    E.addField("Rotate by Quaternion",     QUATERNION);

    addAttribute(inputVectorAttr);
    addAttribute(inputQuatAttr);
    addAttribute(inputRotateAttr);
    addAttribute(inputAxisAttr);
    addAttribute(inputAngleAttr);
    addAttribute(operationAttr);

    outputVectorAttr = T.create("outputVector", "ov", MFnData::kVectorArray, MObject::kNullObj, &status);
    T.setStorable(false);

    addAttribute(outputVectorAttr);

    attributeAffects(inputVectorAttr, outputVectorAttr);
    attributeAffects(inputQuatAttr, outputVectorAttr);
    attributeAffects(inputRotateAttr, outputVectorAttr);
    attributeAffects(inputAxisAttr, outputVectorAttr);
    attributeAffects(inputAngleAttr, outputVectorAttr);
    attributeAffects(operationAttr, outputVectorAttr);

    return MStatus::kSuccess;
}


MStatus RotateVectorArrayNode::compute(const MPlug& plug, MDataBlock& data)
{
    MStatus status;

    if (plug != outputVectorAttr)
    {
        return MStatus::kUnknownParameter;
    }

    short operation = data.inputValue(operationAttr).asShort();

    MDataHandle inputHandle = data.inputValue(inputVectorAttr);
    std::vector<MVector> input = getMayaArray<MVector, MFnVectorArrayData>(inputHandle);

    size_t numberOfValues = input.size();

    std::vector<MVector> output(numberOfValues);

    if (operation == AXIS_ANGLE)
    {
        MDataHandle axisHandle  = data.inputValue(inputAxisAttr);
        MDataHandle angleHandle = data.inputValue(inputAngleAttr);

        std::vector<MVector> axis   = getMayaArray<MVector, MFnVectorArrayData>(axisHandle);
        std::vector<MAngle>  angle  = getUserArray<MAngle, AngleArrayData>(angleHandle);

        axis.resize(numberOfValues);
        angle.resize(numberOfValues);

        for (size_t i = 0; i < numberOfValues; i++)
        {
            // todo: write equation that does this;
        }        
    } else if (operation == EULER_ROTATE) {
        MDataHandle rotateHandle = data.inputValue(inputRotateAttr);

        std::vector<MEulerRotation> rotate = getUserArray<MEulerRotation, EulerArrayData>(rotateHandle);
        rotate.resize(numberOfValues);

        for (size_t i = 0; i < numberOfValues; i++)
        {
            output[i] = input[i].rotateBy(rotate[i]);
        }  
    } else if (operation == AXIS_ANGLE) {
        MDataHandle rotateHandle = data.inputValue(inputQuatAttr);

        std::vector<MQuaternion> rotate = getUserArray<MQuaternion, QuatArrayData>(rotateHandle);
        rotate.resize(numberOfValues);

        for (size_t i = 0; i < numberOfValues; i++)
        {
            output[i] = input[i].rotateBy(rotate[i]);
        }  
    }

    MDataHandle outputHandle = data.outputValue(outputVectorAttr);
    setMayaArray<MVector, MVectorArray, MFnVectorArrayData>(outputHandle, output);

    return MStatus::kSuccess;   
}