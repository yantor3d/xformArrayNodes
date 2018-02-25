/**
Copyright (c) 2017 Ryan Porter
*/

/*
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/**
xformArrayNodes

Maya plugin with nodes designed to operate on transform components.
*/

#include "data/angleArrayData.h"
#include "data/eulerArrayData.h"
#include "data/quatArrayData.h"

#include "commands/getArrayAttrCmd.h"

#include "nodes/angleNodes/angleArrayCtorNode.h"
#include "nodes/angleNodes/angleArrayIterNode.h"
#include "nodes/angleNodes/angleToDoubleArrayNode.h"
#include "nodes/angleNodes/doubleToAngleArrayNode.h"

#include "nodes/eulerNodes/packEulerArrayNode.h"
#include "nodes/eulerNodes/unpackEulerArrayNode.h"

#include "nodes/matrixNodes/composeMatrixArrayNode.h"
#include "nodes/matrixNodes/decomposeMatrixArrayNode.h"
#include "nodes/matrixNodes/matrixArrayOpNode.h"
#include "nodes/matrixNodes/packMatrixArrayNode.h"
#include "nodes/matrixNodes/unpackMatrixArrayNode.h"

#include "nodes/quatNodes/eulerToQuatArrayNode.h"
#include "nodes/quatNodes/packQuatArrayNode.h"
#include "nodes/quatNodes/quatArrayBinaryOpNode.h"
#include "nodes/quatNodes/quatArrayUnaryOpNode.h"
#include "nodes/quatNodes/quatToEulerArrayNode.h"
#include "nodes/quatNodes/slerpQuatArrayNode.h"
#include "nodes/quatNodes/unpackQuatArrayNode.h"

#include "nodes/vectorNodes/lerpVectorArrayNode.h"
#include "nodes/vectorNodes/packVectorArrayNode.h"
#include "nodes/vectorNodes/rotateVectorArrayNode.h"
#include "nodes/vectorNodes/unpackVectorArrayNode.h"
#include "nodes/vectorNodes/vectorArrayBinaryOpNode.h"
#include "nodes/vectorNodes/vectorArrayMatrixOpNode.h"
#include "nodes/vectorNodes/vectorArrayScalarOpNode.h"
#include "nodes/vectorNodes/vectorArrayToDoubleOpNode.h"
#include "nodes/vectorNodes/vectorArrayUnaryOpNode.h"

#include "nodes/vectorNodes/pointToVectorArrayNode.h"
#include "nodes/vectorNodes/vectorToPointArrayNode.h"

#include <maya/MFnPlugin.h>
#include <maya/MObject.h>
#include <maya/MStatus.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>


const char* AUTHOR               = "Ryan Porter";
const char* VERSION              = "1.0.0";
const char* REQUIRED_API_VERSION = "Any";

const MString AngleArrayData::TYPE_NAME       = "angleArray";
const MString EulerArrayData::TYPE_NAME       = "eulerArray";
const MString QuatArrayData::TYPE_NAME        = "quatArray";

const MTypeId AngleArrayData::TYPE_ID       = 0x00126b3a;
const MTypeId EulerArrayData::TYPE_ID       = 0x00126b3b;
const MTypeId QuatArrayData::TYPE_ID        = 0x00126b3c;

MString GetArrayAttrCmd::COMMAND_NAME         = "getArrayAttr";

MString AngleArrayCtorNode::NODE_NAME         = "packAngleArray";
MString AngleArrayIterNode::NODE_NAME         = "unpackAngleArray";
MString AngleToDoubleArrayNode::NODE_NAME     = "angleToDoubleArray";
MString DoubleToAngleArrayNode::NODE_NAME     = "doubleToAngleArray";

MString PackEulerArrayNode::NODE_NAME         = "packEulerArray";
MString UnpackEulerArrayNode::NODE_NAME       = "unpackEulerArray";

MString ComposeMatrixArrayNode::NODE_NAME     = "composeMatrixArray";
MString DecomposeMatrixArrayNode::NODE_NAME   = "decomposeMatrixArray";
MString MatrixArrayOpNode::NODE_NAME          = "matrixArrayOp";
MString PackMatrixArrayNode::NODE_NAME        = "packMatrixArray";
MString UnpackMatrixArrayNode::NODE_NAME      = "unpackMatrixArray"; 
 
MString EulerToQuatArrayNode::NODE_NAME       = "eulerToQuatArray"; 
MString PackQuatArrayNode::NODE_NAME          = "packQuatArray"; 
MString QuatArrayBinaryOpNode::NODE_NAME      = "quatArrayBinaryOp";
MString QuatArrayUnaryOpNode::NODE_NAME       = "quatArrayUnaryOp";
MString QuatToEulerArrayNode::NODE_NAME       = "quatToEulerArray"; 
MString SlerpQuatArrayNode::NODE_NAME         = "slerpQuatArray"; 
MString UnpackQuatArrayNode::NODE_NAME        = "unpackQuatArray";

MString LerpVectorArrayNode::NODE_NAME        = "lerpVectorArray";
MString PackVectorArrayNode::NODE_NAME        = "packVectorArray";
MString RotateVectorArrayNode::NODE_NAME      = "rotateVectorArray";
MString UnpackVectorArrayNode::NODE_NAME      = "unpackVectorArray";
MString VectorArrayBinaryOpNode::NODE_NAME    = "vectorArrayBinaryOp";
MString VectorArrayMatrixOpNode::NODE_NAME    = "vectorArrayMatrixOp";
MString VectorArrayScalarOpNode::NODE_NAME    = "vectorArrayScalarOp";
MString VectorArrayToDoubleOpNode::NODE_NAME  = "vectorArrayToDoubleOp";
MString VectorArrayUnaryOpNode::NODE_NAME     = "vectorArrayUnaryOp";

MString PointToVectorArrayNode::NODE_NAME     = "pointToVectorArray";
MString VectorToPointArrayNode::NODE_NAME     = "vectorToPointArray";

MTypeId AngleArrayCtorNode::NODE_ID         = 0x00126b17;
MTypeId AngleArrayIterNode::NODE_ID         = 0x00126b18;
MTypeId AngleToDoubleArrayNode::NODE_ID     = 0x00126b19;
MTypeId DoubleToAngleArrayNode::NODE_ID     = 0x00126b1a;

MTypeId PackEulerArrayNode::NODE_ID         = 0x00126b1b;
MTypeId UnpackEulerArrayNode::NODE_ID       = 0x00126b1c;

MTypeId ComposeMatrixArrayNode::NODE_ID     = 0x00126b1d;
MTypeId DecomposeMatrixArrayNode::NODE_ID   = 0x00126b1e;
MTypeId MatrixArrayOpNode::NODE_ID          = 0x00126b1f;
MTypeId PackMatrixArrayNode::NODE_ID        = 0x00126b20;
MTypeId UnpackMatrixArrayNode::NODE_ID      = 0x00126b21; 

MTypeId EulerToQuatArrayNode::NODE_ID       = 0x00126b22; 
MTypeId PackQuatArrayNode::NODE_ID          = 0x00126b23; 
MTypeId QuatArrayBinaryOpNode::NODE_ID      = 0x00126b24;
MTypeId QuatArrayUnaryOpNode::NODE_ID       = 0x00126b25;
MTypeId QuatToEulerArrayNode::NODE_ID       = 0x00126b26; 
MTypeId SlerpQuatArrayNode::NODE_ID         = 0x00126b27; 
MTypeId UnpackQuatArrayNode::NODE_ID        = 0x00126b28;

MTypeId LerpVectorArrayNode::NODE_ID        = 0x00126b29;
MTypeId PackVectorArrayNode::NODE_ID        = 0x00126b2a;
MTypeId RotateVectorArrayNode::NODE_ID      = 0x00126b2b;
MTypeId UnpackVectorArrayNode::NODE_ID      = 0x00126b2c;
MTypeId VectorArrayBinaryOpNode::NODE_ID    = 0x00126b2d;
MTypeId VectorArrayMatrixOpNode::NODE_ID    = 0x00126b2e;
MTypeId VectorArrayScalarOpNode::NODE_ID    = 0x00126b2f;
MTypeId VectorArrayToDoubleOpNode::NODE_ID  = 0x00126b30;
MTypeId VectorArrayUnaryOpNode::NODE_ID     = 0x00126b31;

MTypeId PointToVectorArrayNode::NODE_ID     = 0x00126b32;
MTypeId VectorToPointArrayNode::NODE_ID     = 0x00126b33;

#define REGISTER_DATA(DATA)              \
    status = fnPlugin.registerData(      \
        DATA::TYPE_NAME,                 \
        DATA::TYPE_ID,                   \
        DATA::creator                    \
    );                                   \
    CHECK_MSTATUS_AND_RETURN_IT(status); \


#define DEREGISTER_DATA(DATA)            \
    status = fnPlugin.deregisterData(    \
        DATA::TYPE_ID                    \
    );                                   \
    CHECK_MSTATUS_AND_RETURN_IT(status); \


#define REGISTER_NODE(NODE)              \
    status = fnPlugin.registerNode(      \
        NODE::NODE_NAME,                 \
        NODE::NODE_ID,                   \
        NODE::creator,                   \
        NODE::initialize                 \
    );                                   \
    CHECK_MSTATUS_AND_RETURN_IT(status); \


#define DEREGISTER_NODE(NODE)            \
    status = fnPlugin.deregisterNode(    \
        NODE::NODE_ID                    \
    );                                   \
    CHECK_MSTATUS_AND_RETURN_IT(status); \


MStatus initializePlugin(MObject obj)
{
    MStatus status;
    MFnPlugin fnPlugin(obj, AUTHOR, VERSION, REQUIRED_API_VERSION);

    REGISTER_DATA(AngleArrayData);
    REGISTER_DATA(EulerArrayData);
    REGISTER_DATA(QuatArrayData);

    status = fnPlugin.registerCommand(
        GetArrayAttrCmd::COMMAND_NAME,
        GetArrayAttrCmd::creator,
        GetArrayAttrCmd::getSyntax
    );    

    CHECK_MSTATUS_AND_RETURN_IT(status);

    REGISTER_NODE(AngleArrayCtorNode);
    REGISTER_NODE(AngleArrayIterNode);
    REGISTER_NODE(AngleToDoubleArrayNode);
    REGISTER_NODE(DoubleToAngleArrayNode);

    REGISTER_NODE(PackEulerArrayNode);
    REGISTER_NODE(UnpackEulerArrayNode);

    REGISTER_NODE(ComposeMatrixArrayNode);
    REGISTER_NODE(DecomposeMatrixArrayNode);
    REGISTER_NODE(MatrixArrayOpNode);
    REGISTER_NODE(PackMatrixArrayNode);
    REGISTER_NODE(UnpackMatrixArrayNode);

    REGISTER_NODE(EulerToQuatArrayNode);
    REGISTER_NODE(PackQuatArrayNode);
    REGISTER_NODE(QuatArrayBinaryOpNode);
    REGISTER_NODE(QuatArrayUnaryOpNode);
    REGISTER_NODE(QuatToEulerArrayNode);
    REGISTER_NODE(SlerpQuatArrayNode);
    REGISTER_NODE(UnpackQuatArrayNode);

    REGISTER_NODE(LerpVectorArrayNode);
    REGISTER_NODE(PackVectorArrayNode);
    REGISTER_NODE(RotateVectorArrayNode);
    REGISTER_NODE(UnpackVectorArrayNode);
    REGISTER_NODE(VectorArrayBinaryOpNode);
    REGISTER_NODE(VectorArrayMatrixOpNode);
    REGISTER_NODE(VectorArrayScalarOpNode);
    REGISTER_NODE(VectorArrayToDoubleOpNode);
    REGISTER_NODE(VectorArrayUnaryOpNode);

    REGISTER_NODE(PointToVectorArrayNode);
    REGISTER_NODE(VectorToPointArrayNode);

    return status;
}


MStatus uninitializePlugin(MObject obj)
{
    MStatus status;
    MFnPlugin fnPlugin(obj, AUTHOR, VERSION, REQUIRED_API_VERSION);
    
    DEREGISTER_DATA(AngleArrayData);
    DEREGISTER_DATA(EulerArrayData);
    DEREGISTER_DATA(QuatArrayData);

    status = fnPlugin.deregisterCommand(
        GetArrayAttrCmd::COMMAND_NAME
    );    
    
    CHECK_MSTATUS_AND_RETURN_IT(status);

    DEREGISTER_NODE(AngleArrayCtorNode);
    DEREGISTER_NODE(AngleArrayIterNode);
    DEREGISTER_NODE(AngleToDoubleArrayNode);
    DEREGISTER_NODE(DoubleToAngleArrayNode);

    DEREGISTER_NODE(PackEulerArrayNode);
    DEREGISTER_NODE(UnpackEulerArrayNode);

    DEREGISTER_NODE(ComposeMatrixArrayNode);
    DEREGISTER_NODE(DecomposeMatrixArrayNode);
    DEREGISTER_NODE(MatrixArrayOpNode);
    DEREGISTER_NODE(PackMatrixArrayNode);
    DEREGISTER_NODE(UnpackMatrixArrayNode);

    DEREGISTER_NODE(EulerToQuatArrayNode);
    DEREGISTER_NODE(PackQuatArrayNode);
    DEREGISTER_NODE(QuatArrayBinaryOpNode);
    DEREGISTER_NODE(QuatArrayUnaryOpNode);
    DEREGISTER_NODE(QuatToEulerArrayNode);
    DEREGISTER_NODE(SlerpQuatArrayNode);
    DEREGISTER_NODE(UnpackQuatArrayNode);

    DEREGISTER_NODE(LerpVectorArrayNode);
    DEREGISTER_NODE(PackVectorArrayNode);
    DEREGISTER_NODE(RotateVectorArrayNode);
    DEREGISTER_NODE(UnpackVectorArrayNode);
    DEREGISTER_NODE(VectorArrayBinaryOpNode);
    DEREGISTER_NODE(VectorArrayMatrixOpNode);
    DEREGISTER_NODE(VectorArrayScalarOpNode);
    DEREGISTER_NODE(VectorArrayToDoubleOpNode);
    DEREGISTER_NODE(VectorArrayUnaryOpNode);

    DEREGISTER_NODE(PointToVectorArrayNode);
    DEREGISTER_NODE(VectorToPointArrayNode);

    return status;
}