/**
    Copyright (c) 2017 Ryan Porter
    You may use, distribute, or modify this code under the terms of the MIT license.
*/

#pragma once

#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MPlug.h>
#include <maya/MPxNode.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>
#include <maya/MVector.h>

class UnpackVectorArrayNode : public MPxNode
{
public:
    virtual MStatus         compute(const MPlug& plug, MDataBlock& data);
    static  void*           creator();
    static  MStatus         initialize();

    static MStatus          setElement(MDataHandle &elementHandle, MVector v);
public:
    static MTypeId          NODE_ID;
    static MString          NODE_NAME;

    static MObject          inputVectorAttr;

    static MObject          outputVectorAttr;
    static MObject          outputVectorXAttr;
    static MObject          outputVectorYAttr;
    static MObject          outputVectorZAttr;
    static MObject          outputXAttr;
    static MObject          outputYAttr;
    static MObject          outputZAttr;
};