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
* Description:  Backup Plugin interface inline methods
*
*/


#include "logger.h"

using namespace java::backup;

inline CBackupPlugin* CBackupPlugin::NewL(const TDesC8& aMatchString)
{

    TEComResolverParams params;
    params.SetDataType(aMatchString);
    ELOG(EBackup, "Inside Backupplugin");

    return REINTERPRET_CAST(CBackupPlugin*,
                            REComSession::CreateImplementationL(
                                KBackupEcomIfTUid,
                                _FOFF(CBackupPlugin,
                                      iDtor_ID_Key), params));

}

inline CBackupPlugin::~CBackupPlugin()
{
    REComSession::DestroyedImplementation(iDtor_ID_Key);
}

inline void CBackupPlugin::ListAllImplementationsL(RImplInfoPtrArray& aImplInfoArray)
{
    REComSession::ListImplementationsL(KBackupEcomIfTUid, aImplInfoArray);
}
