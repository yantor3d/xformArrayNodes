/**
    Copyright (c) 2017 Ryan Porter
    You may use, distribute, or modify this code under the terms of the MIT license.
*/

/**
packQuatArray node
This node packs quaternion rotation components into an array.

    inputQuat (iq) compound[]
        List of quaternion rotation components. Used if inputMethod is ARRAY ELEMENTS.

    inputQuatX (iqx) doubleArray
        X component of a quaternion rotation.

    inputQuatY (iqx)
        Y component of a quaternion rotation.

    inputQuatZ (iqz)
        Z component of a quaternion rotation. 

    inputQuatW (iqw)
        W component of a quaternion rotation.

    inputtX (ix) doubleArray
        X components of quaternion rotations. Used if inputMethod is COMPONENTS.

    input (iy) doubleArray
        Y components of quaternion rotations. Used if inputMethod is COMPONENTS.

    inputZ (iz) doubleArray
        Z components of quaternion rotations. Used if inputMethod is COMPONENTS.    

    inputW (iw) doubleArray
        W components of quaternion rotations. Used if inputMethod is COMPONENTS.  

    inputVector1 (iv1) vectorArray
        Vectors to rotate from. Used if inputMethod is VECTORS.

    inputVector2 (iv2) vectorArray
        Vectors to rotate to. Used if inputMethod is VECTORS.

    inputAxis (iax) vectorArray
        Pivot axes of quaternion rotations. Used if inputMethod is AXIS ANGLE.

    inputAngle (ia) angleArray
        Rotations about an axis of quaternion rotations. Used if inputMethod is AXIS ANGLE.

    fillValue (fv) double4
        Default value.

    size (s) int
        Size of the output array. If the output is larger than the input, 
        missing values will be filled in with the fillValue.

    inputMethod (im) enum
        Specifies which values to use to express a quaternion rotation.

        Elements   (0) creates quaternions from quartets of components.
        Components (1) creates quaternions from a quartet of component arrays.
        Axis Angle (2) creates quaternions by rotating about a pivot axis.
        Vectors    (3) creates quaternions by rotating vector1 into vector2 about their mutually perpendicular axis.
        
    outputQuat (oq) quatArray
        Array of quaternion rotations.

*/

#include "packQuatArrayNode.h"
#include "../nodeData.h"
#include "../../data/angleArrayData.h"
#include "../../data/quatArrayData.h"

#include <algorithm>
#include <vector>

#include <maya/MAngle.h>
#include <maya/MArrayDataHandle.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
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
#include <maya/MQuaternion.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>
#include <maya/MVector.h>
#include <maya/MVectorArray.h>


const short ELEMENTS   = 0;
const short COMPONENTS = 1; 
const short AXIS_ANGLE = 2;
const short VECTORS    = 3;


MObject PackQuatArrayNode::inputQuatAttr;
MObject PackQuatArrayNode::inputQuatXAttr;
MObject PackQuatArrayNode::inputQuatYAttr;
MObject PackQuatArrayNode::inputQuatZAttr;
MObject PackQuatArrayNode::inputQuatWAttr;
MObject PackQuatArrayNode::inputXAttr;
MObject PackQuatArrayNode::inputYAttr;
MObject PackQuatArrayNode::inputZAttr;
MObject PackQuatArrayNode::inputWAttr;
MObject PackQuatArrayNode::inputVector1Attr;
MObject PackQuatArrayNode::inputVector2Attr;
MObject PackQuatArrayNode::inputAxisAttr;
MObject PackQuatArrayNode::inputAngleAttr;
MObject PackQuatArrayNode::fillValueAttr;
MObject PackQuatArrayNode::fillValueXAttr;
MObject PackQuatArrayNode::fillValueYAttr;
MObject PackQuatArrayNode::fillValueZAttr;
MObject PackQuatArrayNode::fillValueWAttr;
MObject PackQuatArrayNode::sizeAttr;
MObject PackQuatArrayNode::inputMethodAttr;

MObject PackQuatArrayNode::outputQuatAttr;


void* PackQuatArrayNode::creator()
{
    return new PackQuatArrayNode();
}


MStatus PackQuatArrayNode::initialize()
{
    MStatus status;

    MFnCompoundAttribute C;
    MFnEnumAttribute E;
    MFnNumericAttribute N;
    MFnTypedAttribute T;

    inputQuatXAttr = N.create("inputQuatX", "iqx", MFnNumericData::kDouble, 0, &status);
    inputQuatYAttr = N.create("inputQuatY", "iqy", MFnNumericData::kDouble, 0, &status);
    inputQuatZAttr = N.create("inputQuatZ", "iqz", MFnNumericData::kDouble, 0, &status);
    inputQuatWAttr = N.create("inputQuatW", "iqw", MFnNumericData::kDouble, 0, &status);
    inputQuatAttr  = C.create("inputQuat", "iq", &status);
    C.addChild(inputQuatXAttr);
    C.addChild(inputQuatYAttr);
    C.addChild(inputQuatZAttr);
    C.addChild(inputQuatWAttr);
    C.setArray(true);

    inputXAttr = T.create("inputX", "ix", MFnData::kDoubleArray, MObject::kNullObj, &status);
    inputYAttr = T.create("inputY", "iy", MFnData::kDoubleArray, MObject::kNullObj, &status);
    inputZAttr = T.create("inputZ", "iz", MFnData::kDoubleArray, MObject::kNullObj, &status);
    inputWAttr = T.create("inputW", "iw", MFnData::kDoubleArray, MObject::kNullObj, &status);

    inputVector1Attr = T.create("inputVector1", "iv1", MFnData::kVectorArray, MObject::kNullObj, &status);
    inputVector2Attr = T.create("inputVector2", "iv2", MFnData::kVectorArray, MObject::kNullObj, &status);

    inputAxisAttr  = T.create("inputAxis", "iax", MFnData::kVectorArray, MObject::kNullObj, &status);
    inputAngleAttr = T.create("inputAngle", "ia", AngleArrayData::TYPE_ID, MObject::kNullObj, &status);

    fillValueXAttr = N.create("fillValueX", "fvx", MFnNumericData::kDouble, 0, &status);
    fillValueYAttr = N.create("fillValueY", "fvy", MFnNumericData::kDouble, 0, &status);
    fillValueZAttr = N.create("fillValueZ", "fvZ", MFnNumericData::kDouble, 0, &status);
    fillValueWAttr = N.create("fillValueW", "fvw", MFnNumericData::kDouble, 0, &status);
    fillValueAttr = C.create("fillValue", "fv", &status);
    C.addChild(fillValueXAttr);
    C.addChild(fillValueYAttr);
    C.addChild(fillValueZAttr);
    C.addChild(fillValueWAttr);
    C.setChannelBox(true);
    C.setKeyable(true);

    sizeAttr = N.create("size", "s", MFnNumericData::kInt, 8, &status);
    N.setMin(0);
    
    inputMethodAttr = E.create("inputMethod", "im", 0, &status);
    E.setStorable(true);
    E.addField("Array Elements", ELEMENTS);
    E.addField("Components",     COMPONENTS);
    E.addField("Axis Angle",     AXIS_ANGLE);
    E.addField("Vectors",        VECTORS);

    addAttribute(inputQuatAttr);
    addAttribute(inputXAttr);
    addAttribute(inputYAttr);
    addAttribute(inputZAttr);
    addAttribute(inputWAttr);

    addAttribute(inputVector1Attr);
    addAttribute(inputVector2Attr);

    addAttribute(inputAxisAttr);
    addAttribute(inputAngleAttr);

    addAttribute(fillValueAttr);
    addAttribute(sizeAttr);
    addAttribute(inputMethodAttr);

    outputQuatAttr = T.create("outputQuat", "oq", QuatArrayData::TYPE_ID, MObject::kNullObj, &status);
    T.setStorable(false);

    addAttribute(outputQuatAttr);

    attributeAffects(inputQuatAttr, outputQuatAttr);
    attributeAffects(inputXAttr, outputQuatAttr);
    attributeAffects(inputYAttr, outputQuatAttr);
    attributeAffects(inputZAttr, outputQuatAttr);
    attributeAffects(inputWAttr, outputQuatAttr);

    attributeAffects(inputVector1Attr, outputQuatAttr);
    attributeAffects(inputVector2Attr, outputQuatAttr);

    attributeAffects(inputAxisAttr, outputQuatAttr);
    attributeAffects(inputAngleAttr, outputQuatAttr);

    attributeAffects(fillValueAttr, outputQuatAttr);
    attributeAffects(sizeAttr, outputQuatAttr);
    attributeAffects(inputMethodAttr, outputQuatAttr);

    return MStatus::kSuccess;
}


MStatus PackQuatArrayNode::compute(const MPlug& plug, MDataBlock& data)
{
    MStatus status;

    if (plug != outputQuatAttr)
    {
        return MStatus::kInvalidParameter;
    }

    short inputMethod = data.inputValue(inputMethodAttr).asShort();
    size_t size = (size_t) data.inputValue(sizeAttr).asInt();

    MDataHandle fillValueHandle = data.inputValue(fillValueAttr);

    MQuaternion fillValue(
        fillValueHandle.child(fillValueXAttr).asDouble(),
        fillValueHandle.child(fillValueYAttr).asDouble(),
        fillValueHandle.child(fillValueZAttr).asDouble(),
        fillValueHandle.child(fillValueWAttr).asDouble()
    );

    std::vector<MQuaternion> output;

    switch (inputMethod)
    {
        case ELEMENTS: 
            status = fromArrayElements(data, size, fillValue, output);
            break;
        case COMPONENTS:
            status = fromComponents(data, size, fillValue, output);
            break;
        case AXIS_ANGLE:
            status = fromAxisAngle(data, size, fillValue, output);
            break;
        case VECTORS:
            status = fromVectors(data, size, fillValue, output);
            break;
    }

    MDataHandle outputHandle = data.outputValue(outputQuatAttr);

    setUserArray<MQuaternion, QuatArrayData>(outputHandle, output);

    return MStatus::kSuccess;   
}


MStatus PackQuatArrayNode::fromArrayElements(MDataBlock& data, size_t size, MQuaternion fillValue, std::vector<MQuaternion> &output)
{
    MStatus status;

    MArrayDataHandle inputArrayHandle = data.inputArrayValue(inputQuatAttr);
    output = getArrayElements<MQuaternion>(inputArrayHandle, &PackQuatArrayNode::getElement, (unsigned) size, fillValue);

    return MStatus::kSuccess;
}


MStatus PackQuatArrayNode::fromComponents(MDataBlock& data, size_t size, MQuaternion fillValue, std::vector<MQuaternion> &output)
{
    MStatus status;

    MDataHandle inputXHandle = data.inputValue(inputXAttr);        
    MDataHandle inputYHandle = data.inputValue(inputYAttr);        
    MDataHandle inputZHandle = data.inputValue(inputZAttr);        
    MDataHandle inputWHandle = data.inputValue(inputWAttr);        

    std::vector<double> inputX = getMayaArray<double, MFnDoubleArrayData>(inputXHandle);
    std::vector<double> inputY = getMayaArray<double, MFnDoubleArrayData>(inputYHandle);
    std::vector<double> inputZ = getMayaArray<double, MFnDoubleArrayData>(inputZHandle);
    std::vector<double> inputW = getMayaArray<double, MFnDoubleArrayData>(inputWHandle);

    size_t numberOfInputs = 0;
    numberOfInputs = std::max(numberOfInputs, inputX.size());
    numberOfInputs = std::max(numberOfInputs, inputY.size());
    numberOfInputs = std::max(numberOfInputs, inputZ.size());
    numberOfInputs = std::max(numberOfInputs, inputW.size());

    inputX.resize(numberOfInputs, fillValue.x);
    inputY.resize(numberOfInputs, fillValue.y);
    inputZ.resize(numberOfInputs, fillValue.z);
    inputW.resize(numberOfInputs, fillValue.w);
    output.resize(size, fillValue);

    size_t numberOfOutputs = std::min(size, numberOfInputs);

    for (size_t i = 0; i < numberOfOutputs; i++)
    {
        output[i] = MQuaternion(inputX[i], inputY[i], inputZ[i], inputW[i]);
    }   

    return MStatus::kSuccess;
}


MStatus PackQuatArrayNode::fromAxisAngle(MDataBlock& data, size_t size, MQuaternion fillValue, std::vector<MQuaternion> &output)
{
    MStatus status;

    MDataHandle inputAxisHandle = data.inputValue(inputAxisAttr);
    MDataHandle inputAngleHandle = data.inputValue(inputAngleAttr);

    std::vector<MVector> inputAxis  = getMayaArray<MVector, MFnVectorArrayData>(inputAxisHandle);
    std::vector<MAngle>  inputAngle = getUserArray<MAngle,  AngleArrayData>(inputAngleHandle);

    size_t numberOfInputs = std::max(inputAngle.size(), inputAxis.size());

    inputAxis.resize(numberOfInputs);
    inputAngle.resize(numberOfInputs);
    output.resize(size, fillValue);

    size_t numberOfOutputs = std::min(size, numberOfInputs);

    for (size_t i = 0; i < numberOfOutputs; i++)
    {
        output[i] = MQuaternion(inputAngle[i].asRadians(), inputAxis[i]);
    }   

    return MStatus::kSuccess;
}


MStatus PackQuatArrayNode::fromVectors(MDataBlock& data, size_t size, MQuaternion fillValue, std::vector<MQuaternion> &output)
{
    MStatus status;

    MDataHandle inputVector1Handle = data.inputValue(inputVector1Attr);
    MDataHandle inputVector2Handle = data.inputValue(inputVector2Attr);

    std::vector<MVector> inputVector1 = getMayaArray<MVector, MFnVectorArrayData>(inputVector1Handle);
    std::vector<MVector> inputVector2 = getMayaArray<MVector, MFnVectorArrayData>(inputVector2Handle);

    size_t numberOfInputs = std::max(inputVector1.size(), inputVector2.size());
    size_t numberOfOutputs = std::min(size, numberOfInputs);

    inputVector1.resize(numberOfInputs);
    inputVector2.resize(numberOfInputs);
    output.resize(size, fillValue);

    for (size_t i = 0; i < numberOfOutputs; i++)
    {
        output[i] = MQuaternion(inputVector1[i], inputVector2[i]);
    }  

    return MStatus::kSuccess;
}


MQuaternion PackQuatArrayNode::getElement(MDataHandle &elementHandle)
{
    return MQuaternion(
        elementHandle.child(inputQuatXAttr).asDouble(),
        elementHandle.child(inputQuatYAttr).asDouble(),
        elementHandle.child(inputQuatZAttr).asDouble(),
        elementHandle.child(inputQuatWAttr).asDouble()
    );
}