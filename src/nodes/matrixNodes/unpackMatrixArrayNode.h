/**
    Copyright (c) 2017 Ryan Porter
    You may use, distribute, or modify this code under the terms of the MIT license.
*/

#pragma once

#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MMatrix.h>
#include <maya/MPlug.h>
#include <maya/MPxNode.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>

class UnpackMatrixArrayNode : public MPxNode
{
public:
    virtual MStatus         compute(const MPlug& plug, MDataBlock& data);
    static  void*           creator();
    static  MStatus         initialize();

    static MStatus          setElement(MDataHandle &elementHandle, MMatrix value);
    
public:
    static MTypeId          NODE_ID;
    static MString          NODE_NAME;

    static MObject          inputMatrixAttr;

    static MObject          outputMatrixAttr;
    static MObject          outputRow0Attr;
    static MObject          outputRow1Attr;
    static MObject          outputRow2Attr;
    static MObject          outputRow3Attr;
};