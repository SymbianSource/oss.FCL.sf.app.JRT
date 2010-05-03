/*
* Copyright (c) 2001-2005 Nokia Corporation and/or its subsidiary(-ies).
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
#include "lcdgr.h"

#include "javax_microedition_lcdui_ChoiceGroup.h"

LOCAL_C void CreateChoiceGroupL
(
    CMIDToolkit*        aToolkit,
    jweak               aRef,
    const TDesC*        aLabel,
    TInt                aType,
    jint*               aHandle
)
{
    ASSERT(aLabel);
    MMIDComponentFactory& factory = *aToolkit->ComponentFactory();
    RArray<TPtrC> dummyTextArray;
    RArray<MMIDImage*> dummyImageArray;
    MMIDChoiceGroup* choice = factory.CreateChoiceGroupL(*aLabel, aType, dummyTextArray, dummyImageArray);
    CleanupDisposePushL(choice);
    *aHandle = aToolkit->RegisterComponentL(choice, aRef);
    CleanupPopComponent(choice);
}

JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_ChoiceGroup__1create
(
    JNIEnv*         aJni,
    jobject         aChoiceGroup,
    jint            aToolkit,
    jstring         aLabel,
    jint            aType
)
{
    jweak ref = aJni->NewWeakGlobalRef(aChoiceGroup);
    if (ref == 0)
    {
        return KErrNoMemory;
    }

    RJString label(*aJni, aLabel);

    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);

    jint handle = 0;
    jint error = toolkit->ExecuteTrap(
                     &CreateChoiceGroupL,
                     toolkit,
                     ref,
                     (const TDesC*)&label,
                     (TInt)aType,
                     &handle
                 );

    if (error != KErrNone)
    {
        ASSERT(ref);
        aJni->DeleteWeakGlobalRef(ref);
        return error;
    }

    return handle;
}

LOCAL_C void InsertElementL(MMIDChoiceGroup* aChoice,TInt aIndex,const TDesC* aText,MMIDImage* aImage)
{
    aChoice->InsertElementL(aIndex,*aText, aImage);
}
JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_ChoiceGroup__1insert
(JNIEnv* aEnv,jobject,jint aItem,jint aToolkit,jint aIndex,jstring aStringItem,jint aImage)
{
    RJString string(*aEnv,aStringItem);
    MMIDImage*       image  = MIDUnhand<MMIDImage>(aImage);
    MMIDChoiceGroup* choice = MIDUnhandObject<MMIDChoiceGroup>(aItem);
    return JavaUnhand<CMIDToolkit>(aToolkit)->ExecuteTrap(&InsertElementL,choice,(TInt)aIndex,(const TDesC*)&string,image);
}

LOCAL_C void DeleteElementL(MMIDChoiceGroup* aChoice,TInt aIndex)
{
    aChoice->DeleteElementL(aIndex);
}
JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_ChoiceGroup__1delete
(JNIEnv *, jobject, jint aItem, jint aToolkit, jint aIndex)
{
    MMIDChoiceGroup* choice = MIDUnhandObject<MMIDChoiceGroup>(aItem);
    return JavaUnhand<CMIDToolkit>(aToolkit)->ExecuteTrap(&DeleteElementL,choice,(TInt)aIndex);
}

LOCAL_C void DeleteAllL(MMIDChoiceGroup* aChoice)
{
    aChoice->DeleteAllL();
}
JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_ChoiceGroup__1deleteAll
(JNIEnv *, jobject, jint aItem, jint aToolkit)
{
    MMIDChoiceGroup* choice = MIDUnhandObject<MMIDChoiceGroup>(aItem);
    return JavaUnhand<CMIDToolkit>(aToolkit)->ExecuteTrap(&DeleteAllL,choice);
}

LOCAL_C void SetElementL(MMIDChoiceGroup* aChoice,TInt aIndex,const TDesC* aText,MMIDImage* aImage)
{
    aChoice->SetElementL(aIndex,*aText, aImage);
}
JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_ChoiceGroup__1set
(JNIEnv* aEnv,jobject,jint aItem,jint aToolkit,jint aIndex,jstring aStringItem,jint aImage)
{
    RJString string(*aEnv,aStringItem);
    MMIDImage* image = MIDUnhand<MMIDImage>(aImage);
    MMIDChoiceGroup* choice = MIDUnhandObject<MMIDChoiceGroup>(aItem);
    return JavaUnhand<CMIDToolkit>(aToolkit)->ExecuteTrap(&SetElementL,choice,(TInt)aIndex,(const TDesC*)&string,image);
}

LOCAL_C TBool IsSelected(MMIDChoiceGroup* aChoice,TInt aIndex)
{
    return aChoice->IsSelected(aIndex);
}
JNIEXPORT jboolean JNICALL Java_javax_microedition_lcdui_ChoiceGroup__1isSelected
(JNIEnv*,jobject,jint aItem,jint aToolkit,jint aIndex)
{
    MMIDChoiceGroup* choice = MIDUnhandObject<MMIDChoiceGroup>(aItem);
    return (jboolean)JavaUnhand<CMIDToolkit>(aToolkit)->Execute(&IsSelected,choice,(TInt)aIndex);
}

LOCAL_C void SelectElementL(MMIDChoiceGroup* aChoice,TInt aIndex,TBool aSelected)
{
    aChoice->SelectElementL(aIndex,aSelected);
}
JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_ChoiceGroup__1select
(JNIEnv*,jobject,jint aItem,jint aToolkit,jint aIndex,jboolean aSelected)
{
    MMIDChoiceGroup* choice = MIDUnhandObject<MMIDChoiceGroup>(aItem);
    return JavaUnhand<CMIDToolkit>(aToolkit)->ExecuteTrap(&SelectElementL,choice,(TInt)aIndex,(TBool)aSelected);
}

LOCAL_C void SetFontL(MMIDChoiceGroup* aChoice,TInt aIndex, MMIDFont* aFont)
{
    aChoice->SetFontL(aIndex,aFont);
}
JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_ChoiceGroup__1setFont
(JNIEnv*,jobject,jint aItem,jint aToolkit,jint aIndex,jint aFont)
{
    MMIDChoiceGroup* choice = MIDUnhandObject<MMIDChoiceGroup>(aItem);
    MMIDFont* font = MIDUnhandObject<MMIDFont>(aFont);
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    return toolkit->ExecuteTrap(&SetFontL,choice,(TInt)aIndex,font);
}

LOCAL_C void SetFitPolicyL(MMIDChoiceGroup* aChoice,TInt aFitPolicy)
{
    aChoice->SetFitPolicyL(aFitPolicy);
}
JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_ChoiceGroup__1setFitPolicy
(JNIEnv*,jobject,jint aItem,jint aToolkit,jint aFitPolicy)
{
    MMIDChoiceGroup* choice = MIDUnhandObject<MMIDChoiceGroup>(aItem);
    return JavaUnhand<CMIDToolkit>(aToolkit)->ExecuteTrap(&SetFitPolicyL,choice,(TInt)aFitPolicy);
}
