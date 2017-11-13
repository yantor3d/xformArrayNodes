/**
    Copyright (c) 2017 Ryan Porter
    You may use, distribute, or modify this code under the terms of the MIT license.
*/

#pragma once

#include <vector>

#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MPlug.h>
#include <maya/MPxNode.h>
#include <maya/MQuaternion.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>

class PackQuatArrayNode : public MPxNode
{
public:
    virtual MStatus         compute(const MPlug& plug, MDataBlock& data);
    static  void*           creator();
    static  MStatus         initialize();

    static MQuaternion      getElement(MDataHandle &elementHandle);

private:
    MStatus fromArrayElements(MDataBlock& data, size_t size, MQuaternion fillValue, std::vector<MQuaternion> &output);
    MStatus fromComponents(MDataBlock& data, size_t size, MQuaternion fillValue, std::vector<MQuaternion> &output);
    MStatus fromAxisAngle(MDataBlock& data, size_t size, MQuaternion fillValue, std::vector<MQuaternion> &output);
    MStatus fromVectors(MDataBlock& data, size_t size, MQuaternion fillValue, std::vector<MQuaternion> &output);

public:
    static MTypeId          NODE_ID;
    static MString          NODE_NAME;

    static MObject          inputQuatAttr;
    static MObject          inputQuatXAttr;
    static MObject          inputQuatYAttr;
    static MObject          inputQuatZAttr;
    static MObject          inputQuatWAttr;

    static MObject          inputXAttr;
    static MObject          inputYAttr;
    static MObject          inputZAttr;
    static MObject          inputWAttr;

    static MObject          inputVector1Attr;
    static MObject          inputVector2Attr;

    static MObject          inputAxisAttr;
    static MObject          inputAngleAttr;

    static MObject          fillValueAttr;
    static MObject          fillValueXAttr;
    static MObject          fillValueYAttr;
    static MObject          fillValueZAttr;
    static MObject          fillValueWAttr;
    
    static MObject          sizeAttr;
    static MObject          inputMethodAttr;

    static MObject          outputQuatAttr;
};