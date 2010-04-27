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


#ifndef CSTSPATH_H
#define CSTSPATH_H

//  INCLUDES
#include "cstsasn1sequence.h"

namespace java
{
namespace satsa
{

// CLASS DECLARATION
/**
 *  Path type module
 *
 *  Path ::= SEQUENCE {
 *      path OCTET STRING,
 *      index INTEGER (0..pkcs15-ub-index) OPTIONAL,
 *      length [0] INTEGER (0..pkcs15-ub-index) OPTIONAL
 *      }( WITH COMPONENTS {..., index PRESENT, length PRESENT}|
 *      WITH COMPONENTS {..., index ABSENT, length ABSENT})
 *
 *  @since 3.0
 */
NONSHARABLE_CLASS(CSTSPath): public CSTSASN1Sequence
{
public: // Constructors and destructor
    /**
     * Two-phased constructor.
     */
    static CSTSPath* NewLC();

    static CSTSPath* NewL();

    /**
     * Destructor.
     */
    virtual ~CSTSPath();

public: // New functions

    /**
     * Getter for Path
     * @since 3.0
     * @return Path in TDesC reference
     */
    const TDesC8& Path() const;

public: // Functions from base classes

    /**
     * From MSTSASN1Module
     * @since 3.0
     * @param aRawData raw-data to be parsed in this instance
     */
    void DecodeL(const TDesC8& aRawData);

protected: // New functions

    /**
     * Protected construction to allow derivation
     */
    void ConstructL();

    /**
     * C++ default constructor.
     */
    CSTSPath();

protected: // Data
    // FID or absolute/relative path depending on situation, owned
    HBufC8* iPath;

};

} // namespace satsa
} // namespace java
#endif // CSTSPATH_H
// End of File

