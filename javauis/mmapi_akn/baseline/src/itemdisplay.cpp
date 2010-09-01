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
* Description:  This class has JNI wrappers for CMMAItemDisplay
*
*/


#include <jutils.h>
#include "com_nokia_microedition_media_control_VideoItem.h"
#include "cmmaeventsource.h"
#include "cmmaitemdisplay.h"

JNIEXPORT jint JNICALL Java_com_nokia_microedition_media_control_VideoItem__1sizeChanged
(JNIEnv*, jobject,
 jint aEventSourceHandle,
 jint aDisplayHandle,
 jint aWidth,
 jint aHeight)
{
    CMMAItemDisplay* itemDisplay = JavaUnhand< CMMAItemDisplay >(aDisplayHandle);
    CMMAEventSource* eventSource = JavaUnhand< CMMAEventSource >(aEventSourceHandle);

    __ASSERT_DEBUG(itemDisplay != NULL,
                   User::Panic(_L("itemdisplay::control is null"),
                               KErrArgument));
    __ASSERT_DEBUG(eventSource != NULL,
                   User::Panic(_L("itemdisplay::eventsource is null"),
                               KErrArgument));

    TInt err = eventSource->ExecuteTrap(&CMMAItemDisplay::SizeChangedL,
                                        itemDisplay,
                                        aWidth,
                                        aHeight);

    return err;
}

JNIEXPORT jint JNICALL Java_com_nokia_microedition_media_control_VideoItem__1getMinContentWidth
(JNIEnv*, jobject,
 jint aEventSourceHandle,
 jint aDisplayHandle)
{
    CMMAItemDisplay* itemDisplay = JavaUnhand< CMMAItemDisplay >(aDisplayHandle);
    CMMAEventSource* eventSource = JavaUnhand< CMMAEventSource >(aEventSourceHandle);
    TSize size;
    eventSource->ExecuteV(CMMAItemDisplay::StaticSourceSize,
                          itemDisplay,
                          (TSize*)&size);
    return size.iWidth;
}

JNIEXPORT jint JNICALL Java_com_nokia_microedition_media_control_VideoItem__1getMinContentHeight
(JNIEnv*, jobject,
 jint aEventSourceHandle,
 jint aDisplayHandle)
{
    CMMAItemDisplay* itemDisplay = JavaUnhand< CMMAItemDisplay >(aDisplayHandle);
    CMMAEventSource* eventSource = JavaUnhand< CMMAEventSource >(aEventSourceHandle);

    TSize size;
    eventSource->ExecuteV(CMMAItemDisplay::StaticSourceSize,
                          itemDisplay,
                          (TSize*)&size);
    return size.iHeight;
}


JNIEXPORT jint JNICALL Java_com_nokia_microedition_media_control_VideoItem__1getPrefContentHeight
(JNIEnv*, jobject,
 jint aEventSourceHandle,
 jint aDisplayHandle,
 jint /*aTentative*/)  // tentative value is ignored because current
// implementation returns always -1
{
    CMMAItemDisplay* itemDisplay = JavaUnhand< CMMAItemDisplay >(aDisplayHandle);
    CMMAEventSource* eventSource = JavaUnhand< CMMAEventSource >(aEventSourceHandle);

    TSize size;
    eventSource->ExecuteV(CMMAItemDisplay::StaticSourceSize,
                          itemDisplay,
                          (TSize*)&size);
    return size.iHeight;
}

JNIEXPORT jint JNICALL Java_com_nokia_microedition_media_control_VideoItem__1getPrefContentWidth
(JNIEnv*, jobject,
 jint aEventSourceHandle,
 jint aDisplayHandle,
 jint /*aTentative*/)  // tentative value is ignored because current
// implementation returns always -1
{
    CMMAItemDisplay* itemDisplay = JavaUnhand< CMMAItemDisplay >(aDisplayHandle);
    CMMAEventSource* eventSource = JavaUnhand< CMMAEventSource >(aEventSourceHandle);

    TSize size;
    eventSource->ExecuteV(CMMAItemDisplay::StaticSourceSize,
                          itemDisplay,
                          (TSize*)&size);
    return size.iWidth;
}


//  END OF FILE
