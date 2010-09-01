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
#include "javax_microedition_lcdui_Device.h"

const TInt KAlphaLevels = 256;

LOCAL_C TInt GetDeviceAttribs(CMIDToolkit* aToolkit, jint* aAttribArray, jint aArrayLength)
{
    if (aArrayLength < javax_microedition_lcdui_Device_ATTRIB_COUNT)
    {
        return KErrOverflow;
    }

    MMIDUtils& utils = *aToolkit->Utils();
    MMIDGraphicsFactory& factory = aToolkit->GraphicsFactory();

    jint caps = 0;

    if (factory.DoubleBuffer())
    {
        caps |= javax_microedition_lcdui_Device_CAPS_DOUBLE_BUFFERED;
    }
    if (utils.HasPointerEvents())
    {
        caps |= javax_microedition_lcdui_Device_CAPS_POINTER_EVENTS;
    }
    if (utils.HasPointerMotionEvents())
    {
        caps |= javax_microedition_lcdui_Device_CAPS_POINTER_MOTION_EVENTS;
    }
    if (utils.HasRepeatEvents())
    {
        caps |= javax_microedition_lcdui_Device_CAPS_KEY_REPEAT_EVENTS;
    }



    TDisplayMode frameBufferMode = factory.DisplayMode();
    if (frameBufferMode > EGray256)
    {
        caps |= javax_microedition_lcdui_Device_CAPS_COLOR;
    }

    CCoeEnv* controlEnv = CCoeEnv::Static();
    CWsScreenDevice* screenDevice = controlEnv->ScreenDevice();

    TSize screenSize = screenDevice->SizeInPixels();

    aAttribArray[ javax_microedition_lcdui_Device_ATTRIB_CAPABILITIES ] = caps;
    aAttribArray[ javax_microedition_lcdui_Device_ATTRIB_NUM_COLORS   ] = TDisplayModeUtils::NumDisplayModeColors(frameBufferMode);
    aAttribArray[ javax_microedition_lcdui_Device_ATTRIB_NUM_ALPHAS   ] = KAlphaLevels;
    aAttribArray[ javax_microedition_lcdui_Device_ATTRIB_SCREEN_WIDTH ] = screenSize.iWidth;
    aAttribArray[ javax_microedition_lcdui_Device_ATTRIB_SCREEN_HEIGHT] = screenSize.iHeight;

    return KErrNone;
}

JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_Device__1getAttribs(JNIEnv* aJni, jobject, jint aToolkit, jintArray aAttribArray)
{
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);

    if (aJni->GetArrayLength(aAttribArray) != javax_microedition_lcdui_Device_ATTRIB_COUNT)
    {
        return KErrArgument;
    }

    jint attribArray[javax_microedition_lcdui_Device_ATTRIB_COUNT];
    jint attribCount = javax_microedition_lcdui_Device_ATTRIB_COUNT;
    jint err;

    err = toolkit->Execute(&GetDeviceAttribs, toolkit, &attribArray[0], attribCount);
    if (KErrNone == err)
    {
        aJni->SetIntArrayRegion(aAttribArray, 0, attribCount, &attribArray[0]);
    }

    return err;
}

LOCAL_C void InvokeCanvasSizeWithoutKeypad(MMIDDisplayable* aDisplayable, jint* aSizeArray)
{
    TRect onScreenKeypadRect = aDisplayable->GetCanvasRectFromLaf();
    aSizeArray[0] = onScreenKeypadRect.Width();
    aSizeArray[1] = onScreenKeypadRect.Height();
}

JNIEXPORT void JNICALL Java_javax_microedition_lcdui_Device__1getCanvasSizeWithoutKeypad
(JNIEnv* aEnv,jobject, jint aToolkit, jint aDisplayable, jintArray aSize)
{
    MMIDDisplayable* displayable = MIDUnhandObject<MMIDDisplayable>(aDisplayable);
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    jint sizeArray[2];
    toolkit->Execute(&InvokeCanvasSizeWithoutKeypad, displayable, sizeArray);
    aEnv->SetIntArrayRegion(aSize, 0, 2, sizeArray);
}
