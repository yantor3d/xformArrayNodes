/**
    Copyright (c) 2017 Ryan Porter
    You may use, distribute, or modify this code under the terms of the MIT license.
*/

#pragma once

#include <maya/MAngle.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MPlug.h>
#include <maya/MPxNode.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>

class AngleArrayCtorNode : public MPxNode
{
public:
    virtual MStatus         compute(const MPlug& plug, MDataBlock& data);
    static  void*           creator();
    static  MStatus         initialize();

    static MAngle           getElement(MDataHandle &elementHandle);

public:
    static MTypeId          NODE_ID;
    static MString          NODE_NAME;

    static MObject          inputAttr;
    static MObject          fillValueAttr;
    static MObject          sizeAttr;

    static MObject          outputAttr;
};