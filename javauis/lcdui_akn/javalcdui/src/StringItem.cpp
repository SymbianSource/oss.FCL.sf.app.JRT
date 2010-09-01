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
#include "javax_microedition_lcdui_StringItem.h"


LOCAL_C void CreateL(CMIDToolkit* aToolkit,TInt* aHandle,jobject aStringItem,const TDesC* aLabel,const TDesC* aText,TInt aAppearanceMode)
{
    MMIDStringItem* item = aToolkit->ComponentFactory()->CreateStringItemL(*aLabel,*aText,(MMIDItem::TAppearance)aAppearanceMode);
    CleanupDisposePushL(item);
    *aHandle = aToolkit->RegisterComponentL(item, aStringItem);
    CleanupPopComponent(item);
}
JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_StringItem__1create
(JNIEnv* aJni,jobject,jint aToolkit,jobject aStringItem,jstring aLabel,jstring aText,jint aAppearanceMode)
{
    jobject stringItem = aJni->NewWeakGlobalRef(aStringItem);
    if (stringItem == 0)
        return KErrNoMemory;
    //
    CMIDToolkit* toolkit =JavaUnhand<CMIDToolkit>(aToolkit);
    RJString text(*aJni,aText);
    RJString label(*aJni,aLabel);
    TInt h=0;
    TInt err = toolkit->ExecuteTrap(&CreateL,toolkit,&h,stringItem,(const TDesC*)&label,(const TDesC*)&text,(TInt)aAppearanceMode);
    if (err!=KErrNone)
    {
        aJni->DeleteWeakGlobalRef((jweak)stringItem);
        return err;
    }
    return h;
}

LOCAL_C void SetTextL(MMIDStringItem* aStringItem,const TDesC* aText)
{
    aStringItem->SetTextL(*aText);
}
JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_StringItem__1setText
(JNIEnv* aEnv,jobject,jint aStringItem,jint aToolkit,jstring aText)
{
    MMIDStringItem* stringItem = MIDUnhandObject<MMIDStringItem>(aStringItem);
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    RJString text(*aEnv,aText);
    return toolkit->ExecuteTrap(&SetTextL,stringItem,(const TDesC*)&text);
}

LOCAL_C void SetFontL(MMIDStringItem* aStringItem,MMIDFont* aFont)
{
    aStringItem->SetFontL(aFont);
}
JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_StringItem__1setFont
(JNIEnv*,jobject,jint aStringItem,jint aToolkit,jint aFont)
{
    MMIDStringItem* stringItem = MIDUnhandObject<MMIDStringItem>(aStringItem);
    MMIDFont* font = MIDUnhandObject<MMIDFont>(aFont);
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    return toolkit->ExecuteTrap(&SetFontL,stringItem,font);
}
