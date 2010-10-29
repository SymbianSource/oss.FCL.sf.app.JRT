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



// INCLUDE FILES



#include "cstsseprompt.h"

#ifdef RD_JAVA_S60_RELEASE_10_1_ONWARDS

#include <hbdevicedialog.h>
#include <QtCore/qvariant.h>
const QString KMessageTextKey = "text";

#else 

#include <AknGlobalNote.h>
#include <avkon.rsg>
#include <caosynchronizer.h>

#endif 


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSTSSEPrompt::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSTSSEPrompt* CSTSSEPrompt::NewLC()
{
    CSTSSEPrompt* self = new(ELeave) CSTSSEPrompt;
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

// Destructor
CSTSSEPrompt::~CSTSSEPrompt()
{
#ifndef RD_JAVA_S60_RELEASE_10_1_ONWARDS
    delete iGlobalNote;
    delete iSynchronizer;
#endif
}

// -----------------------------------------------------------------------------
// CSTSSEPrompt::DisplayPromptL
// Displays the prompt
// -----------------------------------------------------------------------------
//
void CSTSSEPrompt::DisplayPromptL(const TDesC& aPrompt)
{
    #ifdef RD_JAVA_S60_RELEASE_10_1_ONWARDS
            HbDeviceDialog* dialog;
            dialog = new(ELeave) HbDeviceDialog();
            
            QVariantMap parameters;
            QString qString((QChar*)aPrompt.Ptr(),aPrompt.Length());
            parameters[QString(KMessageTextKey)] = qString;
            
            dialog->show("com.nokia.hb.devicemessagebox/1.0", parameters);
            
            delete dialog;
    #else
            
    iGlobalNote->ShowNoteL(
        iSynchronizer->iStatus,
        EAknGlobalConfirmationNote, 
        aPrompt);
    iSynchronizer->ExecuteL((TTimeIntervalMicroSeconds32) 0);
            
    #endif
}

// -----------------------------------------------------------------------------
// CSTSSEPrompt::CSTSSEPrompt
// c++ constructor, may not leave
// -----------------------------------------------------------------------------
//
CSTSSEPrompt::CSTSSEPrompt()
{

}

// -----------------------------------------------------------------------------
// CSTSSEPrompt::ConstructL
// Second phase constructor, may leave
// -----------------------------------------------------------------------------
void CSTSSEPrompt::ConstructL()
{
    #ifndef RD_JAVA_S60_RELEASE_10_1_ONWARDS
    iGlobalNote = CAknGlobalNote::NewL();
    iGlobalNote->SetSoftkeys(R_AVKON_SOFTKEYS_OK_EMPTY);
    iSynchronizer = CAOSynchronizer::NewL();
    #endif
}


