/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "com_nokia_microedition_m2g_M2GSVGAnimationElement.h"
#include "MM2GSVGProxy.h"

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
JNIEXPORT void JNICALL
Java_com_nokia_microedition_m2g_M2GSVGAnimationElement__1beginElementAt(
    JNIEnv* aJni,
    jobject,
    jint aSvgProxyHandle,
    jint aDocumentHandle,
    jint aElementHandle,
    jfloat aOffset,
    jfloat aCurrentTime)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGAnimationElement ) _beginElementAt - begin");
    TInt err = KM2GNotOk;

    if (aSvgProxyHandle && aDocumentHandle)
    {
        MM2GSVGProxy* aProxy =    JavaUnhand< MM2GSVGProxy >(aSvgProxyHandle);
        TInt16 restartAttribute;

        TRAP(err, aProxy->GetEnumTraitL(
                 STATIC_CAST(TM2GSvgElementHandle, aElementHandle),
                 STATIC_CAST(TM2GSvgAttrType, KM2GRestartAttribute),
                 restartAttribute);
            )

        TInt isActive;
        TRAP(err, aProxy->IsActiveL(STATIC_CAST(TM2GSvgElementHandle, aElementHandle), isActive);)

        if (isActive && (restartAttribute == KM2GRestartWhenNotActive))
        {
            M2G_DEBUG_1("M2G_DEBUG: M2GSVGAnimationElement::DoBeginElementAtL() - active & restart att:%d", restartAttribute);
        }
        else if ((aCurrentTime != 0) && (restartAttribute == KM2GRestartNever))
        {
            // Cannot restart even if animation hasn't ended?
            M2G_DEBUG_1("M2G_DEBUG: M2GSVGAnimationElement::DoBeginElementAtL() - not active & restart att:%d", restartAttribute);
        }
        else
        {
            M2G_DEBUG_2("M2G_DEBUG: M2GSVGAnimationElement::DoBeginElementAtL() - offset:%f & current:%f", aOffset, aCurrentTime);
            TRAP(err,   aProxy->BeginElementAtL(
                     STATIC_CAST(TM2GSvgDocumentHandle, aDocumentHandle),
                     STATIC_CAST(TM2GSvgElementHandle, aElementHandle),
                     (aOffset+aCurrentTime));
                )
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
JNIEXPORT void JNICALL
Java_com_nokia_microedition_m2g_M2GSVGAnimationElement__1endElementAt(
    JNIEnv* aJni,
    jobject,
    jint aSvgProxyHandle,
    jint aDocumentHandle,
    jint aElementHandle,
    jfloat aOffset)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGAnimationElement ) _endElementAt - begin");
    TInt err = KM2GNotOk;

    MM2GSVGProxy* aProxy = JavaUnhand< MM2GSVGProxy >(aSvgProxyHandle);

    TRAP(err,  aProxy->EndElementAtL(STATIC_CAST(TM2GSvgDocumentHandle, aDocumentHandle),
                                     STATIC_CAST(TM2GSvgElementHandle, aElementHandle),
                                     aOffset);
        )

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
JNIEXPORT jboolean JNICALL
Java_com_nokia_microedition_m2g_M2GSVGAnimationElement__1isActive(
    JNIEnv* aJni,
    jobject,
    jint aSvgProxyHandle,
    jint aElementHandle)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGAnimationElement ) _isActive - begin");
    TInt err = KM2GNotOk;
    TInt active = 0;
    MM2GSVGProxy* aProxy = JavaUnhand< MM2GSVGProxy >(aSvgProxyHandle);

    TRAP(err, aProxy->IsActiveL(
             STATIC_CAST(TM2GSvgElementHandle, aElementHandle),
             active);
        )

    M2GGeneral::CheckErrorCode(aJni, err);
    M2G_DEBUG_1("M2G_DEBUG: JNI ( M2GSVGAnimationElement ) _isActive: %d - end", active);
    return STATIC_CAST(jboolean, (active == 1 ? ETrue : EFalse));
}

M2G_NS_END


