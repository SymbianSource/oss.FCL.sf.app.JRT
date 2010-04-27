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
#include "javax_microedition_lcdui_Font.h"


LOCAL_C void CreateL(CMIDToolkit* aToolkit, TInt* aHandle, TUint aStyle,
                     TUint aSize, TInt aFace, TBool aIsFreeSizeFont)
{
    MMIDFont* font = aToolkit->ComponentFactory()->CreateFontL(
                         aStyle, aSize, aFace, aIsFreeSizeFont);
    CleanupDisposePushL(font);
    *aHandle = aToolkit->RegisterComponentL(font, NULL);
    CleanupPopComponent(font);
}
JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_Font__1create
(JNIEnv*, jobject, jint aToolkit, jint aFace, jint aStyle, jint aSize,
 jboolean aIsFreeSizeFont)
{
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    TInt h = 0;
    TInt err = toolkit->ExecuteTrap(&CreateL, toolkit, &h,
                                    (TUint)aStyle, (TUint)aSize, (TInt)aFace, (TBool)aIsFreeSizeFont);
    return err == KErrNone ? h : err;
}

/*
 * Returns width of aString font in aFont.
 * Returned width is correctly scaled.
 */
LOCAL_C TInt Width(MMIDFont* aFont,const TDesC* aString)
{
    return aFont->Width(*aString);
}
JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_Font__1width
(JNIEnv* aJni,jobject,jint aFont,jint aToolkit,jstring aString)
{
    MMIDFont* font = MIDUnhandObject<MMIDFont>(aFont);
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    RJString string(*aJni,aString);
    return toolkit->Execute(&Width,font,(const TDesC*)&string);
}

/*
 * Returns height of aFont font.
 * Returned height is correctly scaled.
 */
LOCAL_C TInt Height(MMIDFont* aFont)
{
    return aFont->Height();
}
JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_Font__1height(JNIEnv*,jobject,jint aFont,jint aToolkit)
{
    MMIDFont* font = MIDUnhandObject<MMIDFont>(aFont);
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    return toolkit->Execute(&Height,font);
}

/*
 * Returns position of baseline in aFont font.
 * Returned position is correctly scaled.
 */
LOCAL_C TInt Baseline(MMIDFont* aFont)
{
    return aFont->Baseline();
}
JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_Font__1baseline(JNIEnv*,jobject,jint aFont,jint aToolkit)
{
    MMIDFont* font = MIDUnhandObject<MMIDFont>(aFont);
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    return toolkit->Execute(&Baseline,font);
}

LOCAL_C void GetFontSpecs(CMIDToolkit* aToolkit,TInt aFontSpecifier,TInt* aFace,TInt* aStyle,TInt* aSize)
{
    SFontSpec fontSpec = aToolkit->Utils()->FontSpecifierSpecs(MMIDFont::TFontSpecifier(aFontSpecifier));
    *aFace = fontSpec.iFace;
    *aStyle = fontSpec.iStyle;
    *aSize = fontSpec.iSize;
}
JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_Font__1getFontSpec(JNIEnv*,jclass,jint aToolkit,jint aFontSpecifier)
{
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    TInt face;
    TInt style;
    TInt size;
    toolkit->ExecuteV(&GetFontSpecs,toolkit,(TInt)aFontSpecifier,&face,&style,&size);
    return (face<<16|style<<8|size);
}
