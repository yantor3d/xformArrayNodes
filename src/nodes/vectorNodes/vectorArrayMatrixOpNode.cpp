/**
    Copyright (c) 2017 Ryan Porter
    You may use, distribute, or modify this code under the terms of the MIT license.
*/

/**
vectorArrayMatrixOp node
This node performs binary operations on vector/matrix pairs.

    inputVector (iv) vectorArray
        Array of vectors.

    inputMatrix (im) matrixArray
        Array of matrices.

    operation (op) enum
        Specifies the operation to be performed.

        No Operation          (0) returns the array of vectors.
        Vector Matrix Product (1) returns the result of vector matrix multiplication.
        Point Matrix Product  (2) returns the result of point matrix multiplication.

    outputVector (ov) vectorArray        
        Array of vectors calculated by this node.

*/

#include "../nodeData.h"
#include "vectorArrayMatrixOpNode.h"

#include <algorithm>
#include <vector>

#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MFnData.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnMatrixArrayData.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnVectorArrayData.h>
#include <maya/MMatrix.h>
#include <maya/MMatrixArray.h>
#include <maya/MPlug.h>
#include <maya/MPoint.h>
#include <maya/MPxNode.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>
#include <maya/MVector.h>
#include <maya/MVectorArray.h>

const short VECTOR_MATRIX_PRODUCT = 1;
const short POINT_MATRIX_PRODUCT = 2;

MObject VectorArrayMatrixOpNode::inputVectorAttr;
MObject VectorArrayMatrixOpNode::inputMatrixAttr;
MObject VectorArrayMatrixOpNode::operationAttr;

MObject VectorArrayMatrixOpNode::outputVectorAttr;


void* VectorArrayMatrixOpNode::creator()
{
    return new VectorArrayMatrixOpNode();
}


MStatus VectorArrayMatrixOpNode::initialize()
{
    MStatus status;

    MFnEnumAttribute E;
    MFnTypedAttribute T;

    inputVectorAttr = T.create("inputVector", "iv", MFnData::kVectorArray, MObject::kNullObj, &status);
    inputMatrixAttr = T.create("inputMatrix", "im", MFnData::kMatrixArray, MObject::kNullObj, &status);

    operationAttr = E.create("operation", "op", 1, &status);
    E.setChannelBox(true);
    E.setKeyable(true);
    E.addField("No Operation", 0);
    E.addField("Vector Matrix Product", 1);
    E.addField("Point Matrix Product", 2);

    addAttribute(inputVectorAttr);
    addAttribute(inputMatrixAttr);
    addAttribute(operationAttr);

    outputVectorAttr = T.create("outputVector", "ov", MFnData::kVectorArray, MObject::kNullObj, &status);
    T.setStorable(false);
    
    addAttribute(outputVectorAttr);

    attributeAffects(inputVectorAttr, outputVectorAttr);
    attributeAffects(inputMatrixAttr, outputVectorAttr);
    attributeAffects(operationAttr, outputVectorAttr);

    return MStatus::kSuccess;
}


MStatus VectorArrayMatrixOpNode::compute(const MPlug& plug, MDataBlock& data)
{
    MStatus status;

    if (plug != outputVectorAttr)
    {
        return MStatus::kInvalidParameter;
    }

    short operation = data.inputValue(operationAttr).asShort();

    MDataHandle inputVectorHandle = data.inputValue(inputVectorAttr);
    MDataHandle inputMatrixHandle = data.inputValue(inputMatrixAttr);

    std::vector<MVector> vector_ = getMayaArray<MVector, MFnVectorArrayData>(inputVectorHandle);
    std::vector<MMatrix> matrix = getMayaArray<MMatrix, MFnMatrixArrayData>(inputMatrixHandle);

    size_t numberOfValues = std::max(vector_.size(), matrix.size());

    vector_.resize(numberOfValues);
    matrix.resize(numberOfValues);

    std::vector<MVector> output(numberOfValues);

    MVector (*F)(MVector, MMatrix) = &VectorArrayMatrixOpNode::nop;

    switch (operation)
    {
        case VECTOR_MATRIX_PRODUCT: F = &VectorArrayMatrixOpNode::vectorMatrixProduct; break;
        case POINT_MATRIX_PRODUCT:  F = &VectorArrayMatrixOpNode::pointMatrixProduct;  break;
    }

    for (size_t i = 0; i < numberOfValues; i++)
    {
        output[i] = F(vector_[i], matrix[i]);
    }

    MDataHandle outputHandle = data.outputValue(outputVectorAttr);
    setMayaArray<MVector, MVectorArray, MFnVectorArrayData>(outputHandle, output);

    return MStatus::kSuccess;   
}

MVector VectorArrayMatrixOpNode::nop(MVector v, MMatrix m)                 { return v; }
MVector VectorArrayMatrixOpNode::vectorMatrixProduct(MVector v, MMatrix m) { return v * m; }
MVector VectorArrayMatrixOpNode::pointMatrixProduct(MVector v, MMatrix m)  { return MVector(MPoint(v) * m); }