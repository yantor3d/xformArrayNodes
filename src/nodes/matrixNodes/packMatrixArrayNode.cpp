/**
    Copyright (c) 2017 Ryan Porter
    You may use, distribute, or modify this code under the terms of the MIT license.
*/

/**
packMatrixArray node
This node outputs an array of 4x4 matrices based on the input values.

    inputMatrix (im) matrix[]
        List of 4x4 matrices.

    inputRows (ir) compound

    inputRow0 (ir0) vectorArray
        List of values describing the first row of a matrix.

    inputRow1 (ir1) vectorArray
        List of values describing the second row of a matrix.

    inputRow2 (ir2) vectorArray
        List of values describing the third row of a matrix.

    inputRow3 (ir3) vectorArray
        List of values describing the last row of a matrix.

    fillValue (fv) matrix
        Default value for the output array. 

    size (s) int
        Size of the output array. If the output is larger than the input, 
        missing values will be filled in with the fillValue.

    inputMethod (im) enum
        Specifies the source of the matrix components.

        Elements   (0) uses the values in the "inputMatrix" list.
        Components (1) uses the rows in the "inputRow" arrays.

    outputMatrix (om) matrixArray
        Array of matrices.

*/

#include "packMatrixArrayNode.h"
#include "../nodeData.h"

#include <algorithm>
#include <vector>

#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MFloatMatrix.h>
#include <maya/MFnData.h>
#include <maya/MFnCompoundAttribute.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnMatrixAttribute.h>
#include <maya/MFnMatrixArrayData.h>
#include <maya/MFnMatrixData.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnNumericData.h>
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


MObject PackMatrixArrayNode::inputMatrixAttr;
MObject PackMatrixArrayNode::inputRow0Attr;
MObject PackMatrixArrayNode::inputRow1Attr;
MObject PackMatrixArrayNode::inputRow2Attr;
MObject PackMatrixArrayNode::inputRow3Attr;
MObject PackMatrixArrayNode::fillValueAttr;
MObject PackMatrixArrayNode::inputMethodAttr;
MObject PackMatrixArrayNode::sizeAttr;

MObject PackMatrixArrayNode::outputMatrixAttr;


void* PackMatrixArrayNode::creator()
{
    return new PackMatrixArrayNode();
}


MStatus PackMatrixArrayNode::initialize()
{
    MStatus status;

    MFnEnumAttribute E;
    MFnMatrixAttribute M;
    MFnNumericAttribute N;
    MFnTypedAttribute T;

    inputMatrixAttr = T.create("inputMatrix", "im", MFnData::kMatrix, MObject::kNullObj, &status);
    T.setArray(true);

    inputRow0Attr = T.create("inputRow0", "ir0", MFnData::kVectorArray, MObject::kNullObj, &status);
    inputRow1Attr = T.create("inputRow1", "ir1", MFnData::kVectorArray, MObject::kNullObj, &status);
    inputRow2Attr = T.create("inputRow2", "ir2", MFnData::kVectorArray, MObject::kNullObj, &status);
    inputRow3Attr = T.create("inputRow3", "ir3", MFnData::kVectorArray, MObject::kNullObj, &status);

    fillValueAttr = M.create("fillValue", "fv", MFnMatrixAttribute::kDouble, &status);

    sizeAttr = N.create("size", "s", MFnNumericData::kInt, 8, &status);
    N.setMin(0);

    inputMethodAttr = E.create("inputMethod", "mtd", 0, &status);
    E.setChannelBox(true);
    E.setKeyable(true);
    E.addField("Elements", 0);
    E.addField("Components", 1);

    addAttribute(inputMatrixAttr);
    addAttribute(inputRow0Attr);
    addAttribute(inputRow1Attr);
    addAttribute(inputRow2Attr);
    addAttribute(inputRow3Attr);
    addAttribute(fillValueAttr);
    addAttribute(sizeAttr);
    addAttribute(inputMethodAttr);

    outputMatrixAttr = T.create("outputMatrix", "om", MFnData::kMatrixArray, MObject::kNullObj, &status);
    T.setStorable(false);

    addAttribute(outputMatrixAttr);

    attributeAffects(inputMatrixAttr, outputMatrixAttr);
    attributeAffects(inputRow0Attr, outputMatrixAttr);
    attributeAffects(inputRow1Attr, outputMatrixAttr);
    attributeAffects(inputRow2Attr, outputMatrixAttr);
    attributeAffects(inputRow3Attr, outputMatrixAttr);
    attributeAffects(fillValueAttr, outputMatrixAttr);
    attributeAffects(sizeAttr, outputMatrixAttr);
    attributeAffects(inputMethodAttr, outputMatrixAttr);

    return MStatus::kSuccess;
}


MStatus PackMatrixArrayNode::compute(const MPlug& plug, MDataBlock& data)
{
    MStatus status;

    if (plug != outputMatrixAttr)
    {
        return MStatus::kInvalidParameter;
    }

    MFnMatrixData fnMatrixData;

    MMatrix fillValue = data.inputValue(fillValueAttr).asMatrix();
    short inputMethod = data.inputValue(inputMethodAttr).asShort();
    unsigned size     = (unsigned) data.inputValue(sizeAttr).asInt();

    std::vector<MMatrix> outputMatrix; 

    if (inputMethod == 0)
    {
        MArrayDataHandle inputMatrixArrayHandle = data.inputArrayValue(inputMatrixAttr);

        outputMatrix = getArrayElements(
            inputMatrixArrayHandle,
            &PackMatrixArrayNode::getElement,
            size,
            fillValue
        );
    } else {
        MDataHandle inputRow0Handle = data.inputValue(inputRow0Attr);
        MDataHandle inputRow1Handle = data.inputValue(inputRow1Attr);
        MDataHandle inputRow2Handle = data.inputValue(inputRow2Attr);
        MDataHandle inputRow3Handle = data.inputValue(inputRow3Attr);

        std::vector<MVector> inputRow0 = getMayaArray<MVector, MFnVectorArrayData>(inputRow0Handle);
        std::vector<MVector> inputRow1 = getMayaArray<MVector, MFnVectorArrayData>(inputRow1Handle);
        std::vector<MVector> inputRow2 = getMayaArray<MVector, MFnVectorArrayData>(inputRow2Handle);
        std::vector<MVector> inputRow3 = getMayaArray<MVector, MFnVectorArrayData>(inputRow3Handle);

        unsigned numberOfInputs = 0;

        numberOfInputs = std::max(numberOfInputs, (unsigned) inputRow0.size());
        numberOfInputs = std::max(numberOfInputs, (unsigned) inputRow1.size());
        numberOfInputs = std::max(numberOfInputs, (unsigned) inputRow2.size());
        numberOfInputs = std::max(numberOfInputs, (unsigned) inputRow3.size());

        inputRow0.resize(numberOfInputs, MVector::xAxis);
        inputRow1.resize(numberOfInputs, MVector::yAxis);
        inputRow2.resize(numberOfInputs, MVector::zAxis);
        inputRow3.resize(numberOfInputs, MVector::zero);

        outputMatrix.resize(numberOfInputs);

        double m[4][4] = {
            {1.0, 0.0, 0.0, 0.0},
            {0.0, 1.0, 0.0, 0.0},
            {0.0, 0.0, 1.0, 0.0},
            {1.0, 0.0, 0.0, 1.0}
        };

        unsigned numberOfOutputs = std::min(size, numberOfInputs);

        for (unsigned i = 0; i < numberOfOutputs; i++)
        {
            MVector &row0 = inputRow0[i];
            MVector &row1 = inputRow1[i];
            MVector &row2 = inputRow2[i];
            MVector &row3 = inputRow3[i];

            m[0][0] = row0.x; m[0][1] = row0.y; m[0][2] = row0.z;
            m[1][0] = row1.x; m[1][1] = row1.y; m[1][2] = row1.z;
            m[2][0] = row2.x; m[2][1] = row2.y; m[2][2] = row2.z;
            m[3][0] = row3.x; m[3][1] = row3.y; m[3][2] = row3.z;

            outputMatrix[i] = MMatrix(m);
        }
    }

    MDataHandle outputMatrixHandle = data.outputValue(outputMatrixAttr);
    setMayaArray<MMatrix, MMatrixArray, MFnMatrixArrayData>(outputMatrixHandle, outputMatrix);

    return MStatus::kSuccess;   
}

MMatrix PackMatrixArrayNode::getElement(MDataHandle &elementHandle)
{
    MObject data = elementHandle.data();
    MFnMatrixData fnData(data);

    return fnData.matrix();
}