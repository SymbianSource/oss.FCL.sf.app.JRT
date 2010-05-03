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


#ifndef CSTSRESPAPDU_H
#define CSTSRESPAPDU_H

//  INCLUDES
#include "cstsapdu.h"

namespace java
{
namespace satsa
{

// CLASS DECLARATION
/**
 *  Common response APDU class. Base class for detailed response APDU classes.
 *
 *  @since 3.0
 */
NONSHARABLE_CLASS(CSTSRespApdu): public CBase
{

public: // connection target types
    enum TSTSConnectionTarget
    {
        ESTSUSAT = 0, //SAT connection
        ESTSAID = 1 //AID connection
    };

public: // Constructors and destructor
    /**
     * Two-phased constructor.
     */
    static CSTSRespApdu* NewLC(
        TInt aMaxLength = 2,//minimum length is 2
        CSTSApdu::TSTSApduStandard aStandard = CSTSApdu::ESTSUICC);

    static CSTSRespApdu* NewL(
        TInt aMaxLength = 2,//minimum length is 2
        CSTSApdu::TSTSApduStandard aStandard = CSTSApdu::ESTSUICC);

    /**
     * Destructor.
     */
    virtual ~CSTSRespApdu();

public: // New functions

    /**
     * Checks is response normal OK ending or not.
     * @since 3.0
     * @return ETrue if was normal ending, otherwise EFalse
     */
    TBool IsNormalEnding() const;

    /**
     * Getter for response apdu bytes. Through this reference bytes can
     * be appended and manipulated.
     * @since 3.0
     * @return Bytes in modifiable TPtr8 reference
     */
    TPtr8& ResponseBytes() const;

    /**
     * Checks does response indicate that GET RESPONSE apdu is needed
     * @since 3.0
     * @param On return will contain new Le which can be used in
     * Get Response APDU.
     * @return ETrue if GET RESPONSE is needed, otherwise EFalse
     */
    TBool GetResponseNeeded(TUint8& aNewLength) const;

    /**
     * Checks does response indicate that we need to resend previous apdu.
     * @since 3.0
     * @param On return will contain new Le which can be used as a
     * parameter of apdu which will be resend.
     * @return ETrue if resend is needed, otherwise EFalse
     */
    TBool ResendNeeded(TUint8& aNewLength) const;

    /**
     * Sets connection target. GetResponseNeeded method's behaviour is
     * little bit different depending on are we connected to USAT
     * application or NOT.
     * @since 3.0
     * @param aConnTarget Type or target.
     */
    void SetConnectionTarget(TSTSConnectionTarget aConnTarget);

protected: // New functions

    /**
     * C++ default constructor.
     */
    CSTSRespApdu();

    /**
     * Protected construction to allow derivation
     */
    void ConstructL(TInt aMaxLength = 2, //minimum length is 2
                    CSTSApdu::TSTSApduStandard aStandard = CSTSApdu::ESTSUICC);

    /**
     * Returns SW1 byte of response apdu
     * @since 3.0
     * @return SW1 byte
     */
    TUint8 GetSW1() const;

    /**
     * Returns SW2 byte of response apdu
     * @since 3.0
     * @return SW2 byte
     */
    TUint8 GetSW2() const;

protected: // Data

    CSTSApdu* iRespData; //owned

    TSTSConnectionTarget iConnTarget;

};

} // namespace satsa
} // namespace java
#endif // CSTSRESPAPDU_H
// End of File
