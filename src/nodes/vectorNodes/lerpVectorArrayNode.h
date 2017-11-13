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

class LerpVectorArrayNode : public MPxNode
{
public:
    virtual MStatus         compute(const MPlug& plug, MDataBlock& data);
    static  void*           creator();
    static  MStatus         initialize();

private:
    static MVector          lerp(MVector start, MVector end, double tween);
    static MVector          vectorSlerp(MVector start, MVector end, double tween);

public:
    static MTypeId          NODE_ID;
    static MString          NODE_NAME;

    static MObject          inputVector1Attr;
    static MObject          inputVector2Attr;
    static MObject          tweenAttr;
    static MObject          slerpAttr;

    static MObject          outputVectorAttr;
};