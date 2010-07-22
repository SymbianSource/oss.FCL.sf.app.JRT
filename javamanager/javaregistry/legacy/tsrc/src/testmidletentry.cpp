/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <stdio.h>
#include <string>

#include <CppUTest/TestHarness.h>

#include "javacommonutils.h"
#include "javaoslayer.h"
#include "javaprocessconstants.h"
#include "javastorage.h"
#include "javastorageexception.h"
#include "javastoragenames.h"
#include "javasymbianoslayer.h"
#include "javauid.h"
#include "logger.h"
#include "mjavaregistry.h"
#include "storagetestutils.h"

using namespace std;
using namespace java::storage;
using namespace java::util;

/**
 * NOTE: Some of MJavaRegistryMIDletEntry interface methods are already tested
 * at testlegacyregistry.cpp test case.
 */
TEST_GROUP(TestMIDletEntry)
{
    JavaStorage* js;
    JavaStorageTestUtils* jtu;

    TEST_SETUP()
    {
        js = JavaStorage::createInstance();
        jtu = new JavaStorageTestUtils();
    }
    TEST_TEARDOWN()
    {
        try
        {
            js->rollbackTransaction();
            js->close();
            delete js;
            js = NULL;
        }
        catch (...)
        {
            // No can do
        }

        delete js;
        js = 0;
        delete jtu;
        jtu = 0;
    }
};

/**
 * Test CJavaRegistryMIDletEntry::SizeL() method.
 * 1. Test install MIDlet and check it size.
 */
TEST(TestMIDletEntry, TestSizeL)
{
    LOG(EJavaStorage, EInfo, "+TestSizeL");

    MJavaRegistry* registry = MJavaRegistry::CreateL();
    CHECK(registry != NULL);

    RProcess rJavaInstaller;
    TFileName fileName;
    TBuf<400> commandLine;

    // Build command line used to pass all necessary info to Java Installer
    std::auto_ptr<HBufC> installerStarterDll(
        stringToDes(java::runtime::JAVA_INSTALLER_STARTER_DLL));
    commandLine = installerStarterDll->Des();
    commandLine.Append(_L(" install -jar=c:\\HelloWorld1.jar"));
    commandLine.Append(_L(" -silent -upgrade=yes -ocsp=no"));
    commandLine.Append(_L(" -download=no -upgrade_data=yes"));

    // start JavaInstaller
    std::auto_ptr<HBufC> installerProcess(
        stringToDes(java::runtime::JAVA_PROCESS));
    TInt err = rJavaInstaller.Create(installerProcess->Des(), commandLine);

    TRequestStatus status;

    if (KErrNone == err)
    {
        LOG(EJavaStorage, EInfo, "Calling Logon");
        // This call will wait until Java Installer exits (or panics)
        rJavaInstaller.Logon(status);

        LOG(EJavaStorage, EInfo, "Calling Resume");
        rJavaInstaller.Resume();
    }
    else
    {
        ELOG1(EJavaStorage, "Cannot start Installer, error %d", err);
        User::Leave(err);
    }

    LOG(EJavaStorage, EInfo, "Calling RProcess::Close");

    // now wait until Java Installer exits
    User::WaitForRequest(status);

    // free resources before returning
    rJavaInstaller.Close();

    RArray<TUid> uids;
    registry->InstalledMIDletUidsL(uids);
    CHECK(uids.Count() == 1);

    MJavaRegistryMIDletEntry* midletEntry
    = registry->MIDletEntryL(uids[0]);
    CHECK(midletEntry != NULL);

    CHECK(midletEntry->SizeL() > 4000);

    midletEntry->Release();

    // UNINSTALL
    // Build command line used to pass all necessary info to Java Installer
    commandLine = installerStarterDll->Des();
    commandLine.Append(_L(" uninstall -uid="));
    TUidName uidName = (uids[0]).Name();
    commandLine.Append(uidName);
    uids.Reset();
    uids.Close();
    commandLine.Append(_L(" -silent"));

    // start JavaInstaller
    err = rJavaInstaller.Create(installerProcess->Des(), commandLine);

    status = KRequestPending;

    if (KErrNone == err)
    {
        LOG(EJavaStorage, EInfo, "Calling Logon");
        // This call will wait until Java Installer exits (or panics)
        rJavaInstaller.Logon(status);

        LOG(EJavaStorage, EInfo, "Calling Resume");
        rJavaInstaller.Resume();
    }
    else
    {
        ELOG1(EJavaStorage, "Cannot start Installer, error %d", err);
        User::Leave(err);
    }

    // now wait until Java Installer exits
    User::WaitForRequest(status);

    LOG(EJavaStorage, EInfo, "Calling RProcess::Close");
    // free resources before returning
    rJavaInstaller.Close();

    registry->Release();

    LOG(EJavaStorage, EInfo, "-TestSizeL");
}
