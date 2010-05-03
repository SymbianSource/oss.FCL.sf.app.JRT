/*
* Copyright (c) 1999-2002 Nokia Corporation and/or its subsidiary(-ies).
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
#include "javax_microedition_lcdui_Display.h"


LOCAL_C TBool FlashBacklight(MMIDUtils* aUtils,TInt aDuration)
{
    TBool ret = EFalse;
    TInt err;
    TRAP(err,ret = aUtils->FlashBacklightL((TTimeIntervalMicroSeconds32)aDuration));
    return ret;
}

LOCAL_C TBool Vibrate(MMIDUtils* aUtils,TInt aDuration)
{
    return aUtils->Vibrate((TTimeIntervalMicroSeconds32)aDuration);
}

LOCAL_C TInt GetColor(MMIDUtils* aUtils,MMIDUtils::TColorType aColorSpecifier)
{
    return aUtils->Color(aColorSpecifier);
}

LOCAL_C TInt GetBorderStyle(MMIDUtils* aUtils,TBool aHighlighted)
{
    return (TInt)aUtils->BorderStyle(aHighlighted);
}

LOCAL_C TInt BestImageWidth(MMIDUtils* aUtils,MMIDUtils::TImageType aImageType)
{
    return aUtils->BestImageSize(aImageType).iWidth;
}

LOCAL_C TInt BestImageHeight(MMIDUtils* aUtils,MMIDUtils::TImageType aImageType)
{
    return aUtils->BestImageSize(aImageType).iHeight;
}


//
JNIEXPORT jboolean JNICALL Java_javax_microedition_lcdui_Display__1flashBacklight
(JNIEnv *, jobject, jint aToolkit, jint aDuration)
{
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    return (jboolean)toolkit->Execute(&FlashBacklight,toolkit->Utils(),(TInt)aDuration*1000);
}

//
JNIEXPORT jboolean JNICALL Java_javax_microedition_lcdui_Display__1vibrate
(JNIEnv *, jobject, jint aToolkit, jint aDuration)
{
    TInt maxMillisecondDuration = KMaxTInt32 / 1000;
    TInt microsecondDuration = (aDuration<=maxMillisecondDuration) ? aDuration*1000 : KMaxTInt32;

    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    return (jboolean)toolkit->Execute(&Vibrate,toolkit->Utils(), microsecondDuration);
}

//
JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_Display__1getColor
(JNIEnv *, jobject, jint aToolkit, jint aColorSpecifier)
{
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    return toolkit->Execute(&GetColor,toolkit->Utils(),(MMIDUtils::TColorType)aColorSpecifier);
}

//
JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_Display__1getBorderStyle
(JNIEnv *, jobject, jint aToolkit, jboolean aHighlighted)
{
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    return toolkit->Execute(&GetBorderStyle,toolkit->Utils(),(TBool) aHighlighted);
}

//
JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_Display__1getBestImageWidth
(JNIEnv *, jobject, jint aToolkit, jint aImageType)
{
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    return toolkit->Execute(&BestImageWidth,toolkit->Utils(),(MMIDUtils::TImageType)aImageType);
}

//
JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_Display__1getBestImageHeight
(JNIEnv *, jobject, jint aToolkit, jint aImageType)
{
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    return toolkit->Execute(&BestImageHeight,toolkit->Utils(),(MMIDUtils::TImageType)aImageType);
}

