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
#include "javax_microedition_lcdui_Form.h"

struct TFormCreate
{
    jweak iRef;
    jint  iDisplayable;
};

struct TFormAttribs
{
    jint  iHandle;
    TSize iSize;
};

LOCAL_C void CreateFormL(CMIDToolkit* aToolkit, TFormCreate* aCreate, TFormAttribs* aAttribs)
{
    MMIDDisplayable* displayable = MIDUnhand<MMIDDisplayable>(aCreate->iDisplayable);
    MMIDForm* form = aToolkit->ComponentFactory()->CreateFormL(*displayable);
    CleanupDisposePushL(form);
    aAttribs->iHandle = aToolkit->RegisterComponentL(form, aCreate->iRef);
    CleanupPopComponent(form);
    aCreate->iRef = 0;
    aAttribs->iSize.iWidth  = form->Width();
    aAttribs->iSize.iHeight = form->Height();
}

JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_Form__1create
(
    JNIEnv* aJni,
    jobject aForm,
    jint    aToolkit,
    jint    aDisplayable,
    jintArray aFormAttribReturn
)
{
    TFormCreate  create;
    TFormAttribs attribs;

    create.iDisplayable = aDisplayable;
    create.iRef = aJni->NewWeakGlobalRef(aForm);
    attribs.iHandle = 0;

    jint error;
    if (create.iRef)
    {
        CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
        error = toolkit->ExecuteTrap(&CreateFormL, toolkit, &create, &attribs);
    }
    else
    {
        error = KErrNoMemory;
    }

    if (create.iRef)
    {
        // CreateFormL clears if successful
        aJni->DeleteWeakGlobalRef(create.iRef);
    }

    jint attribArray[3];
    attribArray[0]=attribs.iHandle;
    attribArray[1]=attribs.iSize.iWidth;
    attribArray[2]=attribs.iSize.iHeight;
    aJni->SetIntArrayRegion(aFormAttribReturn, 0, 3, attribArray);

    return error;
}

LOCAL_C void RefreshItemL(MMIDForm* aForm,TInt aIndex)
{
    aForm->RefreshItemL(aIndex);
}
JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_Form__1refresh
(JNIEnv*,jobject,jint aForm,jint aToolkit,jint aIndex)
{
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    MMIDForm* form = MIDUnhandObject<MMIDForm>(aForm);
    return toolkit->ExecuteTrap(&RefreshItemL,form,(TInt)aIndex);
}

LOCAL_C TBool IsVisible(MMIDForm* aForm,TInt aIndex)
{
    return aForm->IsItemVisible(aIndex);
}
JNIEXPORT jboolean JNICALL Java_javax_microedition_lcdui_Form__1isVisible
(JNIEnv*,jobject,jint aForm,jint aToolkit,jint aIndex)
{
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    MMIDForm* form = MIDUnhandObject<MMIDForm>(aForm);
    return (jboolean)toolkit->Execute(&IsVisible,form,(TInt)aIndex);
}

LOCAL_C void InsertItemL(MMIDForm* aForm,MMIDItem* aItem,TInt aIndex)
{
    aForm->InsertItemL(*aItem,aIndex);
}
JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_Form__1insert
(JNIEnv*,jobject,jint aForm,jint aToolkit,jint aIndex,jint aItem)
{
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    MMIDForm* form = MIDUnhandObject<MMIDForm>(aForm);
    MMIDItem* item = MIDUnhandObject<MMIDItem>(aItem);
    return toolkit->ExecuteTrap(&InsertItemL,form,item,(TInt)aIndex);
}

LOCAL_C void SetItemL(MMIDForm* aForm,MMIDItem* aItem,TInt aIndex)
{
    aForm->SetItemL(*aItem,aIndex);
}
JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_Form__1set
(JNIEnv*,jobject,jint aForm,jint aToolkit,jint aIndex,jint aItem)
{
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    MMIDForm* form = MIDUnhandObject<MMIDForm>(aForm);
    MMIDItem* item = MIDUnhandObject<MMIDItem>(aItem);
    return toolkit->ExecuteTrap(&SetItemL,form,item,(TInt)aIndex);
}

LOCAL_C void SetAllItemsL(MMIDForm* aForm,RArray<MMIDItem*>* aItems)
{
    aForm->SetAllItemsL(*aItems);
}
JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_Form__1setAll
(JNIEnv* aJni,jobject,jint aForm,jint aToolkit,jintArray aItems)
{
    jint* items = aJni->GetIntArrayElements(aItems,0);
    TInt count = aJni->GetArrayLength(aItems);
    RArray<MMIDItem*> MIDItems(count);
    TInt ret = KErrNone;
    for (TInt i=0; i<count; i++)
        ret = MIDItems.Append(MIDUnhandObject<MMIDItem>(items[i]));
    aJni->ReleaseIntArrayElements(aItems,items,JNI_ABORT);
    if (ret == KErrNone)
    {
        CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
        MMIDForm* form = MIDUnhandObject<MMIDForm>(aForm);
        ret =  toolkit->ExecuteTrap(&SetAllItemsL,form,&MIDItems);
    }
    MIDItems.Close();
    return ret;
}

LOCAL_C void DeleteItemL(MMIDForm* aForm,TInt aIndex)
{
    aForm->DeleteItemL(aIndex);
}
JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_Form__1delete
(JNIEnv*,jobject,jint aForm,jint aToolkit,jint aIndex)
{
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    MMIDForm* form = MIDUnhandObject<MMIDForm>(aForm);
    return toolkit->ExecuteTrap(&DeleteItemL,form,(TInt)aIndex);
}

LOCAL_C void DeleteAllItemsL(MMIDForm* aForm)
{
    aForm->DeleteAllItemsL();
}
JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_Form__1deleteAll
(JNIEnv*,jobject,jint aForm,jint aToolkit)
{
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    MMIDForm* form = MIDUnhandObject<MMIDForm>(aForm);
    return toolkit->ExecuteTrap(&DeleteAllItemsL,form);
}

LOCAL_C void NotifyStateChanged(MMIDEnv* aEnv,MMIDForm* aForm,TInt aIndex)
{
    aEnv->PostJavaEvent(*aForm,EDisplayable,aIndex);
}
JNIEXPORT void JNICALL Java_javax_microedition_lcdui_Form__1notifyStateChanged
(JNIEnv*,jobject,jint aForm,jint aToolkit,jint aIndex)
{
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    MMIDForm* form = MIDUnhandObject<MMIDForm>(aForm);
    toolkit->ExecuteV(&NotifyStateChanged,toolkit->Env(),form,(TInt)aIndex);
}

LOCAL_C void SetCurrentItemL(MMIDForm* aForm,TInt aIndex)
{
    aForm->SetCurrentItemL(aIndex);
}
JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_Form__1setCurrentItem
(JNIEnv*,jobject,jint aForm,jint aToolkit,jint aIndex)
{
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    MMIDForm* form = MIDUnhandObject<MMIDForm>(aForm);
    return toolkit->ExecuteTrap(&SetCurrentItemL,form,(TInt)aIndex);
}

LOCAL_C void GetFormSize(MMIDForm* aForm, TSize* aSize)
{
    aSize->iWidth  = aForm->Width();
    aSize->iHeight = aForm->Height();
}

JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_Form__1getFormSize
(JNIEnv*,jobject,jint aForm,jint aToolkit)
{
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    if (!toolkit)
    {
        return KErrGeneral;
    }

    MMIDForm* form = MIDUnhandObject<MMIDForm>(aForm);
    TSize size;
    toolkit->ExecuteV(&GetFormSize, form, &size);
    return (size.iWidth << 16) | (size.iHeight & 0x0000ffff);
}
