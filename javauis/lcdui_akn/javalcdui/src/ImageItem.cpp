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
#include "javax_microedition_lcdui_ImageItem.h"
#include "lcdgr.h"


LOCAL_C void CreateL(CMIDToolkit* aToolkit,TInt* aHandle,jobject aImageItem,const TDesC* aLabel,MMIDImage* aImage,TInt aLayout,TInt aAppearanceMode,const TDesC* aAltText)
{
    MMIDImageItem* item = aToolkit->ComponentFactory()->CreateImageItemL(*aLabel,aImage,(MMIDItem::TLayout)aLayout,(MMIDItem::TAppearance)aAppearanceMode,*aAltText);
    CleanupDisposePushL(item);
    *aHandle = aToolkit->RegisterComponentL(item, aImageItem);
    CleanupPopComponent(item);
}
JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_ImageItem__1create
(JNIEnv* aJni,jobject,jint aToolkit,jobject aImageItem,jstring aLabel,jint aImage,jint aLayout,jint aAppearanceMode,jstring aAltText)
{
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);

    TInt    handle    = 0;
    jobject imageItem = aJni->NewWeakGlobalRef(aImageItem);
    if (imageItem == 0)
        return KErrNoMemory;

    MMIDImage* image = MIDUnhandObject<MMIDImage>(aImage);
    RJString label(*aJni,aLabel);
    RJString altText(*aJni,aAltText);

    TInt err = toolkit->ExecuteTrap(&CreateL,toolkit,&handle,imageItem,(const TDesC*)&label,image,(TInt)aLayout,(TInt)aAppearanceMode,(const TDesC*)&altText);
    if (err!=KErrNone)
    {
        aJni->DeleteWeakGlobalRef((jweak)imageItem);
        return err;
    }
    return err == KErrNone ? handle : err;
}

LOCAL_C void SetImageL(MMIDImageItem* aImageItem,MMIDImage* aImage)
{
    aImageItem->SetImageL(aImage);
}
JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_ImageItem__1setImage
(JNIEnv*,jobject,jint aImageItem,jint aToolkit,jint aImage)
{
    MMIDImageItem*  imageItem = MIDUnhandObject<MMIDImageItem>(aImageItem);
    MMIDImage*      image     = MIDUnhandObject<MMIDImage>(aImage);
    return JavaUnhand<CMIDToolkit>(aToolkit)->ExecuteTrap(&SetImageL,imageItem,image);
}

LOCAL_C void SetAltTextL(MMIDImageItem* aImageItem,const TDesC* aAltText)
{
    aImageItem->SetAltTextL(*aAltText);
}
JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_ImageItem__1setAltText
(JNIEnv* aEnv,jobject,jint aImageItem,jint aToolkit,jstring aAltText)
{
    MMIDImageItem* imageItem = MIDUnhandObject<MMIDImageItem>(aImageItem);
    RJString text(*aEnv,aAltText);
    return JavaUnhand<CMIDToolkit>(aToolkit)->ExecuteTrap(&SetAltTextL,imageItem,(const TDesC*)&text);
}
