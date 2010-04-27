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


#ifndef CSTSACCESSCONTROL_H
#define CSTSACCESSCONTROL_H

//  INCLUDES
#include <e32base.h>
#include "stsapduconstants.h"   //for TPinApduType

namespace java
{
namespace satsa
{

class CSTSPKCS15Reader;
class CSTSAce;
class CSTSAuthType;
class CSTSMidletInfo;

// CLASS DECLARATION

/**
 *  Handels the security and access logic of SATSA-APDU.
 *  Uses ACF information and PIN related information found in the card's
 *  PKCS#15 structure and checks security and access related information
 *  when asked.
 */
NONSHARABLE_CLASS(CSTSAccessControl): public CBase
{

public: // Constructors and destructor


    static CSTSAccessControl* NewL(CSTSMidletInfo* aMidletInfo);

    virtual ~CSTSAccessControl();

public:

    // Reads Access Control related files from the smart card.
    void ReadFilesL();

    /**
     * Checks is current J2ME application domain allowed to use
     * selected application in the card.
     * @return ETrue if domain is allowed to use application, else EFalse
     */
    TBool IsAllowedDomain();

    /**
     * Checks is the gived apdu allowed to be sent to card application.
     * @param aMidletApduHeader APDU header part, which will be checked
     * @return ETrue if apdu is allowed, EFalse if not
     */
    TBool IsAllowedApduL(const TDesC8& aMidletApduHeader);

    /**
     * Chekcs is the gived PIN method supported in the card application.
     * @return ETrue if method is allowed to use, EFalse if not
     */
    TBool IsAllowedPinMethod();

    /**
     * Gets recommended PIN header for the gived PIN method if there is any
     * @param aPinID Id of the PIN
     * @param aPinApduType Which PIN header is tried to get
     * @return Pin header if found or KNullDesC8() if not found
     */
    const TDesC8& GetPinHeaderL(TInt aPinID, TPinApduType aPinApduType);

    /**
     * Gets PIN attributes which can be used for creating proper PIN UI
     * @param aPinID Id of the PIN
     * @return Reference to CSTSAuthType object, contains PIN information.
     */
    const CSTSAuthType& GetPinAttributesL(TInt aPinID);

    /**
     * Gets token label which can be used for creating proper PIN UI
     * @return Reference to descriptor, contains token label.
     */
    const TDesC& TokenLabelL();

    // Sets card application AID
    void SetAIDL(const TDesC8& aAID);

    // Closes connection to PKCS15 application if it is still open
    void Close();

    /**
     * Checks change disabled, unblock disabled and disable allowed PIN
     * flags depending on which PIN apdu type is used and leaves if
     * flag informs that operation is disabled or not allowed to do.
     * @param aPinApduType Which PIN header is tried to get
     */
    void CheckPinFlagsL(TPinApduType aPinApduType);

private:

    CSTSAccessControl(CSTSMidletInfo* aMidletInfo);

    void ConstructL();

    /**
     * Finds correct authType of gived PinId. Saves found authType
     * to member variable. Leaves, if correct authType will not found.
     * @return Reference to descriptor, contains token label.
     */
    void FindAuthTypeL(TInt aPinID);

    /**
     * Checks is gived ace valid for current Domain, RootId or EndEntityId.
     * @param aAce Ace entry which will be used in comparison.
     * @return True if ace was proper, otherwice false
     */
    TBool IsProperAce(const CSTSAce* aAce) const;

private:

    // PKCS15 structure reader, owned
    CSTSPKCS15Reader* iPKCS15Reader;
    CSTSMidletInfo* iMidletInfo; //not owned

    // information of principals, permissions and userAuthentications
    CArrayPtrFlat< CSTSAce>* iAces;

    // contains PIN related information, owned
    CSTSAuthType* iAuthType;
    CArrayPtrFlat< CSTSAuthType>* iAuthTypes;

    // buffer for pin header, owned
    HBufC8* iApduHeader;

};

} // namespace satsa
} // namespace java
#endif // CSTSACCESSCONTROL_H
// End of File
