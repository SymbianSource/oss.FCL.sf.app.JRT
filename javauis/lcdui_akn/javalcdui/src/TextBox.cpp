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


#include "CMIDToolkit.h"
#include "MIDUtils.h"
#include "javax_microedition_lcdui_TextBox.h"


// assigned maximum capacity may be smaller than requested so must
// go native to get real max size.
// iMaxSize = NativeError.check(_getMaxSize(getContentHandle(),iToolkit.getHandle()));
struct TTextBoxCreate
{
    jweak iRef;
    jint  iDisplayable;
    jint  iConstraints;
    TPtrC iText;
};

struct TTextBoxAttribs
{
    jint    iHandle;    // out
    TSize   iSize;      // out
    jint    iMaxSize;   // in-out
};

LOCAL_C void CreateTextBoxL(CMIDToolkit* aToolkit, TTextBoxCreate* aCreate, TTextBoxAttribs* aAttribs)
{
    MMIDComponentFactory& factory = *aToolkit->ComponentFactory();

    TInt constraints = aCreate->iConstraints;
    TInt maxSize = aAttribs->iMaxSize;
    const TDesC& text = aCreate->iText;
    MMIDDisplayable& container = *MIDUnhand<MMIDDisplayable>(aCreate->iDisplayable);

    MMIDTextBox* textComponent = factory.CreateTextBoxL(constraints, maxSize, text, container);
    CleanupDisposePushL(textComponent);
    aAttribs->iHandle = aToolkit->RegisterComponentL(textComponent, aCreate->iRef);
    CleanupPopComponent(textComponent);
    aCreate->iRef = 0;
    aAttribs->iSize = container.ContentSize();
    aAttribs->iMaxSize = textComponent->GetMaxSize();
}

JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_TextBox__1create
(
    JNIEnv* aJni,
    jobject aTextBox,
    jint    aToolkit,
    jint    aDisplayable,
    jint    aConstraints,
    jint    aMaxSize,
    jstring aText,
    jintArray aCreateTextBoxReturn
)
{
    RJString text(*aJni,aText);

    TTextBoxCreate create;
    create.iRef = aJni->NewWeakGlobalRef(aTextBox);
    create.iDisplayable = aDisplayable;
    create.iConstraints = aConstraints;
    create.iText.Set(text);

    TTextBoxAttribs attribs;
    attribs.iHandle=0;
    attribs.iMaxSize=aMaxSize;

    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);

    jint error;
    if (create.iRef)
    {
        error = toolkit->ExecuteTrap(&CreateTextBoxL, toolkit, &create, &attribs);
    }
    else
    {
        error = KErrNoMemory;
    }

    if (create.iRef)
    {
        // cleared in CreateTextBoxL if successful
        aJni->DeleteWeakGlobalRef(create.iRef);
    }

    ASSERT(aJni->GetArrayLength(aCreateTextBoxReturn) == 4);
    jint attribArray[4];
    attribArray[0]=attribs.iHandle;
    attribArray[1]=attribs.iSize.iWidth;
    attribArray[2]=attribs.iSize.iHeight;
    attribArray[3]=attribs.iMaxSize;
    aJni->SetIntArrayRegion(aCreateTextBoxReturn, 0, 4, attribArray);

    return error;
}

LOCAL_C TInt GetCaretPosition(MMIDTextBox* aTextComponent)
{
    return aTextComponent->GetCaretPosition();
}
JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_TextBox__1getCaretPosition
(JNIEnv*,jobject,jint aTextComponent,jint aToolkit)
{
    MMIDTextBox* textComponent = MIDUnhandObject<MMIDTextBox>(aTextComponent);
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    return toolkit->Execute(&GetCaretPosition,textComponent);
}


LOCAL_C void GetTextL(MMIDTextBox* aTextComponent, HBufC** aString)
{
    *aString = aTextComponent->GetTextL();
}
JNIEXPORT jstring JNICALL Java_javax_microedition_lcdui_TextBox__1getString
(JNIEnv* aJni,jobject,jint aTextComponent,jint aToolkit)
{
    MMIDTextBox* textComponent = MIDUnhandObject<MMIDTextBox>(aTextComponent);
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    HBufC* text=NULL;
    TInt error = toolkit->ExecuteTrap(&GetTextL, textComponent, &text);
    jstring str = (text!=NULL && error==KErrNone) ? CreateJavaString(*aJni,*text):NULL;
    delete text;
    return str;
}


LOCAL_C void SetTextL(MMIDTextBox* aTextComponent,const TDesC* aText)
{
    aTextComponent->SetTextL(*aText);
}
JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_TextBox__1setString
(JNIEnv* aJni,jobject,jint aTextComponent,jint aToolkit,jstring aText)
{
    MMIDTextBox* textComponent = MIDUnhandObject<MMIDTextBox>(aTextComponent);
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    RJString text(*aJni,aText);
    return toolkit->ExecuteTrap(&SetTextL,textComponent,(const TDesC*)&text);
}


LOCAL_C void InsertTextL(MMIDTextBox* aTextComponent,const TDesC* aText,TInt aPosition)
{
    aTextComponent->InsertTextL(*aText,aPosition);
}
JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_TextBox__1insertString
(JNIEnv* aJni,jobject,jint aTextComponent,jint aToolkit,jstring aText,jint aPosition)
{
    MMIDTextBox* textComponent = MIDUnhandObject<MMIDTextBox>(aTextComponent);
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    RJString text(*aJni,aText);
    return toolkit->ExecuteTrap(&InsertTextL,textComponent,(const TDesC*)&text,(TInt)aPosition);
}


LOCAL_C void DeleteTextL(MMIDTextBox* aTextComponent,TInt aOffset,TInt aLength)
{
    aTextComponent->DeleteTextL(aOffset,aLength);
}
JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_TextBox__1delete
(JNIEnv*,jobject,jint aTextComponent,jint aToolkit,jint aOffset,jint aLength)
{
    MMIDTextBox* textComponent = MIDUnhandObject<MMIDTextBox>(aTextComponent);
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    return toolkit->ExecuteTrap(&DeleteTextL,textComponent,(TInt)aOffset,(TInt)aLength);
}


LOCAL_C void SetMaxSizeL(MMIDTextBox* aTextComponent,TInt aSize, TInt* aMaxSize)
{
    *aMaxSize = aTextComponent->SetMaxSizeL(aSize);
}
JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_TextBox__1setMaxSize
(JNIEnv*,jobject,jint aTextComponent,jint aToolkit,jint aSize)
{
    MMIDTextBox* textComponent = MIDUnhandObject<MMIDTextBox>(aTextComponent);
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    // Do server side as if the current contents of the TextBox are larger than
    // maxSize,the contents are truncated to fit.
    TInt maxSize = 0;
    TInt err =  toolkit->ExecuteTrap(&SetMaxSizeL,textComponent,(TInt)aSize, &maxSize);
    return err == KErrNone ? maxSize : err;
}


LOCAL_C TInt GetMaxSize(MMIDTextBox* aTextComponent)
{
    return aTextComponent->GetMaxSize();
}
JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_TextBox__1getMaxSize
(JNIEnv*,jobject,jint aTextComponent,jint aToolkit)
{
    MMIDTextBox* textComponent = MIDUnhandObject<MMIDTextBox>(aTextComponent);
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    return toolkit->Execute(&GetMaxSize,textComponent);
}


LOCAL_C TInt Size(MMIDTextBox* aTextComponent)
{
    return aTextComponent->Size();
}
JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_TextBox__1size(JNIEnv*,jobject,jint aTextComponent,jint aToolkit)
{
    MMIDTextBox* textComponent = MIDUnhandObject<MMIDTextBox>(aTextComponent);
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    return toolkit->Execute(&Size,textComponent);
}


LOCAL_C void SetConstraintsL(MMIDTextBox* aTextComponent,TUint aConstraints)
{
    aTextComponent->SetConstraintsL(aConstraints);
}
JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_TextBox__1setConstraints
(JNIEnv*,jobject,jint aTextComponent,jint aToolkit,jint aConstraints)
{
    MMIDTextBox* textComponent = MIDUnhandObject<MMIDTextBox>(aTextComponent);
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    return toolkit->ExecuteTrap(&SetConstraintsL,textComponent,(TUint)aConstraints);
}


LOCAL_C void SetInitialInputModeL(MMIDTextBox* aTextComponent,const TDesC* aCharacterSubset)
{
    aTextComponent->SetInitialInputModeL(*aCharacterSubset);
}
JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_TextBox__1setInitialInputMode
(JNIEnv* aJni,jobject,jint aItem,jint aToolkit,jstring aCharacterSubset)
{
    MMIDTextBox* textComponent = MIDUnhandObject<MMIDTextBox>(aItem);
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    RJString charSubset(*aJni,aCharacterSubset);
    return toolkit->ExecuteTrap(&SetInitialInputModeL,textComponent,(const TDesC*)&charSubset);
}
