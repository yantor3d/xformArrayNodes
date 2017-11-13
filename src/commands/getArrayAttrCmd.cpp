/**
    Copyright (c) 2017 Ryan Porter    
    You may use, distribute, or modify this code under the terms of the MIT license.
*/

/**
getArrayAttr command
This command returns the values of attributes with non-numeric array data.
 */

#include "getArrayAttrCmd.h"

#include "../data/angleArrayData.h"
#include "../data/eulerArrayData.h"
#include "../data/quatArrayData.h"

#include <stdio.h>

#include <maya/MAngle.h>
#include <maya/MArgList.h>
#include <maya/MArgDatabase.h>
#include <maya/MDataHandle.h>
#include <maya/MDGContext.h>
#include <maya/MEulerRotation.h>
#include <maya/MFn.h>
#include <maya/MFnAttribute.h>
#include <maya/MFnDoubleArrayData.h>
#include <maya/MFnFloatArrayData.h>
#include <maya/MFnIntArrayData.h>
#include <maya/MFnMatrixArrayData.h>
#include <maya/MFnPluginData.h>
#include <maya/MFnPointArrayData.h>
#include <maya/MFnVectorArrayData.h>
#include <maya/MFnData.h>
#include <maya/MGlobal.h>
#include <maya/MMatrix.h>
#include <maya/MPlug.h>
#include <maya/MPoint.h>
#include <maya/MPxCommand.h>
#include <maya/MPxData.h>
#include <maya/MQuaternion.h>
#include <maya/MSelectionList.h>
#include <maya/MString.h>
#include <maya/MStringArray.h>
#include <maya/MStatus.h>
#include <maya/MSyntax.h>
#include <maya/MTypeId.h>
#include <maya/MVector.h>

GetArrayAttrCmd::GetArrayAttrCmd()  {}
GetArrayAttrCmd::~GetArrayAttrCmd() {}

void* GetArrayAttrCmd::creator()
{
    return new GetArrayAttrCmd();
}

MSyntax GetArrayAttrCmd::getSyntax()
{
    MSyntax syntax;

    syntax.setObjectType(MSyntax::MObjectFormat::kStringObjects);

    return syntax;
}

MStatus GetArrayAttrCmd::doIt(const MArgList& argList)
{
    MStatus status;

    MArgDatabase argData(this->syntax(), argList, &status);
    CHECK_MSTATUS_AND_RETURN_IT(status);

    MStringArray objectNames;
    status = argData.getObjects(objectNames);
    CHECK_MSTATUS_AND_RETURN_IT(status);

    unsigned numberOfObjects = objectNames.length();

    if (numberOfObjects == 1)
    {
        MString requestedAttributeName = objectNames[0];
        
        MSelectionList selection;
        status = selection.add(requestedAttributeName);

        if (!status)
        {
            MString errorMessage("No object matches name: ^1s;");
            errorMessage.format(errorMessage, requestedAttributeName);
            MGlobal::displayError(errorMessage);
            return status;
        }

        status = selection.getPlug(0, this->requestedAttribute);

        if (!status)
        {
            MString errorMessage("Object is not an attribute: ^1s;");
            errorMessage.format(errorMessage, requestedAttributeName);
            MGlobal::displayError(errorMessage);
            return status;
        }
    } else {
        MGlobal::displayError("This command requires exactly one attribute.");
        return MStatus::kFailure;
    }

    return this->redoIt();
}

MStatus GetArrayAttrCmd::redoIt()
{
    MStatus status;

    MObject attribute = this->requestedAttribute.attribute();

    if (attribute.apiType() != MFn::Type::kTypedAttribute)
    {
        MString errorMessage("This command requires an attribute with array data.");
        MGlobal::displayError(errorMessage);
        return MStatus::kFailure;
    }

    MDataHandle handle = this->requestedAttribute.asMDataHandle(MDGContext::fsNormal, &status);
    CHECK_MSTATUS_AND_RETURN_IT(status);

    MObject data = handle.data();
    CHECK_MSTATUS_AND_RETURN_IT(status);

    MFn::Type type = data.apiType();

    MDoubleArray result; 

    if (type == MFn::Type::kPluginData)
    {
        MTypeId typeId = handle.typeId();

        MFnPluginData fnData(data);
        MPxData *pluginDataPtr = fnData.data();

        if (typeId == AngleArrayData::TYPE_ID)
        {
            MAngle::Unit unit = MAngle::uiUnit();
            AngleArrayData* pluginData = (AngleArrayData*) pluginDataPtr;
            std::vector<MAngle> values = (*pluginData).getArray();
            result.setLength((unsigned) values.size());

            for (unsigned i = 0; i < values.size(); i++)
            {   
                result[i] = values[i].as(unit);
            }
        } else if (typeId == EulerArrayData::TYPE_ID) {
            MAngle::Unit unit = MAngle::uiUnit();
            EulerArrayData* pluginData = (EulerArrayData*) pluginDataPtr;
            std::vector<MEulerRotation> values = (*pluginData).getArray();
            result.setLength((unsigned) values.size() * 3);

            for (unsigned i = 0; i < values.size(); i++)
            {                   
                MEulerRotation &r = values[i];
                result[(i*3)+0] = MAngle(r.x).as(unit);
                result[(i*3)+1] = MAngle(r.y).as(unit);
                result[(i*3)+2] = MAngle(r.z).as(unit);
            }
        } else if (typeId == QuatArrayData::TYPE_ID) {
            QuatArrayData* pluginData = (QuatArrayData*) pluginDataPtr;
            std::vector<MQuaternion> values = (*pluginData).getArray();
            result.setLength((unsigned) values.size() * 4);

            for (unsigned i = 0; i < values.size(); i++)
            {   
                MQuaternion &q = values[i];
                result[(i*4)+0] = q.x;
                result[(i*4)+1] = q.y;
                result[(i*4)+2] = q.z;
                result[(i*4)+3] = q.w;
            }
        } else {
            status = MStatus::kUnknownParameter;
        }
    } else {        
        if (type == MFn::Type::kDoubleArrayData)
        {
            MFnDoubleArrayData fnData(data);
            fnData.copyTo(result);
        } else if (type == MFn::Type::kFloatArrayData) {
            MFnFloatArrayData fnData(data);
            result.setLength(fnData.length());
            
            for (unsigned i = 0; i < fnData.length(); i++)
            {
                result.set(fnData[i], i);
            }
        } else if (type == MFn::Type::kIntArrayData) {
            MFnIntArrayData fnData(data);
            result.setLength(fnData.length());

            for (unsigned i = 0; i < fnData.length(); i++)
            {
                result.set((double) fnData[i], i);
            }
        } else if (type == MFn::Type::kPointArrayData) {
            MFnPointArrayData fnData(data);
            result.setLength(fnData.length() * 3);
            
            for (unsigned i = 0; i < fnData.length(); i++)
            {
                MPoint &p = fnData[i];
                result[(i*3)+0] = p.x;
                result[(i*3)+1] = p.y;
                result[(i*3)+2] = p.z;
            }
        } else if (type == MFn::Type::kVectorArrayData) {
            MFnVectorArrayData fnData(data);
            result.setLength(fnData.length() * 3);

            for (unsigned i = 0; i < fnData.length(); i++)
            {
                MVector &v = fnData[i];
                result[(i*3)+0] = v.x;
                result[(i*3)+1] = v.y;
                result[(i*3)+2] = v.z;
            }
        } else if (type == MFn::Type::kMatrixArrayData) {
            MFnMatrixArrayData fnData(data);
            result.setLength(fnData.length() * 16);

            for (unsigned i = 0; i < fnData.length(); i++)
            {
                MMatrix &m = fnData[i];
                for (unsigned row = 0; row < 4; row++)
                {
                    for (unsigned col = 0; col < 4; col++)
                    {
                        result[(i*16)+(row*4)+col] = (m(row, col));
                    }
                }
            }
        } else {
            status = MStatus::kUnknownParameter;
        }
    }

    if (status)
    {
        if (result.length() == 0)
        {
            warnEmptyArray();
        }

        this->setResult(result);
    } else {
        MString errorMessage("This command requires an attribute with array data.");
        MGlobal::displayError(errorMessage);
    }

    return status;
}

void  GetArrayAttrCmd::warnEmptyArray()
{
    MString msg("The requested attribute '^1s' has no data.");
    msg.format(msg, this->requestedAttribute.name());
    MGlobal::displayWarning(msg);
}
