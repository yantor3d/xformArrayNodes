/**
    Copyright (c) 2017 Ryan Porter    
    You may use, distribute, or modify this code under the terms of the MIT license.
*/

#pragma once

#include <maya/MArgList.h>
#include <maya/MArgDatabase.h>
#include <maya/MPlug.h>
#include <maya/MPxCommand.h>
#include <maya/MString.h>
#include <maya/MStatus.h>
#include <maya/MSyntax.h>

class GetArrayAttrCmd : public MPxCommand
{
public:
                        GetArrayAttrCmd();
    virtual             ~GetArrayAttrCmd();

    static void*        creator();
    static MSyntax      getSyntax();

    virtual MStatus     doIt(const MArgList& argList);
    virtual MStatus     redoIt();

    virtual bool        isUndoable() const { return false; }
    virtual bool        hasSyntax()  const { return true; }
    
private:
    void                warnEmptyArray();
        
public:
    static MString      COMMAND_NAME;

private:
    MPlug               requestedAttribute;
};