/**
    Copyright (c) 2017 Ryan Porter
    You may use, distribute, or modify this code under the terms of the MIT license.
*/

/**
slerpQuatArray node
This node computes the slerp (Spherical linear interpolation) of pairs of quaternions.

    inputQuat1 (iq1) quatArray
        Array of quaternions to rotate from.

    inputQuat2 (iq1) quatArray
        Array of quaternions to rotate to.

    tween (t) double
        Interpolation values.

    spin (s) int
        Number of complete revolutions around the axis.

    outputQuat (oq)  quatArray
        Array of iterpolated quaternions.

*/

#include "../../data/quatArrayData.h"
#include "../nodeData.h"
#include "slerpQuatArrayNode.h"

#include <algorithm>
#include <vector>

#include <maya/MDataBlock.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnNumericData.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MPlug.h>
#include <maya/MPxNode.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>


MObject SlerpQuatArrayNode::inputQuat1Attr;
MObject SlerpQuatArrayNode::inputQuat2Attr;
MObject SlerpQuatArrayNode::tweenAttr;
MObject SlerpQuatArrayNode::spinAttr;

MObject SlerpQuatArrayNode::outputQuatAttr;


void* SlerpQuatArrayNode::creator()
{
    return new SlerpQuatArrayNode();
}


MStatus SlerpQuatArrayNode::initialize()
{
    MStatus status;

    MFnNumericAttribute N;
    MFnTypedAttribute T;

    inputQuat1Attr = T.create("inputQuat1", "iq1", QuatArrayData::TYPE_ID, MObject::kNullObj, &status);
    inputQuat2Attr = T.create("inputQuat2", "iq1", QuatArrayData::TYPE_ID, MObject::kNullObj, &status);

    tweenAttr = N.create("tween", "t", MFnNumericData::kDouble, 0.5, &status);
    N.setMin(0.0);
    N.setMax(1.0);

    spinAttr  = N.create("spin",  "s", MFnNumericData::kInt, 1, &status);
    N.setMin(-1);

    addAttribute(inputQuat1Attr);
    addAttribute(inputQuat2Attr);
    addAttribute(tweenAttr);
    addAttribute(spinAttr);

    outputQuatAttr = T.create("outputQuat", "oq", QuatArrayData::TYPE_ID, MObject::kNullObj, &status);
    T.setStorable(false);

    addAttribute(outputQuatAttr);

    attributeAffects(inputQuat1Attr, outputQuatAttr);
    attributeAffects(inputQuat2Attr, outputQuatAttr);
    attributeAffects(tweenAttr, outputQuatAttr);
    attributeAffects(spinAttr, outputQuatAttr);

    return MStatus::kSuccess;
}


MStatus SlerpQuatArrayNode::compute(const MPlug& plug, MDataBlock& data)
{
    MStatus status;
    
    if (plug != outputQuatAttr)
    {
        return MStatus::kSuccess;
    }

    MDataHandle input1Handle = data.inputValue(inputQuat1Attr);
    MDataHandle input2Handle = data.inputValue(inputQuat2Attr);
    
    double tween = data.inputValue(tweenAttr).asDouble();
    short spin = (short) data.inputValue(spinAttr).asInt();

    if (spin == 0) { spin = 1; }
    
    std::vector<MQuaternion> input1 = getUserArray<MQuaternion, QuatArrayData>(input1Handle);
    std::vector<MQuaternion> input2 = getUserArray<MQuaternion, QuatArrayData>(input2Handle);

    size_t numberOfValues = std::max(input1.size(), input2.size());

    input1.resize(numberOfValues);
    input2.resize(numberOfValues);

    std::vector<MQuaternion> output(numberOfValues);

    for (size_t i = 0; i < numberOfValues; i++)
    {
        output[i] = slerp(input1[i], input2[i], tween, spin);
    }

    MDataHandle outputHandle = data.outputValue(outputQuatAttr);
    setUserArray<MQuaternion, QuatArrayData>(outputHandle, output);

    return MStatus::kSuccess;   
}