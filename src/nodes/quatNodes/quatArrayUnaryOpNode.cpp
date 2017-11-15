/**
    Copyright (c) 2017 Ryan Porter
    You may use, distribute, or modify this code under the terms of the MIT license.
*/

/**
quatArrayUnaryOp node
This node performs unary operations on quaternions.

    inputQuat (iq) quatArray
        An array of quaternions.

    operation (op) enum
        Specifies the unary operation to be performed.

        No Operation (0) copies the values of "inputQuat" to "outputQuat".
        Conjugate    (1) calculates the conjugate of each input.
        Inverse      (2) calculates the inverse of each input.
        Negate       (3) calculates the negative of each input.
        Normalize    (4) calculates the normal of each input.

    outputQuat (oq)
        Results of the unary operations.

*/

#include "../../data/quatArrayData.h"
#include "../nodeData.h"
#include "quatArrayUnaryOpNode.h"

#include <vector>

#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MPlug.h>
#include <maya/MPxNode.h>
#include <maya/MQuaternion.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>

const short CONJUGATE = 1;
const short INVERSE   = 2;
const short NEGATE    = 3;
const short NORMALIZE = 4;

MObject QuatArrayUnaryOpNode::inputQuatAttr;
MObject QuatArrayUnaryOpNode::operationAttr;

MObject QuatArrayUnaryOpNode::outputQuatAttr;


void* QuatArrayUnaryOpNode::creator()
{
    return new QuatArrayUnaryOpNode();
}


MStatus QuatArrayUnaryOpNode::initialize()
{
    MStatus status;

    MFnEnumAttribute E;
    MFnTypedAttribute T;

    inputQuatAttr = T.create("inputQuat", "iq", QuatArrayData::TYPE_ID, MObject::kNullObj, &status);

    operationAttr = E.create("operation", "op", NORMALIZE, &status);
    E.setChannelBox(true);
    E.setKeyable(true);
    E.addField("No Operation", 0);
    E.addField("Conjugate",    CONJUGATE);
    E.addField("Inverse",      INVERSE);
    E.addField("Negate",       NEGATE);
    E.addField("Normalize",    NORMALIZE);

    addAttribute(inputQuatAttr);
    addAttribute(operationAttr);

    outputQuatAttr = T.create("outputQuat", "oq", QuatArrayData::TYPE_ID, MObject::kNullObj, &status);
    T.setStorable(false);

    addAttribute(outputQuatAttr);

    attributeAffects(inputQuatAttr, outputQuatAttr);
    attributeAffects(operationAttr, outputQuatAttr);

    return MStatus::kSuccess;
}


MStatus QuatArrayUnaryOpNode::compute(const MPlug& plug, MDataBlock& data)
{
    MStatus status;
    
    if (plug != outputQuatAttr)
    {
        return MStatus::kInvalidParameter;
    }

    MDataHandle inputHandle = data.inputValue(inputQuatAttr);
    short operation = data.inputValue(operationAttr).asShort();

    std::vector<MQuaternion> values = getUserArray<MQuaternion, QuatArrayData>(inputHandle);

    void (*F)(MQuaternion&) = &QuatArrayUnaryOpNode::quatNop;

    switch (operation)
    {
        case CONJUGATE: F = &QuatArrayUnaryOpNode::quatConjugate; break;
        case INVERSE:   F = &QuatArrayUnaryOpNode::quatInverse;   break;
        case NEGATE:    F = &QuatArrayUnaryOpNode::quatNegate;    break;
        case NORMALIZE: F = &QuatArrayUnaryOpNode::quatNormalize; break;
    }

    for (size_t i = 0; i < values.size(); i++)
    {
        F(values[i]);
    }

    MDataHandle outputHandle = data.outputValue(outputQuatAttr);
    setUserArray<MQuaternion, QuatArrayData>(outputHandle, values);

    return MStatus::kSuccess;  
}

void QuatArrayUnaryOpNode::quatNop(MQuaternion &q)       {}
void QuatArrayUnaryOpNode::quatConjugate(MQuaternion &q) {q.conjugateIt(); }
void QuatArrayUnaryOpNode::quatInverse(MQuaternion &q)   {q.invertIt(); }
void QuatArrayUnaryOpNode::quatNegate(MQuaternion &q)    {q.negateIt(); }
void QuatArrayUnaryOpNode::quatNormalize(MQuaternion &q) {q.normalizeIt(); }