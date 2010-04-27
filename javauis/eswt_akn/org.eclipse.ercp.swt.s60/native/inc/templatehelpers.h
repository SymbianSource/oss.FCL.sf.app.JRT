/*******************************************************************************
 * Copyright (c) 2005, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/


#ifndef TEMPLATEHELPERS_H
#define TEMPLATEHELPERS_H


#include "swtdisplay.h"
#include "methodcall.h"
#include "utils.h"


//lint -e401
//lint -esym(528, NewWidget)
//lint -esym(528, NewMobileWidget)
//lint -esym(528, CallBoolTextMethodL)
//lint -esym(528, CallNonLeavingGetter)
//lint -esym(528, CallBoolTextMethodThrow)
//lint -esym(528, CallNonLeavingCoordsGetter)
//lint -esym(528, CallTextGetterThrow)
//lint -esym(528, CallTextMethodThrow)
//lint -esym(528, CallTextMethodL)
//lint -esym(528, NewCoreWidget)
//lint -esym(528, NewCoreWidget)
//lint -esym(528, NewExpandedWidget)


/**
 * Helper template function to ease calling a method with a string as only parameter
 */
template<class C, class A>
void CallTextMethodL(JNIEnv* aJniEnv, C* aObject, void (C::*aMethodL)(A), const jstring aText)
{
    HBufC* convertedText = ConvertStringLC(aJniEnv, aText); // Without this comment, LeaveScan misreports here; with, it doesn't. Go figure!
    TPtr des(convertedText->Des());
    CallMethodL(aObject, aMethodL, des);                    // LeaveScan definitely misreports here, it doesn't like method pointers in argument lists

    if (convertedText != NULL)
    {
        CleanupStack::PopAndDestroy(convertedText);
    }
}


/**
 * Helper template function to ease calling a method with a string as only parameter
 */
template<class C, class A>
void CallTextMethodThrow(JNIEnv* aJniEnv, TInt aHandle, void (C::*aMethodL)(A), const jstring aText)
{
    C* object = reinterpret_cast<C*>(aHandle);
    TRAPD(error, CallTextMethodL(aJniEnv, object, aMethodL, aText));
    ThrowIfError(error, aJniEnv);
}


/**
 * Helper template function to ease calling a TBool returning method with a string as only parameter
 */
template<class C, class A>
TBool CallBoolTextMethodL(JNIEnv* aJniEnv, C* aObject, TBool(C::*aMethodL)(A), const jstring aText)
{
    HBufC* convertedText = ConvertStringLC(aJniEnv, aText); // Without this comment, LeaveScan misreports here; with, it doesn't. Go figure!
    TPtr des(convertedText->Des());
    TBool result;
    CallMethodL(result, aObject, aMethodL, des);            // LeaveScan definitely misreports here, it doesn't like method pointers in argument lists
    CleanupStack::PopAndDestroy(convertedText);
    return result;
}


/**
 * Helper template function to ease calling a TBool returning method with a string as only parameter
 */
template<class C, class A>
TBool CallBoolTextMethodThrow(JNIEnv* aJniEnv, TInt aHandle, TBool(C::*aMethodL)(A), const jstring aText, TBool aDefaultResult)
{
    C* object = reinterpret_cast<C*>(aHandle);
    TBool result = aDefaultResult;
    TRAPD(error, (result=CallBoolTextMethodL(aJniEnv, object, aMethodL, aText)));
    ThrowIfError(error, aJniEnv);
    return result;
}


/**
 * Helper template function to ease calling a text-retrieving leaving const method.
 *
 * @return A Java string containing the text or NULL.
 */
template<class C>
jstring CallTextGetterThrow(JNIEnv* aJniEnv, TInt aHandle, HBufC*(C::*aMethodL)(void)const)
{
    const C* object = reinterpret_cast<C*>(aHandle);
    HBufC* text = NULL;
    TRAPD(error, CallMethodL(text, object, aMethodL));
    if (text == NULL)
    {
        ThrowIfError(error, aJniEnv);
        return NULL;
    }

    jstring result = NewJavaString(aJniEnv, *text);
    DeleteInUiHeap(text);
    return result;
}


/**
 * Helper template function to ease calling a non-leaving const method
 *        that returns a value.
 */
template<class C, class R>
inline R CallNonLeavingGetter(TInt aHandle, R(C::*aMethod)(void) const)
{
    const C* object = reinterpret_cast<C*>(aHandle);
    R result;
    CallMethod(result, object, aMethod);
    return result;
}


/**
 * Helper template function to ease calling a non-leaving const method
 *        that returns a TSize, a TPoint or a TRect.
 */
template<class C, class R>
inline R CallNonLeavingCoordsGetter(TInt aHandle, R(C::*aMethod)(void) const)
{
    const C* object = reinterpret_cast<C*>(aHandle);
    R result(R::EUninitialized);
    CallMethod(result, object, aMethod);
    return result;
}


/**
 * Helper function for creating a widget instance.
 */
template<class C, class F>
jint NewWidget(JNIEnv* aJniEnv, const F* aFactory, C*(F::*aMethodL)(MSwtDisplay&, TSwtPeer, MSwtComposite&, TInt) const, CSwtDisplay& aDisplay, jobject aPeer, jint aParent, jint aStyle)
{
    jweak peerRef = aJniEnv->NewWeakGlobalRef(aPeer);
    if (peerRef == NULL)
        return 0;

    MSwtComposite* parent = reinterpret_cast<MSwtComposite*>(aParent);
    C*             result = NULL;

    TRAPD(error, CallMethodL(result, aFactory, aMethodL, aDisplay, peerRef, *parent, aStyle));
    if (error)
    {
        aJniEnv->DeleteWeakGlobalRef(peerRef);
        Throw(error, aJniEnv);
    }

    return reinterpret_cast<jint>(result);
}


#endif // TEMPLATEHELPERS_H
