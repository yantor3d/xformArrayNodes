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

class DecomposeMatrixArrayNode : public MPxNode
{
public:
    virtual MStatus         compute(const MPlug& plug, MDataBlock& data);
    static  void*           creator();
    static  MStatus         initialize();

public:
    static MTypeId          NODE_ID;
    static MString          NODE_NAME;

    static MObject          inputMatrixAttr;
    static MObject          inputRotateOrderAttr;

    static MObject          outputAttr;
    static MObject          outputTranslateAttr;
    static MObject          outputRotateAttr;
    static MObject          outputQuatAttr;
    static MObject          outputScaleAttr;
    static MObject          outputShearAttr;
};