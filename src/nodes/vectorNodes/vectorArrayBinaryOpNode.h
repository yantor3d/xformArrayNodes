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

class VectorArrayBinaryOpNode : public MPxNode
{
public:
    virtual MStatus         compute(const MPlug& plug, MDataBlock& data);
    static  void*           creator();
    static  MStatus         initialize();

private:
    static inline MVector   nop(MVector v1, MVector v2);
    static inline MVector   add(MVector v1, MVector v2);
    static inline MVector   subtract(MVector v1, MVector v2);
    static inline MVector   cross(MVector v1, MVector v2);

public:
    static MTypeId          NODE_ID;
    static MString          NODE_NAME;

    static MObject          inputVector1Attr;
    static MObject          inputVector2Attr;
    static MObject          operationAttr;

    static MObject          outputVectorAttr;
};