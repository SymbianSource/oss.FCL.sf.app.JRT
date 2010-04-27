/*
* Copyright (c) 2008 - 2009 Nokia Corporation and/or its subsidiary(-ies).
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
*/

#include <f32file.h>
#include <centralrepository.h>
#include <SWInstallerInternalCRKeys.h>
#include "comms.h"
#include "commsclientendpoint.h"
#include "commsmessage.h"
#include "securitycommsmessagedefs.h"

static const TUid KCRUidJavaSecurity = { 0x2001B289 };
static const TUint32 KPolicy = 0x03;
static const TUint32 KWarningsMode = 0x04;

using namespace java::comms;

static void DoL()
{
    RFile policyConfigFile;
    RFile certsFile;
    RFs fsSession;
    User::LeaveIfError(fsSession.Connect());
    TInt err = policyConfigFile.Open(fsSession,_L("c:\\private\\102033E6\\security\\tmp\\old_policy.txt"), EFileStreamText | EFileRead);
    TBuf8<256> oldPolicy;
    TBuf8<256> newPolicy;
    TBuf8<256> oldWarningsMode;
    TBuf8<256> newWarningsMode;
    bool updateCerts = false;
    bool removeCerts = false;
    if (err == KErrNone)
    {
        policyConfigFile.Read(oldPolicy);
        policyConfigFile.Close();
    }
    else
    {
        err = policyConfigFile.Open(fsSession,_L("c:\\private\\102033E6\\security\\tmp\\new_policy.txt"), EFileStreamText | EFileRead);
        if (err == KErrNone)
        {
            policyConfigFile.Read(newPolicy);
            policyConfigFile.Close();
        }
    }
    err = policyConfigFile.Open(fsSession,_L("c:\\private\\102033E6\\security\\tmp\\old_warnings_mode.txt"), EFileStreamText | EFileRead);
    if (err == KErrNone)
    {
        policyConfigFile.Read(oldWarningsMode);
        policyConfigFile.Close();
    }
    else
    {
        err = policyConfigFile.Open(fsSession,_L("c:\\private\\102033E6\\security\\tmp\\new_warnings_mode.txt"), EFileStreamText | EFileRead);
        if (err == KErrNone)
        {
            policyConfigFile.Read(newWarningsMode);
            policyConfigFile.Close();
        }
    }
    err = certsFile.Open(fsSession,_L("c:\\private\\102033E6\\security\\tmp\\update_certs"), EFileStreamText | EFileRead);
    if (err == KErrNone)
    {
        updateCerts = true;
        certsFile.Close();
    }
    err = certsFile.Open(fsSession,_L("c:\\private\\102033E6\\security\\tmp\\remove_certs"), EFileStreamText | EFileRead);
    if (err == KErrNone)
    {
        removeCerts = true;
        certsFile.Close();
    }
    if (oldPolicy.Length() > 0)
    {
        // restore the old policy
        CRepository* repository = CRepository::NewLC(KCRUidJavaSecurity);
        HBufC* tmp = HBufC::NewLC(oldPolicy.Length());
        TPtr tmpPtr(tmp->Des());
        tmpPtr.Copy(oldPolicy);
        repository->Set(KPolicy, tmpPtr);
        CleanupStack::PopAndDestroy(tmp);
        CleanupStack::PopAndDestroy(repository);
        // remove the  directory
        CFileMan* fileMan = CFileMan::NewL(fsSession);
        fileMan->RmDir(_L("c:\\private\\102033E6\\security\\tmp"));
        delete fileMan;
    }
    else if (newPolicy.Length() > 0)
    {
        // save the old policy into old_policy.txt
        CRepository* repository = CRepository::NewLC(KCRUidJavaSecurity);
        HBufC* buf = HBufC::NewLC(NCentralRepositoryConstants::KMaxUnicodeStringLength);
        TPtr policy(buf->Des());
        repository->Get(KPolicy, policy);
        TInt err = policyConfigFile.Replace(fsSession,_L("c:\\private\\102033E6\\security\\tmp\\old_policy.txt"), EFileStreamText | EFileWrite);
        if (err == KErrNone)
        {
            HBufC8* tmp8 = HBufC8::NewLC(policy.Length());
            TPtr8 tmpPtr8(tmp8->Des());
            tmpPtr8.Copy(policy);
            policyConfigFile.Write(tmpPtr8);
            CleanupStack::PopAndDestroy(tmp8);
            policyConfigFile.Close();
            // write the new policy
            HBufC* tmp = HBufC::NewLC(newPolicy.Length());
            TPtr tmpPtr(tmp->Des());
            tmpPtr.Copy(newPolicy);
            repository->Set(KPolicy, tmpPtr);
            CleanupStack::PopAndDestroy(tmp);
        }
        CleanupStack::PopAndDestroy(buf);
        CleanupStack::PopAndDestroy(repository);
    }
    if (oldWarningsMode.Length() > 0)
    {
        // restore the old value
        CRepository* repository = CRepository::NewLC(KCRUidJavaSecurity);
        if (oldWarningsMode == _L8("1"))
        {
            repository->Set(KWarningsMode, 1);
        }
        else
        {
            repository->Set(KWarningsMode, 2);
        }
        CleanupStack::PopAndDestroy(repository);
        // remove the directory
        CFileMan* fileMan = CFileMan::NewL(fsSession);
        fileMan->RmDir(_L("c:\\private\\102033E6\\security\\tmp"));
        delete fileMan;
    }
    else if (newWarningsMode.Length() > 0)
    {
        // save the old value
        CRepository* repository = CRepository::NewLC(KCRUidJavaSecurity);
        TInt tmp;
        repository->Get(KWarningsMode, tmp);
        TInt err = policyConfigFile.Replace(fsSession,_L("c:\\private\\102033E6\\security\\tmp\\old_warnings_mode.txt"), EFileStreamText | EFileWrite);
        if (err == KErrNone)
        {
            if (tmp == 1)
            {
                policyConfigFile.Write(_L8("1"));
            }
            else
            {
                policyConfigFile.Write(_L8("2"));
            }
            policyConfigFile.Close();
            // write the new value
            if (newWarningsMode == _L8("1"))
            {
                repository->Set(KWarningsMode, 1);
            }
            else
            {
                repository->Set(KWarningsMode, 2);
            }
        }
        CleanupStack::PopAndDestroy(repository);
    }
    if (updateCerts || removeCerts)
    {
        if (removeCerts)
        {
            CFileMan* fileMan = CFileMan::NewL(fsSession);
            fileMan->RmDir(_L("c:\\private\\102033E6\\security\\tmp"));
            delete fileMan;
        }
        else
        {
            TInt err = certsFile.Replace(fsSession,_L("c:\\private\\102033E6\\security\\tmp\\remove_certs"), EFileStreamText | EFileWrite);
            certsFile.Close();
        }

        // send the COMMS message to JavaCertStore to do the refresh
        CommsClientEndpoint cli_conn;
        if (cli_conn.connect(IPC_ADDRESS_JAVA_CAPTAIN_C) == 0)
        {
            CommsMessage s_msg;
            CommsMessage r_msg;
            s_msg.setModuleId(PLUGIN_ID_JAVA_CERT_STORE_EXTENSION_C);
            s_msg.setMessageId(JAVA_CERT_STORE_MSG_ID_REQUEST);
            s_msg << JAVA_CERT_STORE_OPERATION_REFRESH_CERTS;
            cli_conn.send(s_msg);
        }
    }
    fsSession.Close();
}

TInt E32Main()
{
    //__UHEAP_MARK;
    CTrapCleanup* cleanupStack = CTrapCleanup::New();
    TRAPD(error, DoL());
    delete cleanupStack;
    //__UHEAP_MARKEND;
    return error;
}
// eof
