/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  midp2backupdataids definition
*
*/

#include <e32base.h>

#ifndef MIDP2BACKUPDATAIDS_H
#define MIDP2BACKUPDATAIDS_H

namespace java
{
namespace backup
{
/**
 * State of streaming during restore
 */
enum TRestoreState
{
    EVersion = 0,
    EStorage,
    EAppArc,
    EInIcon,
    EInSize,
    EInstall,
    EScr
};

} //namespace backup
} //namespace java


#endif // MIDP2BACKUPDATAIDS_H
