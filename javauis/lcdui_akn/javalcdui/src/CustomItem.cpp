/*
* Copyright (c) 1999 - 2003 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32def.h> // MAKE_TINT_64
#include "javax_microedition_lcdui_CustomItem.h"
#include "CMIDToolkit.h"
#include "MIDUtils.h"

LOCAL_C void CreateL
(
    CMIDToolkit*    aToolkit,
    TInt*       aHandle,
    jobject     aReference,
    const TDesC*    aLabel
)
//
// Creates the plataform dependent MMIDCustomItem
//
{
    MMIDCustomItem* customItem = aToolkit->ComponentFactory()->CreateCustomItemL(*aLabel);
    CleanupDisposePushL(customItem);
    *aHandle = aToolkit->RegisterComponentL(customItem, aReference);
    CleanupPopComponent(customItem);
}
JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_CustomItem__1create
(
    JNIEnv* aJni,
    jobject,
    jint    aToolkit,
    jobject aCustomItem,
    jstring aLabel
)
{
    jobject customItem = aJni->NewWeakGlobalRef(aCustomItem);
    if (customItem == 0)
        return KErrNoMemory;

    //
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    RJString label(*aJni,aLabel);

    TInt handle = 0;
    TInt err = toolkit->ExecuteTrap(&CreateL,toolkit,&handle,customItem,(const TDesC*)&label);
    if (err!=KErrNone)
    {
        aJni->DeleteWeakGlobalRef((jweak)customItem);
        return err;
    }

    return handle;
}



LOCAL_C TInt InvokeInteractionModes(MMIDCustomItem* aItem)
{
    return aItem->InteractionModes();
}
JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_CustomItem__1getInteractionModes
(
    JNIEnv*,
    jobject,
    jint    aToolkit,
    jint    aItem
)
{
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    MMIDCustomItem* customItem = MIDUnhand<MMIDCustomItem>(aItem);
    return toolkit->Execute(InvokeInteractionModes, customItem);
}

LOCAL_C void InvokeSetFocusAndScroll(MMIDCustomItem* aItem, TBool aKeepFocus, const TRect* aRect,TInt aDirection)
{
    aItem->SetFocusAndScroll(aKeepFocus, aRect, aDirection);
}
JNIEXPORT void JNICALL Java_javax_microedition_lcdui_CustomItem__1setFocusAndScroll
(JNIEnv* aJni,jobject,jint aToolkit,jint aItem,jboolean aKeepFocus,jintArray aScrollRect,jint aDirection)
{
    TRect rect;
    const TRect* pRect = (aScrollRect != NULL ? &rect : NULL);
    if (pRect)
    {
        ASSERT(sizeof(TRect) == 4*sizeof(jint));
        aJni->GetIntArrayRegion(aScrollRect, 0, 4, reinterpret_cast<jint*>(&rect));
        rect.iBr.iX+=rect.iTl.iX;
        rect.iBr.iY+=rect.iTl.iY;
    }
    MMIDCustomItem* item = MIDUnhandObject<MMIDCustomItem>(aItem);
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);

    toolkit->ExecuteV(InvokeSetFocusAndScroll, item, (TBool)aKeepFocus, pRect,(TInt)aDirection);
}

LOCAL_C void Invalidate(MMIDCustomItem* aCustomItem, TSize aMinSize, TSize aPrefSize)
{
    aCustomItem->Invalidate(aMinSize,aPrefSize);
}
JNIEXPORT void JNICALL Java_javax_microedition_lcdui_CustomItem__1invalidate
(JNIEnv*,jobject,jint aToolkit,jint aCustomItem,jint aMinWidth,jint aMinHeight,jint aPreWidth,jint aPreHeight)
{
    CMIDToolkit*    toolkit    = JavaUnhand<CMIDToolkit>(aToolkit);
    MMIDCustomItem* customItem = MIDUnhand<MMIDCustomItem>(aCustomItem);
    TSize minSize(aMinWidth,aMinHeight);
    TSize prefSize(aPreWidth, aPreHeight);

    toolkit->ExecuteV(Invalidate, customItem, minSize, prefSize);
}
