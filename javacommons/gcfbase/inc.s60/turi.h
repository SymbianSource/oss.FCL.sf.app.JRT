/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/


#ifndef TURI_H
#define TURI_H

#include <e32std.h>



/**
 * A generic class for parsing the parts of URI's.
 *
 * NOTE: This uses the folding version of some descriptor methods, which has the
 * undesired affect of removing the distinctions between characters with and without
 * accents, etc. This is not good, but we do not want to implement our own
 * comparison ignoring case.
 */
class TUri : public TPtrC16
{
public:
    static TBool IsUri(const TDesC16& aName);

public:
    TInt Parse(const TDesC16& aDes, TBool aCheckForSlashes = EFalse);
    inline const TDesC16& Scheme() const;
    inline const TDesC16& Host() const;
    inline const TDesC16& Port() const;
    inline const TDesC16& Path() const;
    inline const TDesC16& Query() const;
    inline const TDesC16& Parameters() const;
    TInt PortAsUint(TUint& aPortNum) const;
    TInt GetParameterValue(const TDesC16& aParamName, TBool& aValue) const;
    TInt GetParameterValueInt(const TDesC16& aParamName, TInt& aValue) const;
    TInt GetParameterValue(const TDesC16& aParamName, TPtrC16& aValue) const;
    TInt CheckParameterValidity(const RPointerArray<TDesC16>& aParamNames) const;
    TInt CheckNoDuplicateParameters() const;

private:
    TInt FindParamNameIndex(const TDesC16& aParamName,
                            const TDesC16& aParametersSegment) const;
    TInt CheckParameter(const RPointerArray<TDesC>& aParamNames,
                        const TDesC16& aRemainder) const;
    static TBool MatchParamName(const TDesC16& aName1, const TDesC16& aName2);
    static TBool MatchParamNamePtr(const TPtrC16& aName1, const TPtrC16& aName2);

private:
    TPtrC16 iScheme;
    TPtrC16 iSchemeData;
    TPtrC16 iHost;
    TPtrC16 iPort;
    TPtrC16 iPath;
    TPtrC16 iParameters;
    TPtrC16 iQuery;
};



inline const TDesC16& TUri::Scheme() const
{
    return iScheme;
}
inline const TDesC16& TUri::Host() const
{
    return iHost;
}
inline const TDesC16& TUri::Port() const
{
    return iPort;
}
inline const TDesC16& TUri::Path() const
{
    return iPath;
}
inline const TDesC16& TUri::Query() const
{
    return iQuery;
}
inline const TDesC16& TUri::Parameters() const
{
    return iParameters;
}



#endif // TURI_H


