/**
    Copyright (c) 2017 Ryan Porter
    You may use, distribute, or modify this code under the terms of the MIT license.
*/

/**
lerpVectorArray node
This node interpolates vectors using either linear or spherical interpolation.

    inputVector1 (iv1) vectorArray
        Array of vectors to interpolate from.

    inputVector2 (iv2) vectorArray
        Array of vectors to interpolate to.

    tween (t) double
        Placeholder description for tween

    slerp (slerp) bool
        If true, compute the slerp (spherical linear interpolation) between pairs of vectors.
        Otherwise, calculate the lerp (linear interpolation).

    outputVector (ov) vectorArray
        Array of vectors calculated by this node.

*/

#include "../nodeData.h"
#include "lerpVectorArrayNode.h"

#include <algorithm>
#include <math.h>
#include <vector>

#include <maya/MDataBlock.h>
#include <maya/MFnData.h>
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


MObject LerpVectorArrayNode::inputVector1Attr;
MObject LerpVectorArrayNode::inputVector2Attr;
MObject LerpVectorArrayNode::tweenAttr;
MObject LerpVectorArrayNode::slerpAttr;

MObject LerpVectorArrayNode::outputVectorAttr;


void* LerpVectorArrayNode::creator()
{
    return new LerpVectorArrayNode();
}


MStatus LerpVectorArrayNode::initialize()
{
    MStatus status;

    MFnNumericAttribute N;
    MFnTypedAttribute T;

    inputVector1Attr = T.create("inputVector1", "iv1", MFnData::kVectorArray, MObject::kNullObj, &status);
    inputVector2Attr = T.create("inputVector2", "iv2", MFnData::kVectorArray, MObject::kNullObj, &status);

    tweenAttr = N.create("tween", "t", MFnNumericData::kDouble, 0.5, &status);
    N.setChannelBox(true);
    N.setKeyable(true);
    N.setMin(0.0);
    N.setMax(1.0);

    slerpAttr = N.create("slerp", "slerp", MFnNumericData::kBoolean, true, &status);
    N.setChannelBox(true);
    N.setKeyable(true);

    addAttribute(inputVector1Attr);
    addAttribute(inputVector2Attr);
    addAttribute(tweenAttr);
    addAttribute(slerpAttr);

    outputVectorAttr = T.create("outputVector", "ov", MFnData::kVectorArray, MObject::kNullObj, &status);
    T.setStorable(false);

    addAttribute(outputVectorAttr);

    attributeAffects(inputVector1Attr, outputVectorAttr);
    attributeAffects(inputVector2Attr, outputVectorAttr);
    attributeAffects(tweenAttr, outputVectorAttr);
    attributeAffects(slerpAttr, outputVectorAttr);

    return MStatus::kSuccess;
}


MStatus LerpVectorArrayNode::compute(const MPlug& plug, MDataBlock& data)
{
    MStatus status;

    if (plug != outputVectorAttr)
    {
        return MStatus::kInvalidParameter;
    }

    MDataHandle input1Handle = data.inputValue(inputVector1Attr);
    MDataHandle input2Handle = data.inputValue(inputVector2Attr);
    double tween = data.inputValue(tweenAttr).asDouble();
           tween = std::min(1.0, std::max(0.0, tween));

    bool useSlerp = data.inputValue(slerpAttr).asBool();

    std::vector<MVector> input1 = getMayaArray<MVector, MFnVectorArrayData>(input1Handle);
    std::vector<MVector> input2 = getMayaArray<MVector, MFnVectorArrayData>(input2Handle);

    size_t numberOfValues = std::max(input1.size(), input2.size());

    std::vector<MVector> output(numberOfValues);

    input1.resize(numberOfValues);
    input2.resize(numberOfValues);

    MVector (*INTERP)(MVector, MVector, double) = useSlerp ? &LerpVectorArrayNode::vectorSlerp : &LerpVectorArrayNode::lerp;

    for (size_t i = 0; i < numberOfValues; i++)
    {
        output[i] = INTERP(input1[i], input2[i], tween);
    }

    MDataHandle outputHandle = data.outputValue(outputVectorAttr);

    setMayaArray<MVector, MVectorArray, MFnVectorArrayData>(outputHandle, output);

    return MStatus::kSuccess;   
}


MVector LerpVectorArrayNode::lerp(MVector start, MVector end, double tween)
{
    return start + ((end - start) * tween);
}


MVector LerpVectorArrayNode::vectorSlerp(MVector start, MVector end, double tween)
{
    double dot = std::min(1.0, std::max(0.0, start * end));
    double theta = std::acos(dot) * tween;

    MVector v = (end - start * dot);

    return (start * std::cos(theta)) + (v * std::sin(theta));
}