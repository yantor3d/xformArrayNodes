/**
    Copyright (c) 2017 Ryan Porter
    You may use, distribute, or modify this code under the terms of the MIT license.
*/

/**
quatToEulerArray node
This node converts quaternion rotations to euler rotations.

    inputQuat (iq) quatArray 
        Array of quaternion rotations.

    inputRotateOrder (iro) enum
        Rotation order of the euler rotations.

    outputRotate (or) eulerArray
        Array of euler rotations.

*/

#include "../../data/eulerArrayData.h"
#include "../../data/quatArrayData.h"
#include "../nodeData.h"
#include "quatToEulerArrayNode.h"

#include <vector>

#include <maya/MDataBlock.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MPlug.h>
#include <maya/MPxNode.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>


MObject QuatToEulerArrayNode::inputQuatAttr;
MObject QuatToEulerArrayNode::inputRotateOrderAttr;

MObject QuatToEulerArrayNode::outputRotateAttr;


void* QuatToEulerArrayNode::creator()
{
    return new QuatToEulerArrayNode();
}


MStatus QuatToEulerArrayNode::initialize()
{
    MStatus status;

    MFnEnumAttribute E;
    MFnTypedAttribute T;

    inputQuatAttr = T.create("inputQuat", "iq", QuatArrayData::TYPE_ID, MObject::kNullObj, &status);

    inputRotateOrderAttr = E.create("inputRotateOrder", "iro", 0, &status);
    E.setChannelBox(true);
    E.setKeyable(true);
    E.addField("xyz", 0);
    E.addField("yzx", 1);
    E.addField("zxy", 2);
    E.addField("xzy", 3);
    E.addField("yxz", 4);
    E.addField("zyx", 5);

    addAttribute(inputQuatAttr);
    addAttribute(inputRotateOrderAttr);

    outputRotateAttr = T.create("outputRotate", "or", EulerArrayData::TYPE_ID, MObject::kNullObj, &status);
    T.setStorable(false);
    
    addAttribute(outputRotateAttr);

    attributeAffects(inputQuatAttr, outputRotateAttr);
    attributeAffects(inputRotateOrderAttr, outputRotateAttr);

    return MStatus::kSuccess;
}


MStatus QuatToEulerArrayNode::compute(const MPlug& plug, MDataBlock& data)
{
    MStatus status;
    
    if (plug != outputRotateAttr)
    {
        return MStatus::kInvalidParameter;
    }

    MDataHandle inputHandle = data.inputValue(inputQuatAttr);
    std::vector<MQuaternion> input = getUserArray<MQuaternion, QuatArrayData>(inputHandle);

    short rotateOrderIndex = data.inputValue(inputRotateOrderAttr).asShort();
    MEulerRotation::RotationOrder rotateOrder = (MEulerRotation::RotationOrder) rotateOrderIndex;

    std::vector<MEulerRotation> output(input.size());

    for (size_t i = 0; i < input.size(); i++)
    {
        output[i] = input[i].asEulerRotation();
        output[i].reorderIt(rotateOrder);
    }

    MDataHandle outputHandle = data.outputValue(outputRotateAttr);
    setUserArray<MEulerRotation, EulerArrayData>(outputHandle, output);

    return MStatus::kSuccess;   
}