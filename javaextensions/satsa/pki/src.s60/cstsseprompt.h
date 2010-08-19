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


#ifndef CSTSSEPROMPT_H
#define CSTSSEPROMPT_H

// INCLUDE
#include <e32base.h>

// FORWARD DECLARATIONS
#ifndef RD_JAVA_S60_RELEASE_10_1_ONWARDS

class CAOSynchronizer;
class CAknGlobalNote;

#endif
// CLASS DECLARATION

/**
* Shows a security element prompt. Security element prompt informs
* the user to insert a security element. The actual text shown in the
* prompt is given by the midlet.
*/
NONSHARABLE_CLASS(CSTSSEPrompt): public CBase
{
public: // Constructors and destructor

    /**
        * Two-phased constructor.
        */
    static CSTSSEPrompt* NewLC();

    /**
        * Destructor.
        */
    ~CSTSSEPrompt();

public: // New functions

    /**
        * Shows the prompt with an OK button.
        */
    void DisplayPromptL(const TDesC& aPrompt);

private: // Constructors and destructor

    /**
        * C++ constructor
        */
    CSTSSEPrompt();

    /**
        * Second phase constructor
        */
    void ConstructL();


private: // data
    #ifndef RD_JAVA_S60_RELEASE_10_1_ONWARDS
    // displays the prompt asynchronously, owned
    CAknGlobalNote* iGlobalNote;
    // waits for the prompt to finish, owned
    CAOSynchronizer* iSynchronizer;
    
    #endif
    

};



#endif // CSTSSEPROMPT_H
