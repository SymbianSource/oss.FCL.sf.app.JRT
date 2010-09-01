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
#include "javax_microedition_lcdui_List.h"
#include "lcdgr.h"

struct TListCreate
{
    jweak   iRef;
    jint    iDisplayable;
    jint    iType;
};

struct TListAttribs
{
    jint  iHandle;
    TSize iSize;
};

LOCAL_C void CreateListL(CMIDToolkit* aToolkit, TListCreate* aCreate, TListAttribs* aAttribs)
{
    MMIDDisplayable& displayable = *MIDUnhand<MMIDDisplayable>(aCreate->iDisplayable);
    MMIDComponentFactory& factory = *aToolkit->ComponentFactory();
    RArray<TPtrC>      dummyTextArray;
    RArray<MMIDImage*> dummyIconArray;
    TInt type = aCreate->iType;
    MMIDList* list = factory.CreateListL(type, displayable, dummyTextArray, dummyIconArray);
    CleanupDisposePushL(list);
    aAttribs->iHandle = aToolkit->RegisterComponentL(list, aCreate->iRef);
    CleanupPopComponent(list);
    aCreate->iRef = 0;
    aAttribs->iSize = displayable.ContentSize();
}

JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_List__1create
(
    JNIEnv*         aJni,
    jobject         aList,
    jint            aToolkit,
    jint            aDisplayable,
    jint            aType,
    jintArray       aCreateListReturn
)
{
    jint error = 0;

    TListCreate create;
    create.iRef = aJni->NewWeakGlobalRef(aList);
    create.iDisplayable=aDisplayable;
    create.iType = aType;

    TListAttribs attribs;
    attribs.iHandle = 0;

    if (create.iRef)
    {
        CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
        error = toolkit->ExecuteTrap(CreateListL, toolkit, &create, &attribs);
    }
    else
    {
        error = KErrNoMemory;
    }

    if (create.iRef)
    {
        // cleared in CreateListL if successful
        aJni->DeleteWeakGlobalRef(create.iRef);
    }

    ASSERT(aJni->GetArrayLength(aCreateListReturn) == 3);
    jint arrayReturn[3];
    arrayReturn[0] = attribs.iHandle;
    arrayReturn[1] = attribs.iSize.iWidth;
    arrayReturn[2] = attribs.iSize.iHeight;
    aJni->SetIntArrayRegion(aCreateListReturn, 0, 3, arrayReturn);

    return error;
}

LOCAL_C void InvokeSetSelectCommand(jint aList, jint aID)
{
    MMIDList*    list    = MIDUnhandObject<MMIDList>(aList);
    list->SetSelectCommand(aID);
}

JNIEXPORT void JNICALL Java_javax_microedition_lcdui_List__1setSelectCommand(JNIEnv*, jobject, jint aList, jint aToolkit, jint aID)
{
    JavaUnhand<CMIDToolkit>(aToolkit)->ExecuteV(&InvokeSetSelectCommand, aList, aID);
}

LOCAL_C void InsertElementL(MMIDList* aChoice,TInt aIndex,const TDesC* aText,MMIDImage* aImage)
{
    aChoice->InsertElementL(aIndex,*aText,aImage);
}
JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_List__1insert
(
    JNIEnv* aEnv,
    jobject,
    jint    aList,
    jint    aToolkit,
    jint    aIndex,
    jstring aStringItem,
    jint    aImage
)
{
    RJString string(*aEnv,aStringItem);
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    MMIDList*  list  = MIDUnhandObject<MMIDList>(aList);
    MMIDImage* image = MIDUnhandObject<MMIDImage>(aImage);
    return toolkit->ExecuteTrap(
               &InsertElementL,
               list,
               (TInt)aIndex,
               (const TDesC*)&string,
               image
           );
}

LOCAL_C void DeleteElementL(MMIDList* aChoice,TInt aIndex)
{
    aChoice->DeleteElementL(aIndex);
}

JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_List__1delete
(JNIEnv *, jobject, jint aList, jint aToolkit, jint aIndex)
{
    MMIDList* list = MIDUnhandObject<MMIDList>(aList);
    return JavaUnhand<CMIDToolkit>(aToolkit)->ExecuteTrap(&DeleteElementL,list,(TInt)aIndex);
}

LOCAL_C void DeleteAllL(MMIDList* aChoice)
{
    aChoice->DeleteAllL();
}

JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_List__1deleteAll
(JNIEnv *, jobject, jint aList, jint aToolkit)
{
    MMIDList* list = MIDUnhandObject<MMIDList>(aList);
    return JavaUnhand<CMIDToolkit>(aToolkit)->ExecuteTrap(&DeleteAllL,list);
}

LOCAL_C void SetElementL(MMIDList* aChoice,TInt aIndex,const TDesC* aText,MMIDImage* aImage)
{
    aChoice->SetElementL(aIndex,*aText,aImage);
}

JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_List__1set
(JNIEnv* aEnv,jobject,jint aList,jint aToolkit,jint aIndex,jstring aStringItem,jint aImage)
{
    RJString string(*aEnv,aStringItem);
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    MMIDList*  list  = MIDUnhandObject<MMIDList>(aList);
    MMIDImage* image = MIDUnhandObject<MMIDImage>(aImage);
    return toolkit->ExecuteTrap(&SetElementL, list,(TInt)aIndex,(const TDesC*)&string,image);
}

LOCAL_C TBool IsSelected(MMIDList* aChoice,TInt aIndex)
{
    return aChoice->IsSelected(aIndex);
}

JNIEXPORT jboolean JNICALL Java_javax_microedition_lcdui_List__1isSelected
(JNIEnv*,jobject,jint aList,jint aToolkit,jint aIndex)
{
    MMIDList* list = MIDUnhandObject<MMIDList>(aList);
    return (jboolean)JavaUnhand<CMIDToolkit>(aToolkit)->Execute(&IsSelected,list,(TInt)aIndex);
}

LOCAL_C void SelectElementL(MMIDList* aChoice,TInt aIndex,TBool aSelected)
{
    aChoice->SelectElementL(aIndex,aSelected);
}

JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_List__1select
(JNIEnv*,jobject,jint aList,jint aToolkit,jint aIndex,jboolean aSelected)
{
    MMIDList* list = MIDUnhandObject<MMIDList>(aList);
    return JavaUnhand<CMIDToolkit>(aToolkit)->ExecuteTrap(&SelectElementL,list,(TInt)aIndex,(TBool)aSelected);
}

LOCAL_C void SetFontL(MMIDList* aChoice,TInt aIndex, MMIDFont* aFont)
{
    aChoice->SetFontL(aIndex,aFont);
}
JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_List__1setFont
(JNIEnv*,jobject,jint aItem,jint aToolkit,jint aIndex,jint aFont)
{
    MMIDList* choice = MIDUnhandObject<MMIDList>(aItem);
    MMIDFont* font = MIDUnhandObject<MMIDFont>(aFont);
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    return toolkit->ExecuteTrap(&SetFontL,choice,(TInt)aIndex,font);
}

LOCAL_C void SetFitPolicyL(MMIDList* aChoice,TInt aFitPolicy)
{
    aChoice->SetFitPolicyL(aFitPolicy);
}
JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_List__1setFitPolicy
(JNIEnv*,jobject,jint aItem,jint aToolkit,jint aFitPolicy)
{
    MMIDList* choice = MIDUnhandObject<MMIDList>(aItem);
    return JavaUnhand<CMIDToolkit>(aToolkit)->ExecuteTrap(&SetFitPolicyL,choice,(TInt)aFitPolicy);
}
