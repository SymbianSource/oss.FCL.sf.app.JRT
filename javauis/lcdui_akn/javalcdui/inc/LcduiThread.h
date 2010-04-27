/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef LCDUITHREAD_H
#define LCDUITHREAD_H

#include <e32std.h>
#include <apparc.h>
#include <Lcdglue.h>
#include <JesServer.h>

typedef int (*TLcduiPluginFunction)();

NONSHARABLE_CLASS(RLcdui) : public MLcdui, public MJesShutdown
{
public:
    RLcdui();

    IMPORT_C static RLcdui* Get();

    void OpenL(RServer2& serverHandle);
    void Close();
    IMPORT_C TInt CreateAppUi();

public:
    virtual void AttachL(MLcduiPlugin* aPlugin);
    virtual void Detach();
    virtual MLcduiPlugin* Plugin();

public:
    // MJesShutdown
    virtual void Shutdown();


private:
    RLibrary      iLibrary;
    MLcduiPlugin* iPlugin;
    CJesServer*   iServer;
};

#endif // LCDUITHREAD_H
