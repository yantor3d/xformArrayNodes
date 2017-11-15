#pragma once 

#include <functional>
#include <vector>

#include "../data/angleArrayData.h"
#include "../data/eulerArrayData.h"
#include "../data/quatArrayData.h"

#include <maya/MAngle.h>
#include <maya/MArrayDataBuilder.h>
#include <maya/MArrayDataHandle.h>
#include <maya/MDataHandle.h>
#include <maya/MDataBlock.h>
#include <maya/MDoubleArray.h>
#include <maya/MEulerRotation.h>
#include <maya/MFnCompoundAttribute.h>
#include <maya/MFnDoubleArrayData.h>
#include <maya/MFnMatrixArrayData.h>
#include <maya/MFnPluginData.h>
#include <maya/MFnPointArrayData.h>
#include <maya/MFnVectorArrayData.h>
#include <maya/MFnData.h>
#include <maya/MGlobal.h>
#include <maya/MMatrix.h>
#include <maya/MMatrixArray.h>
#include <maya/MObject.h>
#include <maya/MPoint.h>
#include <maya/MPointArray.h>
#include <maya/MQuaternion.h>
#include <maya/MVector.h>
#include <maya/MVectorArray.h>

template<class T, class FN>
std::vector<T> getMayaArray(MDataHandle &arrayHandle)
{
    std::vector<T> result;

    MObject dataObj = arrayHandle.data();

    if (!dataObj.isNull())
    {
        FN arrayData(dataObj);
        result.resize(arrayData.length());

        for (unsigned i = 0; i < arrayData.length(); i++)
        {
            result[i] = T(arrayData[i]);
        }
    }

    return result;
}

template std::vector<double>  getMayaArray<double,  MFnDoubleArrayData> (MDataHandle &arrayHandle);
template std::vector<MMatrix> getMayaArray<MMatrix, MFnMatrixArrayData> (MDataHandle &arrayHandle);
template std::vector<MPoint>  getMayaArray<MPoint,  MFnPointArrayData>  (MDataHandle &arrayHandle);
template std::vector<MVector> getMayaArray<MVector, MFnVectorArrayData> (MDataHandle &arrayHandle);

template<class T, class MA, class FN>
MStatus setMayaArray(MDataHandle &arrayHandle, std::vector<T> &values)
{
    MStatus status;
    
    unsigned numberOfValues = (unsigned) values.size();
    MA output(numberOfValues);

    for (unsigned i = 0; i < numberOfValues; i++)
    {
        output[i] = values[i];
    }

    FN fnData;
    MObject dataObj = fnData.create(output, &status);
    CHECK_MSTATUS_AND_RETURN_IT(status);

    status = arrayHandle.setMObject(dataObj);
    CHECK_MSTATUS_AND_RETURN_IT(status);

    arrayHandle.setClean();
    
    return MStatus::kSuccess;
}

template MStatus setMayaArray<double,  MDoubleArray, MFnDoubleArrayData>(MDataHandle &arrayHandle, std::vector<double> &values);
template MStatus setMayaArray<MMatrix, MMatrixArray, MFnMatrixArrayData>(MDataHandle &arrayHandle, std::vector<MMatrix> &values);
template MStatus setMayaArray<MPoint,  MPointArray,  MFnPointArrayData>(MDataHandle &arrayHandle, std::vector<MPoint> &values);
template MStatus setMayaArray<MVector, MVectorArray, MFnVectorArrayData>(MDataHandle &arrayHandle, std::vector<MVector> &values);

template<class T, class DATA>
std::vector<T> getUserArray(MDataHandle& arrayHandle)
{
    std::vector<T> result;

    MObject dataObj = arrayHandle.data();

    if (!dataObj.isNull())
    {
        MFnPluginData fnData(dataObj);
        DATA* userData = (DATA*) fnData.data();
        result = userData->getArray();
    }

    return result;
}

template std::vector<MAngle>         getUserArray<MAngle, AngleArrayData>(MDataHandle& arrayHandle);
template std::vector<MEulerRotation> getUserArray<MEulerRotation, EulerArrayData>(MDataHandle& arrayHandle);
template std::vector<MQuaternion>    getUserArray<MQuaternion, QuatArrayData>(MDataHandle& arrayHandle);

template<class T, class DATA>
MStatus setUserArray(MDataHandle& arrayHandle, std::vector<T> &data)
{
    MStatus status;

    MFnPluginData fnData;
    MObject dataObj = fnData.create(DATA::TYPE_ID, &status);

    DATA* userData = (DATA*) fnData.data(&status);
    userData->setArray(data);

    status = arrayHandle.setMPxData(userData);
    CHECK_MSTATUS_AND_RETURN_IT(status);

    arrayHandle.setClean();

    return status;
}

template MStatus setUserArray<MAngle, AngleArrayData>(MDataHandle& arrayHandle, std::vector<MAngle> &data);
template MStatus setUserArray<MEulerRotation, EulerArrayData>(MDataHandle& arrayHandle, std::vector<MEulerRotation> &data);
template MStatus setUserArray<MQuaternion, QuatArrayData> (MDataHandle& arrayHandle, std::vector<MQuaternion> &data);

template<class T>
std::vector<T> getArrayElements(MArrayDataHandle& arrayHandle, T (*getElement)(MDataHandle&), unsigned size, T fillValue)
{
    std::vector<T> result(size, fillValue);

    unsigned numberOfInputs = (unsigned) arrayHandle.elementCount();

    std::vector<T> values(numberOfInputs);
    std::vector<unsigned> indices(numberOfInputs);

    for (unsigned i = 0; i < numberOfInputs; i++)
    {
        MDataHandle elementHandle = arrayHandle.inputValue();

        values[i] = getElement(elementHandle);
        indices[i] = arrayHandle.elementIndex();

        arrayHandle.next();
    }

    for (unsigned i = 0; i < numberOfInputs; i++)
    {
        if (indices[i] < size) 
        {
            result[indices[i]] = values[i];
        }
    }
    
    return result;
}

template std::vector<MAngle>         getArrayElements(MArrayDataHandle& arrayHandle, MAngle (*getElement)(MDataHandle&),         unsigned size,  MAngle fillValue);
template std::vector<MEulerRotation> getArrayElements(MArrayDataHandle& arrayHandle, MEulerRotation (*getElement)(MDataHandle&), unsigned size, MEulerRotation fillValue);
template std::vector<MMatrix>        getArrayElements(MArrayDataHandle& arrayHandle, MMatrix (*getElement)(MDataHandle&),        unsigned size, MMatrix fillValue);
template std::vector<MQuaternion>    getArrayElements(MArrayDataHandle& arrayHandle, MQuaternion (*getElement)(MDataHandle&),    unsigned size, MQuaternion fillValue);
template std::vector<MVector>        getArrayElements(MArrayDataHandle& arrayHandle, MVector (*getElement)(MDataHandle&),        unsigned size, MVector fillValue);

template<class T>
MStatus setArrayElements(MArrayDataHandle& arrayHandle, std::vector<T> &values, MStatus (*setElement)(MDataHandle&, T))
{ 
    MStatus status;
    MArrayDataBuilder outputArray = arrayHandle.builder(&status);
    CHECK_MSTATUS_AND_RETURN_IT(status);

    unsigned numberOfValues = (unsigned) values.size();

    for (unsigned i = 0; i < numberOfValues; i++)
    {
        MDataHandle outputHandle = outputArray.addElement(i, &status);
        CHECK_MSTATUS_AND_RETURN_IT(status);

        status = setElement(outputHandle, values[i]);
        CHECK_MSTATUS_AND_RETURN_IT(status);
    }

    status = arrayHandle.set(outputArray);
    CHECK_MSTATUS_AND_RETURN_IT(status);

    status = arrayHandle.setAllClean();
    CHECK_MSTATUS_AND_RETURN_IT(status);

    return status;
}

template MStatus setArrayElements(MArrayDataHandle& arrayHandle, std::vector<MAngle> &values,         MStatus (*setElement)(MDataHandle&, MAngle));
template MStatus setArrayElements(MArrayDataHandle& arrayHandle, std::vector<MEulerRotation> &values, MStatus (*setElement)(MDataHandle&, MEulerRotation));
template MStatus setArrayElements(MArrayDataHandle& arrayHandle, std::vector<MMatrix> &values ,       MStatus (*setElement)(MDataHandle&, MMatrix));
template MStatus setArrayElements(MArrayDataHandle& arrayHandle, std::vector<MQuaternion> &values,    MStatus (*setElement)(MDataHandle&, MQuaternion));
template MStatus setArrayElements(MArrayDataHandle& arrayHandle, std::vector<MVector> &values,        MStatus (*setElement)(MDataHandle&, MVector));
