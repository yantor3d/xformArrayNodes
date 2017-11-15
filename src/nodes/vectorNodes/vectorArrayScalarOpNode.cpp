/**
    Copyright (c) 2017 Ryan Porter
    You may use, distribute, or modify this code under the terms of the MIT license.
*/

/**
vectorArrayScalarOp node
This node performs scalar operations on vectors.

    inputVector (iv) vectorArray 
        Array of vectors.

    scalar (sc) double
        Scalar value.

    operation (op) enum
        Specifies the operation to be performed.

        No Operation  (0) returns the array of vectors.
        Multiply      (1) returns the result of vector scalar multiplication.
        Divide        (2) returns the result of point scalar division.

    outputVector (ov) vectorArray 
        Array of vectors calculated by this node.
*/

#include "../nodeData.h"
#include "vectorArrayScalarOpNode.h"

#include <vector>

#include <maya/MDataBlock.h>
#include <maya/MFnData.h>
#include <maya/MFnEnumAttribute.h>
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


const short MULTIPLY = 1;
const short DIVIDE   = 2;

MObject VectorArrayScalarOpNode::inputVectorAttr;
MObject VectorArrayScalarOpNode::scalarAttr;
MObject VectorArrayScalarOpNode::operationAttr;

MObject VectorArrayScalarOpNode::outputVectorAttr;


void* VectorArrayScalarOpNode::creator()
{
    return new VectorArrayScalarOpNode();
}


MStatus VectorArrayScalarOpNode::initialize()
{
    MStatus status;

    MFnEnumAttribute    E;
    MFnNumericAttribute N;
    MFnTypedAttribute   T;

    inputVectorAttr = T.create("inputVector", "iv", MFnData::kVectorArray, MObject::kNullObj, &status);

    scalarAttr = N.create("scalar", "sc", MFnNumericData::kDouble, 0, &status);
    N.setChannelBox(true);
    N.setKeyable(true);

    operationAttr =   E.create("operation", "op", 1, &status);
    E.setChannelBox(true);
    E.setKeyable(true);
    E.addField("No Operation", 0);
    E.addField("Multiply",     MULTIPLY);
    E.addField("Divide",       DIVIDE);

    addAttribute(inputVectorAttr);
    addAttribute(scalarAttr);
    addAttribute(operationAttr);

    outputVectorAttr = T.create("outputVector", "ov", MFnData::kVectorArray, MObject::kNullObj, &status);
    T.setStorable(false);

    addAttribute(outputVectorAttr);

    attributeAffects(inputVectorAttr, outputVectorAttr);
    attributeAffects(operationAttr,   outputVectorAttr);
    attributeAffects(scalarAttr,      outputVectorAttr);

    return MStatus::kSuccess;
}


MStatus VectorArrayScalarOpNode::compute(const MPlug& plug, MDataBlock& data)
{
    MStatus status;

    if (plug != outputVectorAttr)
    {
        return MStatus::kInvalidParameter;
    }

    short operation = data.inputValue(operationAttr).asShort();

    MDataHandle inputVectorHandle = data.inputValue(inputVectorAttr);

    std::vector<MVector> vector_ = getMayaArray<MVector, MFnVectorArrayData>(inputVectorHandle);
    double               scalar = data.inputValue(scalarAttr).asDouble();

    size_t numberOfValues = vector_.size();

    std::vector<MVector> output(numberOfValues);

    MVector (*F)(MVector, double) = &VectorArrayScalarOpNode::nop;

    switch (operation)
    {
        case MULTIPLY: F = &VectorArrayScalarOpNode::multiply; break;
        case DIVIDE:   F = &VectorArrayScalarOpNode::divide;  break;
    }

    for (size_t i = 0; i < numberOfValues; i++)
    {
        output[i] = F(vector_[i], scalar);
    }

    MDataHandle outputHandle = data.outputValue(outputVectorAttr);
    setMayaArray<MVector, MVectorArray, MFnVectorArrayData>(outputHandle, output);

    return MStatus::kSuccess;   
}


MVector VectorArrayScalarOpNode::nop(MVector v, double s)      { return v; }
MVector VectorArrayScalarOpNode::multiply(MVector v, double s) { return v * s; }
MVector VectorArrayScalarOpNode::divide(MVector v, double s)   { return v / s; }