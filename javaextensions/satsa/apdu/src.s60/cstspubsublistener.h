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


#ifndef CSTSPUBSUBLISTENER_H
#define CSTSPUBSUBLISTENER_H

//  INCLUDES
#include <e32base.h>
#include <e32property.h>

namespace java
{
namespace satsa
{

// FORWARD DECLARATIONS
class MSTSCloseObserver;

// CLASS DECLARATION
/**
 *  Abstract listener class for listening phone's events.
 *
 *  @since 3.0
 */
NONSHARABLE_CLASS(CSTSPubSubListener): public CActive
{

public: // Constructors and destructor

    /**
     * Destructor.
     */
    virtual ~CSTSPubSubListener();

public: // New functions

    /**
     * Starts listening
     * @since 3.0
     */
    void Start();

public: // Functions from base classes

    /**
     * From CActive Cancel request
     * @return void
     */
    void DoCancel();

protected: //New fuctions

    /**
     * C++ default constructor.
     */
    CSTSPubSubListener();

    /**
     * Derived class implements here actual starting implementation
     * @since 3.0
     */
    virtual void DoStart() = 0;

private: //New fuctions

protected: // Data

    MSTSCloseObserver* iObserver; //not owned, derived class sets this

    // status property, derived class sets this
    RProperty iProperty;

};

} // namespace satsa
} // namespace java
#endif // CSTSPUBSUBLISTENER_H
// End of File
