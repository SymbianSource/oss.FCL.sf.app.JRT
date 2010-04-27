/*
* Copyright (c) 2002-2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CInstalledAppsRegistryEntry class's inline methods are here.
*
*/


// -----------------------------------------------------------------------------
// CInstalledAppsRegistryEntry::NewL
// -----------------------------------------------------------------------------
//
inline CInstalledAppsRegistryEntry* CInstalledAppsRegistryEntry::NewL()
{
    CInstalledAppsRegistryEntry* self = NewLC();
    CleanupStack::Pop(self);
    return self;
}

// -----------------------------------------------------------------------------
// CInstalledAppsRegistryEntry::NewL
// -----------------------------------------------------------------------------
//
inline CInstalledAppsRegistryEntry* CInstalledAppsRegistryEntry::NewL
(const CInstalledAppsRegistryEntry& aOther)
{
    CInstalledAppsRegistryEntry* self = NewLC(aOther);
    CleanupStack::Pop(self);
    return self;
}

// -----------------------------------------------------------------------------
// CInstalledAppsRegistryEntry::NewL
// -----------------------------------------------------------------------------
//
inline CInstalledAppsRegistryEntry* CInstalledAppsRegistryEntry::NewL
(RReadStream& aStream)
{
    CInstalledAppsRegistryEntry* self = NewLC(aStream);
    CleanupStack::Pop(self);
    return self;
}
