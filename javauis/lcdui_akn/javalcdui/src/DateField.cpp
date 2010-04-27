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
#include "javax_microedition_lcdui_DateField.h"

LOCAL_C void CreateL(CMIDToolkit* aToolkit,TInt* aHandle,jobject aDateField,const TDesC* aLabel,TInt aFieldType)
{
    MMIDDateField* dateField = aToolkit->ComponentFactory()->CreateDateFieldL(*aLabel,(MMIDDateField::TFieldType)aFieldType);
    CleanupDisposePushL(dateField);
    *aHandle = aToolkit->RegisterComponentL(dateField, aDateField);
    CleanupPopComponent(dateField);
}
JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_DateField__1create
(JNIEnv* aJni,jobject,jint aToolkit,jobject aDateField,jstring aLabel,jint aFieldType)
{
    jobject dateField = aJni->NewWeakGlobalRef(aDateField);
    if (dateField == 0)
        return KErrNoMemory;
    //
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    RJString label(*aJni,aLabel);
    TInt h = 0;
    TInt err = toolkit->ExecuteTrap(&CreateL,toolkit,&h,dateField,(const TDesC*)&label,(TInt)aFieldType);
    if (err!=KErrNone)
    {
        aJni->DeleteWeakGlobalRef((jweak)dateField);
        return err;
    }
    return h;
}

LOCAL_C void Date(MMIDDateField* aItem,TTime* aTime)
{
    *aTime = aItem->Date();
}
JNIEXPORT jlong JNICALL Java_javax_microedition_lcdui_DateField__1getDate
(JNIEnv *,jobject,jint aToolkit,jint aItem)
{
    MMIDDateField* item = MIDUnhandObject<MMIDDateField>(aItem);
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    TTime time;
    toolkit->ExecuteV(&Date,item,&time);
    return JavaEpocTime::CreateJavaTime(time);
}

LOCAL_C void SetDate(MMIDDateField* aItem,TTime* aTime)
{
    aItem->SetDate(*aTime);
}
JNIEXPORT void JNICALL Java_javax_microedition_lcdui_DateField__1setDate
(JNIEnv *,jobject,jint aToolkit,jint aItem,jlong aTime)
{
    MMIDDateField* item = MIDUnhandObject<MMIDDateField>(aItem);
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    TTime epocTime = JavaEpocTime::CreateEpocTTime(aTime);
    toolkit->ExecuteV(&SetDate,item,&epocTime);
}

void InvokeSetInputModeL(jint aDateField, jint aInputMode)
{
    MMIDDateField* dateField = MIDUnhandObject<MMIDDateField>(aDateField);
    dateField->SetInputModeL((MMIDDateField::TInputMode)aInputMode);
}

JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_DateField__1setInputMode
(JNIEnv*,jobject,jint aToolkit, jint aItem, jint aInputMode)
{
    //
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    return toolkit->ExecuteTrap(InvokeSetInputModeL, aItem, aInputMode);
}

LOCAL_C void SetUninitialized(MMIDDateField* aItem)
{
    aItem->SetUninitialized();
}
JNIEXPORT void JNICALL Java_javax_microedition_lcdui_DateField__1setUninitialized
(JNIEnv*,jobject,jint aToolkit,jint aDateField)
{
    MMIDDateField* item = MIDUnhandObject<MMIDDateField>(aDateField);
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    toolkit->ExecuteV(&SetUninitialized,item);
}
