/**
    Copyright (c) 2017 Ryan Porter
    You may use, distribute, or modify this code under the terms of the MIT license.
*/

/**
vectorArrayToDoubleOp node
This node performs calculates using pairs of vectors and returning a scalar.

    inputVector1 (iv1) vectorArray
        First array of vectors, used for all operations.

    inputVector2 (iv2) vectorArray
        Second array of vectors, used for the Dot Product and Distance Between operations only.

    operation (op) enum
        Specifies the operation that will be performed.

        No Operation     (0) performs no calculation and returns 0.
        Dot Product      (1) calculates the dot products between pairs of vectors.
        Length           (2) calculates the lengths of the first array of vectors.
        Distance Between (3) calculates the distances between the ends of pairs of vectors.

    output (o) doubleArray
        Array of values calculated by this node.

*/

#include "../nodeData.h"
#include "vectorArrayToDoubleOpNode.h"

#include <algorithm>
#include <vector>

#include <maya/MDataBlock.h>
#include <maya/MDoubleArray.h>
#include <maya/MFnData.h>
#include <maya/MFnDoubleArrayData.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnVectorArrayData.h>
#include <maya/MPlug.h>
#include <maya/MPxNode.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>
#include <maya/MVector.h>
#include <maya/MVectorArray.h>


const short DOT_PRODUCT      = 1;
const short LENGTH           = 2;
const short DISTANCE_BETWEEN = 3;


MObject VectorArrayToDoubleOpNode::inputVector1Attr;
MObject VectorArrayToDoubleOpNode::inputVector2Attr;
MObject VectorArrayToDoubleOpNode::operationAttr;

MObject VectorArrayToDoubleOpNode::outputAttr;


void* VectorArrayToDoubleOpNode::creator()
{
    return new VectorArrayToDoubleOpNode();
}


MStatus VectorArrayToDoubleOpNode::initialize()
{
    MStatus status;

    MFnEnumAttribute E;
    MFnTypedAttribute T;

    inputVector1Attr = T.create("inputVector1", "iv1", MFnData::kVectorArray, MObject::kNullObj, &status);
    inputVector2Attr = T.create("inputVector2", "iv2", MFnData::kVectorArray, MObject::kNullObj, &status);

    operationAttr = E.create("operation", "op", 1, &status);
    E.setChannelBox(true);
    E.setKeyable(true);
    E.addField("No Operation", 0);
    E.addField("Dot Product", 1);
    E.addField("Vector Length", 2);
    E.addField("Distance Between", 3);

    addAttribute(inputVector1Attr);
    addAttribute(inputVector2Attr);
    addAttribute(operationAttr);

    outputAttr = T.create("output", "o", MFnData::kDoubleArray, MObject::kNullObj, &status);
    T.setStorable(true);

    addAttribute(outputAttr);

    attributeAffects(inputVector1Attr, outputAttr);
    attributeAffects(inputVector2Attr, outputAttr);
    attributeAffects(operationAttr, outputAttr);

    return MStatus::kSuccess;
}


MStatus VectorArrayToDoubleOpNode::compute(const MPlug& plug, MDataBlock& data)
{
    MStatus status;

    if (plug != outputAttr)
    {
        return MStatus::kUnknownParameter;
    }

    short operation = data.inputValue(operationAttr).asShort();

    MDataHandle input1Handle = data.inputValue(inputVector1Attr);
    MDataHandle input2Handle = data.inputValue(inputVector2Attr);

    std::vector<MVector> input1 = getMayaArray<MVector, MFnVectorArrayData>(input1Handle);
    std::vector<MVector> input2 = getMayaArray<MVector, MFnVectorArrayData>(input2Handle);

    size_t numberOfValues = std::max(input1.size(), input2.size());

    input1.resize(numberOfValues);
    input2.resize(numberOfValues);

    std::vector<double> output(numberOfValues);

    double (*F)(MVector, MVector) = &VectorArrayToDoubleOpNode::nop;

    switch (operation)
    {
        case DISTANCE_BETWEEN: F = &VectorArrayToDoubleOpNode::distanceBetween; break;
        case DOT_PRODUCT:      F = &VectorArrayToDoubleOpNode::dotProduct;      break;
        case LENGTH:           F = &VectorArrayToDoubleOpNode::length;          break;
    }

    for (size_t i = 0; i < numberOfValues; i++)
    {
        output[i] = F(input1[i], input2[i]);
    }

    MDataHandle outputHandle = data.outputValue(outputAttr);
    setMayaArray<double, MDoubleArray, MFnDoubleArrayData>(outputHandle, output);

    return MStatus::kSuccess;   
}


double VectorArrayToDoubleOpNode::nop(MVector v1, MVector v2)             { return 0.0;}
double VectorArrayToDoubleOpNode::distanceBetween(MVector v1, MVector v2) { return MPoint(v1).distanceTo(MPoint(v2)); }
double VectorArrayToDoubleOpNode::dotProduct(MVector v1, MVector v2)      { return v1 * v2; }
double VectorArrayToDoubleOpNode::length(MVector v1, MVector v2)          { return v1.length(); }
