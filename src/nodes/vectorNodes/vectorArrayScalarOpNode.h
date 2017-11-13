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

class VectorArrayScalarOpNode : public MPxNode
{
public:
    virtual MStatus         compute(const MPlug& plug, MDataBlock& data);
    static  void*           creator();
    static  MStatus         initialize();

private:
    static inline MVector   nop(MVector v, double s);
    static inline MVector   multiply(MVector v, double s);
    static inline MVector   divide(MVector v, double s);

public:
    static MTypeId          NODE_ID;
    static MString          NODE_NAME;

    static MObject          inputVectorAttr;
    static MObject          operationAttr;
    static MObject          scalarAttr;

    static MObject          outputVectorAttr;
};