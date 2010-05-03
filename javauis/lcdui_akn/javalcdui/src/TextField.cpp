/*
* Copyright (c) 1999 - 2003 Nokia Corporation and/or its subsidiary(-ies).
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


#include "MIDUtils.h"
#include "CMIDToolkit.h"
#include "javax_microedition_lcdui_TextField.h"


LOCAL_C void CreateL(CMIDToolkit* aToolkit,TInt* aHandle,jobject aTextComponent,const TDesC* aLabel,const TDesC* aText,TInt aConstraints,TInt aMaxSize)
{
    MMIDTextField* textComponent = aToolkit->ComponentFactory()->CreateTextFieldL(*aLabel,*aText,aConstraints,aMaxSize);
    CleanupDisposePushL(textComponent);
    *aHandle = aToolkit->RegisterComponentL(textComponent, aTextComponent);
    CleanupPopComponent(textComponent);
}
JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_TextField__1create
(JNIEnv* aJni,jobject,jint aToolkit,jobject aTextComponent,jstring aLabel,jstring aText,jint aConstraints,jint aMaxSize)
{
    jobject textComponent = aJni->NewWeakGlobalRef(aTextComponent);
    if (textComponent == 0)
        return KErrNoMemory;
    //
    RJString label(*aJni,aLabel);
    RJString text(*aJni,aText);
    CMIDToolkit* toolkit =JavaUnhand<CMIDToolkit>(aToolkit);
    TInt h=0;
    TInt err = toolkit->ExecuteTrap(&CreateL,toolkit,&h,textComponent,(const TDesC*)&label,(const TDesC*)&text,(TInt)aConstraints,(TInt)aMaxSize);
    if (err!=KErrNone)
    {
        aJni->DeleteWeakGlobalRef((jweak)textComponent);
        return err;
    }
    return h;
}


LOCAL_C TInt GetCaretPosition(MMIDTextField* aTextComponent)
{
    return aTextComponent->GetCaretPosition();
}
JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_TextField__1getCaretPosition
(JNIEnv*,jobject,jint aTextComponent,jint aToolkit)
{
    MMIDTextField* textComponent = MIDUnhandObject<MMIDTextField>(aTextComponent);
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    return toolkit->Execute(&GetCaretPosition,textComponent);
}


LOCAL_C void GetTextL(MMIDTextField* aTextComponent, HBufC** aString)
{
    *aString = aTextComponent->GetTextL();
}
JNIEXPORT jstring JNICALL Java_javax_microedition_lcdui_TextField__1getString
(JNIEnv* aJni,jobject,jint aTextComponent,jint aToolkit)
{
    MMIDTextField* textComponent = MIDUnhandObject<MMIDTextField>(aTextComponent);
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    HBufC* text=NULL;
    TInt error = toolkit->ExecuteTrap(&GetTextL, textComponent, &text);
    jstring str = (text!=NULL && error==KErrNone) ? CreateJavaString(*aJni,*text):NULL;
    delete text;
    return str;
}


LOCAL_C void SetTextL(MMIDTextField* aTextComponent,const TDesC* aText)
{
    aTextComponent->SetTextL(*aText);
}
JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_TextField__1setString
(JNIEnv* aJni,jobject,jint aTextComponent,jint aToolkit,jstring aText)
{
    MMIDTextField* textComponent = MIDUnhandObject<MMIDTextField>(aTextComponent);
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    RJString text(*aJni,aText);
    return toolkit->ExecuteTrap(&SetTextL,textComponent,(const TDesC*)&text);
}


LOCAL_C void InsertTextL(MMIDTextField* aTextComponent,const TDesC* aText,TInt aPosition)
{
    aTextComponent->InsertTextL(*aText,aPosition);
}
JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_TextField__1insertString
(JNIEnv* aJni,jobject,jint aTextComponent,jint aToolkit,jstring aText,jint aPosition)
{
    MMIDTextField* textComponent = MIDUnhandObject<MMIDTextField>(aTextComponent);
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    RJString text(*aJni,aText);
    return toolkit->ExecuteTrap(&InsertTextL,textComponent,(const TDesC*)&text,(TInt)aPosition);
}


LOCAL_C void DeleteTextL(MMIDTextField* aTextComponent,TInt aOffset,TInt aLength)
{
    aTextComponent->DeleteTextL(aOffset,aLength);
}
JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_TextField__1delete
(JNIEnv*,jobject,jint aTextComponent,jint aToolkit,jint aOffset,jint aLength)
{
    MMIDTextField* textComponent = MIDUnhandObject<MMIDTextField>(aTextComponent);
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    return toolkit->ExecuteTrap(&DeleteTextL,textComponent,(TInt)aOffset,(TInt)aLength);
}


LOCAL_C void SetMaxSizeL(MMIDTextField* aTextComponent,TInt aSize, TInt* aMaxSize)
{
    *aMaxSize = aTextComponent->SetMaxSizeL(aSize);
}
JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_TextField__1setMaxSize
(JNIEnv*,jobject,jint aTextComponent,jint aToolkit,jint aSize)
{
    MMIDTextField* textComponent = MIDUnhandObject<MMIDTextField>(aTextComponent);
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    // Do server side as if the current contents of the TextField are larger than
    // maxSize,the contents are truncated to fit.
    TInt maxSize = 0;
    TInt err =  toolkit->ExecuteTrap(&SetMaxSizeL,textComponent,(TInt)aSize, &maxSize);
    return err == KErrNone ? maxSize : err;
}


LOCAL_C TInt GetMaxSize(MMIDTextField* aTextComponent)
{
    return aTextComponent->GetMaxSize();
}
JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_TextField__1getMaxSize
(JNIEnv*,jobject,jint aTextComponent,jint aToolkit)
{
    MMIDTextField* textComponent = MIDUnhandObject<MMIDTextField>(aTextComponent);
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    return toolkit->Execute(&GetMaxSize,textComponent);
}


LOCAL_C TInt Size(MMIDTextField* aTextComponent)
{
    return aTextComponent->Size();
}
JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_TextField__1size(JNIEnv*,jobject,jint aTextComponent,jint aToolkit)
{
    MMIDTextField* textComponent = MIDUnhandObject<MMIDTextField>(aTextComponent);
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    return toolkit->Execute(&Size,textComponent);
}


LOCAL_C void SetConstraintsL(MMIDTextField* aTextComponent,TUint aConstraints)
{
    aTextComponent->SetConstraintsL(aConstraints);
}
JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_TextField__1setConstraints
(JNIEnv*,jobject,jint aTextComponent,jint aToolkit,jint aConstraints)
{
    MMIDTextField* textComponent = MIDUnhandObject<MMIDTextField>(aTextComponent);
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    return toolkit->ExecuteTrap(&SetConstraintsL,textComponent,(TUint)aConstraints);
}


LOCAL_C void SetInitialInputModeL(MMIDTextField* aTextComponent,const TDesC* aCharacterSubset)
{
    aTextComponent->SetInitialInputModeL(*aCharacterSubset);
}
JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_TextField__1setInitialInputMode
(JNIEnv* aJni,jobject,jint aTextComponent,jint aToolkit,jstring aCharacterSubset)
{
    MMIDTextField* textComponent = MIDUnhandObject<MMIDTextField>(aTextComponent);
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    RJString charSubset(*aJni,aCharacterSubset);
    return toolkit->ExecuteTrap(&SetInitialInputModeL,textComponent,(const TDesC*)&charSubset);
}
