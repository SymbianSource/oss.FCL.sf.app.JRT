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


#ifndef CSTSSLOTMANAGER_H
#define CSTSSLOTMANAGER_H

//  INCLUDES
#include <e32base.h>
#include <string>

namespace java
{
namespace satsa
{

// FORWARD DECLARATIONS
class CSTSCardOperationsFacade;
class CSTSSlotInfo;

// CLASS DECLARATION

/**
 *  Handels the slot(=reader) related operations of connection.
 *  Used for gettings slots from the card and checking is some
 *  slot existing one.
 *
 *  @since 3.0
 */
NONSHARABLE_CLASS(CSTSSlotManager): public CBase
{
public: // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CSTSSlotManager* NewL(CSTSCardOperationsFacade* aCOFacade);

    /**
     * Destructor.
     */
    virtual ~CSTSSlotManager();

public: // New functions

    /**
     * Gets all slots of the phone
     * @since 3.0
     * @param aSlotString Slot string on return value
     */
    std::wstring GetSlotsL();

    /**
     * Checks is the gived slot existing one
     * @since 3.0
     * @param aSlot Slot number whic will be checked
     * @return ETrue if slot is existing one, else returns EFalse.
     */
    TBool DoesExistL(TInt aSlot);

private:

    /**
     * C++ default constructor.
     */
    CSTSSlotManager(CSTSCardOperationsFacade* aCOFacade);

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

private: // Data

    // not owned
    CSTSCardOperationsFacade* iCOFacade;
    // array of slot information, owned
    CArrayPtr<CSTSSlotInfo>* iSlotArray;
    // slot string, owned
    HBufC* iSlotString;
    // is slot information got or not
    TBool iGetPerformed;

};

} // namespace satsa
} // namespace java
#endif // CSTSSLOTMANAGER_H
// End of File
