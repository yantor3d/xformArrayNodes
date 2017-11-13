/**
    Copyright (c) 2017 Ryan Porter
    You may use, distribute, or modify this code under the terms of the MIT license.
*/

/**
packEulerArray node
This node outputs an array of euler rotation values.

    inputRotate (ir) doubleAngle3[]
        Trios of angles that describe an array of euler rotations.

    inputAngleX (iax) angleArray
        Angles that describe the X component of a euler rotation.

    inputAngleY (iay) angleArray
        Angles that describe the Y component of a euler rotation.

    inputAngleZ (iaz) angleArray
        Angles that describe the Z component of a euler rotation.

    fillValue (fv) doubleAngle3
        Trio of angles that describe an euler rotation. 

    size (s) int
        Size of the output array. If the output is larger than the input, 
        missing values will be filled in with the fillValue.

    inputMethod (im) enum
        Specifies the source of the angle components.

        Elements   (0) uses the components in the "inputRotate" list.
        Components (1) uses the components in the "inputAngle" arrays.

    outputRotate (or) eulerArray
        An array of euler rotation values.

*/

#include "../../data/angleArrayData.h"
#include "../../data/eulerArrayData.h"
#include "../nodeData.h"
#include "packEulerArrayNode.h"

#include <algorithm>
#include <vector>

#include <maya/MAngle.h>
#include <maya/MArrayDataHandle.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MFnCompoundAttribute.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnNumericData.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MPlug.h>
#include <maya/MPxNode.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>


MObject PackEulerArrayNode::inputRotateAttr;
MObject PackEulerArrayNode::inputRotateXAttr;
MObject PackEulerArrayNode::inputRotateYAttr;
MObject PackEulerArrayNode::inputRotateZAttr;
MObject PackEulerArrayNode::inputAngleXAttr;
MObject PackEulerArrayNode::inputAngleYAttr;
MObject PackEulerArrayNode::inputAngleZAttr;
MObject PackEulerArrayNode::fillValueAttr;
MObject PackEulerArrayNode::fillValueXAttr;
MObject PackEulerArrayNode::fillValueYAttr;
MObject PackEulerArrayNode::fillValueZAttr;
MObject PackEulerArrayNode::inputMethodAttr;
MObject PackEulerArrayNode::sizeAttr;

MObject PackEulerArrayNode::outputRotateAttr;


void* PackEulerArrayNode::creator()
{
    return new PackEulerArrayNode();
}


MStatus PackEulerArrayNode::initialize()
{
    MStatus status;

    MFnCompoundAttribute C;
    MFnEnumAttribute E;
    MFnNumericAttribute N;
    MFnTypedAttribute T;
    MFnUnitAttribute U;

    inputRotateXAttr = U.create("inputRotateX", "irX", MFnUnitAttribute::kAngle, 0, &status);
    inputRotateYAttr = U.create("inputRotateY", "irY", MFnUnitAttribute::kAngle, 0, &status);
    inputRotateZAttr = U.create("inputRotateZ", "irZ", MFnUnitAttribute::kAngle, 0, &status);
    inputRotateAttr = C.create("inputRotate", "ir", &status);
    C.setArray(true);
    C.addChild(inputRotateXAttr);
    C.addChild(inputRotateYAttr);
    C.addChild(inputRotateZAttr);

    inputAngleXAttr = T.create("inputAngleX", "iax", AngleArrayData::TYPE_ID, MObject::kNullObj, &status);
    inputAngleYAttr = T.create("inputAngleY", "iay", AngleArrayData::TYPE_ID, MObject::kNullObj, &status);
    inputAngleZAttr = T.create("inputAngleZ", "iaz", AngleArrayData::TYPE_ID, MObject::kNullObj, &status);

    fillValueXAttr = U.create("fillValueX", "fvx", MFnUnitAttribute::kAngle, 0, &status);
    fillValueYAttr = U.create("fillValueY", "fvy", MFnUnitAttribute::kAngle, 0, &status);
    fillValueZAttr = U.create("fillValueZ", "fvz", MFnUnitAttribute::kAngle, 0, &status);
    fillValueAttr = C.create("fillValue", "fv", &status);
    C.addChild(fillValueXAttr);
    C.addChild(fillValueYAttr);
    C.addChild(fillValueZAttr);
    C.setChannelBox(true);
    C.setKeyable(true);

    sizeAttr = N.create("size", "s", MFnNumericData::kInt, 8, &status);
    N.setChannelBox(true);
    N.setKeyable(true);
    N.setMin(0);

    inputMethodAttr = E.create("inputMethod", "im", 0, &status);
    E.setChannelBox(true);
    E.setKeyable(true);
    E.addField("From Rotate", 0);
    E.addField("From Angles", 1);

    addAttribute(sizeAttr);
    addAttribute(inputRotateAttr);
    addAttribute(inputAngleXAttr);
    addAttribute(inputAngleYAttr);
    addAttribute(inputAngleZAttr);
    addAttribute(fillValueAttr);
    addAttribute(inputMethodAttr);

    outputRotateAttr = T.create("outputRotate", "or", EulerArrayData::TYPE_ID, MObject::kNullObj, &status);
    T.setStorable(false);

    addAttribute(outputRotateAttr);

    attributeAffects(sizeAttr, outputRotateAttr);
    attributeAffects(inputRotateAttr, outputRotateAttr);
    attributeAffects(inputAngleXAttr, outputRotateAttr);
    attributeAffects(inputAngleYAttr, outputRotateAttr);
    attributeAffects(inputAngleZAttr, outputRotateAttr);
    attributeAffects(fillValueAttr, outputRotateAttr);
    attributeAffects(inputMethodAttr, outputRotateAttr);

    return MStatus::kSuccess;
}


MStatus PackEulerArrayNode::compute(const MPlug& plug, MDataBlock& data)
{
    MStatus status;

    if (plug != outputRotateAttr)
    {
        return MStatus::kInvalidParameter;
    }

    short inputMethod = data.inputValue(inputMethodAttr).asShort();
    unsigned size     = (unsigned) data.inputValue(sizeAttr).asInt();
    std::vector<MEulerRotation> outputRotate;

    MDataHandle fillValueHandle = data.inputValue(fillValueAttr);

    MAngle fillValueX = fillValueHandle.child(fillValueXAttr).asAngle();
    MAngle fillValueY = fillValueHandle.child(fillValueYAttr).asAngle();
    MAngle fillValueZ = fillValueHandle.child(fillValueZAttr).asAngle();   
   
    MEulerRotation fillValue(
        fillValueX.value(),
        fillValueY.value(),
        fillValueZ.value()
    );

    if (inputMethod == 0)
    {
        MArrayDataHandle inputRotateArrayHandle = data.inputArrayValue(inputRotateAttr);

        outputRotate = getArrayElements<MEulerRotation>(
            inputRotateArrayHandle,
            &PackEulerArrayNode::getElement,
            size,
            fillValue
        );
    } else if (inputMethod == 1) {
        MDataHandle inputXHandle = data.inputValue(inputAngleXAttr);
        MDataHandle inputYHandle = data.inputValue(inputAngleYAttr);
        MDataHandle inputZHandle = data.inputValue(inputAngleZAttr);
        
        std::vector<MAngle> inputRotateX = getUserArray<MAngle, AngleArrayData>(inputXHandle);
        std::vector<MAngle> inputRotateY = getUserArray<MAngle, AngleArrayData>(inputYHandle);
        std::vector<MAngle> inputRotateZ = getUserArray<MAngle, AngleArrayData>(inputZHandle);

        unsigned numberOfInputs = 0;
        numberOfInputs = std::max(numberOfInputs, (unsigned) inputRotateX.size());
        numberOfInputs = std::max(numberOfInputs, (unsigned) inputRotateY.size());
        numberOfInputs = std::max(numberOfInputs, (unsigned) inputRotateZ.size());

        inputRotateX.resize(numberOfInputs, fillValueX.value());
        inputRotateY.resize(numberOfInputs, fillValueY.value());
        inputRotateZ.resize(numberOfInputs, fillValueZ.value());
        outputRotate.resize(size, fillValue);

        unsigned numberOfOutputs = std::min(size, numberOfInputs);

        for (unsigned i = 0; i < numberOfOutputs; i++)
        {
            outputRotate[i] = MEulerRotation(
                inputRotateX[i].asRadians(),
                inputRotateY[i].asRadians(),
                inputRotateZ[i].asRadians()
            );
        }    
    }       
    
    MDataHandle outputHandle = data.outputValue(outputRotateAttr);
    setUserArray<MEulerRotation, EulerArrayData>(outputHandle, outputRotate);

    return MStatus::kSuccess;   
}

MEulerRotation PackEulerArrayNode::getElement(MDataHandle &elementHandle)
{
    return MEulerRotation(
        elementHandle.child(inputRotateXAttr).asAngle().asRadians(),
        elementHandle.child(inputRotateYAttr).asAngle().asRadians(),
        elementHandle.child(inputRotateZAttr).asAngle().asRadians()
    );
}