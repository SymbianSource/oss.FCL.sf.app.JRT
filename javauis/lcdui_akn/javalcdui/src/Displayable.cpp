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
#include "javax_microedition_lcdui_Displayable.h"

LOCAL_C void CreateContainerL(CMIDToolkit* aToolkit, jweak aRef, jint* aHandle)
{
    MMIDDisplayable* displayable = aToolkit->ComponentFactory()->CreateDisplayableL();
    CleanupDisposePushL(displayable);
    *aHandle = aToolkit->RegisterComponentL(displayable, aRef);
    CleanupPopComponent(displayable);
}

JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_Displayable__1createContainer
(
    JNIEnv* aJni,
    jobject aDisplayable,
    jint    aToolkit
)
{
    jint err    = KErrNoMemory;
    jint handle = 0;
    jweak ref = aJni->NewWeakGlobalRef(aDisplayable);
    if (ref != 0)
    {
        CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
        err = toolkit->ExecuteTrap(CreateContainerL, toolkit, ref, &handle);
        if (err!=KErrNone)
        {
            aJni->DeleteWeakGlobalRef(ref);
            return err;
        }
    }
    return handle;
}

LOCAL_C void InvokeAddCommandL(jint aDisplayable, jint aCommand)
{
    MMIDDisplayable* displayable = MIDUnhandObject<MMIDDisplayable>(aDisplayable);
    MMIDCommand* command = MIDUnhandObject<MMIDCommand>(aCommand);
    displayable->AddCommandL(command);
}

JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_Displayable__1addCommand
(JNIEnv*,jobject,jint aDisplayable,jint aToolkit, jint aCommand)
{
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    return toolkit->ExecuteTrap(&InvokeAddCommandL, aDisplayable,aCommand);
}

LOCAL_C void InvokeRemoveCommand(jint aDisplayable,jint aCommand)
{
    MMIDDisplayable* displayable = MIDUnhandObject<MMIDDisplayable>(aDisplayable);
    MMIDCommand* command = MIDUnhandObject<MMIDCommand>(aCommand);
    displayable->RemoveCommand(command);
}

JNIEXPORT void JNICALL Java_javax_microedition_lcdui_Displayable__1removeCommand
(JNIEnv*,jobject,jint aToolkit,jint aDisplayable, jint aCommand)
{
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    toolkit->ExecuteTrap(&InvokeRemoveCommand,aDisplayable, aCommand);
}

LOCAL_C void InvokeSetTitleL(jint aDisplayable, const TDesC* aTitle)
{
    MMIDDisplayable* displayable = MIDUnhandObject<MMIDDisplayable>(aDisplayable);
    displayable->SetTitleL(aTitle);
}

JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_Displayable__1setTitle
(JNIEnv* aEnv,jobject,jint aDisplayable,jint aToolkit,jstring aString)
{
    const RJString titleDes(*aEnv, aString);
    const TDesC* pTitle = NULL;
    if (NULL != aString)
    {
        pTitle = &titleDes;
    }
    return JavaUnhand<CMIDToolkit>(aToolkit)->ExecuteTrap(&InvokeSetTitleL, aDisplayable, pTitle);
}

LOCAL_C void InvokeSetTickerL(jint aDisplayable,jint aTicker)
{
    MMIDDisplayable* displayable = MIDUnhandObject<MMIDDisplayable>(aDisplayable);
    MMIDTicker* ticker = MIDUnhandObject<MMIDTicker>(aTicker);
    displayable->SetTickerL(ticker);
}

JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_Displayable__1setTicker
(JNIEnv*,jobject,jint aDisplayable,jint aToolkit,jint aTicker)
{
    return JavaUnhand<CMIDToolkit>(aToolkit)->ExecuteTrap(&InvokeSetTickerL,aDisplayable,aTicker);
}

LOCAL_C void InvokeContentSize(jint aDisplayable,TSize* aSize)
{
    MMIDDisplayable* displayable = MIDUnhandObject<MMIDDisplayable>(aDisplayable);
    *aSize = displayable->ContentSize();
}

JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_Displayable__1getContentSize
(JNIEnv*,jobject,jint aDisplayable,jint aToolkit)
{
    TSize size;
    JavaUnhand<CMIDToolkit>(aToolkit)->ExecuteV(&InvokeContentSize, aDisplayable, &size);
    return (size.iWidth << 16) | (size.iHeight & 0x0000ffff);
}

LOCAL_C void InvokeSetCommandListenerExistence(jint aDisplayable, jboolean aExistence)
{
    MMIDDisplayable* displayable = MIDUnhandObject<MMIDDisplayable>(aDisplayable);
    displayable->SetCommandListenerExistence(aExistence);
}

JNIEXPORT void JNICALL Java_javax_microedition_lcdui_Displayable__1setCommandListenerExistence
(JNIEnv*, jobject, jint aDisplayable, jint aToolkit, jboolean aExistence)
{
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    toolkit->ExecuteV(&InvokeSetCommandListenerExistence, aDisplayable, aExistence);
}

LOCAL_C jboolean InvokeSoftKeyLabelLocation(MMIDDisplayable* aDisplayable,
        jint softKeyId,
        jint* aPositionArray,
        jint* aSizeArray)
{
    TSize size;
    TPoint position;
    TBool softkeyExists = aDisplayable->SoftKeyLabelLocation(softKeyId, position, size);
    aPositionArray[0] = position.iX;
    aPositionArray[1] = position.iY;
    aSizeArray[0] = size.iWidth;
    aSizeArray[1] = size.iHeight;
    return softkeyExists;
}

JNIEXPORT jboolean JNICALL Java_javax_microedition_lcdui_Displayable__1getSoftKeyLabelLocation
(JNIEnv* aEnv,jobject, jint aToolkit, jint aDisplayable, jint softKeyId, jintArray aPosition, jintArray aSize)
{
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    MMIDDisplayable* displayable  = MIDUnhand<MMIDDisplayable>(aDisplayable);
    jint positionArray[2];
    jint sizeArray[2];
    jboolean softkeyExists = toolkit->Execute(&InvokeSoftKeyLabelLocation,
                             displayable,
                             softKeyId,
                             positionArray,
                             sizeArray);

    aEnv->SetIntArrayRegion(aPosition, 0, 2, positionArray);
    aEnv->SetIntArrayRegion(aSize,     0, 2, sizeArray);
    return softkeyExists;
}

LOCAL_C jint InvokeSoftKeyLabelAnchor(MMIDDisplayable* aDisplayable, jint softKeyId)
{
    return aDisplayable->SoftKeyLabelAnchor(softKeyId);
}

JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_Displayable__1getSoftKeyLabelAnchor
(JNIEnv*,jobject, jint aToolkit, jint aDisplayable, jint softKeyId)
{
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    MMIDDisplayable* displayable  = MIDUnhand<MMIDDisplayable>(aDisplayable);

    return toolkit->Execute(&InvokeSoftKeyLabelAnchor, displayable, softKeyId);
}

LOCAL_C void GetKeyboardTypeName(MMIDUtils *aUtils, TDes* aText)
{
    aUtils->GetKeyboardTypeName(aText);
}

JNIEXPORT
jstring JNICALL Java_javax_microedition_lcdui_Displayable__1getKeyboardTypeName
(JNIEnv* aEnv, jclass, jint aToolkit)
{
    TBuf<KMaxName> keyboardTypeName;
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    toolkit->ExecuteV(&GetKeyboardTypeName,
                      toolkit->Utils(), (TDes*)&keyboardTypeName);
    jstring str = CreateJavaString(*aEnv, keyboardTypeName);
    return str;
}

LOCAL_C TInt GetKeyScanCode(MMIDUtils *aUtils)
{
    return aUtils->GetKeyScanCode();
}

JNIEXPORT
jint JNICALL Java_javax_microedition_lcdui_Displayable__1getKeyScanCode
(JNIEnv* /*aEnv*/, jclass, jint aToolkit)
{
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    return toolkit->Execute(&GetKeyScanCode, toolkit->Utils());
}

LOCAL_C TInt GetKeyModifier(MMIDUtils *aUtils)
{
    return aUtils->GetKeyModifier();
}

JNIEXPORT
jint JNICALL Java_javax_microedition_lcdui_Displayable__1getKeyModifier
(JNIEnv* /*aEnv*/, jclass, jint aToolkit)
{
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    return toolkit->Execute(&GetKeyModifier, toolkit->Utils());
}
