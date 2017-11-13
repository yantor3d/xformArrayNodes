/**
    Copyright (c) 2017 Ryan Porter
    You may use, distribute, or modify this code under the terms of the MIT license.
*/

#pragma once

#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MPlug.h>
#include <maya/MPxNode.h>
#include <maya/MQuaternion.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>

class UnpackQuatArrayNode : public MPxNode
{
public:
    virtual MStatus         compute(const MPlug& plug, MDataBlock& data);
    static  void*           creator();
    static  MStatus         initialize();

public:
    static MStatus          setElement(MDataHandle &elementHandle, MQuaternion q);
    
public:
    static MTypeId          NODE_ID;
    static MString          NODE_NAME;

    static MObject          inputQuatAttr;

    static MObject          outputQuatAttr;
    static MObject          outputQuatXAttr;
    static MObject          outputQuatYAttr;
    static MObject          outputQuatZAttr;
    static MObject          outputQuatWAttr;
    static MObject          outputXAttr;
    static MObject          outputYAttr;
    static MObject          outputZAttr;
    static MObject          outputWAttr;
    static MObject          outputAxisAttr;
    static MObject          outputAngleAttr;
};