/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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


#include "CMIDToolkit.h"
#include "javax_microedition_lcdui_AlertType.h"


LOCAL_C TBool PlaySound(MMIDUtils* aUtils,TInt aAlertType)
{
    return aUtils->PlaySound(aAlertType);
}

JNIEXPORT jboolean JNICALL Java_javax_microedition_lcdui_AlertType__1playSound
(JNIEnv*,jobject,jint aToolkit,jint aAlertType)
{
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    return (jboolean)toolkit->Execute(&PlaySound,toolkit->Utils(),(TInt)aAlertType);
}
