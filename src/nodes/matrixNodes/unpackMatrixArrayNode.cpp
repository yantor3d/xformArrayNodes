/**
    Copyright (c) 2017 Ryan Porter
    You may use, distribute, or modify this code under the terms of the MIT license.
*/

/**
unpackMatrixArray node
This node outputs the rows of 4x4 matrices.

    inputMatrix (im) matrixArray
        Array of matrices to be unpacked.

    outputMatrix (om) matrix[]
        List of unpacked matrices.

    outputRows (or) compound

    outputRow0 (or0) vectorArray
        List of the first row of the unpacked matrices.

    outputRow1 (or1) vectorArray
        List of the second row of the unpacked matrices.

    outputRow2 (or2) vectorArray
        List of the third row of the unpacked matrices.

    outputRow3 (or3) vectorArray
        List of the fourth row of the unpacked matrices.

*/

#include <vector>

#include "../nodeData.h"
#include "unpackMatrixArrayNode.h"

#include <maya/MArrayDataBuilder.h>
#include <maya/MArrayDataHandle.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MFnData.h>
#include <maya/MFnCompoundAttribute.h>
#include <maya/MFnMatrixData.h>
#include <maya/MFnMatrixArrayData.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnVectorArrayData.h>
#include <maya/MMatrix.h>
#include <maya/MMatrixArray.h>
#include <maya/MPlug.h>
#include <maya/MPxNode.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>
#include <maya/MVector.h>
#include <maya/MVectorArray.h>


MObject UnpackMatrixArrayNode::inputMatrixAttr;

MObject UnpackMatrixArrayNode::outputMatrixAttr;
MObject UnpackMatrixArrayNode::outputRow0Attr;
MObject UnpackMatrixArrayNode::outputRow1Attr;
MObject UnpackMatrixArrayNode::outputRow2Attr;
MObject UnpackMatrixArrayNode::outputRow3Attr;


void* UnpackMatrixArrayNode::creator()
{
    return new UnpackMatrixArrayNode();
}


MStatus UnpackMatrixArrayNode::initialize()
{
    MStatus status;

    MFnCompoundAttribute C;
    MFnTypedAttribute T;

    inputMatrixAttr = T.create("inputMatrix", "im", MFnData::kMatrixArray, MObject::kNullObj, &status);

    addAttribute(inputMatrixAttr);

    outputMatrixAttr = T.create("outputMatrix", "om", MFnData::kMatrix, MObject::kNullObj, &status);
    T.setArray(true);
    T.setStorable(false);
    T.setUsesArrayDataBuilder(true);

    outputRow0Attr = T.create("outputRow0", "or0", MFnData::kVectorArray, MObject::kNullObj, &status);
    T.setStorable(false);

    outputRow1Attr = T.create("outputRow1", "or1", MFnData::kVectorArray, MObject::kNullObj, &status);
    T.setStorable(false);

    outputRow2Attr = T.create("outputRow2", "or2", MFnData::kVectorArray, MObject::kNullObj, &status);
    T.setStorable(false);

    outputRow3Attr = T.create("outputRow3", "or3", MFnData::kVectorArray, MObject::kNullObj, &status);
    T.setStorable(false);

    addAttribute(outputMatrixAttr);
    addAttribute(outputMatrixAttr);
    addAttribute(outputRow0Attr);
    addAttribute(outputRow1Attr);
    addAttribute(outputRow2Attr);
    addAttribute(outputRow3Attr);

    attributeAffects(inputMatrixAttr, outputMatrixAttr);
    attributeAffects(inputMatrixAttr, outputRow0Attr);
    attributeAffects(inputMatrixAttr, outputRow1Attr);
    attributeAffects(inputMatrixAttr, outputRow2Attr);
    attributeAffects(inputMatrixAttr, outputRow3Attr);

    return MStatus::kSuccess;
}


MStatus UnpackMatrixArrayNode::compute(const MPlug& plug, MDataBlock& data)
{
    MStatus status;
    
    if (
        plug != outputMatrixAttr &&
        plug.parent() != outputMatrixAttr &&
        plug != outputRow0Attr &&
        plug != outputRow1Attr &&
        plug != outputRow2Attr &&
        plug != outputRow3Attr
    ) {
        return MStatus::kInvalidParameter;
    }

    MDataHandle inputMatrixHandle = data.inputValue(inputMatrixAttr);
    std::vector<MMatrix> inputMatrix = getMayaArray<MMatrix, MFnMatrixArrayData>(inputMatrixHandle);

    unsigned numberOfValues = (unsigned) inputMatrix.size();

    MArrayDataHandle outputArrayHandle = data.outputArrayValue(outputMatrixAttr);

    setArrayElements(
        outputArrayHandle,
        inputMatrix,
        &UnpackMatrixArrayNode::setElement
    );

    std::vector<MVector> row0(numberOfValues);
    std::vector<MVector> row1(numberOfValues);
    std::vector<MVector> row2(numberOfValues);
    std::vector<MVector> row3(numberOfValues);

    for (unsigned i = 0; i < numberOfValues; i++)
    {
        MMatrix &m = inputMatrix[i];

        row0[i] = MVector(m(0, 0), m(0, 1), m(0, 2));
        row1[i] = MVector(m(1, 0), m(1, 1), m(1, 2));
        row2[i] = MVector(m(2, 0), m(2, 1), m(2, 2));
        row3[i] = MVector(m(3, 0), m(3, 1), m(3, 2));
    }

    MDataHandle outputRow0Handle = data.outputValue(outputRow0Attr);
    MDataHandle outputRow1Handle = data.outputValue(outputRow1Attr);
    MDataHandle outputRow2Handle = data.outputValue(outputRow2Attr);
    MDataHandle outputRow3Handle = data.outputValue(outputRow3Attr);

    setMayaArray<MVector, MVectorArray, MFnVectorArrayData>(outputRow0Handle, row0);
    setMayaArray<MVector, MVectorArray, MFnVectorArrayData>(outputRow1Handle, row1);
    setMayaArray<MVector, MVectorArray, MFnVectorArrayData>(outputRow2Handle, row2);
    setMayaArray<MVector, MVectorArray, MFnVectorArrayData>(outputRow3Handle, row3);

    return MStatus::kSuccess;   
}


MStatus UnpackMatrixArrayNode::setElement(MDataHandle &elementHandle, MMatrix value)
{
    MStatus status;
    
    MFnMatrixData fnData;
    MObject data = fnData.create(value, &status);
    CHECK_MSTATUS_AND_RETURN_IT(status);

    status = elementHandle.setMObject(data);
    CHECK_MSTATUS_AND_RETURN_IT(status);

    return MStatus::kSuccess;
}