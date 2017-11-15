/**
    Copyright (c) 2017 Ryan Porter
    You may use, distribute, or modify this code under the terms of the MIT license.
*/

/**
vectorArrayBinaryOp node
This node performs binary operations of pairs of vectors.

    inputVector1 (iv1) vectorArray
        First array of vectors.

    inputVector2 (iv2) vectorArray
        Second array of vectors. 

    operation (op) enum
        Specifies the binary operation performed.

        No Operation  (0) returns the first array of vectors.
        Add           (1) returns the sums of the first and second vectors.
        Subtract      (2) returns the differences between the first and second vectors.
        Cross Product (3) returns the cross products of the first and second vectors.

    outputVector (ov) vectorArray
        Array of vectors calculated by this node.

*/

#include "../nodeData.h"
#include "vectorArrayBinaryOpNode.h"

#include <algorithm>
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


const short ADD      = 1;
const short SUBTRACT = 2;
const short CROSS    = 3 ;


MObject VectorArrayBinaryOpNode::inputVector1Attr;
MObject VectorArrayBinaryOpNode::inputVector2Attr;
MObject VectorArrayBinaryOpNode::operationAttr;

MObject VectorArrayBinaryOpNode::outputVectorAttr;


void* VectorArrayBinaryOpNode::creator()
{
    return new VectorArrayBinaryOpNode();
}


MStatus VectorArrayBinaryOpNode::initialize()
{
    MStatus status;

    MFnEnumAttribute E;
    MFnTypedAttribute T;

    inputVector1Attr = T.create("inputVector1", "iv1", MFnData::kVectorArray, MObject::kNullObj, &status);
    inputVector2Attr = T.create("inputVector2", "iv2", MFnData::kVectorArray, MObject::kNullObj, &status);

    operationAttr = E.create("operation", "op", 1, &status);
    E.setChannelBox(true);
    E.setKeyable(true);
    E.addField("No Operation",  0);
    E.addField("Add",           ADD);
    E.addField("Subtract",      SUBTRACT);
    E.addField("Cross Product", CROSS);

    addAttribute(inputVector1Attr);
    addAttribute(inputVector2Attr);
    addAttribute(operationAttr);

    outputVectorAttr = T.create("outputVector", "ov", MFnData::kVectorArray, MObject::kNullObj, &status);
    T.setStorable(false);

    addAttribute(outputVectorAttr);

    attributeAffects(inputVector1Attr, outputVectorAttr);
    attributeAffects(inputVector2Attr, outputVectorAttr);
    attributeAffects(operationAttr, outputVectorAttr);

    return MStatus::kSuccess;
}


MStatus VectorArrayBinaryOpNode::compute(const MPlug& plug, MDataBlock& data)
{
    MStatus status;

    if (plug != outputVectorAttr)
    {
        return MStatus::kInvalidParameter;
    }

    short operation = data.inputValue(operationAttr).asShort();

    MDataHandle input1Handle = data.inputValue(inputVector1Attr);
    MDataHandle input2Handle = data.inputValue(inputVector2Attr);

    std::vector<MVector> input1 = getMayaArray<MVector, MFnVectorArrayData>(input1Handle);
    std::vector<MVector> input2 = getMayaArray<MVector, MFnVectorArrayData>(input2Handle);

    size_t numberOfValues = std::max(input1.size(), input2.size());

    input1.resize(numberOfValues);
    input2.resize(numberOfValues);

    std::vector<MVector> output(numberOfValues);

    MVector (*F)(MVector, MVector) = &VectorArrayBinaryOpNode::nop;

    switch (operation)
    {
        case ADD:      F = &VectorArrayBinaryOpNode::add; break;
        case SUBTRACT: F = &VectorArrayBinaryOpNode::subtract; break;
        case CROSS:    F = &VectorArrayBinaryOpNode::cross; break;
    }

    for (size_t i = 0; i < numberOfValues; i++)
    {
        output[i] = F(input1[i], input2[i]);
    }

    MDataHandle outputHandle = data.outputValue(outputVectorAttr);
    setMayaArray<MVector, MVectorArray, MFnVectorArrayData>(outputHandle, output);

    return MStatus::kSuccess;   
}

MVector VectorArrayBinaryOpNode::nop(MVector v1, MVector v2)      { return v1; }
MVector VectorArrayBinaryOpNode::add(MVector v1, MVector v2)      { return v1 + v2; }
MVector VectorArrayBinaryOpNode::subtract(MVector v1, MVector v2) { return v1 - v2; }
MVector VectorArrayBinaryOpNode::cross(MVector v1, MVector v2)    { return v1 ^ v2; }