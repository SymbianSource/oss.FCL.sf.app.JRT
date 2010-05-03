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


#ifndef CSTSCMDAPDU_H
#define CSTSCMDAPDU_H

//  INCLUDES
#include "cstsapdu.h"
#include "stsapduconstants.h"

//  CONSTANTS
//Apdu CLA bytes
const TUint8 KSTSCLABasicGSM = 0xA0;
const TUint8 KSTSCLABasicUMTS = 0x00;

namespace java
{
namespace satsa
{

// CLASS DECLARATION
/**
 *  Common command APDU class. Base class for detailed command apdu classes.
 *
 *  @since 3.0
 */
NONSHARABLE_CLASS(CSTSCmdApdu): public CBase
{
public: // command apdu byte locations
    enum TSTSApduByteType
    {
        ESTSCLA = 0,
        ESTSINS = 1,
        ESTSP1 = 2,
        ESTSP2 = 3,
        ESTSLc = 4,
        ESTSData = 5 //only if data is one byte long
    };

public: // Constructors and destructor
    /**
     * Two-phased constructor.
     */
    static CSTSCmdApdu* NewLC(TDesC8& aApduBytes,
                              CSTSApdu::TSTSApduStandard aStandard);

    static CSTSCmdApdu* NewL(TDesC8& aApduBytes,
                             CSTSApdu::TSTSApduStandard aStandard);

    /**
     * Destructor.
     */
    virtual ~CSTSCmdApdu();

public: // New functions

    /**
     * Check is apdu in correct form according to JSR177. Leaves with proper
     * error code if needed.
     * @since 3.0
     * @param aUsedChannel Channel number which will be used.
     */
    void CheckL(TInt aUsedChannel) const;

    /**
     * Getter for command apdu bytes
     * @since 3.0
     * @return Bytes in modifiable TPtr8 reference
     */
    TPtr8& ApduBytes() const;

    /**
     * Sets channel bits to APDU data bytes
     * @since 3.0
     */
    void SetChannel(TInt aChannel);

    /**
     * Returns apdu header part
     * @since 3.0
     * @return Apdu header part
     */
    const TDesC8& Header();

    /**
     * Gets wanted apdu byte from APDU data bytes
     * @since 3.0
     * @param aByteType Type of the byte
     * @return Apdu header part
     */
    TUint8 ApduByte(TSTSApduByteType aByteType) const;

    /**
     * Appends Lc byte and data bytes after header part. Header must exist.
     * Takes copy and reallocates apdu buffer if needed.
     * @since 3.0
     * @param aDataBytes Data bytes.
     */
    void AppendDatabytesL(const TDesC8& aDataBytes);

    /**
     * Sets Le byte to APDU data bytes
     * @param aLe Le byte
     *
     * @since 3.0
     */
    void SetLe(TUint8 aLe);

protected: //New fuctions

    /**
     * C++ default constructor.
     */
    CSTSCmdApdu();

    /**
     * Protected to allow derivation
     */
    void ConstructL(TDesC8& aApduBytes,
                    CSTSApdu::TSTSApduStandard aStandard);

    /**
     * Checks is apdu length correct and leaves proper error code if not.
     * @since 3.0
     */
    void CheckApduLengthL() const;

    /**
     * Sets header bytes to APDU data buffer
     * @param aCla CLA byte
     * @param aIns CLA byte
     * @param aP1 P1 byte
     * @param aP2 P2 byte
     *
     * @since 3.0
     */
    void SetHeader(TUint8 aCla, TUint8 aIns, TUint8 aP1, TUint8 aP2);

    /**
     * Sets wanted apdu byte to APDU data bytes
     * @param aByte Byte to be set
     * @param aByteType Type of the byte
     *
     * @since 3.0
     */
    void SetApduByte(TUint8 aByte, TSTSApduByteType aByteType);

private: //New fuctions

    /**
     * Check is used apdu phone application selection apdu and leaves
     * with security leave if so.
     *
     * @since 3.0
     */
    void CheckPhoneAppSelectionL() const;

    /**
     * Checks is gived file id illegal and leaves with security leave if so.
     * @param aFileId File id
     *
     * @since 3.0
     */
    void CheckIllegalFileIdL(const TDesC8& aFileId) const;

    /**
     * Checks does apdu lead to file selection
     * @return ETrue if proper file selection apdu. Otherwise EFalse.
     *
     * @since 3.0
     */
    TBool IsProperFileSelectionApdu(TInt aDataLength) const;

    /**
     * Checks is there proper path and does it contain illegal file ids.
     * @param aData Data field part of the apdu
     *
     * @since 3.0
     */
    void CheckPathL(const TDesC8& aData) const;

    /**
     * Checks is there proper file id and is it illegal.
     * @param aData Data field part of the apdu
     *
     * @since 3.0
     */
    void CheckFileIdL(const TDesC8& aData) const;

protected: // Data

    CSTSApdu* iApduData; //owned

private: // Data

    TPtrC8 iHeaderPtr;
};

} // namespace satsa
} // namespace java
#endif // CSTSCMDAPDU_H
// End of File
