/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  URI parser specific to symbian
*
*/


#include "turi.h"



const TInt KMinSizeOfEqualsAndValue = 2;
const TUint16 KCharColon = ':';
const TUint16 KCharSemiColon = ';';
const TUint16 KCharForwardSlash = '/';
const TUint16 KCharEquals = '=';
const TUint16 KCharQuestionMark = '?';
_LIT(KSlashes, "//");
_LIT(KIPv6PrefixForIPv4MappedAddress, "::ffff:");
_LIT(KParamValueTrue, "true");
_LIT(KParamValueFalse, "false");



// ----------------------------------------------------------------------------
// TUri
//

/**
There is not much else to distinguish the most basic URI from other strings,
so all we test here is if it contains a colon.
*/
TBool TUri::IsUri(const TDesC16& aName)
{
    return (aName.Locate(KCharColon) > 0);
}



/**
Parses a URI, based on the format detailed in RFC 2396, e.g.
<scheme>://<authority><path>?<query>
or more generally:
<scheme>:<scheme-specific-part>
if aCheckForSlashes is ETrue and scheme-specific-part
doesn't start with 2 slashes, Parse returns KErrArgument
*/
TInt TUri::Parse(const TDesC16& aDes , TBool aCheckForSlashes)
{
    iScheme.Set(KNullDesC());
    iSchemeData.Set(KNullDesC());
    iHost.Set(KNullDesC());
    iPort.Set(KNullDesC());
    iPath.Set(KNullDesC());
    iParameters.Set(KNullDesC());
    iQuery.Set(KNullDesC());

    // Set this descriptor to equal the given URI
    Set(aDes);
    // Ensure that the colon is present and determine the scheme and scheme-specific
    // part
    TInt colonIndex = Locate(KCharColon);
    // The scheme must not be empty
    if (colonIndex <= 0)
    {
        return KErrArgument;
    }
    iScheme.Set(Left(colonIndex));
    iSchemeData.Set(Mid(colonIndex + 1));

    // Parse the scheme data further - we assume it is going to start with 2 slashes
    // and may contain stuff like a host, port, path, parameters. If it does not
    // have 2 slashes we do no more parsing.
    if (iSchemeData.Find(KSlashes()) == 0)
    {
        TPtrC16 portionToParse(iSchemeData.Mid(KSlashes().Length()));

        // Parse location (host and port)
        TInt pathStartIndex = portionToParse.Locate(KCharForwardSlash);
        TInt parametersStartIndex = portionToParse.Locate(KCharSemiColon);
        TInt queryStartIndex = portionToParse.Locate(KCharQuestionMark);
        // Find the length of the location portion of the URI
        TInt locationLength = portionToParse.Length();
        if (pathStartIndex != KErrNotFound)
        {
            locationLength = pathStartIndex;
        }
        else if (parametersStartIndex != KErrNotFound)
        {
            locationLength = parametersStartIndex;
        }
        else if (queryStartIndex != KErrNotFound)
        {
            locationLength = queryStartIndex;
        }
        // Extract the host and port from the location portion, separated by a colon
        const TPtrC16 location(portionToParse.Left(locationLength));
        // handle the case of an IPv4-mapped address, like ::ffff:10.10.10.10
        TPtrC16 IPv4Location;
        TInt prefixLength = 0;
        if (location.FindF(KIPv6PrefixForIPv4MappedAddress()) == 0)
        {
            prefixLength = KIPv6PrefixForIPv4MappedAddress().Length();
            IPv4Location.Set(location.Mid(prefixLength));
        }
        else
        {
            IPv4Location.Set(location);
        }
        TInt locationColonIndex = IPv4Location.Locate(KCharColon);
        if (locationColonIndex != KErrNotFound)
        {
            iHost.Set(location.Left(prefixLength + locationColonIndex));
            iPort.Set(IPv4Location.Mid(locationColonIndex + 1));
        }
        else
        {
            iHost.Set(location);
        }

        // Parse path (and Query)
        if (pathStartIndex != KErrNotFound)
        {
            // Skip past the '/' character that marks the start of the path
            portionToParse.Set(portionToParse.Mid(locationLength + 1));
        }
        queryStartIndex = portionToParse.Locate(KCharQuestionMark);
        if (queryStartIndex != KErrNotFound)
        {
            iQuery.Set(portionToParse.Mid(queryStartIndex + 1));
            portionToParse.Set(portionToParse.Left(queryStartIndex));
        }
        if (pathStartIndex != KErrNotFound)
        {
            iPath.Set(portionToParse);
        }

        // Parse parameters
        parametersStartIndex = portionToParse.Locate(KCharSemiColon);
        if (parametersStartIndex != KErrNotFound)
        {
            // Do not skip the ';' character that marks the start of the parameters
            iParameters.Set(portionToParse.Mid(parametersStartIndex));
        }

    }
    else
    {
        if (aCheckForSlashes)
            return KErrArgument;
        else
        {
            //not checking for slashes, but do expect there to be parameters
            //used for CommConnection URIs
            TInt parametersStartIndex = iSchemeData.Locate(KCharSemiColon);
            if (parametersStartIndex != KErrNotFound)
            {
                iPort.Set(iSchemeData.Left(parametersStartIndex));
                // Do not skip the ';' character that marks the start of the parameters
                iParameters.Set(iSchemeData.Mid(parametersStartIndex));
            }
            else
            {
                iPort.Set(iSchemeData);
            }
        }
    }
    return KErrNone;
}



TInt TUri::PortAsUint(TUint& aPortNum) const
{
    if (iPort.Length() == 0)
    {
        return KErrNotFound;
    }
    TLex16 portParser(iPort);
    TInt error = portParser.Val(aPortNum);
    // Check that the whole port string was parsed to a number
    if (error != KErrNone || !portParser.Eos())
    {
        return KErrArgument;
    }
    return KErrNone;
}



/**
Looks for the boolean value of the named parameter.
@param aParamName The name of the boolean parameter.
@param aValue The value that is set to the value of the parameter. This is
not changed if an error code is returned.
@return KErrNotFound if the parameter is not defined, KErrArgument if there
is a parsing error, otherwise KErrNone.
*/
TInt TUri::GetParameterValue(const TDesC16& aParamName, TBool& aValue) const
{
    // Assign default value
    TBool value;
    TPtrC16 paramValue;
    TInt error = GetParameterValue(aParamName, paramValue);
    if (error != KErrNone)
    {
        return error;
    }
    if (paramValue.CompareF(KParamValueTrue()) == 0)
    {
        value = ETrue;
    }
    else if (paramValue.CompareF(KParamValueFalse()) == 0)
    {
        value = EFalse;
    }
    else
    {
        return KErrArgument;
    }
    aValue = value;
    return KErrNone;
}



/**
Looks for the integer value of the named parameter. If the parameter is
not found, it dos not change the value of aValue.
@return KErrNotFound if the parameter is not defined, KErrArgument if there
is a parsing error, otherwise KErrNone.
*/
TInt TUri::GetParameterValueInt(const TDesC16& aParamName, TInt& aValue) const
{
    TPtrC16 paramValue;
    TInt error = GetParameterValue(aParamName, paramValue);
    if (error != KErrNone)
    {
        return error;
    }
    TLex16 numberParser(paramValue);
    TInt value;
    error = numberParser.Val(value);
    if (error != KErrNone)
    {
        return KErrArgument;
    }
    aValue = value;
    return KErrNone;
}



TInt TUri::GetParameterValue(const TDesC16& aParamName, TPtrC16& aValue) const
{
    __ASSERT_DEBUG(aParamName.Length() > 0, User::Invariant());

    TInt index = FindParamNameIndex(aParamName, iParameters);
    if (index < KErrNone)
    {
        return index;
    }
    // We know the value is defined in the parameter list
    // Ensure that the parameter is defined only once in the list
    TPtrC16 restOfParameterList(iParameters.Mid(index + aParamName.Length()));
    TInt secondParamIndex = FindParamNameIndex(aParamName, restOfParameterList);
    if (secondParamIndex >= KErrNone)
    {
        // Parameter is defined twice, which is invalid
        return KErrArgument;
    }
    // Skip over the name and equals sign
    index += aParamName.Length();
    ++index;
    // Extract the value itself
    const TPtrC16 remainder(iParameters.Mid(index));
    TInt nextSemiColonIndex = remainder.Locate(KCharSemiColon);
    TInt paramValueLength = (nextSemiColonIndex != KErrNotFound) ?
                            nextSemiColonIndex : iParameters.Length();
    aValue.Set(remainder.Left(paramValueLength));
    return KErrNone;
}



/**
Finds the index of the parameter name. Checks that there are enough characters
after the name for the equals sign and a value of length 1, and that the name
has a semi-colon in front and an equals after it.
*/
TInt TUri::FindParamNameIndex(const TDesC16& aParamName,
                              const TDesC16& aParametersSegment) const
{
    TPtrC16 parameters(aParametersSegment);
    TInt nameIndex = KErrNotFound;
    TInt paramSegmentIndex = 0;
    do
    {
//      DEBUG( "TUri::GetParameterValue(): finding param name: %S", &aParamName );
        // FindF is used because the case is not relevant for data within the URI
        TInt index = parameters.FindF(aParamName);
        if (index == KErrNotFound)
        {
            return KErrNotFound;
        }

        TInt endOfParamNameIndex = index + aParamName.Length();
        // Check there are enough extra characters after the parameter name for the
        // equals and value
        TPtrC16 afterParamNameString(parameters.Mid(endOfParamNameIndex));
        if (afterParamNameString.Length() < KMinSizeOfEqualsAndValue)
        {
            return KErrArgument;
        }

        // Check the parameter name found is a complete name and not part of a
        // larger string, i.e. it has a semi-colon in front and an equals after
        if (index == 0 || parameters[index - 1] != KCharSemiColon ||
                parameters[endOfParamNameIndex] != KCharEquals)
        {
            paramSegmentIndex += endOfParamNameIndex;
            parameters.Set(parameters.Mid(endOfParamNameIndex));
        }
        else
        {
            nameIndex = paramSegmentIndex + index;
        }
    }
    while (nameIndex == KErrNotFound);
    return nameIndex;
}



/**
Iterates through the parameters of the URI, checking that each parameter name is
present in the supplied list of valid names.
@return KErrNone if all parameter names match the legal ones, or if there are
no parameters in the URI, as parameters are optional anyway.
*/
TInt TUri::CheckParameterValidity(const RPointerArray<TDesC16>& aLegalParams) const
{
    if (iParameters.Length() == 0)
    {
        return KErrNone;
    }
    TPtrC16 remainder(iParameters);
    TInt semiColonIndex = 0;
    do
    {
        remainder.Set(remainder.Mid(semiColonIndex + 1));
        TInt error = CheckParameter(aLegalParams, remainder);
        if (error != KErrNone)
        {
            return error;
        }
        semiColonIndex = remainder.Locate(KCharSemiColon);
    }
    while (semiColonIndex != KErrNotFound);
    return KErrNone;
}



/**
Checks that the parameter name at the start of aRemainder is present in the
list of legal parameters.
*/
TInt TUri::CheckParameter(const RPointerArray<TDesC16>& aLegalParams,
                          const TDesC16& aRemainder) const
{
    // Find the end of the parameter name
    TInt equalsIndex = aRemainder.Locate(KCharEquals);
    if (equalsIndex == KErrNotFound)
    {
        return KErrArgument;
    }
    // Search for the name in the legal names list
    TPtrC16 paramName(aRemainder.Left(equalsIndex));
    TIdentityRelation<TDesC16> relation(&TUri::MatchParamName);
    TInt nameIndex = aLegalParams.Find(&paramName, relation);
    if (nameIndex == KErrNotFound)
    {
        return KErrArgument;
    }
    return KErrNone;
}



/**
Utility method for use with RArray::Find().
*/
TBool TUri::MatchParamName(const TDesC16& aName1, const TDesC16& aName2)
{
    return (aName1.CompareF(aName2) == 0);
}



/**
Check there are no duplicate parameters
*/
TInt TUri::CheckNoDuplicateParameters() const
{
    if (iParameters.Length() == 0)
    {
        return KErrNone;
    }

    RArray<TPtrC16> parameters;
    TPtrC16 remainder(iParameters);
    TInt semiColonIndex = 0;
    do
    {
        remainder.Set(remainder.Mid(semiColonIndex + 1));

        TInt equalsIndex = remainder.Locate(KCharEquals);
        if (equalsIndex == KErrNotFound)
        {
            parameters.Close();
            return KErrArgument;
        }

        TPtrC16 paramName(remainder.Left(equalsIndex));

        TIdentityRelation<TPtrC16> relation(&TUri::MatchParamNamePtr);
        TInt nameIndex = parameters.Find(paramName, relation);

        TInt error = KErrNone;
        if (nameIndex == KErrNotFound)
        {
            error = parameters.Append(paramName);
        }
        else if (nameIndex >= 0)
        {
            error = KErrAlreadyExists;
        }
        else
        {
            error = nameIndex;
        }

        if (error != KErrNone)
        {
            parameters.Close();
            return error;
        }
        semiColonIndex = remainder.Locate(KCharSemiColon);
    }
    while (semiColonIndex != KErrNotFound);

    parameters.Close();
    return KErrNone;
}


/**
Utility method for use with RArray::Find().
*/
TBool TUri::MatchParamNamePtr(const TPtrC16& aName1, const TPtrC16& aName2)
{
    return (aName1.CompareF(aName2) == 0);
}
