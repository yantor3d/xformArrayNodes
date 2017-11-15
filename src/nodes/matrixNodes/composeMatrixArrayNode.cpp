/**
    Copyright (c) 2017 Ryan Porter
    You may use, distribute, or modify this code under the terms of the MIT license.
*/

/**
composeMatrixArray node
This node constructs matrices from transform components.

    inputTranslate (it) pointArray
        Translate component of a transform.

    inputRotate (it) eulerArray
        Rotation component of a transform as an euler rotation.

    inputQuat (iq) quatArray 
        Rotation component of a transform as an quaternion rotation.

    inputScale (is) vectorArray
        Scale component of a transform.

    inputShear (ish) vectorArray
        Shear component of a transform.

    inputRotateOrder (iro) enum
        Rotation order of an euler rotation.

    useEulerRotation (uer) bool
        If true, the matrices' will be built using the euler rotation input.
        Otherwise, the matrices' will be built using the quaternion input.

    outputMatrix (om) matrixArray
        The matrices composed from transform components.

*/

#include "composeMatrixArrayNode.h"

#include "../nodeData.h"
#include "../../data/angleArrayData.h"
#include "../../data/eulerArrayData.h"
#include "../../data/quatArrayData.h"

#include <algorithm>
#include <vector>

#include <maya/MDataBlock.h>
#include <maya/MEulerRotation.h>
#include <maya/MFnData.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnMatrixArrayData.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnNumericData.h>
#include <maya/MFnPluginData.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnVectorArrayData.h>
#include <maya/MMatrix.h>
#include <maya/MMatrixArray.h>
#include <maya/MPlug.h>
#include <maya/MPxNode.h>
#include <maya/MQuaternion.h>
#include <maya/MString.h>
#include <maya/MTransformationMatrix.h>
#include <maya/MTypeId.h>
#include <maya/MVector.h>
#include <maya/MVectorArray.h>


MObject ComposeMatrixArrayNode::inputTranslateAttr;
MObject ComposeMatrixArrayNode::inputRotateAttr;
MObject ComposeMatrixArrayNode::inputQuatAttr;
MObject ComposeMatrixArrayNode::inputScaleAttr;
MObject ComposeMatrixArrayNode::inputShearAttr;
MObject ComposeMatrixArrayNode::inputRotateOrderAttr;
MObject ComposeMatrixArrayNode::useEulerRotationAttr;

MObject ComposeMatrixArrayNode::outputMatrixAttr;


void* ComposeMatrixArrayNode::creator()
{
    return new ComposeMatrixArrayNode();
}


MStatus ComposeMatrixArrayNode::initialize()
{
    MStatus status;

    MFnEnumAttribute E;
    MFnNumericAttribute N;
    MFnTypedAttribute T;

    inputTranslateAttr   = T.create("inputTranslate",   "it",  MFnData::kVectorArray,    MObject::kNullObj, &status);
    inputRotateAttr      = T.create("inputRotate",      "ir",  EulerArrayData::TYPE_ID,  MObject::kNullObj, &status);
    inputQuatAttr        = T.create("inputQuat",        "iq",  QuatArrayData::TYPE_ID,   MObject::kNullObj, &status);
    inputScaleAttr       = T.create("inputScale",       "is",  MFnData::kVectorArray,    MObject::kNullObj, &status);
    inputShearAttr       = T.create("inputShear",       "ish", MFnData::kVectorArray,    MObject::kNullObj, &status);

    useEulerRotationAttr = N.create("useEulerRotation", "uer", MFnNumericData::kBoolean, true, &status);
    N.setChannelBox(true);
    N.setKeyable(true);

    inputRotateOrderAttr = E.create("inputRotateOrder", "iro", 0, &status);
    E.setChannelBox(true);
    E.setKeyable(true);
    E.addField("xyz", 0);
    E.addField("yzx", 1);
    E.addField("zxy", 2);
    E.addField("xzy", 3);
    E.addField("yxz", 4);
    E.addField("zyx", 5);

    addAttribute(inputTranslateAttr);
    addAttribute(inputRotateAttr);
    addAttribute(inputRotateOrderAttr);
    addAttribute(useEulerRotationAttr);
    addAttribute(inputQuatAttr);
    addAttribute(inputScaleAttr);
    addAttribute(inputShearAttr);

    outputMatrixAttr = T.create("outputMatrix", "om", MFnData::kMatrixArray, MObject::kNullObj, &status);
    T.setStorable(false);
    
    addAttribute(outputMatrixAttr);

    attributeAffects(inputTranslateAttr, outputMatrixAttr);
    attributeAffects(inputRotateAttr, outputMatrixAttr);
    attributeAffects(inputQuatAttr, outputMatrixAttr);
    attributeAffects(inputScaleAttr, outputMatrixAttr);
    attributeAffects(inputShearAttr, outputMatrixAttr);
    attributeAffects(inputRotateOrderAttr, outputMatrixAttr);
    attributeAffects(useEulerRotationAttr, outputMatrixAttr);

    return MStatus::kSuccess;
}


MStatus ComposeMatrixArrayNode::compute(const MPlug& plug, MDataBlock& data)
{
    MStatus status;

    if (plug != outputMatrixAttr)
    {
        return MStatus::kInvalidParameter;
    }

    MDataHandle inputTranslateHandle = data.inputValue(inputTranslateAttr);
    MDataHandle inputRotateHandle    = data.inputValue(inputRotateAttr);
    MDataHandle inputQuateHandle     = data.inputValue(inputQuatAttr);
    MDataHandle inputScaleHandle     = data.inputValue(inputScaleAttr);
    MDataHandle inputShearHandle     = data.inputValue(inputShearAttr);

    std::vector<MVector>        translate   = getMayaArray<MVector, MFnVectorArrayData>(inputTranslateHandle);
    std::vector<MVector>        scale       = getMayaArray<MVector, MFnVectorArrayData>(inputScaleHandle);
    std::vector<MVector>        shear       = getMayaArray<MVector, MFnVectorArrayData>(inputShearHandle);
    std::vector<MEulerRotation> eulerRotate = getUserArray<MEulerRotation, EulerArrayData>(inputRotateHandle);
    std::vector<MQuaternion>    quatRotate  = getUserArray<MQuaternion, QuatArrayData>(inputRotateHandle);

    bool useEulerRotation = data.inputValue(useEulerRotationAttr).asBool();

    size_t numberOfTranslates = translate.size();
    size_t numberOfEulerRotates = eulerRotate.size();
    size_t numberOfQuatRotates = quatRotate.size();
    size_t numberOfScales = scale.size();
    size_t numberOfShears = shear.size();

    size_t numberOfOutputs = 0;
    numberOfOutputs = std::max(numberOfOutputs, numberOfTranslates);
    numberOfOutputs = std::max(numberOfOutputs, useEulerRotation ? numberOfEulerRotates : numberOfQuatRotates);
    numberOfOutputs = std::max(numberOfOutputs, numberOfScales);
    numberOfOutputs = std::max(numberOfOutputs, numberOfShears);

    double values[3] {0.0, 0.0, 0.0};
    std::vector<MTransformationMatrix> matrix(numberOfOutputs);
    
    for (size_t i = 0; i < numberOfTranslates; i++)
    {
        matrix[i].setTranslation(translate[i], MSpace::Space::kWorld);
    }

    if (useEulerRotation)
    {
        short rotateOrderIdx  = data.inputValue(inputRotateOrderAttr).asShort();
        MTransformationMatrix::RotationOrder rotateOrder = (MTransformationMatrix::RotationOrder) rotateOrderIdx;

        for (size_t i = 0; i < numberOfEulerRotates; i++)
        {
            MEulerRotation &e = eulerRotate[i];
            values[0] = e.x;
            values[1] = e.y;
            values[2] = e.z;

            matrix[i].setRotation(values, rotateOrder);
        }
    } else {
        for (size_t i = 0; i < numberOfQuatRotates; i++)
        {
            MQuaternion &q = quatRotate[i];
            matrix[i].setRotationQuaternion(q.x, q.y, q.z, q.w);
        }
    }

    for (size_t i = 0; i < numberOfScales; i++)
    {
        MVector &s = scale[i];
        values[0] = s.x;
        values[1] = s.y;
        values[2] = s.z;
        matrix[i].setScale(values, MSpace::Space::kWorld);
    }

    for (size_t i = 0; i < numberOfShears; i++)
    {
        MVector &sh = shear[i];
        values[0] = sh.x;
        values[1] = sh.y;
        values[2] = sh.z;
        matrix[i].setShear(values, MSpace::Space::kWorld);
    }

    std::vector<MMatrix> output(numberOfOutputs);

    for (size_t i = 0; i < numberOfOutputs; i++)
    {
        output[i] = matrix[i].asMatrix();
    }

    MDataHandle outputHandle = data.outputValue(outputMatrixAttr);
    setMayaArray<MMatrix, MMatrixArray, MFnMatrixArrayData>(outputHandle, output);

    return MStatus::kSuccess;   
}