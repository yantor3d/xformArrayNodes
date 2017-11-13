#pragma once 

#include <functional>
#include <vector>

#include "../data/angleArrayData.h"
#include "../data/eulerArrayData.h"
#include "../data/quatArrayData.h"

#include <maya/MAngle.h>
#include <maya/MArrayDataHandle.h>
#include <maya/MDataHandle.h>
#include <maya/MDataBlock.h>
#include <maya/MDoubleArray.h>
#include <maya/MEulerRotation.h>
#include <maya/MFnCompoundAttribute.h>
#include <maya/MFnDoubleArrayData.h>
#include <maya/MFnMatrixArrayData.h>
#include <maya/MFnPointArrayData.h>
#include <maya/MFnVectorArrayData.h>
#include <maya/MFnData.h>
#include <maya/MMatrix.h>
#include <maya/MMatrixArray.h>
#include <maya/MObject.h>
#include <maya/MPoint.h>
#include <maya/MPointArray.h>
#include <maya/MQuaternion.h>
#include <maya/MVector.h>
#include <maya/MVectorArray.h>

template<class T, class FN>
std::vector<T> getMayaArray(MDataHandle &arrayHandle);

template std::vector<double>  getMayaArray<double,  MFnDoubleArrayData> (MDataHandle &arrayHandle);
template std::vector<MMatrix> getMayaArray<MMatrix, MFnMatrixArrayData> (MDataHandle &arrayHandle);
template std::vector<MPoint>  getMayaArray<MPoint,  MFnPointArrayData>  (MDataHandle &arrayHandle);
template std::vector<MVector> getMayaArray<MVector, MFnVectorArrayData> (MDataHandle &arrayHandle);

template<class T, class MA, class FN>
MStatus setMayaArray(MDataHandle &arrayHandle, std::vector<T> &values);

template MStatus setMayaArray<double,  MDoubleArray, MFnDoubleArrayData>(MDataHandle &arrayHandle, std::vector<double> &values);
template MStatus setMayaArray<MMatrix, MMatrixArray, MFnMatrixArrayData>(MDataHandle &arrayHandle, std::vector<MMatrix> &values);
template MStatus setMayaArray<MPoint,  MPointArray,  MFnPointArrayData>(MDataHandle &arrayHandle, std::vector<MPoint> &values);
template MStatus setMayaArray<MVector, MVectorArray, MFnVectorArrayData>(MDataHandle &arrayHandle, std::vector<MVector> &values);

template<class T, class DATA>
std::vector<T> getUserArray(MDataHandle& arrayHandle);

template std::vector<MAngle>         getUserArray<MAngle, AngleArrayData>(MDataHandle& arrayHandle);
template std::vector<MEulerRotation> getUserArray<MEulerRotation, EulerArrayData>(MDataHandle& arrayHandle);
template std::vector<MQuaternion>    getUserArray<MQuaternion, QuatArrayData>(MDataHandle& arrayHandle);

template<class T, class DATA>
MStatus setUserArray(MDataHandle& arrayHandle, std::vector<T> &data);

template MStatus setUserArray<MAngle, AngleArrayData>(MDataHandle& arrayHandle, std::vector<MAngle> &data);
template MStatus setUserArray<MEulerRotation, EulerArrayData>(MDataHandle& arrayHandle, std::vector<MEulerRotation> &data);
template MStatus setUserArray<MQuaternion, QuatArrayData> (MDataHandle& arrayHandle, std::vector<MQuaternion> &data);

template<class T>
std::vector<T> getArrayElements(MArrayDataHandle& arrayHandle, T (*getElement)(MDataHandle&), unsigned size, T fillValue);

template std::vector<MAngle>         getArrayElements(MArrayDataHandle& arrayHandle, MAngle (*getElement)(MDataHandle&),         unsigned size,  MAngle fillValue);
template std::vector<MEulerRotation> getArrayElements(MArrayDataHandle& arrayHandle, MEulerRotation (*getElement)(MDataHandle&), unsigned size, MEulerRotation fillValue);
template std::vector<MMatrix>        getArrayElements(MArrayDataHandle& arrayHandle, MMatrix (*getElement)(MDataHandle&),        unsigned size, MMatrix fillValue);
template std::vector<MQuaternion>    getArrayElements(MArrayDataHandle& arrayHandle, MQuaternion (*getElement)(MDataHandle&),    unsigned size, MQuaternion fillValue);
template std::vector<MVector>        getArrayElements(MArrayDataHandle& arrayHandle, MVector (*getElement)(MDataHandle&),        unsigned size, MVector fillValue);

template<class T>
MStatus setArrayElements(MArrayDataHandle& arrayHandle, std::vector<T> &values, MStatus (*setElement)(MDataHandle&, T));

template MStatus setArrayElements(MArrayDataHandle& arrayHandle, std::vector<MAngle> &values,         MStatus (*setElement)(MDataHandle&, MAngle));
template MStatus setArrayElements(MArrayDataHandle& arrayHandle, std::vector<MEulerRotation> &values, MStatus (*setElement)(MDataHandle&, MEulerRotation));
template MStatus setArrayElements(MArrayDataHandle& arrayHandle, std::vector<MMatrix> &values ,       MStatus (*setElement)(MDataHandle&, MMatrix));
template MStatus setArrayElements(MArrayDataHandle& arrayHandle, std::vector<MQuaternion> &values,    MStatus (*setElement)(MDataHandle&, MQuaternion));
template MStatus setArrayElements(MArrayDataHandle& arrayHandle, std::vector<MVector> &values,        MStatus (*setElement)(MDataHandle&, MVector));
