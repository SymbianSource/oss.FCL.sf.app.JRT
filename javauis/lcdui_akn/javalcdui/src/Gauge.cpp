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
#include "javax_microedition_lcdui_Gauge.h"

LOCAL_C void CreateL(CMIDToolkit* aToolkit,TInt* aHandle,jobject aGauge,const TDesC* aLabel,TBool aInteractive,TInt aMaxValue,TInt aInitialValue)
{
    MMIDGauge* gauge = aToolkit->ComponentFactory()->CreateGaugeL(*aLabel,aInteractive,aMaxValue,aInitialValue);
    CleanupDisposePushL(gauge);
    *aHandle = aToolkit->RegisterComponentL(gauge, aGauge);
    CleanupPopComponent(gauge);
}

JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_Gauge__1create
(JNIEnv* aJni,jobject,jint aToolkit,jobject aGauge,jstring aLabel,jboolean aInteractive,
 jint aMaxValue,jint aIntialValue)
{
    jobject gauge = aJni->NewWeakGlobalRef(aGauge);
    if (gauge == 0)
        return KErrNoMemory;
    //
    CMIDToolkit* toolkit =JavaUnhand<CMIDToolkit>(aToolkit);
    RJString label(*aJni,aLabel);
    TInt h=0;
    TInt err = toolkit->ExecuteTrap(&CreateL,toolkit,&h,gauge,(const TDesC*)&label,(TBool)aInteractive,(TInt)aMaxValue,(TInt)aIntialValue);
    if (err!=KErrNone)
    {
        aJni->DeleteWeakGlobalRef((jweak)gauge);
        return err;
    }
    return h;
}

LOCAL_C jint GetValue(MMIDGauge* aGauge)
{
    return aGauge->GetValue();
}
JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_Gauge__1getValue(JNIEnv*,jobject,jint aToolkit,jint aItem)
{
    MMIDGauge* gauge = MIDUnhandObject<MMIDGauge>(aItem);
    return JavaUnhand<CMIDToolkit>(aToolkit)->Execute(GetValue,gauge);
}

LOCAL_C void SetValueL(MMIDGauge* aGauge,TInt aValue)
{
    aGauge->SetValueL(aValue);
}
JNIEXPORT int JNICALL Java_javax_microedition_lcdui_Gauge__1setValue
(JNIEnv*,jobject,jint aToolkit,jint aItem,jint aValue)
{
    MMIDGauge* gauge = MIDUnhandObject<MMIDGauge>(aItem);
    return JavaUnhand<CMIDToolkit>(aToolkit)->ExecuteTrap(&SetValueL,gauge,(TInt)aValue);
}

LOCAL_C void InvokeSetMaxValueL(jint aHandle, jint aMaxValue, jint aValue)
{
    MMIDGauge* gauge = MIDUnhandObject<MMIDGauge>(aHandle);

    gauge->SetMaxValueL(aMaxValue);
    gauge->SetValueL(aValue);
}

JNIEXPORT int JNICALL Java_javax_microedition_lcdui_Gauge__1setMaxValue
(JNIEnv*,jobject,jint aToolkit,jint aGauge,jint aMaxValue, jint aValue)
{
    return JavaUnhand<CMIDToolkit>(aToolkit)->ExecuteTrap(&InvokeSetMaxValueL,aGauge, aMaxValue, aValue);
}
