/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32std.h>
#include <j2me/jdebug.h>
#include "javax_microedition_lcdui_UiEventServerFactory.h"
#include "coreuiavkonlcdui.h"


JNIEXPORT jint JNICALL
Java_javax_microedition_lcdui_UiEventServerFactory__1createUiServer(
    JNIEnv* /*aJni*/, jobject /*aServer*/, jint aUid)
{
    TUid uid = TUid::Uid(aUid);
    java::ui::CoreUiAvkonLcdui& avkonLcdui = java::ui::CoreUiAvkonLcdui::getInstance();

    java::ui::CoreUiParams coreUiParams;

    // LCDUI is always started to background.
    coreUiParams.setBackgroundStart(true);
    avkonLcdui.ensureInitialized(uid, &coreUiParams);
    return avkonLcdui.getLcduiSupport().getEventServerHandle();
}
