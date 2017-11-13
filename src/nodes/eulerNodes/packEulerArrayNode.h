/**
    Copyright (c) 2017 Ryan Porter
    You may use, distribute, or modify this code under the terms of the MIT license.
*/

#pragma once

#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MEulerRotation.h>
#include <maya/MPlug.h>
#include <maya/MPxNode.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>

class PackEulerArrayNode : public MPxNode
{
public:
    virtual MStatus         compute(const MPlug& plug, MDataBlock& data);
    static  void*           creator();
    static  MStatus         initialize();

    static MEulerRotation   getElement(MDataHandle &elementHandle);

public:
    static MTypeId          NODE_ID;
    static MString          NODE_NAME;

    static MObject          sizeAttr;
    static MObject          inputRotateAttr;
    static MObject          inputRotateXAttr;
    static MObject          inputRotateYAttr;
    static MObject          inputRotateZAttr;
    static MObject          inputAngleXAttr;
    static MObject          inputAngleYAttr;
    static MObject          inputAngleZAttr;
    static MObject          fillValueAttr;
    static MObject          fillValueXAttr;
    static MObject          fillValueYAttr;
    static MObject          fillValueZAttr;
    static MObject          inputMethodAttr;

    static MObject          outputRotateAttr;
};