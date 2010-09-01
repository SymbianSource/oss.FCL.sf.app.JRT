/*
* Copyright (c) 2003-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ?Description
*
*/


#include <e32def.h>
#include <s60commonutils.h>

#include "CMIDAppUi.h"
// using GetResourceFileName function

#include "coreuiavkonlcdui.h"

_LIT(KLcduiResourceFileName, "lcdui.rsc");

void doCreateAppL()
{
    CMIDAppUi* appui = new(ELeave) CMIDAppUi();
    appui->ConstructL();
    CCoeEnv* env = CCoeEnv::Static();

    TFileName fileName;
    fileName.Append(KLcduiResourceFileName);
    fileName = java::util::S60CommonUtils::ResourceLanguageFileNameL(fileName);

    int offset = 0;
    TRAP_IGNORE(offset = env->AddResourceFileL(fileName));
    // The CoreUi owns now the appui.
    java::ui::CoreUiAvkonLcdui::getInstance().getJavaUiAppUi()->setLcduiChild(appui, offset);
}

EXPORT_C int createApp()
{
    TRAPD(err, doCreateAppL());
    return err;
}
