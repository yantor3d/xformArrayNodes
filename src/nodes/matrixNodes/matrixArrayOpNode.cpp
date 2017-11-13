/**
    Copyright (c) 2017 Ryan Porter
    You may use, distribute, or modify this code under the terms of the MIT license.
*/

/**
matrixArrayOp node
This node computes the result of a matrix operation.

    inputMatrix1 (im1) matrixArray
        First array of matrices, used for all operations.

    inputMatrix2 (im2) matrixArray
        Second array of matrices, used for the "Multiply" operation.

    operation (op) enum
        Computation to be performed. If the computation is unary, only the 
        values of inputMatrix1 are used.

        No Operation (0) outputs the input matrices.
        Multiply     (1) outputs the product of the matrices.
        Invert       (2) outputs the inverse of the matrices.
        Transpose    (3) outputs the transposed matrices.
        As Translate (4) outputs the matrices as a position matrix.
        As Rotate    (5) outputs the matrices as a rotation matrix.
        As Scale     (6) outputs the matrices as a scale matrix.

    outputMatrix (om) matrixArray
        List of output matrices.

*/

#include "matrixArrayOpNode.h"
#include "../nodeData.h"

#include <algorithm>
#include <vector>

#include <maya/MDataBlock.h>
#include <maya/MFnData.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnMatrixArrayData.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MMatrix.h>
#include <maya/MMatrixArray.h>
#include <maya/MPlug.h>
#include <maya/MPxNode.h>
#include <maya/MString.h>
#include <maya/MTransformationMatrix.h>
#include <maya/MTypeId.h>


MObject MatrixArrayOpNode::inputMatrix1Attr;
MObject MatrixArrayOpNode::inputMatrix2Attr;
MObject MatrixArrayOpNode::operationAttr;

MObject MatrixArrayOpNode::outputMatrixAttr;


const short NO_OPERATION = 0;
const short MULTIPY      = 1;
const short INVERT       = 2;
const short TRANSPOSE    = 3;
const short AS_TRANSLATE = 4;
const short AS_ROTATE    = 5;
const short AS_SCALE     = 6;


void* MatrixArrayOpNode::creator()
{
    return new MatrixArrayOpNode();
}


MStatus MatrixArrayOpNode::initialize()
{
    MStatus status;

    MFnEnumAttribute E;
    MFnTypedAttribute T;

    inputMatrix1Attr = T.create("inputMatrix1", "im1", MFnData::kMatrixArray, MObject::kNullObj, &status);
    inputMatrix2Attr = T.create("inputMatrix2", "im2", MFnData::kMatrixArray, MObject::kNullObj, &status);

    operationAttr = E.create("operation", "op", 1, &status);
    E.setChannelBox(true);
    E.setKeyable(true);
    E.addField("No Operation",        NO_OPERATION);
    E.addField("Multiply",            MULTIPY);
    E.addField("Invert",              INVERT);
    E.addField("Transpose",           TRANSPOSE);
    E.addField("As Translate Matrix", AS_TRANSLATE);
    E.addField("As Rotate Matrix",    AS_ROTATE);
    E.addField("As Scale Matrix",     AS_SCALE);

    addAttribute(inputMatrix1Attr);
    addAttribute(inputMatrix2Attr);
    addAttribute(operationAttr);

    outputMatrixAttr = T.create("outputMatrix", "om", MFnData::kMatrixArray, MObject::kNullObj, &status);
    T.setStorable(false);
    
    addAttribute(outputMatrixAttr);

    attributeAffects(inputMatrix1Attr, outputMatrixAttr);
    attributeAffects(inputMatrix2Attr, outputMatrixAttr);
    attributeAffects(operationAttr, outputMatrixAttr);

    return MStatus::kSuccess;
}


MStatus MatrixArrayOpNode::compute(const MPlug& plug, MDataBlock& data)
{
    MStatus status;

    if (plug != outputMatrixAttr) 
    { 
        return MStatus::kInvalidParameter;
    }

    short operation = data.inputValue(operationAttr).asShort();

    MDataHandle inputMatrix1Handle = data.inputValue(inputMatrix1Attr);
    std::vector<MMatrix> inputMatrix1 = getMayaArray<MMatrix, MFnMatrixArrayData>(inputMatrix1Handle);

    unsigned numberOfInputs = (unsigned) inputMatrix1.size();

    std::vector<MMatrix> outputMatrix(numberOfInputs);

    if (operation == MULTIPY)
    {
        MDataHandle inputMatrix2Handle = data.inputValue(inputMatrix2Attr);
        std::vector<MMatrix> inputMatrix2 = getMayaArray<MMatrix, MFnMatrixArrayData>(inputMatrix2Handle);

        numberOfInputs = std::max(numberOfInputs, (unsigned) inputMatrix2.size());

        inputMatrix1.resize(numberOfInputs);
        inputMatrix2.resize(numberOfInputs);
        outputMatrix.resize(numberOfInputs);

        for (unsigned i = 0; i < numberOfInputs; i++)
        {
            outputMatrix[i] = inputMatrix1[i] * inputMatrix2[i];
        }
    } else if (operation == INVERT) { 
        for (unsigned i = 0; i < numberOfInputs; i++)
        {
            outputMatrix[i] = inputMatrix1[i].inverse();
        }
    } else if (operation == TRANSPOSE) {
        for (unsigned i = 0; i < numberOfInputs; i++)
        {
            outputMatrix[i] = inputMatrix1[i].transpose();
        }
    } else if (operation == AS_TRANSLATE) {
        MTransformationMatrix xformMatrix;
        
        for (unsigned i = 0; i < numberOfInputs; i++)
        {
            MMatrix &m = inputMatrix1[i];
            MVector t(m(3, 0), m(3, 1), m(3, 2));
            xformMatrix.setTranslation(t, MSpace::Space::kWorld);
            outputMatrix[i] = xformMatrix.asMatrix();
        }
    } else if (operation == AS_ROTATE) {
        for (unsigned i = 0; i < numberOfInputs; i++)
        {
            MTransformationMatrix xformMatrix(inputMatrix1[i]);
            MMatrix rotateMatrix = xformMatrix.asRotateMatrix();
            MMatrix scaleMatrix  = xformMatrix.asScaleMatrix();
            outputMatrix[i] = rotateMatrix * scaleMatrix.inverse();
        }
    } else if (operation == AS_SCALE) {
        for (unsigned i = 0; i < numberOfInputs; i++)
        {
            MTransformationMatrix xformMatrix(inputMatrix1[i]);
            outputMatrix[i] = xformMatrix.asScaleMatrix();
        }
    } else {
        std::copy(
            inputMatrix1.begin(),
            inputMatrix1.end(),
            outputMatrix.begin()
        );
    }

    MDataHandle outputMatrixHandle = data.outputValue(outputMatrixAttr);
    setMayaArray<MMatrix, MMatrixArray, MFnMatrixArrayData>(outputMatrixHandle, outputMatrix);

    return MStatus::kSuccess;   
}