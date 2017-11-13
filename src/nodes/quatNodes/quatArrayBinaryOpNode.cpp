/**
    Copyright (c) 2017 Ryan Porter
    You may use, distribute, or modify this code under the terms of the MIT license.
*/

/**
quatArrayBinaryOp node
This node performs a binary operation on pairs of quaternions.

    inputQuat1 (iq1) quatArray
        First array of quaternions, used for all computations.

    inputQuat2 (iq1) quatArray
        Second array of quaternions, used for all computations.

    operation (op) enum
        Specifies the binary operation to be performed.

        No Operation (0) copies the values of "inputQuat" to "outputQuat".
        Add          (2) calculates the sum of pairs of quaternions.
        Subtract     (3) calculates the difference between pairs of quaternions.
        Product      (4) calculates the product of pairs of quaternions.

    outputQuat (oq) quatArray
        Results of the binary operations.

*/

#include "../../data/quatArrayData.h"
#include "../nodeData.h"
#include "quatArrayBinaryOpNode.h"

#include <algorithm>
#include <vector>

#include <maya/MDataBlock.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MPlug.h>
#include <maya/MPxNode.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>

const short ADD      = 1;
const short SUBTRACT = 2;
const short PRODUCT  = 3;

MObject QuatArrayBinaryOpNode::inputQuat1Attr;
MObject QuatArrayBinaryOpNode::inputQuat2Attr;
MObject QuatArrayBinaryOpNode::operationAttr;

MObject QuatArrayBinaryOpNode::outputQuatAttr;


void* QuatArrayBinaryOpNode::creator()
{
    return new QuatArrayBinaryOpNode();
}


MStatus QuatArrayBinaryOpNode::initialize()
{
    MStatus status;

    MFnEnumAttribute E;
    MFnTypedAttribute T;

    inputQuat1Attr = T.create("inputQuat1", "iq1", QuatArrayData::TYPE_ID, MObject::kNullObj, &status);
    inputQuat2Attr = T.create("inputQuat2", "iq1", QuatArrayData::TYPE_ID, MObject::kNullObj, &status);

    operationAttr = E.create("operation", "op", 1, &status);
    E.setChannelBox(true);
    E.setKeyable(true);
    E.addField("No Operation", 0);
    E.addField("Add", ADD);
    E.addField("Subtract", SUBTRACT);
    E.addField("Product", PRODUCT);

    addAttribute(inputQuat1Attr);
    addAttribute(inputQuat2Attr);
    addAttribute(operationAttr);

    outputQuatAttr = T.create("outputQuat", "oq", QuatArrayData::TYPE_ID, MObject::kNullObj, &status);
    T.setStorable(false);

    addAttribute(outputQuatAttr);

    attributeAffects(inputQuat1Attr, outputQuatAttr);
    attributeAffects(inputQuat2Attr, outputQuatAttr);
    attributeAffects(operationAttr, outputQuatAttr);

    return MStatus::kSuccess;
}


MStatus QuatArrayBinaryOpNode::compute(const MPlug& plug, MDataBlock& data)
{
    MStatus status;
    
    if (plug != outputQuatAttr)
    {
        return MStatus::kSuccess;
    }

    MDataHandle input1Handle = data.inputValue(inputQuat1Attr);
    MDataHandle input2Handle = data.inputValue(inputQuat2Attr);
    short operation = data.inputValue(operationAttr).asShort();

    std::vector<MQuaternion> input1 = getUserArray<MQuaternion, QuatArrayData>(input1Handle);
    std::vector<MQuaternion> input2 = getUserArray<MQuaternion, QuatArrayData>(input2Handle);

    size_t numberOfValues = std::max(input1.size(), input2.size());

    input1.resize(numberOfValues);
    input2.resize(numberOfValues);

    std::vector<MQuaternion> output(numberOfValues);

    MQuaternion (*F)(MQuaternion, MQuaternion) = &QuatArrayBinaryOpNode::quatNop;

    switch (operation)
    {
        case ADD:      F = &QuatArrayBinaryOpNode::quatAdd; break;
        case SUBTRACT: F = &QuatArrayBinaryOpNode::quatSub; break;
        case PRODUCT:  F = &QuatArrayBinaryOpNode::quatProd; break;
    }

    for (size_t i = 0; i < numberOfValues; i++)
    {
        output[i] = F(input1[i], input2[i]);
    }

    MDataHandle outputHandle = data.outputValue(outputQuatAttr);
    setUserArray<MQuaternion, QuatArrayData>(outputHandle, output);

    return MStatus::kSuccess;   
}

MQuaternion QuatArrayBinaryOpNode::quatAdd(MQuaternion q1, MQuaternion q2)  { return q1 + q2; }
MQuaternion QuatArrayBinaryOpNode::quatSub(MQuaternion q1, MQuaternion q2)  { return q1 - q2; }
MQuaternion QuatArrayBinaryOpNode::quatProd(MQuaternion q1, MQuaternion q2) { return q1 * q2; }
MQuaternion QuatArrayBinaryOpNode::quatNop(MQuaternion q1, MQuaternion q2)  { return MQuaternion(q1); }