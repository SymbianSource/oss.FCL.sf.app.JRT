/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Definition of CPropertyValuePair class.
*
*/


#ifndef PROPERTYVALUEPAIR_H
#define PROPERTYVALUEPAIR_H

// INCLUDE FILES

#include <e32base.h>

// FORWARD DECLARATIONS

class RWriteStream;
class RReadStream;

/**
* Class used by CInstalledAppsRegistryEntry. This class is not exposed
* outside of the entry. It contains a single Language/Package/Vendor triplet.
* Each entry contains a collection of these language details.
*/
class CPropertyValuePair : public CBase
{
public:
    /**
    * Two-phase constructors.
    */
    static CPropertyValuePair* NewLC(TUid aPropertyId,
                                     const TDesC8& aValue);
    static CPropertyValuePair* NewLC(const CPropertyValuePair& aOther);
    static CPropertyValuePair* NewLC(RReadStream& aStream);

    /**
    * Destructor.
    */
    virtual ~CPropertyValuePair();

    // attribute getters
    /**
    * Returns the id of this property.
    */
    TUid PropertyId() const;

    /**
    * Returns the value of this property.
    */
    const TDesC8& Value() const;

    /**
    * Method used to (re-)set the value of this attribute.
    */
    void SetValueL(const TDesC8& aValue);

    // Streaming
    /**
    * Method used for serilazation of this object.
    * @param aStream Stream into which data is written to.
    */
    void ExternalizeL(RWriteStream& aStream) const;

    /**
    * Returns the number of bytes a buffer, used for streaming, must
    * at least consist of.
    * @return Number of bytes required by a buffer used for storing.
    */
    TInt SizeForStreamingL() const;

private:
    // 1st and 2nd phase constructors
    CPropertyValuePair(TUid aPropertyId = TUid());
    void ConstructL(const TDesC8& aValue);
    void ConstructL(RReadStream& aStream);

private:
    TUid iPropertyId;
    HBufC8* iValue;     // Owned.
};

#endif // PROPERTYVALUEPAIR_H
