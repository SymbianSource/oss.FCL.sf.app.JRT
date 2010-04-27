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
#include "MIDUtils.h"
#include "javax_microedition_lcdui_Spacer.h"

LOCAL_C void CreateL(CMIDToolkit* aToolkit,TInt* aHandle,TSize aSize)
{
    MMIDSpacer* spacer = aToolkit->ComponentFactory()->CreateSpacerL(aSize);
    CleanupDisposePushL(spacer);
    *aHandle = aToolkit->RegisterComponentL(spacer, NULL);
    CleanupPopComponent(spacer);
}
JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_Spacer__1create
(JNIEnv*,jobject,jint aToolkit,jint aWidth,jint aHeight)
{
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    TInt h = 0;
    TInt err = toolkit->ExecuteTrap(&CreateL,toolkit,&h,TSize((TInt)aWidth,(TInt)aHeight));
    return err == KErrNone ? h : err;
}

LOCAL_C void SetMinimumSizeL(MMIDSpacer* aSpacer,TInt aWidth,TInt aHeight)
{
    TSize size(aWidth,aHeight);
    aSpacer->SetMinimumSizeL(size);
}
JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_Spacer__1setMinimumSize
(JNIEnv*,jobject,jint aSpacer,jint aToolkit,jint aWidth,jint aHeight)
{
    MMIDSpacer* spacer = MIDUnhandObject<MMIDSpacer>(aSpacer);
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    return toolkit->ExecuteTrap(&SetMinimumSizeL,spacer,(TInt)aWidth,(TInt)aHeight);
}


