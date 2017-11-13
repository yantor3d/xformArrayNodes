#include "arrayData.h"

#include <stdio.h>

#include <istream>
#include <ostream>
#include <vector>

#include <maya/MArgList.h>
#include <maya/MGlobal.h>
#include <maya/MStatus.h>
#include <maya/MStatus.h>

std::vector<double> readASCIIData(unsigned numberOfElementsPerItem, const MArgList &args, unsigned int &end, MStatus *ReturnStatus)
{
    std::vector<double> result;
    MStatus status;

    unsigned numberOfItems = (unsigned) args.asInt(end++, &status);
    unsigned int numberOfArguments = args.length() - end;
    unsigned int numberOfValues = numberOfItems * numberOfElementsPerItem;

    if (status)
    {
        if (
            numberOfItems != numberOfArguments &&
            (numberOfItems * numberOfElementsPerItem) != numberOfArguments
        ) {
            status = MStatus::kInvalidParameter;
        }
    }

    if (status)
    {
        int valuesParsed = 0;

        result.resize(numberOfItems * numberOfElementsPerItem);

        bool argumentsAreInTuples = numberOfElementsPerItem > 1 && (numberOfItems == numberOfArguments);

        if (argumentsAreInTuples)
        {
            unsigned n = numberOfElementsPerItem;

            for (unsigned i = 0; i < numberOfItems; i++)
            {
                unsigned int idx = end; end++;
                MStringArray argStr = args.asStringArray(idx, &status);

                if (status && argStr.length() == n)
                {
                    for (unsigned j = 0; j < n; j++)
                    {
                        if (argStr[j].isDouble())
                        {
                            result[(i*n)+j] = argStr[j].asDouble();
                            valuesParsed++;
                        }
                    }
                } 
            }
        } else {
            for (unsigned i = 0; i < numberOfArguments; i++)
            {
                MString arg = args.asString(end++, &status);

                if (status && arg.isDouble())
                {
                    result[i] = arg.asDouble();
                    valuesParsed++;
                }
            }
        }

        status = valuesParsed == numberOfValues ? MStatus::kSuccess : MStatus::kFailure;
    }


    if (ReturnStatus) { *ReturnStatus = status; }

    return result;
}


std::vector<double> readBinaryData(unsigned numberOfElementsPerItem, std::istream &in, unsigned int length, MStatus *ReturnStatus)
{
    bool readFailed = false;

    std::vector<double> result;

    if (length > 0)
    {
        unsigned numberOfItems;
        in.read((char*) &numberOfItems, sizeof(numberOfItems));

        if (in.fail())
        {
            readFailed = true;
        } else {
            result.resize(numberOfItems * numberOfElementsPerItem);

            for (unsigned i = 0; i < numberOfItems * numberOfElementsPerItem; i++)
            {
                in.read((char*) &result[i], sizeof(result[i]));
                readFailed = in.fail();

                if (readFailed) { break; }
            }
        }
    }

    if (ReturnStatus) 
    { 
        *ReturnStatus = readFailed ? MStatus::kFailure : MStatus::kSuccess; 
    }

    return result;
}


MStatus writeASCIIData(std::vector<double> values, unsigned numberOfItems, std::ostream &out)
{
    MStatus status;

    size_t numberOfValues = values.size();

    out << numberOfItems << " ";

    if (out.fail())
    {
        status =  MStatus::kFailure;
    } else {
        for (size_t i = 0; i < numberOfValues; i++)
        {
            out << values[i] << " ";

            if (out.fail())
            {
                status = MStatus::kFailure;
                break;
            }
        }
    }

    return status;
}


MStatus writeBinaryData(std::vector<double> values, unsigned numberOfItems, std::ostream &out)
{
    MStatus status;
    
    size_t numberOfValues = values.size();
    out.write((char*) &numberOfItems, sizeof(numberOfItems));

    if (out.fail())
    {
        status = MStatus::kFailure;
    } else {
        for (size_t i = 0; i < numberOfValues; i++)
        {
            out.write((char*) &values[i], sizeof(values[i]));

            if (out.fail())
            {
                status = MStatus::kFailure;
                break;
            }
        }
    }

    return status; 
}
