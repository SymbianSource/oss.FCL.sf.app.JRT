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


#ifndef CSTSEFACIFILE_H
#define CSTSEFACIFILE_H

//  INCLUDES
#include "cstsfile.h"

namespace java
{
namespace satsa
{

// FORWARD DECLARATIONS
class CSTSAcie;
class CSTSFileDataManager;

// CLASS DECLARATION
/**
 *  ACIF card file class for handling file selection, reading and data
 *  decoding.
 *
 *  @since 3.0
 */
NONSHARABLE_CLASS(CSTSEFACIFile): public CSTSFile
{
public: // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CSTSEFACIFile* NewLC(CSTSFileDataManager* aFileDataManager);

    /**
     * Destructor.
     */
    virtual ~CSTSEFACIFile();

public: // New functions

    /**
     * Sets records array. This method MUST be called
     * before read method is used.
     * @since 3.0
     * @param aRecordsArr Pointer to array where records will be added.
     */
    void SetArray(CArrayPtr< CSTSAcie>* aRecordsArr);

    /**
     * Setter for AID which will be used to find correct record
     * This method MUST be called before derived ReadL method is called
     * @since 3.0
     * @param aAID Application Identifier of the card application
     */
    void SetAIDL(const TDesC8& aAID);

protected: //New fuctions

    /**
     * C++ default constructor.
     */
    CSTSEFACIFile(CSTSFileDataManager* aFileDataManager);

    /**
     * Protected construction to allow derivation
     */
    void ConstructL();

protected: // Functions from base classes

    TBool DecodeAndAppendRowL(const TDesC8& aRowData,
                              TInt aMaxValidDataAmount);

    void ResetData();

    const TDesC8& RetrieveContentL();

private: // Data

    //array of Acie type of records
    CArrayPtr< CSTSAcie>* iRecordsArr; //not owned

    //card application aid
    HBufC8* iAID; //owned
};

} // namespace satsa
} // namespace java
#endif // CSTSEFACIFILE_H
// End of File

