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


#ifndef CSTSFILE_H
#define CSTSFILE_H

//  INCLUDES
#include "cstsasn1sequence.h"

namespace java
{
namespace satsa
{

// FORWARD DECLARATIONS
class MSTSASN1Module;
class CSTSFileDataManager;

// CLASS DECLARATION
/**
 *  Abstract card file class for handling file selection, reading and data
 *  decoding.
 *
 *  @since 3.0
 */
NONSHARABLE_CLASS(CSTSFile): public CSTSASN1Sequence
{
public: // Constructors and destructor

    /**
     * Destructor.
     */
    virtual ~CSTSFile();

public: // New functions

    /**
     * Reads file and decodes it's data and appends decoded data to
     * array.
     * @since 3.0
     * @param aMaxValidDataAmount Maximum amount of valid data which
     * will be appended to array.
     */
    void ReadL(TInt aMaxValidDataAmount);

    /**
     * Getter for amount of found suitable records
     * @since 3.0
     * @return Amount of found records
     */
    TInt FoundRecords() const;

public: // Functions from base classes


    /**
     * From MSTSASN1Module
     * @since 3.0
     * @param aRawData raw-data to be parsed in this instance
     */
    void DecodeL(const TDesC8& aRawData);

protected: //New fuctions

    /**
     * C++ default constructor.
     */
    CSTSFile();

    /**
     * Decodes current row and appends decoded object to array, if
     * it is wanted object. Derived class must implement this.
     * @since 3.0
     * @param aRowData Row data in bytes
     * @param aMaxValidDataAmount Maximum amount of valid data which
     * will be appended to array. Must be set to zero if all found
     * data can be appended.
     * @return True if all valid data is found, EFalse if not
     */
    virtual TBool DecodeAndAppendRowL(
        const TDesC8& aRowData,
        TInt aMaxValidDataAmount) = 0;

    /**
     * Resets readed file data which is saved to member variable.
     * Derived class must implement this.
     * @since 3.0
     */
    virtual void ResetData() = 0;

    /**
     * Gets content of the file. Derived class must implement this.
     * @since 3.0
     */
    virtual const TDesC8& RetrieveContentL() = 0;

private: //New fuctions

protected: // Data

    TPtrC8 iPath;

    CSTSFileDataManager* iFileDataManager; //not owned

    TUint16 iFileSize;

    TInt iFoundRecords;

private: // Data

    HBufC8 * iDataBuf; //owned
    TInt iMaxValidDataAmount;

};

} // namespace satsa
} // namespace java
#endif // CSTSFILE_H
// End of File

