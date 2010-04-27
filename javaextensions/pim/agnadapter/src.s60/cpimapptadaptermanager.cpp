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
* Description:  Provides static information about appointment lists and creates
 *                appointment list adapters.
 *
*/


// INCLUDE FILES
#include "cpimapptadaptermanager.h"
#include "cpimeventlistadapter.h"
#include "cpimagnapptadapter.h"
#include "logger.h"

// -----------------------------------------------------------------------------
// CPIMApptAdapterManager::CPIMApptAdapterManager
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPIMApptAdapterManager::CPIMApptAdapterManager()
{
    JELOG2(EPim);
}

// -----------------------------------------------------------------------------
// CPIMApptAdapterManager::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPIMApptAdapterManager::ConstructL(const TDesC& aListName)
{
    JELOG2(EPim);
    CPIMEventAdapterManager::ConstructL(aListName);

    iSupportedFields
    = new(ELeave) CArrayFixFlat<TPIMField> (KPIMSupportedApptFieldsCount);

    iSupportedFields->AppendL(KPIMSupportedApptFields,
                              KPIMSupportedApptFieldsCount);
}

// -----------------------------------------------------------------------------
// CPIMApptAdapterManager::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPIMApptAdapterManager* CPIMApptAdapterManager::NewL(const TDesC& aListName)
{
    JELOG2(EPim);
    CPIMApptAdapterManager* self = new(ELeave) CPIMApptAdapterManager();

    CleanupStack::PushL(self);
    self->ConstructL(aListName);
    CleanupStack::Pop(self);

    return self;
}

// Destructor
CPIMApptAdapterManager::~CPIMApptAdapterManager()
{
    JELOG2(EPim);
    delete iSupportedFields;
}

// -----------------------------------------------------------------------------
// CPIMApptAdapterManager::GetAdapterManager
// Returns: A MPIMAdapterManager representation of this object.
// -----------------------------------------------------------------------------
//
MPIMAdapterManager* CPIMApptAdapterManager::GetAdapterManager()
{
    JELOG2(EPim);
    return this;
}

//  End of File
