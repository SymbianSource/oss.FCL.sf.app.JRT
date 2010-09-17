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
* Description: Avkon CoreUI JNI methods.
*
*/

#include "com_nokia_mj_impl_coreuiqt_CoreUiImpl.h"

#ifdef __SYMBIAN32__
#include <tstasksettings.h>
#endif // __SYMBIAN32__

#include "logger.h"

/**
 * Hide or show application at TaskManager.
 */
JNIEXPORT void JNICALL Java_com_nokia_mj_impl_coreuiqt_CoreUiImpl__1hideApplication
(JNIEnv* /*env*/, jobject /*peer*/, jboolean hide)
{
    JELOG2(EJavaUI);
#ifdef __SYMBIAN32__
    TsTaskSettings taskSettings;
    (void)taskSettings.setVisibility(hide);
#endif // __SYMBIAN32__
}
