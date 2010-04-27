/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  JNI methods
*
*/

// INCLUDE FILES
#include "CM2GEventSource.h"
#include "com_nokia_microedition_m2g_M2GSVGAnimationElement.h"
#include "MM2GSVGProxy.h"
#include <methodwrappers.h>

M2G_NS_START

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

// ================================ FUNCTIONS ==================================

// -----------------------------------------------------------------------------
// Java_com_nokia_microedition_m2g_M2GSVGAnimationElement::_beginElementAt
// -----------------------------------------------------------------------------
/**
 * Calls MM2GSVGProxy::BeginElementAtL method.
 * @since Series S60 3.0
 * @param aProxy Proxy instance.
 * @param aDocumentHandle Document handle
 * @param aElementHandle Element handle
 * @param aOffset Offset time
 * @throws Exception if not ok.
 */
LOCAL_C void DoBeginElementAtL(
    MM2GSVGProxy* aProxy,
    TInt aDocumentHandle,
    TInt aElementHandle,
    TReal32* aOffset, TReal32* aCurrentTime)
{
    TInt16 restartAttribute;
    aProxy->GetEnumTraitL(
        STATIC_CAST(TM2GSvgElementHandle, aElementHandle),
        STATIC_CAST(TM2GSvgAttrType, KM2GRestartAttribute),
        restartAttribute);
    TInt isActive;
    aProxy->IsActiveL(STATIC_CAST(TM2GSvgElementHandle, aElementHandle), isActive);

    if (isActive && (restartAttribute == KM2GRestartWhenNotActive))
    {
        M2G_DEBUG_1("M2G_DEBUG: M2GSVGAnimationElement::DoBeginElementAtL() - active & restart att:%d", restartAttribute);
    }
    else if ((*aCurrentTime != 0) && (restartAttribute == KM2GRestartNever))
    {
        // Cannot restart even if animation hasn't ended?
        M2G_DEBUG_1("M2G_DEBUG: M2GSVGAnimationElement::DoBeginElementAtL() - not active & restart att:%d", restartAttribute);
    }
    else
    {
        M2G_DEBUG_2("M2G_DEBUG: M2GSVGAnimationElement::DoBeginElementAtL() - offset:%f & current:%f", *aOffset, *aCurrentTime);
        aProxy->BeginElementAtL(
            STATIC_CAST(TM2GSvgDocumentHandle, aDocumentHandle),
            STATIC_CAST(TM2GSvgElementHandle, aElementHandle),
            *aOffset + *aCurrentTime);
    }
}

/**
 * JNI method
 */
JNIEXPORT void JNICALL
Java_com_nokia_microedition_m2g_M2GSVGAnimationElement__1beginElementAt(
    JNIEnv* aJni,
    jobject,
    jint aEventSourceHandle,
    jint aSvgProxyHandle,
    jint aDocumentHandle,
    jint aElementHandle,
    jfloat aOffset,
    jfloat aCurrentTime,
    jboolean aUIToolkit)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGAnimationElement ) _beginElementAt - begin");
    TInt err = KM2GNotOk;
    if (aUIToolkit)
    {
        MSwtClient* client = reinterpret_cast< MSwtClient* >(aEventSourceHandle);
        TInt16 restartAttribute;
        TInt   isActive;
        if (client && aSvgProxyHandle)
        {
            typedef void (MM2GSVGProxy::*pBeginElementAtL)(const TInt&, const TInt&,const TReal32&);
            typedef void (MM2GSVGProxy::*pGetEnumTraitL)(const TInt&, const TInt16&, TInt16&);
            typedef void (MM2GSVGProxy::*pIsActiveL)(const TInt&, TInt&);
            pBeginElementAtL BeginElementAtL = &MM2GSVGProxy::BeginElementAtL;
            pGetEnumTraitL     GetEnumTraitL = &MM2GSVGProxy::GetEnumTraitL;
            pIsActiveL           IsActiveL = &MM2GSVGProxy::IsActiveL;
            MM2GSVGProxy* aProxy = JavaUnhand< MM2GSVGProxy >(aSvgProxyHandle);
            TMethodWrapper3<MM2GSVGProxy,const TInt&,const TInt&, const TReal32& >
            BeginElementAt(
                *aProxy,
                BeginElementAtL,
                aDocumentHandle,
                aElementHandle,
                aOffset + aCurrentTime
            );
            TMethodWrapper3<MM2GSVGProxy,const TInt&,const TInt16&, TInt16& >
            GetEnumTrait(
                *aProxy,
                GetEnumTraitL,
                aElementHandle,
                KM2GRestartAttribute,
                restartAttribute
            );
            TMethodWrapper2<MM2GSVGProxy,const TInt&, TInt& >
            lIsActive(
                *aProxy,
                IsActiveL,
                aElementHandle,
                isActive
            );
            client->Execute(GetEnumTrait);
            client->Execute(lIsActive);
            if (isActive && (restartAttribute == KM2GRestartWhenNotActive))
            {
                M2G_DEBUG_1("M2G_DEBUG: M2GSVGAnimationElement::DoBeginElementAtL() - active & restart att:%d", restartAttribute);
            }
            else if ((aCurrentTime != 0) && (restartAttribute == KM2GRestartNever))
            {
                M2G_DEBUG_1("M2G_DEBUG: M2GSVGAnimationElement::DoBeginElementAtL() - not active & restart att:%d", restartAttribute);
            }
            else
            {
                err = client->Execute(BeginElementAt);
            }
        }
    }
    else
    {
        CM2GEventSource* eventSource = JavaUnhand< CM2GEventSource >(aEventSourceHandle);
        if (eventSource && aSvgProxyHandle && aDocumentHandle)
        {
            err = eventSource->ExecuteTrap(
                      &DoBeginElementAtL,
                      JavaUnhand< MM2GSVGProxy >(aSvgProxyHandle),
                      aDocumentHandle, aElementHandle,
                      REINTERPRET_CAST(TReal32*, &aOffset),
                      REINTERPRET_CAST(TReal32*, &aCurrentTime));
        }
    }
    M2GGeneral::CheckErrorCode(aJni, err);

    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGAnimationElement ) _beginElementAt - end");
}

// -----------------------------------------------------------------------------
// Java_com_nokia_microedition_m2g_M2GSVGAnimationElement::_endElementAt
// -----------------------------------------------------------------------------
/**
 * Calls MM2GSVGProxy::EndElementAt method.
 * @since Series S60 3.0
 * @param aProxy Proxy instance.
 * @param aDocumentHandle Document handle
 * @param aElementHandle Element handle
 * @param aOffset Offset time
 */
LOCAL_C void DoEndElementAtL(
    MM2GSVGProxy* aProxy,
    TInt aDocumentHandle,
    TInt aElementHandle,
    TReal32* aOffset)
{
    aProxy->EndElementAtL(
        STATIC_CAST(TM2GSvgDocumentHandle, aDocumentHandle),
        STATIC_CAST(TM2GSvgElementHandle, aElementHandle),
        *aOffset);
}

/**
 * JNI method
 */
JNIEXPORT void JNICALL
Java_com_nokia_microedition_m2g_M2GSVGAnimationElement__1endElementAt(
    JNIEnv* aJni,
    jobject,
    jint aEventSourceHandle,
    jint aSvgProxyHandle,
    jint aDocumentHandle,
    jint aElementHandle,
    jfloat aOffset,
    jboolean aUIToolkit)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGAnimationElement ) _endElementAt - begin");
    TInt err = KM2GNotOk;
    if (aUIToolkit)
    {
        MSwtClient* client = reinterpret_cast< MSwtClient* >(aEventSourceHandle);
        if (client && aSvgProxyHandle)
        {
            typedef void (MM2GSVGProxy::*pEndElementAtL)(const TInt&, const TInt&,const TReal32&);
            pEndElementAtL EndElementAtL = &MM2GSVGProxy::EndElementAtL;
            MM2GSVGProxy* aProxy = JavaUnhand< MM2GSVGProxy >(aSvgProxyHandle);
            TMethodWrapper3<MM2GSVGProxy,const TInt&,const TInt&, const TReal32& >
            EndElementAt(
                *aProxy,
                EndElementAtL,
                aDocumentHandle,
                aElementHandle,
                aOffset
            );
            err = client->Execute(EndElementAt);
        }
    }
    else
    {
        CM2GEventSource* eventSource = JavaUnhand< CM2GEventSource >(aEventSourceHandle);
        if (eventSource && aSvgProxyHandle && aDocumentHandle)
        {
            err = eventSource->ExecuteTrap(
                      &DoEndElementAtL,
                      JavaUnhand< MM2GSVGProxy >(aSvgProxyHandle),
                      aDocumentHandle, aElementHandle,
                      REINTERPRET_CAST(TReal32*, &aOffset));
        }
    }
    M2GGeneral::CheckErrorCode(aJni, err);

    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGAnimationElement ) _endElementAt - end");
}

// -----------------------------------------------------------------------------
// Java_com_nokia_microedition_m2g_M2GSVGAnimationElement::_isActive
// -----------------------------------------------------------------------------
/**
 * Calls MM2GSVGProxy::EndElementAt method.
 * @since Series S60 3.0
 * @param aProxy Proxy instance.
 * @param aElementHandle Element handle
 */
LOCAL_C void DoIsActiveL(
    MM2GSVGProxy* aProxy,
    TInt aElementHandle,
    TInt* aActive)
{
    aProxy->IsActiveL(
        STATIC_CAST(TM2GSvgElementHandle, aElementHandle),
        *aActive);
}

/**
 * JNI method
 */
JNIEXPORT jboolean JNICALL
Java_com_nokia_microedition_m2g_M2GSVGAnimationElement__1isActive(
    JNIEnv* aJni,
    jobject,
    jint aEventSourceHandle,
    jint aSvgProxyHandle,
    jint aElementHandle,
    jboolean aUiToolkit)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGAnimationElement ) _isActive - begin");
    TInt err = KM2GNotOk;
    TInt active = 0;
    if (aUiToolkit)
    {
        MSwtClient* client = reinterpret_cast< MSwtClient* >(aEventSourceHandle);
        if (client && aSvgProxyHandle)
        {
            typedef void (MM2GSVGProxy::*pIsActiveL)(const TInt&,TInt&);
            pIsActiveL IsActiveL = &MM2GSVGProxy::IsActiveL;
            MM2GSVGProxy* aProxy = JavaUnhand< MM2GSVGProxy >(aSvgProxyHandle);
            TMethodWrapper2<MM2GSVGProxy,const TInt&,TInt& >
            lIsActive(
                *aProxy,
                IsActiveL,
                aElementHandle,
                active
            );
            err = client->Execute(lIsActive);
        }
    }
    else
    {
        CM2GEventSource* eventSource = JavaUnhand< CM2GEventSource >(aEventSourceHandle);

        if (eventSource && aSvgProxyHandle)
        {
            err = eventSource->ExecuteTrap(
                      &DoIsActiveL,
                      JavaUnhand< MM2GSVGProxy >(aSvgProxyHandle),
                      aElementHandle, &active);
        }
    }

    M2GGeneral::CheckErrorCode(aJni, err);
    M2G_DEBUG_1("M2G_DEBUG: JNI ( M2GSVGAnimationElement ) _isActive: %d - end", active);
    return STATIC_CAST(jboolean, (active == 1 ? ETrue : EFalse));
}


M2G_NS_END


