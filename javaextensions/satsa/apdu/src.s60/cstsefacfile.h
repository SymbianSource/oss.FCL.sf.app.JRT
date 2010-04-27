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


#ifndef CSTSEFACFILE_H
#define CSTSEFACFILE_H

//  INCLUDES
#include "cstsfile.h"

namespace java
{
namespace satsa
{

// FORWARD DECLARATIONS
class CSTSAce;
class CSTSFileDataManager;

// CLASS DECLARATION
/**
 *  ACF card file class for handling file selection, reading and data
 *  decoding.
 *
 *  @since 3.0
 */
NONSHARABLE_CLASS(CSTSEFACFile): public CSTSFile
{
public: // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CSTSEFACFile* NewLC(CSTSFileDataManager* aFileDataManager);

    /**
     * Destructor.
     */
    virtual ~CSTSEFACFile();

public: // New functions

    /**
     * Sets records array. This method MUST be called
     * before read method is used.
     * @since 3.0
     * @param aRecordsArr Pointer to array where records will be added.
     */
    void SetArray(CArrayPtr< CSTSAce>* aRecordsArr);

    void SetPath(const TDesC8& aPath);

protected: //New fuctions

    /**
     * C++ default constructor.
     */
    CSTSEFACFile(CSTSFileDataManager* aFileDataManager);

protected: // Functions from base classes

    TBool DecodeAndAppendRowL(const TDesC8& aRowData,
                              TInt aMaxValidDataAmount);

    void ResetData();

    const TDesC8& RetrieveContentL();

private: // Data

    //array of Ace type of records
    CArrayPtr< CSTSAce>* iRecordsArr; //not owned

};

} // namespace satsa
} // namespace java
#endif // CSTSEFACFILE_H
// End of File
