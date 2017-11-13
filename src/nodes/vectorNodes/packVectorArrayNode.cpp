/**
    Copyright (c) 2017 Ryan Porter
    You may use, distribute, or modify this code under the terms of the MIT license.
*/

/**
packVectorArray node
This node constructs vectors from trios of values.

    inputVector (iv) compound[]
        Trios of vector component values.

    inputVectorX (ivx) double
        X component of a vector.

    inputVectorY (ivy) double
        Y component of a vector.

    inputVectorZ (ivz) double
        Z component of a vector.

    inputX (ix)
        X components of vectors.

    inputY (iy)
        Y components of vectors.    

    inputZ (iz)
        Z components of vectors.

    fillValue (fv) double3
        Default value.

    fillValueX (fvx) double
        X component of the default value.

    fillValueY (fvy) double
        Y component of the default value.

    fillValueZ (fvz) double
        Z component of the default value.

    size (s) int 
        Size of the output array. If the output is larger than the input, 
        missing values will be filled in with the fillValue.

    inputMethod (im) enum
        Specifies the source of the vector components.

        Elements   (0) creates vectors from trios of components.
        Components (1) creates vectors from a trio of component arrays.

    outputVector (ov) vectorArray
        Array of vectors calculated by this node.

*/

#include "../nodeData.h"
#include "packVectorArrayNode.h"

#include <algorithm>
#include <vector>

#include <maya/MDataBlock.h>
#include <maya/MDoubleArray.h>
#include <maya/MFnCompoundAttribute.h>
#include <maya/MFnData.h>
#include <maya/MFnDoubleArrayData.h>
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


MObject PackVectorArrayNode::inputVectorAttr;
MObject PackVectorArrayNode::inputVectorXAttr;
MObject PackVectorArrayNode::inputVectorYAttr;
MObject PackVectorArrayNode::inputVectorZAttr;
MObject PackVectorArrayNode::inputXAttr;
MObject PackVectorArrayNode::inputYAttr;
MObject PackVectorArrayNode::inputZAttr;
MObject PackVectorArrayNode::fillValueAttr;
MObject PackVectorArrayNode::fillValueXAttr;
MObject PackVectorArrayNode::fillValueYAttr;
MObject PackVectorArrayNode::fillValueZAttr;
MObject PackVectorArrayNode::inputMethodAttr;
MObject PackVectorArrayNode::sizeAttr;

MObject PackVectorArrayNode::outputVectorAttr;


void* PackVectorArrayNode::creator()
{
    return new PackVectorArrayNode();
}


MStatus PackVectorArrayNode::initialize()
{
    MStatus status;

    MFnCompoundAttribute C;
    MFnEnumAttribute E;
    MFnNumericAttribute N;
    MFnTypedAttribute T;

    inputVectorXAttr = N.create("inputVectorX", "ivx", MFnNumericData::kDouble, 0, &status);
    inputVectorYAttr = N.create("inputVectorY", "ivy", MFnNumericData::kDouble, 0, &status);
    inputVectorZAttr = N.create("inputVectorZ", "ivz", MFnNumericData::kDouble, 0, &status);
    inputVectorAttr =  C.create("inputVector",  "iv", &status);
    C.setArray(true);
    C.addChild(inputVectorXAttr);
    C.addChild(inputVectorYAttr);
    C.addChild(inputVectorZAttr);

    inputXAttr = T.create("inputX", "ix", MFnData::kDoubleArray, MObject::kNullObj, &status);
    inputYAttr = T.create("inputY", "iy", MFnData::kDoubleArray, MObject::kNullObj, &status);
    inputZAttr = T.create("inputZ", "iz", MFnData::kDoubleArray, MObject::kNullObj, &status);

    fillValueXAttr = N.create("fillValueX", "fvx", MFnNumericData::kDouble, 0, &status);
    fillValueYAttr = N.create("fillValueY", "fvy", MFnNumericData::kDouble, 0, &status);
    fillValueZAttr = N.create("fillValueZ", "fvz", MFnNumericData::kDouble, 0, &status);
    fillValueAttr  = C.create("fillValue",  "fv", &status);
    C.addChild(fillValueXAttr);
    C.addChild(fillValueYAttr);
    C.addChild(fillValueZAttr);
    C.setChannelBox(true);
    C.setKeyable(true);

    sizeAttr = N.create("size", "s", MFnNumericData::kInt, 8, &status);
    N.setMin(0);
    N.setChannelBox(true);
    N.setKeyable(true);

    inputMethodAttr = E.create("inputMethod", "im", 0, &status);
    E.setChannelBox(true);
    E.setKeyable(true);
    E.addField("Elements", 0);
    E.addField("Components", 1);

    addAttribute(inputVectorAttr);
    addAttribute(inputXAttr);
    addAttribute(inputYAttr);
    addAttribute(inputZAttr);
    addAttribute(fillValueAttr);
    addAttribute(sizeAttr);
    addAttribute(inputMethodAttr);

    outputVectorAttr = T.create("outputVector", "ov", MFnData::kVectorArray, MObject::kNullObj, &status);
    T.setStorable(false);

    addAttribute(outputVectorAttr);

    attributeAffects(inputVectorAttr, outputVectorAttr);
    attributeAffects(inputXAttr, outputVectorAttr);
    attributeAffects(inputYAttr, outputVectorAttr);
    attributeAffects(inputZAttr, outputVectorAttr);
    attributeAffects(fillValueAttr, outputVectorAttr);
    attributeAffects(sizeAttr, outputVectorAttr);
    attributeAffects(inputMethodAttr, outputVectorAttr);

    return MStatus::kSuccess;
}


MStatus PackVectorArrayNode::compute(const MPlug& plug, MDataBlock& data)
{
    MStatus status;

    if (plug != outputVectorAttr)
    {
        return MStatus::kUnknownParameter;
    }

    short inputMethod = data.inputValue(inputMethodAttr).asShort();
    unsigned size     = (unsigned) data.inputValue(sizeAttr).asInt();

    MDataHandle fillValueHandle = data.inputValue(fillValueAttr);
    MVector fillValue(
        fillValueHandle.child(fillValueXAttr).asDouble(),
        fillValueHandle.child(fillValueYAttr).asDouble(),
        fillValueHandle.child(fillValueZAttr).asDouble()
    );

    std::vector<MVector> output;

    if (inputMethod == 0)
    {
        MArrayDataHandle inputArrayHandle = data.inputArrayValue(inputVectorAttr);

        output = getArrayElements(
            inputArrayHandle, 
            &PackVectorArrayNode::getElement,
            size,
            fillValue
        );
    } else {
        MDataHandle inputXHandle = data.inputValue(inputXAttr);
        MDataHandle inputYHandle = data.inputValue(inputYAttr);
        MDataHandle inputZHandle = data.inputValue(inputZAttr);

        std::vector<double> inputX = getMayaArray<double, MFnDoubleArrayData>(inputXHandle);
        std::vector<double> inputY = getMayaArray<double, MFnDoubleArrayData>(inputYHandle);
        std::vector<double> inputZ = getMayaArray<double, MFnDoubleArrayData>(inputZHandle);

        unsigned numberOfInputs = 0;
        numberOfInputs = std::max(numberOfInputs, (unsigned) inputX.size());
        numberOfInputs = std::max(numberOfInputs, (unsigned) inputY.size());
        numberOfInputs = std::max(numberOfInputs, (unsigned) inputZ.size());

        inputX.resize(numberOfInputs, fillValue.x);
        inputY.resize(numberOfInputs, fillValue.y);
        inputZ.resize(numberOfInputs, fillValue.z);
        output.resize(size, fillValue);

        unsigned numberOfOutputs = std::min(size, numberOfInputs);

        for (unsigned i = 0; i < numberOfOutputs; i++)
        {
            output[i] = MVector(
                inputX[i],
                inputY[i],
                inputZ[i]
            );
        }    
    }

    MDataHandle outputHandle = data.outputValue(outputVectorAttr);
    setMayaArray<MVector, MVectorArray, MFnVectorArrayData>(outputHandle, output);

    return MStatus::kSuccess;   
}


MVector PackVectorArrayNode::getElement(MDataHandle &elementHandle)
{
    MVector result(
        elementHandle.child(inputVectorXAttr).asDouble(),
        elementHandle.child(inputVectorYAttr).asDouble(),
        elementHandle.child(inputVectorZAttr).asDouble()
    );

    return result;
}