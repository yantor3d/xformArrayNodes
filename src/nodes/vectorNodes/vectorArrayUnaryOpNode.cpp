/**
    Copyright (c) 2017 Ryan Porter
    You may use, distribute, or modify this code under the terms of the MIT license.
*/

/**
vectorArrayUnaryOp node
This node performs unary operations on an array of vectors.

    inputVector (iv) vectorArray
        Array of vectors.

    operation (op) enum
        Specifies the unary operation to perform.

        No Operation (0) returns the input vectors.
        Normalize    (1) attempts to return the normalized input vectors.
        Invert       (2) returns the vectors opposite the input vectors.

    outputVector (ov) vectorArray
        Array of vectors calculated by this node.
*/

#include "../nodeData.h"
#include "vectorArrayUnaryOpNode.h"

#include <vector>

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


const short NORMALIZE = 1;
const short INVERT    = 2;


MObject VectorArrayUnaryOpNode::inputVectorAttr;
MObject VectorArrayUnaryOpNode::operationAttr;

MObject VectorArrayUnaryOpNode::outputVectorAttr;


void* VectorArrayUnaryOpNode::creator()
{
    return new VectorArrayUnaryOpNode();
}


MStatus VectorArrayUnaryOpNode::initialize()
{
    MStatus status;

    MFnEnumAttribute E;
    MFnTypedAttribute T;

    inputVectorAttr = T.create("inputVector", "iv", MFnData::kVectorArray, MObject::kNullObj, &status);

    operationAttr = E.create("operation", "op", 1, &status);
    E.setChannelBox(true);
    E.setKeyable(true);
    E.addField("No Operation", 0);
    E.addField("Normalize",    NORMALIZE);
    E.addField("Invert",       INVERT);

    addAttribute(inputVectorAttr);
    addAttribute(operationAttr);

    outputVectorAttr = T.create("outputVector", "ov", MFnData::kVectorArray, MObject::kNullObj, &status);
    T.setStorable(false);

    addAttribute(outputVectorAttr);

    attributeAffects(inputVectorAttr, outputVectorAttr);
    attributeAffects(operationAttr, outputVectorAttr);

    return MStatus::kSuccess;
}


MStatus VectorArrayUnaryOpNode::compute(const MPlug& plug, MDataBlock& data)
{
    MStatus status;

    if (plug != outputVectorAttr)
    {
        return MStatus::kInvalidParameter;
    }

    short operation = data.inputValue(operationAttr).asShort();

    MDataHandle inputHandle = data.inputValue(inputVectorAttr);

    std::vector<MVector> values = getMayaArray<MVector, MFnVectorArrayData>(inputHandle);

    if (operation == NORMALIZE)
    {
        for (size_t i = 0; i < values.size(); i++)
        {
            values[i].normalize();
        }
    } else if (operation == INVERT) {
        for (size_t i = 0; i < values.size(); i++)
        {
            values[i] *= -1.0;
        }
    }

    MDataHandle outputHandle = data.outputValue(outputVectorAttr);
    setMayaArray<MVector, MVectorArray, MFnVectorArrayData>(outputHandle, values);

    return MStatus::kSuccess;   
}