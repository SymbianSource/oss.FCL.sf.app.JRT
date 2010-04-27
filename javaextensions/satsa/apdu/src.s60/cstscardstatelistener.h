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


#ifndef CSTSCARDSTATELISTENER_H
#define CSTSCARDSTATELISTENER_H

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
 *  Listener type of class for listening states of smart card.
 *
 *  @since 3.0
 */
NONSHARABLE_CLASS(CSTSCardStateListener): public CSTSPubSubListener
{
public: // Constructors and destructor

    /*
     * Two-phased constructor.
     */
    static CSTSCardStateListener* NewLC(MSTSCloseObserver* aObserver);

    static CSTSCardStateListener* NewL(MSTSCloseObserver* aObserver);

    /**
     * Destructor.
     */
    virtual ~CSTSCardStateListener();

public: // New functions

    /**
     * Returns information is card available or not.
     * @since 3.0
     */
    TBool CardIsAvailable();

protected: //New fuctions

    /**
     * C++ default constructor.
     */
    CSTSCardStateListener(MSTSCloseObserver* aObserver);

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
#endif // CSTSCARDSTATELISTENER_H
// End of File
