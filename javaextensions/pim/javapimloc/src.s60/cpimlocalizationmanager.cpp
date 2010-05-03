/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Provides localized strings
*
*/


// CLASS HEADER
#include "cpimlocalizationmanager.h"

// INTERNAL INCLUDES
#include "cpimlocalizationdata.h"
#include "pimlocalizationids.h"
#include "pimlocalizationutils.h"
#include "pimpanics.h"

// EXTERNAL INCLUDES
#include <data_caging_path_literals.hrh>
#include <f32file.h>
#include <barsc.h>
#include <barsread.h>
#include <pimcm.rsg>
#include <pimevent.rsg>
#include <pimtodo.rsg>

#include <StringLoader.h>

// UNNAMED LOCAL NAMESPACE
namespace
{
// CONSTANTS
_LIT(KPIMCMFileName, "pimcm.rsc");
_LIT(KPIMEventFileName, "pimevent.rsc");
_LIT(KPIMToDoFileName, "pimtodo.rsc");
}

// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// CPIMLocalizationManager::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPIMLocalizationManager* CPIMLocalizationManager::NewL()
{
    CPIMLocalizationManager* self = new(ELeave) CPIMLocalizationManager;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

// destructor
CPIMLocalizationManager::~CPIMLocalizationManager()
{
    iRFs.Close();
}

// -----------------------------------------------------------------------------
// CPIMLocalizationManager::GetPIMLocalizationDataL
// Creates an instance of MPIMLocalizationData.
// Caller takes ownership of the returned object.
// Return:  An instance of MPIMLocalizationData.
// -----------------------------------------------------------------------------
//
MPIMLocalizationData* CPIMLocalizationManager::GetPIMLocalizationDataL(
    TPIMLocalizationDataID aType)  // Type of the list
{
    return GetPIMLocalizationDataL(aType, 0);
}

// -----------------------------------------------------------------------------
// CPIMLocalizationManager::GetPIMLocalizationDataL
// Creates an instance of MPIMLocalizationData.
// Caller takes ownership of the returned object.
// Return:  An instance of MPIMLocalizationData.
// -----------------------------------------------------------------------------
//
MPIMLocalizationData* CPIMLocalizationManager::GetPIMLocalizationDataL(
    TPIMLocalizationDataID aType, // Type of the list
    TPIMLocalizationDataID aSubType)  // Subtype of the (contact)list
{
    TFileName resourceFileName;
    TInt resourceId(0);
    switch (aType)
    {
    case KPIMLocalizationIdContacts:
    {
        resourceFileName.Append(KPIMCMFileName());
        resourceId = R_PIM_CNT_LISTS;
        break;
    }
    case KPIMLocalizationIdEvent:
    {
        resourceFileName.Append(KPIMEventFileName());
        resourceId = R_PIM_EVENT_LISTS;
        break;
    }
    case KPIMLocalizationIdToDo:
    {
        resourceFileName.Append(KPIMToDoFileName());
        resourceId = R_PIM_TODO_LISTS;
        break;
    }
    default:
    {
        // No other lists available
        User::Leave(KErrArgument);
    }
    }

    RResourceFile resourceFile;
    PIMLocalizationUtils::OpenResourceFileLC(iRFs, resourceFile, resourceFileName);
    // Create resource reader for the resource file
    HBufC8* buffer = resourceFile.AllocReadLC(resourceId);

    TResourceReader reader;
    reader.SetBuffer(buffer);

    CPIMLocalizationData* localization =
        CPIMLocalizationData::NewL(resourceFile,
                                   reader,
                                   aSubType);

    // buffer, resourceFile.Close()
    CleanupStack::PopAndDestroy(2, &resourceFile);
    return localization;
}


// -----------------------------------------------------------------------------
// CPIMLocalizationManager::CPIMLocalizationManager
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPIMLocalizationManager::CPIMLocalizationManager()
{
}

// -----------------------------------------------------------------------------
// CPIMLocalizationManager::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPIMLocalizationManager::ConstructL()
{
    User::LeaveIfError(iRFs.Connect());
}

// End of file
