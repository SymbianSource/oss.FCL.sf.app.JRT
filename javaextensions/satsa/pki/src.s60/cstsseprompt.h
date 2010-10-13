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
class CAknGlobalNote;
class CAOSynchronizer;

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

    // displays the prompt asynchronously, owned
    CAknGlobalNote* iGlobalNote;
    // waits for the prompt to finish, owned
    CAOSynchronizer* iSynchronizer;

};



#endif // CSTSSEPROMPT_H
