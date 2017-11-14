/**
    Copyright (c) 2017 Ryan Porter
    You may use, distribute, or modify this code under the terms of the MIT license.
*/

/**
eulerToQuatArray node
This node converts euler rotations to quaternion rotations.

    inputRotate (ir) eulerArray 
        Array of euler rotations.

    inputRotateOrder (iro) enum
        Rotation order of the euler rotations.

    outputQuat (oq) quatArray
        Array of quaternion rotations.

*/

#include "../../data/eulerArrayData.h"
#include "../../data/quatArrayData.h"
#include "../nodeData.h"
#include "eulerToQuatArrayNode.h"

#include <vector>

#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MEulerRotation.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MPlug.h>
#include <maya/MPxNode.h>
#include <maya/MQuaternion.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>


MObject EulerToQuatArrayNode::inputRotateAttr;
MObject EulerToQuatArrayNode::inputRotateOrderAttr;

MObject EulerToQuatArrayNode::outputQuatAttr;


void* EulerToQuatArrayNode::creator()
{
    return new EulerToQuatArrayNode();
}


MStatus EulerToQuatArrayNode::initialize()
{
    MStatus status;

    MFnEnumAttribute E;
    MFnTypedAttribute T;

    inputRotateAttr = T.create("inputRotate", "ir", EulerArrayData::TYPE_ID, MObject::kNullObj, &status);

    inputRotateOrderAttr = E.create("inputRotateOrder", "iro", 0, &status);
    E.setChannelBox(true);
    E.setKeyable(true);
    E.addField("xyz", 0);
    E.addField("yzx", 1);
    E.addField("zxy", 2);
    E.addField("xzy", 3);
    E.addField("yxz", 4);
    E.addField("zyx", 5);

    addAttribute(inputRotateAttr);
    addAttribute(inputRotateOrderAttr);

    outputQuatAttr = T.create("outputQuat", "oq", QuatArrayData::TYPE_ID, MObject::kNullObj, &status);
    T.setStorable(false);
    
    addAttribute(outputQuatAttr);

    attributeAffects(inputRotateAttr, outputQuatAttr);
    attributeAffects(inputRotateOrderAttr, outputQuatAttr);

    return MStatus::kSuccess;
}


MStatus EulerToQuatArrayNode::compute(const MPlug& plug, MDataBlock& data)
{
    MStatus status;
    
    if (plug != outputQuatAttr)
    {
        return MStatus::kSuccess;
    }

    MDataHandle inputHandle = data.inputValue(inputRotateAttr);
    std::vector<MEulerRotation> input = getUserArray<MEulerRotation, EulerArrayData>(inputHandle);

    std::vector<MQuaternion> output(input.size());

    for (size_t i = 0; i < input.size(); i++)
    {
        output[i] = input[i].asQuaternion();
    }

    MDataHandle outputHandle = data.outputValue(outputQuatAttr);
    setUserArray<MQuaternion, QuatArrayData>(outputHandle, output);

    return MStatus::kSuccess;   
}