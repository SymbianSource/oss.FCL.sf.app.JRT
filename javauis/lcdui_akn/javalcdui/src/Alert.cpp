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
#include "javax_microedition_lcdui_Alert.h"

struct TAlertCreate
{
    jweak iRef;
    jint  iDisplayable;
    jint  iType;
    TPtrC iText;
    jint  iImage;
};

struct TAlertAttribs
{
    jint    iHandle;
    TSize   iSize;
};

LOCAL_C void CreateAlertL(CMIDToolkit* aToolkit, TAlertCreate* aCreate, TAlertAttribs* aAttribs)
{
    MMIDComponentFactory& factory = *aToolkit->ComponentFactory();
    MMIDDisplayable* displayable = MIDUnhand<MMIDDisplayable>(aCreate->iDisplayable);
    MMIDImage* image = MIDUnhand<MMIDImage>(aCreate->iImage);
    MMIDAlert* alert = factory.CreateAlertL(*displayable, (MMIDAlert::TAlertType)aCreate->iType, aCreate->iText, image);
    CleanupDisposePushL(alert);
    aAttribs->iHandle = aToolkit->RegisterComponentL(alert, aCreate->iRef);
    CleanupPopComponent(alert);
    aCreate->iRef = 0;      // register takes ownership
    aAttribs->iSize = displayable->ContentSize();
}

JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_Alert__1create
(
    JNIEnv* aJni,
    jobject aAlert,
    jint    aToolkit,
    jint    aDisplayable,
    jstring aString,
    jint    aImage,
    jint    aType,
    jintArray aCreateAlertReturn
)
{
    RJString string(*aJni,aString);

    TAlertCreate create;
    create.iDisplayable=aDisplayable;
    create.iType=aType;
    create.iText.Set(string);
    create.iImage=aImage;
    create.iRef=aJni->NewWeakGlobalRef(aAlert);

    TAlertAttribs attribs;
    attribs.iHandle=0;

    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    jint error;

    if (create.iRef)
    {
        error = toolkit->ExecuteTrap(CreateAlertL, toolkit, &create, &attribs);
    }
    else
    {
        error = KErrNoMemory;
    }

    if (create.iRef)   // should be cleared by CreateAlertL if successful
    {
        aJni->DeleteWeakGlobalRef(create.iRef);
    }

    jint arrayReturn[3];
    arrayReturn[0] = attribs.iHandle;
    arrayReturn[1] = attribs.iSize.iWidth;
    arrayReturn[2] = attribs.iSize.iHeight;
    aJni->SetIntArrayRegion(aCreateAlertReturn, 0, 3, arrayReturn);

    return error;
}

LOCAL_C void SetStringL(MMIDAlert* aAlert,const TDesC* aString)
{
    aAlert->SetStringL(*aString);
}

JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_Alert__1setString
(JNIEnv* aJni,jobject,jint aAlert,jint aToolkit,jstring aString)
{
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    MMIDAlert* alert = MIDUnhandObject<MMIDAlert>(aAlert);
    RJString string(*aJni,aString);
    return toolkit->ExecuteTrap(&SetStringL,alert,(const TDesC*)&string);
}

LOCAL_C void SetImageL(MMIDAlert* aAlert,MMIDImage* aImage)
{
    aAlert->SetImageL(aImage);
}
JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_Alert__1setImage
(JNIEnv*,jobject,jint aAlert,jint aToolkit,jint aImage)
{
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    MMIDAlert* alert = MIDUnhandObject<MMIDAlert>(aAlert);
    MMIDImage* image = MIDUnhandObject<MMIDImage>(aImage);
    return toolkit->ExecuteTrap(&SetImageL,alert,image);
}

LOCAL_C void SetIndicatorL(MMIDAlert* aAlert,MMIDGauge* aGauge)
{
    aAlert->SetIndicatorL(aGauge);
}
JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_Alert__1setIndicator
(JNIEnv*,jobject,jint aAlert,jint aToolkit,jint aGauge)
{
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    MMIDAlert* alert = MIDUnhandObject<MMIDAlert>(aAlert);
    MMIDGauge* gauge = MIDUnhandObject<MMIDGauge>(aGauge);
    return toolkit->ExecuteTrap(&SetIndicatorL,alert,gauge);
}

LOCAL_C void SetTypeL(MMIDAlert* aAlert,TInt aType)
{
    aAlert->SetTypeL((MMIDAlert::TAlertType)aType);
}
JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_Alert__1setType
(JNIEnv*,jobject,jint aAlert,jint aToolkit,jint aType)
{
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    MMIDAlert* alert = MIDUnhandObject<MMIDAlert>(aAlert);
    return toolkit->ExecuteTrap(&SetTypeL,alert,(TInt)aType);
}

LOCAL_C jint DefaultTimeout(MMIDAlert* aAlert)
{
    return aAlert->DefaultTimeout();
}
JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_Alert__1getDefaultTimeout
(JNIEnv*,jobject,jint aAlert,jint aToolkit)
{
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    MMIDAlert* alert = MIDUnhandObject<MMIDAlert>(aAlert);
    return toolkit->Execute(&DefaultTimeout,alert);
}

LOCAL_C void SetTimeoutL(MMIDAlert* aAlert,TInt aTimeout)
{
    aAlert->SetTimeoutL(aTimeout);
}
JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_Alert__1setTimeout
(JNIEnv*,jobject,jint aAlert,jint aToolkit,jint aTimeout)
{
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    MMIDAlert* alert = MIDUnhandObject<MMIDAlert>(aAlert);
    return toolkit->ExecuteTrap(&SetTimeoutL,alert,(TInt)aTimeout);
}

LOCAL_C void SetModalL(MMIDAlert* aAlert)
{
    aAlert->SetModalL();
}
JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_Alert__1setModal
(JNIEnv*,jobject,jint aAlert,jint aToolkit)
{
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    MMIDAlert* alert = MIDUnhandObject<MMIDAlert>(aAlert);
    return toolkit->ExecuteTrap(&SetModalL,alert);
}

LOCAL_C jboolean IsModal(MMIDAlert* aAlert)
{
    return (jboolean)aAlert->IsModal();
}
JNIEXPORT jboolean JNICALL Java_javax_microedition_lcdui_Alert__1isModal(JNIEnv*, jobject,jint aAlert,jint aToolkit)
{
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    MMIDAlert* alert = MIDUnhandObject<MMIDAlert>(aAlert);
    return toolkit->Execute(&IsModal,alert);
}
