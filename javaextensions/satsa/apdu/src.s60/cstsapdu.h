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


#ifndef CSTSAPDU_H
#define CSTSAPDU_H

//  INCLUDES
#include <e32base.h>

namespace java
{
namespace satsa
{

// CLASS DECLARATION
/**
 *  Wrapper class for byte apdu message.
 */
NONSHARABLE_CLASS(CSTSApdu): public CBase
{
public: // data types
    enum TSTSApduStandard
    {
        ESTSGSM,
        ESTSUICC
    };

public:

    static CSTSApdu* NewLC(TDesC8& aApduBytes,
                           TSTSApduStandard aStandard);

    static CSTSApdu* NewL(TDesC8& aApduBytes,
                          TSTSApduStandard aStandard);

    static CSTSApdu* NewLC(TInt aMaxLength,
                           TSTSApduStandard aStandard);

    static CSTSApdu* NewL(TInt aMaxLength,
                          TSTSApduStandard aStandard);

    /**
     * Destructor.
     */
    virtual ~CSTSApdu();

public: // New functions

    /**
     * Getter for command and response Apdu bytes
     * @return Bytes in modifiable TPtr8 reference
     */
    TPtr8& ApduPtr();

    /**
     * Reallocates apdu buffer if needed
     * @param aMaxLength The maximum length of data that the new
     *          apdu buffer is to represent
     */
    void ReAllocL(TInt aMaxLength);

    /**
     * Returns type of apdu standard
     * @return Type of apdu standard
     */
    TSTSApduStandard Standard() const;

private:

    void ConstructL(TDesC8& aApduBytes);

    void ConstructL(TInt aMaxLength);

    CSTSApdu(TSTSApduStandard aStandard);

private:

    TPtr8 iApduBytesPtr;
    TSTSApduStandard iStandard;

    // Apdu bytes, owned
    HBufC8* iApduBytes;

};

} // namespace satsa
} // namespace java
#endif // CSTSAPDU_H
// End of File
