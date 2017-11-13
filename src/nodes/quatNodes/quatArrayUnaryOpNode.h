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

class QuatArrayUnaryOpNode : public MPxNode
{
public:
    virtual MStatus         compute(const MPlug& plug, MDataBlock& data);
    static  void*           creator();
    static  MStatus         initialize();

private:
    static inline void      quatNop(MQuaternion &q);
    static inline void      quatConjugate(MQuaternion &q);
    static inline void      quatInverse(MQuaternion &q);
    static inline void      quatNegate(MQuaternion &q);
    static inline void      quatNormalize(MQuaternion &q);

public:
    static MTypeId          NODE_ID;
    static MString          NODE_NAME;

    static MObject          inputQuatAttr;
    static MObject          operationAttr;

    static MObject          outputQuatAttr;
};