/**
    Copyright (c) 2017 Ryan Porter
    You may use, distribute, or modify this code under the terms of the MIT license.
*/

#pragma once

#include <maya/MDataBlock.h>
#include <maya/MPlug.h>
#include <maya/MPxNode.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>

class UnpackEulerArrayNode : public MPxNode
{
public:
    virtual MStatus         compute(const MPlug& plug, MDataBlock& data);
    static  void*           creator();
    static  MStatus         initialize();

    static MStatus          setElement(MDataHandle &elementHandle, MEulerRotation value);

public:
    static MTypeId          NODE_ID;
    static MString          NODE_NAME;

    static MObject          inputRotateAttr;

    static MObject          outputRotateAttr;
    static MObject          outputRotateXAttr;
    static MObject          outputRotateYAttr;
    static MObject          outputRotateZAttr;
    static MObject          outputAngleXAttr;
    static MObject          outputAngleYAttr;
    static MObject          outputAngleZAttr;
};