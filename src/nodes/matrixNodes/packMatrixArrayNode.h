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

class PackMatrixArrayNode : public MPxNode
{
public:
    virtual MStatus         compute(const MPlug& plug, MDataBlock& data);
    static  void*           creator();
    static  MStatus         initialize();

    static MMatrix          getElement(MDataHandle &elementHandle);

public:
    static MTypeId          NODE_ID;
    static MString          NODE_NAME;

    static MObject          inputMatrixAttr;
    static MObject          inputRow0Attr;
    static MObject          inputRow1Attr;
    static MObject          inputRow2Attr;
    static MObject          inputRow3Attr;
    static MObject          fillValueAttr;
    static MObject          sizeAttr;
    static MObject          inputMethodAttr;

    static MObject          outputMatrixAttr;
};