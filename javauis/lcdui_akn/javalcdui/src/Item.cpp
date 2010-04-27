/*
* Copyright (c) 1999-2001 Nokia Corporation and/or its subsidiary(-ies).
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
#include "javax_microedition_lcdui_Item.h"

/*
 * Server-side methods
 */
LOCAL_C void SetLabelL(jint aItem,const TDesC* aLabel)
{
    MIDUnhand<MMIDItem>(aItem)->SetLabelL(*aLabel);
}

LOCAL_C void SetLayoutL(jint aItem,MMIDItem::TLayout aLayout)
{
    MIDUnhand<MMIDItem>(aItem)->SetLayoutL(aLayout);
}

LOCAL_C void MinimumSize(jint aItem,TSize* aSize)
{
    *aSize = MIDUnhand<MMIDItem>(aItem)->MinimumSize();
}

LOCAL_C void PreferredSize(jint aItem,TSize* aSize)
{
    *aSize = MIDUnhand<MMIDItem>(aItem)->PreferredSize();
}

LOCAL_C void SetPreferredSizeL(jint aItem, TInt aWidth, TInt aHeight)
{
    TSize size(aWidth,aHeight);
    MIDUnhand<MMIDItem>(aItem)->SetPreferredSizeL(size);
}

LOCAL_C void AddCommandL(jint aItem, jint aCommand)
{
    MIDUnhand<MMIDItem>(aItem)->AddCommandL(MIDUnhand<MMIDCommand>(aCommand));
}

LOCAL_C void RemoveCommand(jint aItem, jint aCommand)
{
    MIDUnhand<MMIDItem>(aItem)->RemoveCommand(MIDUnhand<MMIDCommand>(aCommand));
}

LOCAL_C void SetDefaultCommand(jint aItem, jint aCommand)
{
    MIDUnhand<MMIDItem>(aItem)->SetDefaultCommand(MIDUnhand<MMIDCommand>(aCommand));
}

//
JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_Item__1setLabel
(JNIEnv* aEnv,jobject,jint aItem,jint aToolkit,jstring aLabel)
{
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    RJString label(*aEnv,aLabel);
    return toolkit->ExecuteTrap(&SetLabelL, aItem, (const TDesC*)&label);
}

JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_Item__1setLayout(JNIEnv*,jobject,jint aItem,jint aToolkit,jint aLayout)
{
    return JavaUnhand<CMIDToolkit>(aToolkit)->ExecuteTrap(&SetLayoutL, aItem, (MMIDItem::TLayout)aLayout);
}

JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_Item__1getMinimumSizeWidth(JNIEnv*,jobject,jint aItem,jint aToolkit)
{
    TSize size;
    JavaUnhand<CMIDToolkit>(aToolkit)->ExecuteV(&MinimumSize, aItem, &size);
    return size.iWidth;
}

JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_Item__1getMinimumSizeHeight(JNIEnv*,jobject,jint aItem,jint aToolkit)
{
    TSize size;
    JavaUnhand<CMIDToolkit>(aToolkit)->ExecuteV(&MinimumSize, aItem, &size);
    return size.iHeight;
}

JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_Item__1getPreferredSizeWidth(JNIEnv*, jobject, jint aItem, jint aToolkit)
{
    TSize size;
    JavaUnhand<CMIDToolkit>(aToolkit)->ExecuteV(&PreferredSize, aItem, &size);
    return size.iWidth;
}


JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_Item__1getPreferredSizeHeight(JNIEnv*, jobject, jint aItem, jint aToolkit)
{
    TSize size;
    JavaUnhand<CMIDToolkit>(aToolkit)->ExecuteV(&PreferredSize, aItem, &size);
    return size.iHeight;
}


JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_Item__1setPreferredSize
(JNIEnv*,jobject,jint aItem,jint aToolkit,jint aWidth,jint aHeight)
{
    return JavaUnhand<CMIDToolkit>(aToolkit)->ExecuteTrap(&SetPreferredSizeL, aItem, aWidth, aHeight);
}

JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_Item__1addCommand
(JNIEnv*,jobject,jint aItem,jint aToolkit,jint aCommand)
{
    return JavaUnhand<CMIDToolkit>(aToolkit)->ExecuteTrap(&AddCommandL, aItem, aCommand);
}

JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_Item__1removeCommand
(JNIEnv*,jobject,jint aItem,jint aToolkit,jint aCommand)
{
    return JavaUnhand<CMIDToolkit>(aToolkit)->ExecuteTrap(&RemoveCommand, aItem, aCommand);
}

JNIEXPORT void JNICALL Java_javax_microedition_lcdui_Item__1setDefaultCommand
(JNIEnv*,jobject,jint aItem,jint aToolkit,jint aCommand)
{
    JavaUnhand<CMIDToolkit>(aToolkit)->ExecuteV(&SetDefaultCommand, aItem, aCommand);
}
