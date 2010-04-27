/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef CSTSACIE_H
#define CSTSACIE_H

#include "cstsasn1sequence.h"

namespace java
{
namespace satsa
{

class CSTSPath;

// CLASS DECLARATION
/**
 *  Acie type module
 *
 *  Acie ::= SEQUENCE {
 *      aid AID OPTIONAL,
 *      acfPath Path,
 *      ... -- for future extensions
 *  }
 *  AID ::= OCTET STRING
 *
 *  Path ::= SEQUENCE {
 *      path OCTET STRING,
 *      index INTEGER (0..pkcs15-ub-index) OPTIONAL,
 *      length [0] INTEGER (0..pkcs15-ub-index) OPTIONAL
 *      }( WITH COMPONENTS {..., index PRESENT, length PRESENT}|
 *      WITH COMPONENTS {..., index ABSENT, length ABSENT})
 *
 */
NONSHARABLE_CLASS(CSTSAcie): public CSTSASN1Sequence
{
public:

    static CSTSAcie* NewLC();

    virtual ~CSTSAcie();

public:

    /**
     * Getter for aid
     * @return aid in TDesC reference
     */
    const TDesC8& AID() const;

    /**
     * Getter for ACF path
     * @return ACF path in CSTSPath reference
     */
    const CSTSPath& ACFPath() const;

public: // Functions from base classes

    /**
     * From MSTSASN1Module
     * @param aRawData raw-data to be parsed in this instance
     */
    void DecodeL(const TDesC8& aRawData);

protected:

    void ConstructL();

    CSTSAcie();

protected:
    // AID, owned
    HBufC8* iAid;
    // ACFPath, owned
    CSTSPath* iACFPath;

};

} // namespace satsa
} // namespace java
#endif // CSTSACIE_H
// End of File
