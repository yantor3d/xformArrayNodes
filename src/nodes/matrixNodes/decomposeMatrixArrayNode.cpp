/**
    Copyright (c) 2017 Ryan Porter
    You may use, distribute, or modify this code under the terms of the MIT license.
*/

/**
decomposeMatrixArray node
This node decomposes matrices into their transform components.

    inputMatrix (im) matrixArray
        List of matrices to be decomposed.

    inputRotateOrder (ori) enum
        Rotation order for the output rotate component.

    outputTranslate (ot) vectorArray
        Translate component of the matrices.

    outputRotate (ot) eulerArray
        Rotation component of the matrices as euler rotations.

    outputQuat (oq) quatArray
        Rotation component of the matrices as quaternion rotations.

    outputScale (os) vectorArray
        Scale component of the matrices.

    outputShear (osh) vectorArray
        Shear component of the matrices.

*/

#include "decomposeMatrixArrayNode.h"

#include "../nodeData.h"
#include "../../data/angleArrayData.h"
#include "../../data/eulerArrayData.h"
#include "../../data/quatArrayData.h"

#include <vector>

#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MFnData.h>
#include <maya/MFnCompoundAttribute.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnMatrixArrayData.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnVectorArrayData.h>
#include <maya/MMatrix.h>
#include <maya/MMatrixArray.h>
#include <maya/MPlug.h>
#include <maya/MPxNode.h>
#include <maya/MString.h>
#include <maya/MTransformationMatrix.h>
#include <maya/MTypeId.h>
#include <maya/MVector.h>
#include <maya/MVectorArray.h>


MObject DecomposeMatrixArrayNode::inputMatrixAttr;
MObject DecomposeMatrixArrayNode::inputRotateOrderAttr;

MObject DecomposeMatrixArrayNode::outputAttr;
MObject DecomposeMatrixArrayNode::outputTranslateAttr;
MObject DecomposeMatrixArrayNode::outputRotateAttr;
MObject DecomposeMatrixArrayNode::outputQuatAttr;
MObject DecomposeMatrixArrayNode::outputScaleAttr;
MObject DecomposeMatrixArrayNode::outputShearAttr;


void* DecomposeMatrixArrayNode::creator()
{
    return new DecomposeMatrixArrayNode();
}


MStatus DecomposeMatrixArrayNode::initialize()
{
    MStatus status;

    MFnCompoundAttribute C;
    MFnEnumAttribute E;
    MFnTypedAttribute T;

    inputMatrixAttr = T.create("inputMatrix", "im", MFnData::kMatrixArray, MObject::kNullObj, &status);

    inputRotateOrderAttr = E.create("inputRotateOrder", "ori", 0, &status);
    E.setChannelBox(true);
    E.setKeyable(true);
    E.addField("xyz", 0);
    E.addField("yzx", 1);
    E.addField("zxy", 2);
    E.addField("xzy", 3);
    E.addField("yxz", 4);
    E.addField("zyx", 5);

    addAttribute(inputMatrixAttr);
    addAttribute(inputRotateOrderAttr);

    outputTranslateAttr = T.create("outputTranslate", "ot", MFnData::kVectorArray, MObject::kNullObj, &status);
    T.setStorable(false);

    outputRotateAttr = T.create("outputRotate", "or", EulerArrayData::TYPE_ID, MObject::kNullObj, &status);
    T.setStorable(false);

    outputQuatAttr = T.create("outputQuat", "oq", QuatArrayData::TYPE_ID, MObject::kNullObj, &status);
    T.setStorable(false);

    outputScaleAttr = T.create("outputScale", "os", MFnData::kVectorArray, MObject::kNullObj, &status);
    T.setStorable(false);

    outputShearAttr = T.create("outputShear", "osh", MFnData::kVectorArray, MObject::kNullObj, &status);
    T.setStorable(false);

    outputAttr = C.create("output", "o", &status);
    C.addChild(outputTranslateAttr);
    C.addChild(outputRotateAttr);
    C.addChild(outputQuatAttr);
    C.addChild(outputScaleAttr);
    C.addChild(outputShearAttr);
    C.setStorable(false);

    addAttribute(outputAttr);

    attributeAffects(inputMatrixAttr, outputAttr);
    attributeAffects(inputRotateOrderAttr, outputAttr);

    return MStatus::kSuccess;
}


MStatus DecomposeMatrixArrayNode::compute(const MPlug& plug, MDataBlock& data)
{
    MStatus status;

    if (plug != outputAttr && plug.parent() != outputAttr)
    {
        return MStatus::kInvalidParameter;
    }

    short rotateOrderIdx  = data.inputValue(inputRotateOrderAttr).asShort();
    MEulerRotation::RotationOrder rotateOrder = (MEulerRotation::RotationOrder) rotateOrderIdx;

    MDataHandle inputMatrixHandle = data.inputValue(inputMatrixAttr);

    std::vector<MMatrix> inputMatrix = getMayaArray<MMatrix, MFnMatrixArrayData>(inputMatrixHandle);

    size_t numberOfOutputs = inputMatrix.size();

    std::vector<MVector>        outputTranslate(numberOfOutputs);
    std::vector<MEulerRotation> outputRotate(numberOfOutputs);
    std::vector<MQuaternion>    outputQuat(numberOfOutputs);
    std::vector<MVector>        outputScale(numberOfOutputs);
    std::vector<MVector>        outputShear(numberOfOutputs);

    double values[3] {0.0, 0.0, 0.0};

    for (size_t i = 0; i < numberOfOutputs; i++)
    {
        MTransformationMatrix matrix(inputMatrix[i]);

        outputTranslate[i] = matrix.getTranslation(MSpace::Space::kWorld);
        outputRotate[i]    = MEulerRotation::decompose(inputMatrix[i], rotateOrder);
        outputQuat[i]      = matrix.rotation();

        matrix.getScale(values, MSpace::Space::kWorld);
        outputScale[i] = MVector(values);

        matrix.getShear(values, MSpace::Space::kWorld);
        outputShear[i] = MVector(values);
    }

    MDataHandle outputTranslateHandle = data.outputValue(outputTranslateAttr);
    MDataHandle outputRotateHandle    = data.outputValue(outputRotateAttr);
    MDataHandle outputQuatHandle      = data.outputValue(outputQuatAttr);
    MDataHandle outputScaleHandle     = data.outputValue(outputScaleAttr);
    MDataHandle outputShearHandle     = data.outputValue(outputShearAttr);

    setMayaArray<MVector, MVectorArray, MFnVectorArrayData>(outputTranslateHandle, outputTranslate);
    setMayaArray<MVector, MVectorArray, MFnVectorArrayData>(outputScaleHandle, outputScale);
    setMayaArray<MVector, MVectorArray, MFnVectorArrayData>(outputShearHandle, outputShear);

    setUserArray<MEulerRotation, EulerArrayData>(outputRotateHandle, outputRotate);
    setUserArray<MQuaternion, QuatArrayData>(outputQuatHandle, outputQuat);

    return MStatus::kSuccess;   
}