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
#include <e32base.h>
#include <vector>
#include "logger.h"
#include "storagehandler.h"

using namespace java::tools::usersettingsconfigurator;

static void DoL()
{
    // initializations
    std::auto_ptr<StorageHandler> storage(StorageHandler::NewL());
    std::vector<MidletSuiteSecuritySettings*> midletSuitesSecuritySettings;
    std::vector<MidletSuiteCustomAttributes*> midletSuitesCustomAttributes;

    // read the settings from the legacy java DB
    storage->readMidletSuitesPropertiesL(midletSuitesSecuritySettings, midletSuitesCustomAttributes);

    // write the settings into the OMJ storage
    storage->writeMidletSuitePropertiesL(midletSuitesSecuritySettings, midletSuitesCustomAttributes);

    storage->convertAPNSettingsL();

    // Cleanup
    for (int i=0; i<midletSuitesSecuritySettings.size(); i++)
    {
        for (int j=0; j<midletSuitesSecuritySettings[i]->iSecuritySettings.size(); j++)
        {
            delete midletSuitesSecuritySettings[i]->iSecuritySettings[j];
        }
        delete midletSuitesSecuritySettings[i];
    }
    for (int i=0; i<midletSuitesCustomAttributes.size(); i++)
    {
        for (int j=0; j<midletSuitesCustomAttributes[i]->iCustomAttributes.size(); j++)
        {
            delete midletSuitesCustomAttributes[i]->iCustomAttributes[j];
        }
        delete midletSuitesCustomAttributes[i];
    }
}

TInt E32Main()
{
    //__UHEAP_MARK;
    CTrapCleanup* cleanupStack = CTrapCleanup::New();
    TRAPD(error, DoL());
    if (KErrNone != error)
    {
        ELOG1(EJavaConverters,
              "Usersettingsconfigurator DoL leved with error: %d ", error);
    }
    delete cleanupStack;
    //__UHEAP_MARKEND;
    return error;
}
