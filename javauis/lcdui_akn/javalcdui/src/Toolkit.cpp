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
#include "javax_microedition_lcdui_Toolkit.h"
#include "lcdgr.h"
#include "CMIDEvent.h"
#include <mevents.h>
#include <CJavaEventServer.h>

#ifdef LCDUI_DEBUG_ON
#include <jdebug.h>
#define LCDUI_DEBUG(msg) DEBUG(msg)
#define LCDUI_DEBUG_INT(msg,x) DEBUG_INT(msg,x)
#define LCDUI_DEBUG_INT2(msg,x,y) DEBUG_INT2(msg, x, y)
#else
#define LCDUI_DEBUG(msg)
#define LCDUI_DEBUG_INT(msg, x)
#define LCDUI_DEBUG_INT2(msg, x, y)
#endif

/**
 * Flag indexes and total number of flags
 * @since S60 9.2
 */
enum TFlags
{
    EFlagNgaEnabledIndex = 0,
    EFlagCount
};

//
// Close the Java peers handle onto the toolkit.
//
// This stops any remaining events from being posted or dispatched.
//
// If there are any events on the queue the toolkit will continue
// to exists until the last event has been dequeued.
//
LOCAL_C void DisposeToolkit(JNIEnv& aJni, TInt aHandle)
{
    CMIDToolkit::Unhand(aHandle).Dispose(aJni);
}

JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_Toolkit__1create
(
    JNIEnv* aJni,
    jobject aToolkit,
    jint aServer,
    jstring aName,
    jint aUid,
    jstring aHomeDir,
    jobjectArray aAttributes,
    jintArray aFlags
)
{
    RJString name(*aJni,aName);
    RJString homeDir(*aJni,aHomeDir);
    RPointerArray<HBufC>* attribArray = NULL;

    jint error = 0;
    RPointerArray<HBufC> attributes;
    if (aAttributes)
    {
        jint count = aJni->GetArrayLength(aAttributes);
        jint index = 0;
        ASSERT(count%2 == 0);
        while ((index<count) && (0 == error))
        {
            jstring jniString = NULL;

            jniString = (jstring)aJni->GetObjectArrayElement(aAttributes, index);
            {
                RJString rjString(*aJni,jniString);

                HBufC* element = NULL;
                TRAP(error, element = rjString.AllocL())
                if (error == KErrNone)
                {
                    error = attributes.Append(element);
                    if (error) delete element;
                }
            }
            // Delete only after rjString has been destructed
            aJni->DeleteLocalRef(jniString);

            ++index;
        }
    }
    if (error)
    {
        attributes.ResetAndDestroy();
        attributes.Close();
        return error;
    }
    attribArray=&attributes;
    jint handle = CMIDToolkit::New(*aJni, aToolkit, aServer, name, aUid, homeDir, attribArray);
    attributes.ResetAndDestroy();
    attributes.Close();

    jint flags[EFlagCount];
#ifdef RD_JAVA_NGA_ENABLED
    flags[EFlagNgaEnabledIndex] = 1;
#else // !RD_JAVA_NGA_ENABLED
    flags[EFlagNgaEnabledIndex] = 0;
#endif // RD_JAVA_NGA_ENABLED
    aJni->SetIntArrayRegion(aFlags, 0, EFlagCount, &flags[0]);
    return handle;
}

JNIEXPORT void JNICALL Java_javax_microedition_lcdui_Toolkit__1dispose(JNIEnv* aJni,jobject,jint aToolkit)
{
    DisposeToolkit(*aJni, aToolkit);
}

LOCAL_C void InvokeDisposeObject(CMIDToolkit* aToolkit, jint aObject)
{
    MMIDComponent* object = MIDUnhandObject<MMIDComponent>(aObject);
    aToolkit->DisposeObject(object);
}

extern void DoDisposeObject(CMIDToolkit& aToolkit, jint aObject)
{
    aToolkit.ExecuteV(&InvokeDisposeObject, &aToolkit, aObject);
}

JNIEXPORT void JNICALL Java_javax_microedition_lcdui_Toolkit__1disposeObject
(JNIEnv*,jobject,jint aToolkit,jint aObject)
{
    LCDUI_DEBUG_INT("Toolkit_disposeObject(%x)", aObject);
    DoDisposeObject(*JavaUnhand<CMIDToolkit>(aToolkit), aObject);
}

void InvokeSetCurrentL(CMIDToolkit* aToolkit, MMIDDisplayable* aDisplayable)
{
    LCDUI_DEBUG_INT2("Toolkit_setCurrent(%x, [%d])", (TInt)aDisplayable, (TInt)aDisplayable->Component()->Type());
    aToolkit->SetCurrentL(aDisplayable);
}

JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_Toolkit__1setCurrent
(
    JNIEnv*,
    jobject,
    jint aToolkitHandle,
    jint aDisplayable
)
{
    CMIDToolkit* toolkit =JavaUnhand<CMIDToolkit>(aToolkitHandle);
    MMIDDisplayable* displayable = MIDUnhandObject<MMIDDisplayable>(aDisplayable);
    return toolkit->ExecuteTrap(&InvokeSetCurrentL,toolkit,displayable);
}

void InvokeSetForeground(CMIDToolkit* aToolkit, jboolean aForeground)
{
    LCDUI_DEBUG_INT("Toolkit_requestForeground(%d)", (TInt)aForeground);
    if (aForeground)
    {
        aToolkit->BringToForeground();
    }
    else
    {
        aToolkit->SendToBackground();
    }
}

JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_Toolkit__1setForeground
(
    JNIEnv*,
    jobject,
    jint        aToolkit,
    jboolean    aForeground
)
{
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    toolkit->ExecuteV(&InvokeSetForeground,toolkit, aForeground);
    return KErrNone;
}

void InvokeSetCanvasAssumedSize(CMIDToolkit* aToolkit, jint aWidth, jint aHeight)
{
    CMIDEnv& env = *(CMIDEnv*)aToolkit->Env();
    env.SetCanvasAssumedSize(TSize(aWidth, aHeight));
}
JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_Toolkit__1setCanvasSize
(
    JNIEnv*, jobject, jint aToolkit, jint aWidth, jint aHeight
)
{
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    toolkit->ExecuteV(InvokeSetCanvasAssumedSize, toolkit, aWidth, aHeight);
    return KErrNone;
}

void InvokeActivateL(CMIDToolkit* aToolkit)
{
    aToolkit->ActivateL();
}

JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_Toolkit__1activate(JNIEnv*, jobject, jint aToolkit)
{
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    return toolkit->ExecuteTrap(&InvokeActivateL, toolkit);
}

extern "C" void pss();

jint InvokePostEvent(CMIDToolkit* aToolkit, jint aSource, jint aType, jint aEvent)
{
    LCDUI_DEBUG_INT2("InvokePostEvent(%x,%d)", aSource, aEvent);
    //pss();
    MMIDComponent* source = MIDUnhand<MMIDComponent>(aSource);
    switch (aType)
    {
    case EItem:
        return aToolkit->PostItemEvent(*source, (TEventType)aEvent, 0, 0, 0);
    case EDisplayable:
        return aToolkit->PostDisplayableEvent(*source, (TEventType)aEvent, 0, 0);
    case EMIDlet:
        return aToolkit->PostDisplayEvent((TEventType)aEvent);
    case ECanvasGraphicsItemPainterEvent:
        return aToolkit->PostCanvasGraphicsItemPainterEvent(*source, (TEventType)aEvent, 0, 0);
    default:
        ASSERT(EFalse);
    }
    return KErrArgument;
}

JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_Toolkit__1postEvent
(
    JNIEnv* /*aEnv*/,
    jobject /*aToolkit*/,
    jint    aHandle,
    jint    aSource,
    jint    aType,
    jint    aEvent
)
{
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aHandle);
    return toolkit->Execute(&InvokePostEvent, toolkit, aSource, aType, aEvent);
}
