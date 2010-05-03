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
* Description:  StorageServerPlugin
*
*/

#include "logger.h"
#include "coreinterface.h"
#include "pmcinterface.h"
#include "storageserverplugin.h"
#include "javastorage.h"
#include "javastorageexception.h"

extern "C" java::captain::ExtensionPluginInterface* getExtensionPlugin()
{
    return new java::captain::StorageServerPlugin();
}

using namespace java::comms;
using namespace java::storage;
using namespace java::util;
using namespace std;

namespace java
{
namespace captain
{

StorageServerPlugin::StorageServerPlugin() : mCore(0), mJavaStoragePid(0)
{
    JELOG2(EJavaStorage);
}

StorageServerPlugin::~StorageServerPlugin()
{
    JELOG2(EJavaStorage);
    if (mCore != 0)
    {
        mCore = 0;
    }
}

void StorageServerPlugin::startPlugin(CoreInterface* core)
{
    JELOG2(EJavaStorage);
    mCore = core;

    cmdLine_t cmdLine;
    cmdLine.push_back("storageserver");
    mJavaStoragePid = mCore->getPmc()->launch(cmdLine, 0);

    if (mJavaStoragePid > 0)
    {
        sleep(1);

        JavaStorage* js = NULL;

        try
        {
            // Check that javastorage has started up completely by trying to create a client instance
            //printf("StorageServerPlugin, before creating JavaStorage instance.\n");

            for (int i = 0; i < 4; i++)
            {
                try
                {
                    js = JavaStorage::createInstance();
                    break;
                }
                catch (JavaStorageException)
                {
                    // JavaStorage client creation failed so continue to try
                    //printf("StorageServerPlugin, JavaStorageException in creating JavaStorage client.\n");
                    sleep(1);
                }
            }

            if (js)
            {
                //printf("StorageServerPlugin, created JavaStorage instance.\n");
                js->open("JavaStorage");
                js->close();
                //printf("StorageServerPlugin, after open/close to JavaStorage.\n");
                delete js;
            }
        }
        catch (JavaStorageException)
        {
            //printf("StorageServerPlugin, JavaStorageException.\n");
            delete js;
        }
    }

}

void StorageServerPlugin::stopPlugin()
{
    if (mJavaStoragePid > 0)
    {
        mCore->getPmc()->terminate(mJavaStoragePid);
    }
}

void StorageServerPlugin::pmcTerminated(const int& pid, const int& /*exitCode*/)
{
    if (pid == mJavaStoragePid)
    {
        mJavaStoragePid = 0;
        // restart?
    }
}

} // namespace captain
} // namespace java

