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


#ifndef CSTSBTSAPLISTENER_H
#define CSTSBTSAPLISTENER_H

//  INCLUDES
#include "cstspubsublistener.h"

namespace java
{
namespace satsa
{

// FORWARD DECLARATIONS
class MSTSCloseObserver;

// CLASS DECLARATION
/**
 *  Listener type of class for listening Bluetooth remote SIM Access Profile
 *  events(BT SAP).
 *
 *  @since 3.0
 */
NONSHARABLE_CLASS(CSTSBTSapListener): public CSTSPubSubListener
{
public: // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CSTSBTSapListener* NewLC(MSTSCloseObserver* aObserver);

    static CSTSBTSapListener* NewL(MSTSCloseObserver* aObserver);

    /**
     * Destructor.
     */
    virtual ~CSTSBTSapListener();

public: // New functions

    /**
     * Returns information is BT SAP in use or not.
     * @since 3.0
     */
    TBool IsInUse();

protected: //New fuctions

    /**
     * C++ default constructor.
     */
    CSTSBTSapListener(MSTSCloseObserver* aObserver);

protected: // Functions from base classes

    void DoStart();

    /**
     * From CActive RunL
     * @return void
     */
    void RunL();

private: // Data

};

} // namespace satsa
} // namespace java
#endif // CSTSBTSAPLISTENER_H
// End of File
