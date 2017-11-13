/**
    Copyright (c) 2017 Ryan Porter
    You may use, distribute, or modify this code under the terms of the MIT license.
*/

#pragma once

#include <maya/MDataBlock.h>
#include <maya/MPlug.h>
#include <maya/MPxNode.h>
#include <maya/MQuaternion.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>

class QuatArrayBinaryOpNode : public MPxNode
{
public:
    virtual MStatus         compute(const MPlug& plug, MDataBlock& data);
    static  void*           creator();
    static  MStatus         initialize();

private:
    static inline MQuaternion quatNop(MQuaternion q1,  MQuaternion q2);
    static inline MQuaternion quatAdd(MQuaternion q1,  MQuaternion q2);
    static inline MQuaternion quatSub(MQuaternion q1,  MQuaternion q2);
    static inline MQuaternion quatProd(MQuaternion q1, MQuaternion q2);

public:
    static MTypeId          NODE_ID;
    static MString          NODE_NAME;

    static MObject          inputQuat1Attr;
    static MObject          inputQuat2Attr;
    static MObject          operationAttr;

    static MObject          outputQuatAttr;
};