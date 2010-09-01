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
#include "javax_microedition_lcdui_Ticker.h"


LOCAL_C void CreateL(CMIDToolkit* aToolkit,TInt* aHandle,const TDesC* aText)
{
    MMIDTicker* ticker = aToolkit->ComponentFactory()->CreateTickerL(*aText);
    CleanupDisposePushL(ticker);
    *aHandle = aToolkit->RegisterComponentL(ticker, NULL);
    CleanupPopComponent(ticker);
}
JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_Ticker__1create(JNIEnv* aJni,jobject,jint aToolkit,jstring aText)
{
    CMIDToolkit* toolkit =JavaUnhand<CMIDToolkit>(aToolkit);
    RJString text(*aJni,aText);
    TInt h=0;
    TInt err = toolkit->ExecuteTrap(&CreateL,toolkit,&h,(const TDesC*)&text);
    return err == KErrNone ? h : err;
}

LOCAL_C void SetTextL(MMIDTicker* aTicker,const TDesC* aText)
{
    aTicker->SetTextL(*aText);
}
JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_Ticker__1setText
(JNIEnv* aEnv,jobject,jint aHandle,jint aToolkit,jstring aString)
{
    MMIDTicker* ticker = MIDUnhandObject<MMIDTicker>(aHandle);
    RJString string(*aEnv,aString);
    return JavaUnhand<CMIDToolkit>(aToolkit)->ExecuteTrap(&SetTextL,ticker,(const TDesC*)&string);
}
