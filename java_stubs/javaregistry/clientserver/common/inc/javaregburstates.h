/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  javaregburstates definition
*
*/


#ifndef JAVAREGBURSTATES_H
#define JAVAREGBURSTATES_H

namespace Java
{
namespace Manager
{
namespace Registry
{

// Backup states
enum TBackupState
{
    ENoBackup,
    EBackuping,
    ERestoring
};

}//namespace Registry
}//namespace Manager
}//namespace Java

#endif // JAVAREGBURSTATES_H
