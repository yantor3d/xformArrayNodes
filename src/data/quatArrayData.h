/**
    Copyright (c) 2017 Ryan Porter    
    You may use, distribute, or modify this code under the terms of the MIT license.
*/

#pragma once

#include <istream>
#include <ostream>
#include <vector>

#include <maya/MArgList.h>
#include <maya/MPxData.h>
#include <maya/MQuaternion.h>
#include <maya/MString.h>
#include <maya/MStatus.h>
#include <maya/MSyntax.h>

class QuatArrayData : public MPxData
{
public:
                    QuatArrayData();
    virtual         ~QuatArrayData();
    static void*    creator();

    virtual MStatus readASCII(const MArgList &argList, unsigned int &end);
    virtual MStatus writeASCII(std::ostream &out);
    
    virtual MStatus readBinary(std::istream &in, unsigned int length);
    virtual MStatus writeBinary(std::ostream &out);

    virtual void    copy(const MPxData &src);

    virtual unsigned int             length();
    virtual std::vector<MQuaternion> getArray();
    virtual void                     setArray(std::vector<MQuaternion> &array);  

    virtual MTypeId typeId() const;
    virtual MString name()   const;

    static const MTypeId TYPE_ID;
    static const MString TYPE_NAME;

private:
    virtual void                setValues(std::vector<double> &values);
    virtual std::vector<double> getValues();

private:
    std::vector<MQuaternion> data;
};