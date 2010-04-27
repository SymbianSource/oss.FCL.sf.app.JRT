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


#ifndef CSTSPKCS15READER_H
#define CSTSPKCS15READER_H

//  INCLUDES
#include <e32base.h>
#include "cstsselectfile.h"

namespace java
{
namespace satsa
{

// FORWARD DECLARATIONS
class CSTSAce;
class CSTSAuthType;
class CSTSAcie;
class CSTSFileDataManager;

// CLASS DECLARATION

/**
 *  Handels the reading of PKCS15 structure from the card.
 *  Used for getting pin related information for PIN Ui's and
 *  access control related information like ACF reading.
 *
 *  @since 3.0
 */
NONSHARABLE_CLASS(CSTSPKCS15Reader): public CBase
{
public:
    enum TConnectionState
    {
        ENotOpened,
        EChannelOpened,
        EPCKS15Opened,
        EClosed
    };
public: // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CSTSPKCS15Reader* NewL();

    /**
     * Destructor.
     */
    virtual ~CSTSPKCS15Reader();

public: // New functions

    /**
     * Opens channel to smart card and selects PKCS15 application
     * @since 3.0
     */
    void OpenL();

    /**
     * Deselects PKCS15 application and closes used channel
     * @since 3.0
     */
    void Close();

    /**
     * Reads Access control file
     * @since 3.0
     * @param aAces Array of ACE objects where all information will be read
     */
    void ReadACFL(CArrayPtr< CSTSAce>* aAces);

    /**
     * Reads Pin related Authentication objects
     * @since 3.0
     * @param aAuthType AuthType object where all information will be read
     */
    void ReadAuthObjectsL(CArrayPtr< CSTSAuthType>* aAuthTypes,
                          const CArrayPtr< CSTSAce>& aAces);

    /**
     * Returns token label
     * @since 3.0
     * @return Reference to descriptor, contains token label.
     */
    const TDesC& TokenLabelL();

    /**
     * Sets card application AID
     * @since 3.0
     */
    void SetAIDL(const TDesC8& aAID);

private:

    /**
     * C++ default constructor.
     */
    CSTSPKCS15Reader();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

    /**
     * Reads ACIF file for finding path to Access Control Files (ACF).
     * Saves data to member array.
     */
    void ReadACIFL();

private: // Data

    // AID of the application, owned
    HBufC8* iAID;
    //state
    TConnectionState iState;

    // array for all found acies, owned
    CArrayPtrFlat< CSTSAcie>* iAcies;

    CSTSFileDataManager* iFileDataManager; // owned

};

} //namespace satsa
} //namespace java
#endif // CSTSPKCS15READER_H
// End of File
