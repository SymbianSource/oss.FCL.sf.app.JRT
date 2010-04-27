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
* Description:  Definition of CEntryLangDetails class.
*
*/


#ifndef ENTRYLANGDETAILS_H
#define ENTRYLANGDETAILS_H

// INCLUDE FILES

#include <e32base.h>

// FORWARD DECLARATIONS
class RReadStream;
class RWriteStream;

// CLASS DECLARATION

/**
 * Class used by CInstalledAppsRegistryEntry. This class is not exposed
 * outside of the entry. It contains a single Language/Package/Vendor
 * triplet. Each entry contains a collection of these language details.
 */
class CEntryLangDetails : public CBase
{
public:
    /**
    * Two-phase constructors.
    */
    static CEntryLangDetails* NewLC(TLanguage aLang,
                                    const TDesC& aPackageName,
                                    const TDesC& aVendor);
    static CEntryLangDetails* NewLC(RReadStream& aStream);
    static CEntryLangDetails* NewLC(const CEntryLangDetails& aOther);

    /**
    * Destructor.
    */
    virtual ~CEntryLangDetails();

    // attribute getters

    /**
    * Returns language assigned to this entry.
    */
    TLanguage Language() const;

    /**
    * Returns name of this package.
    */
    const TDesC& PackageName() const;

    /**
    * Returns name of the vendor of this package.
    */
    const TDesC& VendorName() const;

    // Streaming
    /**
    * Method used for serializing this object to a stream.
    * @param aStream Stream object used for serilazation.
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
    /**
    * Constructor. Sets language.
    */
    CEntryLangDetails(TLanguage aLang = ELangEnglish);

    /**
    * Second-phase constructors.
    */
    void ConstructL(const TDesC& aPackage, const TDesC& aVendor);
    void ConstructL(RReadStream& aStream);

private:    // data member
    TLanguage iLanguage;
    HBufC* iPackageName;    // Owned.
    HBufC* iVendorName;     // Owned.
};

#endif // ENTRYLANGDETAILS_H
