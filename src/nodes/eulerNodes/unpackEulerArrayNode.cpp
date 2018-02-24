/**
    Copyright (c) 2017 Ryan Porter
    You may use, distribute, or modify this code under the terms of the MIT license.
*/

/**
unpackEulerArray node
This node outputs arrays of the angles that make up euler rotation values.

    inputRotate (ir) eulerArray
        Array of euler rotation values.

    outputRotate (or) doubleAngle3[]
        Trios of angles that describe an array of euler rotations.

    outputAngleX (oax) angleArray
        Angles that describe the X component of a euler rotation.

    outputAngleY (oay) angleArray
        Angles that describe the Y component of a euler rotation.

    outputAngleZ (oaz) angleArray
        Angles that describe the Z component of a euler rotation.

*/

#include "../../data/angleArrayData.h"
#include "../../data/eulerArrayData.h"
#include "../nodeData.h"
#include "unpackEulerArrayNode.h"

#include <vector>

#include <maya/MAngle.h>
#include <maya/MArrayDataBuilder.h>
#include <maya/MArrayDataHandle.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MEulerRotation.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MPlug.h>
#include <maya/MPxNode.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>


MObject UnpackEulerArrayNode::inputRotateAttr;

MObject UnpackEulerArrayNode::outputRotateAttr;
MObject UnpackEulerArrayNode::outputRotateXAttr;
MObject UnpackEulerArrayNode::outputRotateYAttr;
MObject UnpackEulerArrayNode::outputRotateZAttr;
MObject UnpackEulerArrayNode::outputAngleXAttr;
MObject UnpackEulerArrayNode::outputAngleYAttr;
MObject UnpackEulerArrayNode::outputAngleZAttr;


void* UnpackEulerArrayNode::creator()
{
    return new UnpackEulerArrayNode();
}


MStatus UnpackEulerArrayNode::initialize()
{
    MStatus status;

    MFnNumericAttribute N;
    MFnTypedAttribute T;
    MFnUnitAttribute U;

    inputRotateAttr = T.create("inputRotate", "ir", EulerArrayData::TYPE_ID, MObject::kNullObj, &status);

    addAttribute(inputRotateAttr);

    outputRotateXAttr = U.create("outputRotateX", "orX", MFnUnitAttribute::kAngle, 0, &status);
    outputRotateYAttr = U.create("outputRotateY", "orY", MFnUnitAttribute::kAngle, 0, &status);
    outputRotateZAttr = U.create("outputRotateZ", "orZ", MFnUnitAttribute::kAngle, 0, &status);    
    outputRotateAttr  = N.create("outputRotate",  "or", outputRotateXAttr, outputRotateYAttr, outputRotateZAttr, &status);
    N.setArray(true);
    N.setStorable(false);
    N.setUsesArrayDataBuilder(true);

    outputAngleXAttr = T.create("outputAngleX", "oax", AngleArrayData::TYPE_ID, MObject::kNullObj, &status);
    T.setStorable(false);

    outputAngleYAttr = T.create("outputAngleY", "oay", AngleArrayData::TYPE_ID, MObject::kNullObj, &status);
    T.setStorable(false);

    outputAngleZAttr = T.create("outputAngleZ", "oaz", AngleArrayData::TYPE_ID, MObject::kNullObj, &status);
    T.setStorable(false);

    addAttribute(outputRotateAttr);
    addAttribute(outputAngleXAttr);
    addAttribute(outputAngleYAttr);
    addAttribute(outputAngleZAttr);

    attributeAffects(inputRotateAttr, outputRotateAttr);
    attributeAffects(inputRotateAttr, outputAngleXAttr);
    attributeAffects(inputRotateAttr, outputAngleYAttr);
    attributeAffects(inputRotateAttr, outputAngleZAttr);

    return MStatus::kSuccess;
}


MStatus UnpackEulerArrayNode::compute(const MPlug& plug, MDataBlock& data)
{
    MStatus status;

    if (
        plug != outputRotateAttr &&
        plug != outputAngleXAttr &&
        plug != outputAngleYAttr &&
        plug != outputAngleZAttr
    ) {
        return MStatus::kInvalidParameter;
    }

    MDataHandle inputHandle = data.inputValue(inputRotateAttr);
    std::vector<MEulerRotation> inputRotate = getUserArray<MEulerRotation, EulerArrayData>(inputHandle);
    unsigned numberOfInputs = (unsigned) inputRotate.size();

    MArrayDataHandle outputRotateArrayHandle = data.outputArrayValue(outputRotateAttr);

    setArrayElements<MEulerRotation>(
        outputRotateArrayHandle,
        inputRotate,
        &UnpackEulerArrayNode::setElement
    );

    {
        std::vector<MAngle> outputX(numberOfInputs);
        std::vector<MAngle> outputY(numberOfInputs);
        std::vector<MAngle> outputZ(numberOfInputs);

        for (unsigned i = 0; i < numberOfInputs; i++)
        {
            MEulerRotation &r = inputRotate[i];
            outputX[i] = MAngle(r.x);
            outputY[i] = MAngle(r.y);
            outputZ[i] = MAngle(r.z);
        }

        MDataHandle outputXHandle = data.outputValue(outputAngleXAttr);
        MDataHandle outputYHandle = data.outputValue(outputAngleYAttr);
        MDataHandle outputZHandle = data.outputValue(outputAngleZAttr);

        setUserArray<MAngle, AngleArrayData>(outputXHandle, outputX);
        setUserArray<MAngle, AngleArrayData>(outputYHandle, outputY);
        setUserArray<MAngle, AngleArrayData>(outputZHandle, outputZ);
    }

    return MStatus::kSuccess;   
}

MStatus UnpackEulerArrayNode::setElement(MDataHandle &elementHandle, MEulerRotation value)
{
    MDataHandle outputRotateXHandle = elementHandle.child(outputRotateXAttr);
    MDataHandle outputRotateYHandle = elementHandle.child(outputRotateYAttr);
    MDataHandle outputRotateZHandle = elementHandle.child(outputRotateZAttr);

    outputRotateXHandle.setMAngle(MAngle(value.x));
    outputRotateYHandle.setMAngle(MAngle(value.y));
    outputRotateZHandle.setMAngle(MAngle(value.z));

    return MStatus::kSuccess;
}