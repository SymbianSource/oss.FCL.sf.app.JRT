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
#include "com_nokia_microedition_m2g_M2GSVGElement.h"
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
// Java_com_nokia_microedition_m2g_M2GSVGElement::_appendChild
// -----------------------------------------------------------------------------
/**
 * Calls MM2GSVGProxy::AppendChildL method.
 * @since Series S60 3.0
 * @param aProxy Proxy instance.
 * @param aElementHandle Element handle
 * @param aChildElementHandle Child handle
 */
LOCAL_C void DoAppendChildL(
    MM2GSVGProxy* aProxy,
    TInt aElementHandle,
    TInt aChildElementHandle)
{
    aProxy->AppendChildL(
        STATIC_CAST(TM2GSvgElementHandle, aElementHandle),
        STATIC_CAST(TM2GSvgElementHandle, aChildElementHandle));
}
/**
 * JNI method
 */
JNIEXPORT void JNICALL
Java_com_nokia_microedition_m2g_M2GSVGElement__1appendChild(
    JNIEnv* aJni,
    jclass,
    jint aEventSourceHandle,
    jint aSvgProxyHandle,
    jint aElementHandle,
    jint aChildElementHandle,
    jboolean aUiToolkit)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGElement ) _appendChild - begin");
    TInt err = KM2GNotOk;
    if (aUiToolkit)  // If eSWT is the current toolkit
    {
        MSwtClient* client = reinterpret_cast< MSwtClient* >(aEventSourceHandle);
        if (client && aSvgProxyHandle)
        {
            typedef void (MM2GSVGProxy::*pAppendChildL)(const TM2GSvgElementHandle& ,const TM2GSvgElementHandle&);
            pAppendChildL AppendChildL = &MM2GSVGProxy::AppendChildL;
            MM2GSVGProxy* aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);
            TM2GSvgElementHandle lElementHandle = STATIC_CAST(TM2GSvgElementHandle, aElementHandle);
            TM2GSvgElementHandle lChildElementHandle = STATIC_CAST(TM2GSvgElementHandle, aChildElementHandle);
            TMethodWrapper2<MM2GSVGProxy, const TM2GSvgElementHandle&, const TM2GSvgElementHandle&> DoAppendChild(
                *aProxy,
                AppendChildL,
                lElementHandle,
                lChildElementHandle);
            err = client->Execute(DoAppendChild);
        }
    }
    else
    {
        CM2GEventSource* eventSource = JavaUnhand< CM2GEventSource >(aEventSourceHandle);
        if (eventSource && aSvgProxyHandle)
        {
            err = eventSource->ExecuteTrap(
                      &DoAppendChildL,
                      JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle),
                      aElementHandle,
                      aChildElementHandle);
        }
    }
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGElement ) _appendChild - end");
    M2GGeneral::CheckErrorCode(aJni, err);
}

// -----------------------------------------------------------------------------
// Java_com_nokia_microedition_m2g_M2GSVGElement::_checkRemoveable
// -----------------------------------------------------------------------------
/**
 * Calls MM2GSVGProxy::CheckRemoveableL method.
 * @since Series S60 3.0
 * @param aProxy Proxy instance.
 * @param aElementHandle Element handle
 * @param aRemoveable Result
 */
LOCAL_C void DoCheckRemoveableL(
    MM2GSVGProxy* aProxy,
    TInt aElementHandle,
    TInt* aRemoveable)
{
    aProxy->CheckRemoveableL(aElementHandle, *aRemoveable);
}

/**
 * JNI method.
 */
JNIEXPORT jint JNICALL Java_com_nokia_microedition_m2g_M2GSVGElement__1checkRemoveable(
    JNIEnv* aJni,
    jclass,
    jint aEventSourceHandle,
    jint aSvgProxyHandle,
    jint aElementHandle,
    jboolean aUiToolkit)
{
    TInt removeable = -1;
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGElement ) _checkRemoveable - begin");
    TInt err = KM2GNotOk;
    if (aUiToolkit)  // If eSWT is the current toolkit
    {
        MSwtClient* client = reinterpret_cast< MSwtClient* >(aEventSourceHandle);
        if (client && aSvgProxyHandle)
        {
            typedef void (MM2GSVGProxy::*pCheckRemoveableL)(const TM2GSvgElementHandle& ,TInt&);
            pCheckRemoveableL CheckRemoveableL = &MM2GSVGProxy::CheckRemoveableL;
            MM2GSVGProxy* aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);
            TMethodWrapper2<MM2GSVGProxy, const TM2GSvgElementHandle&, TInt&> DoCheckRemoveable(
                *aProxy,
                CheckRemoveableL,
                aElementHandle,
                removeable);
            err = client->Execute(DoCheckRemoveable);
        }
    }
    else
    {
        CM2GEventSource* eventSource = JavaUnhand< CM2GEventSource >(aEventSourceHandle);
        if (eventSource && aSvgProxyHandle)
        {
            err = eventSource->ExecuteTrap(
                      &DoCheckRemoveableL,
                      JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle),
                      aElementHandle,
                      &removeable);
        }
    }
    M2GGeneral::CheckErrorCode(aJni, err);
    M2G_DEBUG_1("M2G_DEBUG: JNI ( M2GSVGElement ) _checkRemoveable: %d - end", removeable);
    return removeable;
}

// -----------------------------------------------------------------------------
// Java_com_nokia_microedition_m2g_M2GSVGElement::_getBBox
// -----------------------------------------------------------------------------
/**
 * Calls MM2GSVGProxy::GetBBoxL method.
 * @since Series S60 3.0
 * @param aProxy Proxy instance.
 * @param aElementHandle Element handle
 * @param aAttributeTypeId Attribute type id
 * @param aRectData Returns rect components
 * @param aResult Result "0" ok
 */
LOCAL_C void DoGetBBoxL(
    MM2GSVGProxy* aProxy,
    TInt aElementHandle,
    TInt16 aAttributeTypeId,
    TM2GRectData* aRectData,
    TInt* aResult)
{
    aProxy->GetBBoxL(
        STATIC_CAST(TM2GSvgElementHandle, aElementHandle),
        STATIC_CAST(TM2GSvgAttrType, aAttributeTypeId),
        *aRectData, *aResult);
}
/**
 * JNI method
 */
JNIEXPORT jint JNICALL
Java_com_nokia_microedition_m2g_M2GSVGElement__1getBBox(
    JNIEnv* aJni,
    jclass,
    jint aEventSourceHandle,
    jint aSvgProxyHandle,
    jint aElementHandle,
    jshort aAttributeTypeId,
    jfloatArray aRectComponents,
    jboolean aUiToolkit)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGElement ) _getBBox - begin");

    TM2GRectData rect;
    TInt result = 0;
    TInt err = KM2GNotOk;
    if (aUiToolkit)  // If eSWT is the current toolkit
    {
        MSwtClient* client = reinterpret_cast< MSwtClient*  >(aEventSourceHandle);
        if (client && aSvgProxyHandle)
        {
            typedef void (MM2GSVGProxy::*pGetBBoxL)(const TM2GSvgElementHandle& ,const TM2GSvgAttrType& ,TM2GRectData& , TInt&);
            pGetBBoxL GetBBoxL = &MM2GSVGProxy::GetBBoxL;
            MM2GSVGProxy* aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);
            TM2GSvgElementHandle lElementHandle = STATIC_CAST(TM2GSvgElementHandle, aElementHandle);
            TM2GSvgAttrType lAttrType = STATIC_CAST(TM2GSvgAttrType, aAttributeTypeId);
            TMethodWrapper4<MM2GSVGProxy, const TM2GSvgElementHandle& ,const TM2GSvgAttrType& ,TM2GRectData& , TInt&> DoGetBBox(
                *aProxy,
                GetBBoxL,
                lElementHandle,
                lAttrType,
                rect,
                result);
            err = client->Execute(DoGetBBox);
        }
    }
    else
    {
        CM2GEventSource* eventSource = JavaUnhand< CM2GEventSource >(aEventSourceHandle);
        if (eventSource && aSvgProxyHandle)
        {
            err = eventSource->ExecuteTrap(
                      &DoGetBBoxL,
                      JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle),
                      aElementHandle,
                      aAttributeTypeId,
                      &rect, &result);

        }
    }
    M2GGeneral::CheckErrorCode(aJni, err);

    aJni->SetFloatArrayRegion(
        REINTERPRET_CAST(jfloatArray, aRectComponents),
        0,
        rect.Count(), REINTERPRET_CAST(jfloat*, rect.Begin()));

    M2G_DEBUG_1("M2G_DEBUG: JNI ( M2GSVGElement ) _getBBox: %d - end", result);
    return result;
}

// -----------------------------------------------------------------------------
// Java_com_nokia_microedition_m2g_M2GSVGElement::_getColorTrait
// -----------------------------------------------------------------------------
/**
 * Calls MM2GSVGProxy::GetColorTraitL method.
 * @since Series S60 3.0
 * @param aProxy Proxy instance.
 * @param aElementHandle Element handle
 * @param aAttributeTypeId Attribute type id
 * @param aColors Color elements ( RGB )
 * @param aTrait Result
 */
LOCAL_C void DoGetColorTraitL(
    MM2GSVGProxy* aProxy,
    TInt aElementHandle,
    TInt16 aAttributeTypeId,
    TM2GColorData* aColor,
    TInt* aResult)
{
    aProxy->GetColorTraitL(
        aElementHandle,
        aAttributeTypeId,
        *aColor,
        *aResult);
}

/**
 * JNI method
 */
JNIEXPORT jint JNICALL
Java_com_nokia_microedition_m2g_M2GSVGElement__1getColorTrait(
    JNIEnv* aJni,
    jclass,
    jint aEventSourceHandle,
    jint aSvgProxyHandle,
    jint aElementHandle,
    jshort aAttributeTypeId,
    jintArray aColorComponents,
    jboolean aUiToolkit)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGElement ) _getColorTrait - begin");
    TM2GColorData color;
    TInt result = -1;
    TInt err = KM2GNotOk;
    if (aUiToolkit)  // If eSWT is the current toolkit
    {
        MSwtClient* client = reinterpret_cast< MSwtClient* >(aEventSourceHandle);
        if (client && aSvgProxyHandle)
        {
            typedef void (MM2GSVGProxy::*pGetColorTraitL)(const TM2GSvgElementHandle& ,const TM2GSvgAttrType& ,TM2GColorData& , TInt&);
            pGetColorTraitL GetColorTraitL = &MM2GSVGProxy::GetColorTraitL;
            MM2GSVGProxy* aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);
            TM2GSvgElementHandle lElementHandle = STATIC_CAST(TM2GSvgElementHandle, aElementHandle);
            TM2GSvgAttrType lAttrType = STATIC_CAST(TM2GSvgAttrType, aAttributeTypeId);
            TMethodWrapper4<MM2GSVGProxy, const TM2GSvgElementHandle& ,const TM2GSvgAttrType& ,TM2GColorData& , TInt&   > GetColorTrait(
                *aProxy,
                GetColorTraitL,
                lElementHandle,
                lAttrType,
                color,
                result);
            err = client->Execute(GetColorTrait);
        }
    }
    else
    {
        CM2GEventSource* eventSource = JavaUnhand< CM2GEventSource >(aEventSourceHandle);
        if (eventSource && aSvgProxyHandle)
        {
            err = eventSource->ExecuteTrap(
                      &DoGetColorTraitL,
                      JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle),
                      aElementHandle,
                      aAttributeTypeId,
                      &color, &result);
        }
    }
    M2GGeneral::CheckErrorCode(aJni, err);
    if (color[ 0 ] == -1 && color[ 1 ] == -1 && color[ 2 ] == -1)
    {
        result = 0;
        aJni->SetIntArrayRegion(aColorComponents, 0, color.Count(), &(color[ 0 ]));
    }
    else if (result != -1)
    {
        result = 1;
        aJni->SetIntArrayRegion(aColorComponents, 0, color.Count(),  color.Begin());
    }

    M2G_DEBUG_4("M2G_DEBUG: JNI ( M2GSVGElement ) _getColorTrait: %d, R=%d, G=%d, B=%d - end", result, color[ 0 ], color[ 1 ], color[ 2 ]);
    return result;
}

// -----------------------------------------------------------------------------
// Java_com_nokia_microedition_m2g_M2GSVGElement::_getElementTypeId
// -----------------------------------------------------------------------------
/**
 * Calls MM2GSVGProxy::GetElementTypeIdL method.
 * @since Series S60 3.0
 * @param aProxy Proxy instance.
 * @param aElementHandle Element handle
 * @param aId
 */
LOCAL_C void DoGetElementTypeIdL(
    MM2GSVGProxy* aProxy,
    TInt aElementHandle,
    TInt16* aId)
{
    aProxy->GetElementTypeIdL(aElementHandle, *aId);
}

/**
 * JNI method
 */
JNIEXPORT jshort JNICALL
Java_com_nokia_microedition_m2g_M2GSVGElement__1getElementTypeId(
    JNIEnv* aJni,
    jclass,
    jint aEventSourceHandle,
    jint aSvgProxyHandle,
    jint aElementHandle,
    jboolean aUiToolkit)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGElement ) _getElementTypeId - begin");
    TInt16 id = 0;
    TInt err = KM2GNotOk;
    if (aUiToolkit)  // If eSWT is the current toolkit
    {
        MSwtClient* client = reinterpret_cast< MSwtClient* >(aEventSourceHandle);
        if (client && aSvgProxyHandle)
        {
            typedef void (MM2GSVGProxy::*pGetElementTypeIdL)(const TM2GSvgElementHandle& , TInt16&);
            pGetElementTypeIdL GetElementTypeIdL = &MM2GSVGProxy::GetElementTypeIdL;
            MM2GSVGProxy* aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);
            TM2GSvgElementHandle lElementHandle = STATIC_CAST(TM2GSvgElementHandle, aElementHandle);
            TMethodWrapper2<MM2GSVGProxy, const TM2GSvgElementHandle& , TInt16&   > GetElementTypeId(
                *aProxy,
                GetElementTypeIdL,
                lElementHandle,
                id);
            err = client->Execute(GetElementTypeId);
        }
    }
    else
    {
        CM2GEventSource* eventSource = JavaUnhand< CM2GEventSource >(aEventSourceHandle);
        if (eventSource && aSvgProxyHandle)
        {
            err = eventSource->ExecuteTrap(
                      &DoGetElementTypeIdL,
                      JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle),
                      aElementHandle,
                      &id);

        }
    }
    M2GGeneral::CheckErrorCode(aJni, err);
    M2G_DEBUG_1("M2G_DEBUG: JNI ( M2GSVGElement ) _getElementTypeId: %d - end", id);
    return id;
}

// -----------------------------------------------------------------------------
// Java_com_nokia_microedition_m2g_M2GSVGElement::_getEnumTrait
// -----------------------------------------------------------------------------
/**
 * Calls MM2GSVGProxy::GetEnumTraitL method.
 * @since Series S60 3.0
 * @param aProxy Proxy instance.
 * @param aElementHandle Element handle
 * @param aAttributeTypeId Attribute type id
 * @param aEnumTrait The enumeration value of the specified attribute
 */
LOCAL_C void DoGetEnumTraitL(
    MM2GSVGProxy* aProxy,
    TInt aElementHandle,
    TInt16 aAttributeTypeId,
    TInt16* aEnumTrait)
{
    aProxy->GetEnumTraitL(
        STATIC_CAST(TM2GSvgElementHandle, aElementHandle),
        STATIC_CAST(TM2GSvgAttrType, aAttributeTypeId),
        *aEnumTrait);
}

/**
 * JNI method
 */
JNIEXPORT jshort JNICALL
Java_com_nokia_microedition_m2g_M2GSVGElement__1getEnumTrait(
    JNIEnv* aJni,
    jclass,
    jint aEventSourceHandle,
    jint aSvgProxyHandle,
    jint aElementHandle,
    jshort aAttributeTypeId,
    jboolean aUiToolkit)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGElement ) _getEnumTrait - begin");
    TInt16 enumTrait = 0;
    TInt err = KM2GNotOk;
    if (aUiToolkit)  // If eSWT is the current toolkit
    {
        MSwtClient* client = reinterpret_cast< MSwtClient* >(aEventSourceHandle);
        if (client && aSvgProxyHandle)
        {
            typedef void (MM2GSVGProxy::*pGetEnumTraitL)(const TM2GSvgElementHandle& ,const TM2GSvgAttrType& ,TInt16&);
            pGetEnumTraitL GetEnumTraitL = &MM2GSVGProxy::GetEnumTraitL;
            MM2GSVGProxy* aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);
            TM2GSvgElementHandle lElementHandle = STATIC_CAST(TM2GSvgElementHandle, aElementHandle);
            TM2GSvgAttrType lAttrType = STATIC_CAST(TM2GSvgAttrType, aAttributeTypeId);
            TMethodWrapper3<MM2GSVGProxy, const TM2GSvgElementHandle& ,const TM2GSvgAttrType& ,TInt16&   > GetEnumTrait(
                *aProxy,
                GetEnumTraitL,
                lElementHandle,lAttrType,enumTrait);
            err = client->Execute(GetEnumTrait);
        }
    }
    else
    {
        CM2GEventSource* eventSource = JavaUnhand< CM2GEventSource >(aEventSourceHandle);
        if (eventSource && aSvgProxyHandle)
        {
            err = eventSource->ExecuteTrap(
                      &DoGetEnumTraitL,
                      JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle),
                      aElementHandle,
                      STATIC_CAST(TInt16, aAttributeTypeId),
                      &enumTrait);
        }
    }
    M2GGeneral::CheckErrorCode(aJni, err);
    M2G_DEBUG_1("M2G_DEBUG: JNI ( M2GSVGElement ) _getEnumTrait: %d - end", enumTrait);
    return enumTrait;
}

// -----------------------------------------------------------------------------
// Java_com_nokia_microedition_m2g_M2GSVGElement::_getFirstElementChild
// -----------------------------------------------------------------------------
/**
 * Calls MM2GSVGProxy::GetFirstElementChildL method.
 * @since Series S60 3.0
 * @param aProxy Proxy instance.
 * @param aElementHandle Element handle
 * @param aHandle Element handle to be returned.
 */
LOCAL_C void DoGetFirstElementChildL(
    MM2GSVGProxy* aProxy,
    TInt aElementHandle,
    TInt* aChildHandle)
{
    aProxy->GetFirstElementChildL(aElementHandle, *aChildHandle);
}

/**
 * JNI method
 */
JNIEXPORT jint JNICALL
Java_com_nokia_microedition_m2g_M2GSVGElement__1getFirstElementChild(
    JNIEnv* aJni,
    jclass,
    jint aEventSourceHandle,
    jint aSvgProxyHandle,
    jint aElementHandle,
    jboolean aUiToolkit)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGElement ) _getFirstElementChild - begin");

    TInt childHandle = M2G_INVALID_HANDLE;
    TInt err = KM2GNotOk;
    if (aUiToolkit)  // If eSWT is the current toolkit
    {
        MSwtClient* client = reinterpret_cast< MSwtClient* >(aEventSourceHandle);
        if (client && aSvgProxyHandle)
        {
            typedef void (MM2GSVGProxy::*pGetFirstElementChildL)(const TInt& ,TInt&);
            pGetFirstElementChildL GetFirstElementChildL = &MM2GSVGProxy::GetFirstElementChildL;
            MM2GSVGProxy* aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);
            TMethodWrapper2<MM2GSVGProxy, const TInt& ,TInt& > GetFirstElementChild(
                *aProxy,
                GetFirstElementChildL,
                aElementHandle,
                childHandle);
            err = client->Execute(GetFirstElementChild);
        }
    }
    else
    {
        CM2GEventSource* eventSource = JavaUnhand< CM2GEventSource >(aEventSourceHandle);
        if (eventSource && aSvgProxyHandle)
        {
            err = eventSource->ExecuteTrap(
                      &DoGetFirstElementChildL,
                      JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle),
                      aElementHandle,
                      &childHandle);

        }
    }
    M2GGeneral::CheckErrorCode(aJni, err);
    M2G_DEBUG_1("M2G_DEBUG: JNI ( M2GSVGElement ) _getFirstElementChild: %d - end", childHandle);
    return childHandle;
}

// -----------------------------------------------------------------------------
// Java_com_nokia_microedition_m2g_M2GSVGElement::_getFloatTrait
// -----------------------------------------------------------------------------
/**
 * Calls MM2GSVGProxy::GetFloatTraitL method.
 * @since Series S60 3.0
 * @param aProxy Proxy instance.
 * @param aElementHandle Element handle
 * @param aAttributeTypeId Attribute type id
 * @param aFloatTrait Float attribute to be returned
 */
LOCAL_C void DoGetFloatTraitL(
    MM2GSVGProxy* aProxy,
    TInt aElementHandle,
    TInt16 aAttributeTypeId,
    TReal32* aFloatTrait)
{
    aProxy->GetFloatTraitL(
        STATIC_CAST(TM2GSvgElementHandle, aElementHandle),
        STATIC_CAST(TM2GSvgAttrType, aAttributeTypeId),
        *aFloatTrait);
}

/**
 * JNI method
 */
JNIEXPORT jfloat JNICALL
Java_com_nokia_microedition_m2g_M2GSVGElement__1getFloatTrait(
    JNIEnv* aJni,
    jclass,
    jint aEventSourceHandle,
    jint aSvgProxyHandle,
    jint aElementHandle,
    jshort aAttributeTypeId,
    jboolean aUiToolkit)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGElement ) _getFloatTrait - begin");
    TInt err = KM2GNotOk;
    TReal32 floatTrait = 0;
    if (aUiToolkit)  // If eSWT is the current toolkit
    {
        MSwtClient* client = reinterpret_cast< MSwtClient* >(aEventSourceHandle);
        if (client && aSvgProxyHandle)
        {
            typedef void (MM2GSVGProxy::*pGetFloatTraitL)(const TM2GSvgElementHandle&, const TM2GSvgAttrType& ,TReal32&);
            pGetFloatTraitL GetFloatTraitL = &MM2GSVGProxy::GetFloatTraitL;
            MM2GSVGProxy* aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);
            // TM2GSvgElementHandle lElementHandle = STATIC_CAST( TM2GSvgElementHandle, aElementHandle );
            TM2GSvgAttrType lAttrType = STATIC_CAST(TM2GSvgAttrType, aAttributeTypeId);
            TMethodWrapper3<MM2GSVGProxy, const TM2GSvgElementHandle&, const TM2GSvgAttrType& ,TReal32&  > GetFloatTrait(
                *aProxy,
                GetFloatTraitL,
                aElementHandle,
                lAttrType,
                floatTrait);
            err = client->Execute(GetFloatTrait);
        }
    }
    else
    {
        CM2GEventSource* eventSource = JavaUnhand< CM2GEventSource >(aEventSourceHandle);

        if (eventSource && aSvgProxyHandle)
        {
            err = eventSource->ExecuteTrap(
                      &DoGetFloatTraitL,
                      JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle),
                      aElementHandle,
                      aAttributeTypeId,
                      &floatTrait);

        }
    }
    M2GGeneral::CheckErrorCode(aJni, err);
    M2G_DEBUG_1("M2G_DEBUG: JNI ( M2GSVGElement ) _getFloatTrait: %f - end", floatTrait);
    return STATIC_CAST(jfloat, floatTrait);
}

// -----------------------------------------------------------------------------
// Java_com_nokia_microedition_m2g_M2GSVGElement::_getMatrixTrait
// -----------------------------------------------------------------------------
/**
 * Calls MM2GSVGProxy::GetMatrixTraitL method.
 * @since Series S60 3.0
 * @param aProxy Proxy instance.
 * @param aElementHandle Element handle
 * @param aAttributeTypeId Attribute type id
 * @param aResult Result: "0" if ok and "-1" if nok
 */
LOCAL_C void DoGetMatrixTraitL(
    MM2GSVGProxy* aProxy,
    TInt aElementHandle,
    TInt16 aAttributeTypeId,
    TM2GMatrixData* aMatrixData,
    TInt* aResult)
{
    aProxy->GetMatrixTraitL(
        STATIC_CAST(TM2GSvgElementHandle, aElementHandle),
        STATIC_CAST(TM2GSvgAttrType, aAttributeTypeId),
        *aMatrixData, *aResult);
}

/**
 * JNI method
 */
JNIEXPORT jint JNICALL
Java_com_nokia_microedition_m2g_M2GSVGElement__1getMatrixTrait(
    JNIEnv* aJni,
    jclass,
    jint aEventSourceHandle,
    jint aSvgProxyHandle,
    jint aElementHandle,
    jshort aAttributeTypeId,
    jfloatArray aMatrixComponents,
    jboolean aUiToolKit)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGElement ) _getMatrixTrait - begin");
    TM2GMatrixData matrix;
    TInt result = -1;
    TInt err = KM2GNotOk;
    if (aUiToolKit)  // If eSWT is the current toolkit
    {
        MSwtClient* client = reinterpret_cast< MSwtClient* >(aEventSourceHandle);
        if (client && aSvgProxyHandle)
        {
            typedef void (MM2GSVGProxy::*pGetMatrixTraitL)(const TM2GSvgElementHandle& ,const TM2GSvgAttrType& ,TM2GMatrixData& , TInt&);
            pGetMatrixTraitL GetMatrixTraitL = &MM2GSVGProxy::GetMatrixTraitL;
            MM2GSVGProxy* aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);
            // TM2GSvgElementHandle lElementHandle = STATIC_CAST( TM2GSvgElementHandle, aElementHandle );
            TM2GSvgAttrType lAttrType = STATIC_CAST(TM2GSvgAttrType, aAttributeTypeId);
            TMethodWrapper4<MM2GSVGProxy, const TM2GSvgElementHandle& ,const TM2GSvgAttrType& ,TM2GMatrixData& , TInt&  > GetMatrixTrait(
                *aProxy,
                GetMatrixTraitL,
                aElementHandle,
                lAttrType,
                matrix,
                result);
            err = client->Execute(GetMatrixTrait);
        }
    }
    else
    {
        CM2GEventSource* eventSource = JavaUnhand< CM2GEventSource >(aEventSourceHandle);
        if (eventSource && aSvgProxyHandle)
        {
            err = eventSource->ExecuteTrap(
                      &DoGetMatrixTraitL,
                      JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle),
                      aElementHandle,
                      aAttributeTypeId,
                      &matrix,
                      &result);
        }
    }
    if (M2GGeneral::CheckErrorCode(aJni, err) == KM2GOk)
    {
        // Init matrix object
        jfloat* components = aJni->GetFloatArrayElements(aMatrixComponents, 0);
        TInt count =
            M2GGeneral::Min(aJni->GetArrayLength(aMatrixComponents), matrix.Count());
        for (TInt index = 0; index < count; index++)
        {
            components[ index ] = matrix[ index ];
        }
        // Release data source
        aJni->ReleaseFloatArrayElements(aMatrixComponents, components, 0);
    }

    M2G_DEBUG_6("M2G_DEBUG: JNI ( M2GSVGElement ) _getMatrixTrait: %f, %f, %f, %f, %f, %f - end", matrix[ 0 ], matrix[ 1 ], matrix[ 2 ], matrix[ 3 ], matrix[ 4 ], matrix[ 5 ]);
    return result;
}

// -----------------------------------------------------------------------------
// Java_com_nokia_microedition_m2g_M2GSVGElement::_getNextElementSibling
// -----------------------------------------------------------------------------
/**
 * Calls MM2GSVGProxy::GetNextElementSiblingL method.
 * @since Series S60 3.0
 * @param aProxy Proxy instance.
 * @param aElementHandle Element pointer.
 * @param aSiblingHandle Sibling handle
 */
LOCAL_C void DoGetNextElementSiblingL(
    MM2GSVGProxy* aProxy,
    TInt aElementHandle,
    TInt* aSiblingHandle)
{
    aProxy->GetNextElementSiblingL(aElementHandle, *aSiblingHandle);
}

/**
 * JNI method
 */
JNIEXPORT jint JNICALL
Java_com_nokia_microedition_m2g_M2GSVGElement__1getNextElementSibling(
    JNIEnv* aJni,
    jclass,
    jint aEventSourceHandle,
    jint aSvgProxyHandle,
    jint aElementHandle,
    jboolean aUiToolKit)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGElement ) _getNextElementSibling - begin");
    TInt siblingHandle = M2G_INVALID_HANDLE;
    TInt err = KM2GNotOk;
    if (aUiToolKit)  // If eSWT is the current toolkit
    {
        MSwtClient* client = reinterpret_cast< MSwtClient* >(aEventSourceHandle);
        if (client && aSvgProxyHandle)
        {
            typedef void (MM2GSVGProxy::*pGetNextElementSiblingL)(const TInt& ,TInt&);
            pGetNextElementSiblingL GetNextElementSiblingL = &MM2GSVGProxy::GetNextElementSiblingL;
            MM2GSVGProxy* aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);
            TMethodWrapper2<MM2GSVGProxy, const TInt& ,TInt& > GetNextElementSibling(
                *aProxy,
                GetNextElementSiblingL,
                aElementHandle,
                siblingHandle);
            err = client->Execute(GetNextElementSibling);
        }
    }
    else
    {
        CM2GEventSource* eventSource = JavaUnhand< CM2GEventSource >(aEventSourceHandle);
        if (eventSource && aSvgProxyHandle)
        {
            err = eventSource->ExecuteTrap(
                      &DoGetNextElementSiblingL,
                      JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle),
                      aElementHandle, &siblingHandle);

        }
    }
    M2GGeneral::CheckErrorCode(aJni, err);
    M2G_DEBUG_1("M2G_DEBUG: JNI ( M2GSVGElement ) _getNextElementSibling: %d - end", siblingHandle);
    return siblingHandle;
}

// -----------------------------------------------------------------------------
// Java_com_nokia_microedition_m2g_M2GSVGElement::_getParent
// -----------------------------------------------------------------------------
/**
 * Calls MM2GSVGProxy::GetParentL method.
 * @since Series S60 3.0
 * @param aProxy Proxy instance.
 * @param aElementHandle Element pointer.
 * @param aParentHandle Parent handle to be returned.
 */
LOCAL_C void DoGetParentL(
    MM2GSVGProxy* aProxy,
    TInt aElementHandle,
    TInt* aParentHandle)
{
    aProxy->GetParentL(aElementHandle, *aParentHandle);
}

/**
 * JNI method
 */
JNIEXPORT jint JNICALL
Java_com_nokia_microedition_m2g_M2GSVGElement__1getParent(
    JNIEnv* aJni,
    jclass,
    jint aEventSourceHandle,
    jint aSvgProxyHandle,
    jint aElementHandle,
    jboolean aUiToolKit)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGElement ) _getParent - begin");
    TInt parentHandle = M2G_INVALID_HANDLE;
    TInt err = KM2GNotOk;
    if (aUiToolKit)  // If eSWT is the current toolkit
    {
        MSwtClient* client = reinterpret_cast< MSwtClient* >(aEventSourceHandle);
        if (client && aSvgProxyHandle)
        {
            typedef void (MM2GSVGProxy::*pGetParentL)(const TInt& ,TInt&);
            pGetParentL GetParentL = &MM2GSVGProxy::GetParentL;
            MM2GSVGProxy* aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);
            TMethodWrapper2<MM2GSVGProxy, const TInt& ,TInt& > GetParent(
                *aProxy,
                GetParentL,
                aElementHandle,
                parentHandle);
            err = client->Execute(GetParent);
        }
    }
    else
    {
        CM2GEventSource* eventSource = JavaUnhand< CM2GEventSource >(aEventSourceHandle);
        if (eventSource && aSvgProxyHandle)
        {
            err = eventSource->ExecuteTrap(
                      &DoGetParentL,
                      JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle),
                      aElementHandle, &parentHandle);

        }
    }
    M2GGeneral::CheckErrorCode(aJni, err);
    M2G_DEBUG_1("M2G_DEBUG: JNI ( M2GSVGElement ) _getParent: %d - end", parentHandle);
    return parentHandle;
}

// -----------------------------------------------------------------------------
// Java_com_nokia_microedition_m2g_M2GSVGElement::_getPathTrait
// -----------------------------------------------------------------------------
/**
 * Calls MM2GSVGProxy::GetPathTraitL method.
 * @since Series S60 3.0
 * @param aProxy Proxy instance.
 * @param aElementHandle Element pointer.
 * @param aAttributeTypeId Attribute type.
 */

LOCAL_C void DoGetPathTraitL(
    MM2GSVGProxy* aProxy,
    TInt aElementHandle,
    TInt16 aAttributeTypeId,
    TInt* aPathTrait)
{
    aProxy->GetPathTraitL(
        STATIC_CAST(TM2GSvgElementHandle, aElementHandle),
        STATIC_CAST(TM2GSvgAttrType, aAttributeTypeId),
        *aPathTrait);
}

/**
 * JNI method
 */
JNIEXPORT jint JNICALL
Java_com_nokia_microedition_m2g_M2GSVGElement__1getPathTrait(
    JNIEnv* aJni,
    jclass,
    jint aEventSourceHandle,
    jint aSvgProxyHandle,
    jint aElementHandle,
    jshort aAttributeTypeId,
    jboolean aUiToolKit)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGElement ) _getPathTrait - begin");
    TInt aPathTrait = 0;
    TInt err = KM2GNotOk;
    if (aUiToolKit)  // If eSWT is the current toolkit
    {
        MSwtClient* client = reinterpret_cast< MSwtClient* >(aEventSourceHandle);
        if (client && aSvgProxyHandle)
        {
            typedef void (MM2GSVGProxy::*pGetPathTraitL)(const TM2GSvgElementHandle& ,const TM2GSvgAttrType& ,TInt&);
            pGetPathTraitL GetPathTraitL = &MM2GSVGProxy::GetPathTraitL;
            MM2GSVGProxy* aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);
            TM2GSvgElementHandle lElementHandle = STATIC_CAST(TM2GSvgElementHandle, aElementHandle);
            TM2GSvgAttrType lAttrType = STATIC_CAST(TM2GSvgAttrType, aAttributeTypeId);
            TMethodWrapper3<MM2GSVGProxy, const TM2GSvgElementHandle& ,const TM2GSvgAttrType& ,TInt& > GetPathTrait(
                *aProxy,
                GetPathTraitL,
                lElementHandle,
                lAttrType,
                aPathTrait);
            err = client->Execute(GetPathTrait);
        }
    }
    else
    {
        CM2GEventSource* eventSource = JavaUnhand< CM2GEventSource >(aEventSourceHandle);
        if (eventSource && aSvgProxyHandle)
        {
            err = eventSource->ExecuteTrap(
                      &DoGetPathTraitL,
                      JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle),
                      aElementHandle,
                      aAttributeTypeId, &aPathTrait);

        }
    }
    M2GGeneral::CheckErrorCode(aJni, err);
    M2G_DEBUG_1("M2G_DEBUG: JNI ( M2GSVGElement ) _getPathTrait: %d - end", aPathTrait);
    return aPathTrait;
}

// -----------------------------------------------------------------------------
// Java_com_nokia_microedition_m2g_M2GSVGElement::_getRectTrait
// -----------------------------------------------------------------------------
/**
 * Calls MM2GSVGProxy::GetRectTraitL method.
 * @since Series S60 3.0
 * @param aProxy Proxy instance.
 * @param aElementHandle Element handle
 * @param aAttributeTypeId Attribute type id
 * @param aRectData Returns rect components
 * @param aResult Result "0" if ok and "-1" if nok
 */
LOCAL_C void DoGetRectTraitL(
    MM2GSVGProxy* aProxy,
    TInt aElementHandle,
    TInt16 aAttributeTypeId,
    TM2GRectData* aRectData,
    TInt* aResult)
{
    aProxy->GetRectTraitL(
        STATIC_CAST(TM2GSvgElementHandle, aElementHandle),
        STATIC_CAST(TM2GSvgAttrType, aAttributeTypeId),
        *aRectData, *aResult);
}
/**
 * JNI method
 */
JNIEXPORT jint JNICALL
Java_com_nokia_microedition_m2g_M2GSVGElement__1getRectTrait(
    JNIEnv* aJni,
    jclass,
    jint aEventSourceHandle,
    jint aSvgProxyHandle,
    jint aElementHandle,
    jshort aAttributeTypeId,
    jfloatArray aRectComponents,
    jboolean aUiToolKit)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGElement ) _getRectTrait - begin");
    TM2GRectData rect;
    TInt result = 0;
    TInt err = KM2GNotOk;
    if (aUiToolKit)  // If eSWT is the current toolkit
    {
        MSwtClient* client = reinterpret_cast< MSwtClient*  >(aEventSourceHandle);
        if (client && aSvgProxyHandle)
        {
            typedef void (MM2GSVGProxy::*pGetRectTraitL)(const TM2GSvgElementHandle& ,const TM2GSvgAttrType& ,TM2GRectData& , TInt&);
            pGetRectTraitL GetRectTraitL = &MM2GSVGProxy::GetRectTraitL;
            MM2GSVGProxy* aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);
            TM2GSvgElementHandle lElementHandle = STATIC_CAST(TM2GSvgElementHandle, aElementHandle);
            TM2GSvgAttrType lAttrType = STATIC_CAST(TM2GSvgAttrType, aAttributeTypeId);
            TMethodWrapper4<MM2GSVGProxy, const TM2GSvgElementHandle& ,const TM2GSvgAttrType& ,TM2GRectData& , TInt& > GetRectTrait(
                *aProxy,
                GetRectTraitL,
                lElementHandle,
                lAttrType,
                rect,
                result);
            err = client->Execute(GetRectTrait);
        }
    }
    else
    {
        CM2GEventSource* eventSource = JavaUnhand< CM2GEventSource >(aEventSourceHandle);
        if (eventSource && aSvgProxyHandle)
        {
            err = eventSource->ExecuteTrap(
                      &DoGetRectTraitL,
                      JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle),
                      aElementHandle,
                      aAttributeTypeId,
                      &rect, &result);
        }
    }
    M2GGeneral::CheckErrorCode(aJni, err);
    aJni->SetFloatArrayRegion(
        REINTERPRET_CAST(jfloatArray, aRectComponents),
        0,
        rect.Count(), REINTERPRET_CAST(jfloat*, rect.Begin()));

    M2G_DEBUG_1("M2G_DEBUG: JNI ( M2GSVGElement ) _getRectTrait: %d - end", result);
    return result;
}

// -----------------------------------------------------------------------------
// Java_com_nokia_microedition_m2g_M2GSVGElement::_getRootElement
// -----------------------------------------------------------------------------
/**
 * Calls MM2GSVGProxy::GetRootElementL method.
 * @since Series S60 3.0
 * @param aProxy Proxy instance.
 * @param aDocumentHandle Document pointer.
 * @param aRootElementHandle Root element handle
 */
LOCAL_C void DoGetRootElementL(MM2GSVGProxy* aProxy, TInt aDocumentHandle,
                               TInt* aRootElementHandle)
{
    aProxy->GetRootElementL(
        STATIC_CAST(TM2GSvgDocumentHandle, aDocumentHandle),
        *aRootElementHandle);
}

/**
 * JNI method
 */
JNIEXPORT jint JNICALL
Java_com_nokia_microedition_m2g_M2GSVGElement__1getRootElement(
    JNIEnv* aJni,
    jclass,
    jint aEventSourceHandle,
    jint aSvgProxyHandle,
    jint aDocumentHandle,
    jboolean aUiToolKit)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGElement ) _getRootElement - begin");
    TInt rootElementHandle = M2G_INVALID_HANDLE;
    TInt err = KM2GNotOk;
    if (aUiToolKit)  // If eSWT is the current toolkit
    {
        MSwtClient* client = reinterpret_cast< MSwtClient* >(aEventSourceHandle);
        if (client && aSvgProxyHandle)
        {
            typedef void (MM2GSVGProxy::*pGetRootElementL)(const TM2GSvgDocumentHandle& ,TInt&);
            pGetRootElementL GetRootElementL = &MM2GSVGProxy::GetRootElementL;
            MM2GSVGProxy* aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);
            TM2GSvgDocumentHandle lHandle = STATIC_CAST(TM2GSvgDocumentHandle, aDocumentHandle);
            TMethodWrapper2<MM2GSVGProxy, const TM2GSvgDocumentHandle& ,TInt&  > GetRootElement(
                *aProxy,
                GetRootElementL,
                lHandle,
                rootElementHandle
            );
            err = client->Execute(GetRootElement);
        }
    }
    else
    {
        CM2GEventSource* eventSource = JavaUnhand< CM2GEventSource >(aEventSourceHandle);

        if (eventSource && aSvgProxyHandle)
        {
            err = eventSource->ExecuteTrap(
                      &DoGetRootElementL,
                      JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle),
                      aDocumentHandle,
                      &rootElementHandle);
        }
    }
    M2GGeneral::CheckErrorCode(aJni, err);

    M2G_DEBUG_1("M2G_DEBUG: JNI ( M2GSVGElement ) _getRootElement: %d - end", rootElementHandle);
    return rootElementHandle;
}

// -----------------------------------------------------------------------------
// Java_com_nokia_microedition_m2g_M2GSVGElement::_getScreenBBox
// -----------------------------------------------------------------------------
/**
 * Calls MM2GSVGProxy::GetScreenBBoxL method.
 * @since Series S60 3.0
 * @param aProxy Proxy instance.
 * @param aElementHandle Element handle
 * @param aRectData Returns rect components
 */
LOCAL_C void DoGetScreenBBoxL(
    MM2GSVGProxy* aProxy,
    TInt aElementHandle,
    TM2GScreenBBoxData* aScreenBBoxData)
{
    aProxy->GetScreenBBoxL(
        STATIC_CAST(TM2GSvgElementHandle, aElementHandle),
        *aScreenBBoxData);
}
/**
 * Class: com_nokia_microedition_m2g_M2GSVGElement
 * Method: getScreenBBox
 * Signature:
 */
JNIEXPORT void JNICALL
Java_com_nokia_microedition_m2g_M2GSVGElement__1getScreenBBox(
    JNIEnv* aJni,
    jclass,
    jint aEventSourceHandle,
    jint aSvgProxyHandle,
    jint aElementHandle,
    jfloatArray aScreenBBoxComponents,
    jboolean aUiToolKit)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGElement ) _getScreenBBox - begin");
    TM2GScreenBBoxData screenBBox;
    TInt err = KM2GNotOk;
    if (aUiToolKit)  // If eSWT is the current toolkit
    {
        MSwtClient* client = reinterpret_cast< MSwtClient* >(aEventSourceHandle);
        if (client && aSvgProxyHandle)
        {
            typedef void (MM2GSVGProxy::*pGetScreenBBoxL)(const TM2GSvgElementHandle& ,TM2GScreenBBoxData&);
            pGetScreenBBoxL GetScreenBBoxL = &MM2GSVGProxy::GetScreenBBoxL;
            MM2GSVGProxy* aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);
            TM2GSvgElementHandle lElementHandle = STATIC_CAST(TM2GSvgElementHandle, aElementHandle);
            TMethodWrapper2<MM2GSVGProxy, const TM2GSvgElementHandle& ,TM2GScreenBBoxData&  > GetScreenBBox(
                *aProxy,
                GetScreenBBoxL,
                lElementHandle,
                screenBBox
            );
            err = client->Execute(GetScreenBBox);
        }
    }
    else
    {
        CM2GEventSource* eventSource = JavaUnhand< CM2GEventSource >(aEventSourceHandle);
        if (eventSource && aSvgProxyHandle)
        {
            err = eventSource->ExecuteTrap(
                      &DoGetScreenBBoxL,
                      JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle),
                      aElementHandle,
                      &screenBBox);
        }
    }
    M2GGeneral::CheckErrorCode(aJni, err);

    aJni->SetFloatArrayRegion(
        REINTERPRET_CAST(jfloatArray, aScreenBBoxComponents),
        0,
        screenBBox.Count(), REINTERPRET_CAST(jfloat*, screenBBox.Begin()));

    M2G_DEBUG_4("M2G_DEBUG: JNI ( M2GSVGElement ) _getScreenBBox: %f, %f, %f, %f - end", screenBBox[ 0 ], screenBBox[ 1 ], screenBBox[ 2 ], screenBBox[ 3 ]);
}

// -----------------------------------------------------------------------------
// Java_com_nokia_microedition_m2g_M2GSVGElement::_getStringTrait
// -----------------------------------------------------------------------------
/**
 * Calls MM2GSVGProxy::GetStringTraitL method.
 * @since Series S60 3.0
 * @param aProxy Proxy instance.
 * @param aElementHandle Element pointer.
 * @param aAttributeTypeId Attribute type.
 * @param aAttribute Returns a string attribute
 */
LOCAL_C void DoGetStringTraitL(
    MM2GSVGProxy* aProxy,
    TInt aElementHandle,
    TInt16 aAttributeTypeId,
    TPtrC16* aAttribute)
{
    aProxy->GetStringTraitL(
        STATIC_CAST(TM2GSvgElementHandle, aElementHandle),
        STATIC_CAST(TM2GSvgAttrType, aAttributeTypeId),
        *aAttribute);
}

/**
 * JNI method
 */
JNIEXPORT jstring JNICALL
Java_com_nokia_microedition_m2g_M2GSVGElement__1getStringTrait(
    JNIEnv* aJni,
    jclass,
    jint aEventSourceHandle,
    jint aSvgProxyHandle,
    jint aElementHandle,
    jshort aAttributeTypeId,
    jboolean aUiToolKit)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGElement ) _getStringTrait - begin");

    TPtrC16 attribute;
    TInt err = KM2GNotOk;
    TInt Res;
    if (aUiToolKit)  // If eSWT is the current toolkit
    {
        MSwtClient* client = reinterpret_cast< MSwtClient* >(aEventSourceHandle);
        if (client && aSvgProxyHandle)
        {
            typedef TInt(MM2GSVGProxy::*pGetStringTraitL)(const TM2GSvgElementHandle& ,const TM2GSvgAttrType& ,TPtrC16&);
            pGetStringTraitL GetStringTraitL = &MM2GSVGProxy::GetStringTraitL;
            MM2GSVGProxy* aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);
            TM2GSvgElementHandle lElementHandle = STATIC_CAST(TM2GSvgElementHandle, aElementHandle);
            TM2GSvgAttrType lAttrType = STATIC_CAST(TM2GSvgAttrType, aAttributeTypeId);
            TMethodWrapperR3< TInt , TInt , MM2GSVGProxy, const TM2GSvgElementHandle& ,const TM2GSvgAttrType& , TPtrC16&  >GetStringTrait(
                Res,
                *aProxy,
                GetStringTraitL,
                lElementHandle,
                lAttrType,
                attribute
            );
            err = client->Execute(GetStringTrait);
        }
    }
    else
    {
        CM2GEventSource* eventSource = JavaUnhand< CM2GEventSource >(aEventSourceHandle);
        if (eventSource && aSvgProxyHandle)
        {
            err = eventSource->ExecuteTrap(
                      &DoGetStringTraitL,
                      JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle),
                      aElementHandle,
                      aAttributeTypeId,
                      &attribute);
        }
    }
    if ((M2GGeneral::CheckErrorCode(aJni, err) == KM2GOk) &&
            (attribute.Length() > 0))
    {
        return CreateJavaString(*aJni, attribute);
    }

    return NULL;
    M2G_DEBUG_1("M2G_DEBUG: JNI ( M2GSVGElement ) _getStringTrait: %d - end", err);
}

// -----------------------------------------------------------------------------
// Java_com_nokia_microedition_m2g_M2GSVGElement::_getUsedFromElement
// -----------------------------------------------------------------------------
/**
 * Calls MM2GSVGProxy::GetUsedFromElementL method.
 * @since Series S60 3.0
 * @param aProxy Proxy instance.
 * @param aElementHandle Element pointer.
 * @param aHandle Return element handle
 */
LOCAL_C void DoGetUsedFromElementL(
    MM2GSVGProxy* aProxy,
    TInt aElementHandle,
    TInt* aHandle)
{
    aProxy->GetUsedFromElementL(
        STATIC_CAST(TM2GSvgElementHandle, aElementHandle),
        *aHandle);
}

/**
 * JNI method
 */
JNIEXPORT jint JNICALL
Java_com_nokia_microedition_m2g_M2GSVGElement__1getUsedFromElement(
    JNIEnv* aJni,
    jclass,
    jint aEventSourceHandle,
    jint aSvgProxyHandle,
    jint aElementHandle,
    jboolean aUiToolKit)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGElement ) _getUsedFromElement - begin");
    TInt handle = M2G_INVALID_HANDLE;
    TInt err = KM2GNotOk;
    if (aUiToolKit)  // If eSWT is the current toolkit
    {
        MSwtClient* client = reinterpret_cast< MSwtClient* >(aEventSourceHandle);
        if (client && aSvgProxyHandle)
        {
            typedef void (MM2GSVGProxy::*pGetUsedFromElementL)(const TM2GSvgElementHandle& , TInt &);
            pGetUsedFromElementL GetUsedFromElementL = &MM2GSVGProxy::GetUsedFromElementL;
            MM2GSVGProxy* aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);
            TM2GSvgElementHandle lElementHandle = STATIC_CAST(TM2GSvgElementHandle, aElementHandle);
            TMethodWrapper2<MM2GSVGProxy, const TM2GSvgElementHandle& ,TInt &  >GetUsedFromElement(
                *aProxy,
                GetUsedFromElementL,
                lElementHandle,
                handle
            );
            err = client->Execute(GetUsedFromElement);
        }
    }
    else
    {
        CM2GEventSource* eventSource = JavaUnhand< CM2GEventSource >(aEventSourceHandle);
        if (eventSource && aSvgProxyHandle)
        {
            err = eventSource->ExecuteTrap(
                      &DoGetUsedFromElementL,
                      JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle),
                      aElementHandle, &handle);

        }
    }
    M2GGeneral::CheckErrorCode(aJni, err);
    M2G_DEBUG_1("M2G_DEBUG: JNI ( M2GSVGElement ) _getUsedFromElement: %d - end", handle);
    return handle;
}


// -----------------------------------------------------------------------------
// Java_com_nokia_microedition_m2g_M2GSVGElement::_insertBefore
// -----------------------------------------------------------------------------
/**
 * Calls MM2GSVGProxy::InsertBeforeL method.
 * @since Series S60 3.0
 * @param aProxy Proxy instance.
 * @param aElementHandle Element handle
 * @param aRectData Returns rect components
 */
LOCAL_C void DoInsertBeforeL(
    MM2GSVGProxy* aProxy,
    TInt aElementHandle,
    TInt aNewChildElementHandle,
    TInt aReferenceElementHandle)
{
    aProxy->InsertBeforeL(
        STATIC_CAST(TM2GSvgElementHandle, aElementHandle),
        STATIC_CAST(TM2GSvgElementHandle, aNewChildElementHandle),
        STATIC_CAST(TM2GSvgElementHandle, aReferenceElementHandle));
}
/**
 * Class: com_nokia_microedition_m2g_M2GSVGElement
 * Method: insertBefore
 * Signature:
 */
JNIEXPORT void JNICALL
Java_com_nokia_microedition_m2g_M2GSVGElement__1insertBefore(
    JNIEnv* aJni,
    jclass,
    jint aEventSourceHandle,
    jint aSvgProxyHandle,
    jint aElementHandle,
    jint aNewChildElementHandle,
    jint aReferenceElementHandle ,
    jboolean aUiToolKit)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGElement ) _insertBefore - begin");
    TInt err = KM2GNotOk;
    if (aUiToolKit)  // If eSWT is the current toolkit
    {
        MSwtClient* client = reinterpret_cast< MSwtClient* >(aEventSourceHandle);
        if (client && aSvgProxyHandle)
        {
            typedef void (MM2GSVGProxy::*pInsertBeforeL)(const TM2GSvgElementHandle& , const TM2GSvgElementHandle&,const TM2GSvgElementHandle&);
            pInsertBeforeL InsertBeforeL = &MM2GSVGProxy::InsertBeforeL;
            MM2GSVGProxy* aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);
            TM2GSvgElementHandle lElementHandle = STATIC_CAST(TM2GSvgElementHandle, aElementHandle);
            TM2GSvgElementHandle lNewChildElementHandle = STATIC_CAST(TM2GSvgElementHandle, aNewChildElementHandle);
            TM2GSvgElementHandle lReferenceElementHandle = STATIC_CAST(TM2GSvgElementHandle, aReferenceElementHandle);
            TMethodWrapper3<MM2GSVGProxy, const TM2GSvgElementHandle& , const TM2GSvgElementHandle&,const TM2GSvgElementHandle&   >InsertBefore(
                *aProxy,
                InsertBeforeL,
                lElementHandle,
                lNewChildElementHandle,
                lReferenceElementHandle
            );
            err = client->Execute(InsertBefore);
        }
    }
    else
    {
        CM2GEventSource* eventSource = JavaUnhand< CM2GEventSource >(aEventSourceHandle);
        if (eventSource && aSvgProxyHandle)
        {
            err = eventSource->ExecuteTrap(
                      &DoInsertBeforeL,
                      JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle),
                      aElementHandle,
                      aNewChildElementHandle,
                      aReferenceElementHandle);
        }
    }
    M2GGeneral::CheckErrorCode(aJni, err);
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGElement ) _insertBefore - end");
}

// -----------------------------------------------------------------------------
// Java_com_nokia_microedition_m2g_M2GSVGElement::_isUsed
// -----------------------------------------------------------------------------
/**
 * Calls MM2GSVGProxy::IsUsedL method.
 * @since Series S60 3.0
 * @param aProxy Proxy instance.
 * @param aElementHandle Element pointer.
 * @param aResult Return "1" if used otherwise returns "0"
 */
LOCAL_C void DoIsUsedL(
    MM2GSVGProxy* aProxy,
    TInt aElementHandle,
    TInt* aResult)
{
    aProxy->IsUsedL(STATIC_CAST(TM2GSvgElementHandle, aElementHandle), *aResult);
}

/**
 * JNI method
 */
JNIEXPORT jint JNICALL
Java_com_nokia_microedition_m2g_M2GSVGElement__1isUsed(
    JNIEnv* aJni,
    jclass,
    jint aEventSourceHandle,
    jint aSvgProxyHandle,
    jint aElementHandle ,
    jboolean aUiToolKit)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGElement ) _isUsed - begin");
    TInt result = KM2GNotOk;
    TInt err = KM2GNotOk;
    if (aUiToolKit)  // If eSWT is the current toolkit
    {
        MSwtClient* client = reinterpret_cast< MSwtClient* >(aEventSourceHandle);
        if (client && aSvgProxyHandle)
        {
            typedef void (MM2GSVGProxy::*pIsUsedL)(const TM2GSvgElementHandle& , TInt&);
            pIsUsedL IsUsedL = &MM2GSVGProxy::IsUsedL;
            MM2GSVGProxy* aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);
            TM2GSvgElementHandle lElementHandle = STATIC_CAST(TM2GSvgElementHandle, aElementHandle);
            TMethodWrapper2<MM2GSVGProxy, const TM2GSvgElementHandle& , TInt&   >IsUsed(
                *aProxy,
                IsUsedL,
                lElementHandle,
                result
            );
            err = client->Execute(IsUsed);
        }
    }
    else
    {
        CM2GEventSource* eventSource = JavaUnhand< CM2GEventSource >(aEventSourceHandle);

        if (eventSource && aSvgProxyHandle)
        {
            err = eventSource->ExecuteTrap(
                      &DoIsUsedL,
                      JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle),
                      aElementHandle, &result);
        }
    }
    M2GGeneral::CheckErrorCode(aJni, err);
    M2G_DEBUG_1("M2G_DEBUG: JNI ( M2GSVGElement ) _isUsed: %d - end", result);
    return result;
}

// -----------------------------------------------------------------------------
// Java_com_nokia_microedition_m2g_M2GSVGElement::_removeChild
// -----------------------------------------------------------------------------
/**
 * Calls MM2GSVGProxy::RemoveChild method.
 * @since Series S60 3.0
 * @param aProxy Proxy instance.
 * @param aElementHandle Element pointer.
 * @param aChildElementHandle child element pointer
 * @param aHandle Handle to removed element.
 */
LOCAL_C void DoRemoveChildL(
    MM2GSVGProxy* aProxy,
    TInt aElementHandle,
    TInt aChildElementHandle,
    TInt* aHandle)
{
    aProxy->RemoveChildL(
        STATIC_CAST(TM2GSvgElementHandle, aElementHandle),
        STATIC_CAST(TM2GSvgElementHandle, aChildElementHandle), *aHandle);
}

/**
 * JNI method
 */
JNIEXPORT jint JNICALL
Java_com_nokia_microedition_m2g_M2GSVGElement__1removeChild(
    JNIEnv* aJni,
    jclass,
    jint aEventSourceHandle,
    jint aSvgProxyHandle,
    jint aElementHandle,
    jint aChildElementHandle,
    jboolean aUiToolKit)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGElement ) _removeChild - begin");
    TInt handle = M2G_INVALID_HANDLE;
    TInt err = KM2GNotOk;
    if (aUiToolKit)  // If eSWT is the current toolkit
    {
        MSwtClient* client = reinterpret_cast< MSwtClient* >(aEventSourceHandle);
        if (client && aSvgProxyHandle)
        {
            typedef void (MM2GSVGProxy::*pRemoveChildL)(const TM2GSvgElementHandle& , const TM2GSvgElementHandle&, TInt&);
            pRemoveChildL RemoveChildL = &MM2GSVGProxy::RemoveChildL;
            MM2GSVGProxy* aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);
            TM2GSvgElementHandle lElementHandle = STATIC_CAST(TM2GSvgElementHandle, aElementHandle);
            TM2GSvgElementHandle lChildElementHandle = STATIC_CAST(TM2GSvgElementHandle, aChildElementHandle);
            TMethodWrapper3<MM2GSVGProxy, const TM2GSvgElementHandle& , const TM2GSvgElementHandle&, TInt&   >RemoveChild(
                *aProxy,
                RemoveChildL,
                lElementHandle,
                lChildElementHandle,
                handle
            );
            err = client->Execute(RemoveChild);
        }
    }
    else
    {
        CM2GEventSource* eventSource = JavaUnhand< CM2GEventSource >(aEventSourceHandle);

        if (eventSource && aSvgProxyHandle)
        {
            err = eventSource->ExecuteTrap(
                      &DoRemoveChildL,
                      JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle),
                      aElementHandle,
                      aChildElementHandle, &handle);
        }
    }
    M2GGeneral::CheckErrorCode(aJni, err);

    M2G_DEBUG_1("M2G_DEBUG: JNI ( M2GSVGElement ) _removeChild: %d - end", handle);
    return handle;
}

// -----------------------------------------------------------------------------
// Java_com_nokia_microedition_m2g_M2GSVGElement::_setColorTrait
// -----------------------------------------------------------------------------
/**
 * Calls MM2GSVGProxy::SetColorTraitL method.
 * @since Series S60 3.0
 * @param aProxy Proxy instance.
 * @param aElementHandle Element pointer.
 * @param aAttributeTypeId -
 * @param aColorData -
 */
LOCAL_C void DoSetColorTraitL(
    MM2GSVGProxy* aProxy,
    TInt aElementHandle,
    TInt16 aAttributeTypeId,
    TM2GColorData* aColorData)
{
    aProxy->SetColorTraitL(
        STATIC_CAST(TM2GSvgElementHandle, aElementHandle),
        STATIC_CAST(TM2GSvgAttrType, aAttributeTypeId),
        *aColorData);
}

/**
 * JNI method
 */
JNIEXPORT void JNICALL
Java_com_nokia_microedition_m2g_M2GSVGElement__1setColorTrait(
    JNIEnv* aJni,
    jclass,
    jint aEventSourceHandle,
    jint aSvgProxyHandle,
    jint aElementHandle,
    jshort aAttributeTypeId,
    jint aRed, jint aGreen, jint aBlue,
    jboolean aUiToolKit)
{
    M2G_DEBUG_3("M2G_DEBUG: JNI ( M2GSVGElement ) _setColorTrait: R=%d, G=%d, B=%d - begin", aRed, aGreen, aBlue);
    TM2GColorData color;
    color[ 0 ] = aRed;
    color[ 1 ] = aGreen;
    color[ 2 ] = aBlue;
    TInt err = KM2GNotOk;

    if (aUiToolKit)  // If eSWT is the current toolkit
    {
        MSwtClient* client = reinterpret_cast< MSwtClient* >(aEventSourceHandle);
        if (client && aSvgProxyHandle)
        {
            typedef void (MM2GSVGProxy::*pSetColorTraitL)(const TM2GSvgElementHandle& , const TM2GSvgAttrType& , const TM2GColorData&);
            pSetColorTraitL SetColorTraitL = &MM2GSVGProxy::SetColorTraitL;
            MM2GSVGProxy* aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);
            TM2GSvgElementHandle lElementHandle = STATIC_CAST(TM2GSvgElementHandle, aElementHandle);
            TM2GSvgAttrType lAttrType = STATIC_CAST(TM2GSvgAttrType, aAttributeTypeId);
            TMethodWrapper3<MM2GSVGProxy, const TM2GSvgElementHandle& , const TM2GSvgAttrType& , const TM2GColorData&  >SetColorTrait(
                *aProxy,
                SetColorTraitL,
                lElementHandle,
                lAttrType,
                color
            );
            err = client->Execute(SetColorTrait);
        }
    }
    else
    {
        CM2GEventSource* eventSource = JavaUnhand< CM2GEventSource >(aEventSourceHandle);
        if (eventSource && aSvgProxyHandle)
        {
            err = eventSource->ExecuteTrap(
                      &DoSetColorTraitL,
                      JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle),
                      aElementHandle,
                      aAttributeTypeId,
                      &color);
        }
    }
    M2GGeneral::CheckErrorCode(aJni, err);
    M2G_DEBUG_1("M2G_DEBUG: JNI ( M2GSVGElement ) _setColorTrait: %d - end", err);
}


// -----------------------------------------------------------------------------
// Java_com_nokia_microedition_m2g_M2GSVGElement::_setEnumTrait
// -----------------------------------------------------------------------------
/**
 * Calls MM2GSVGProxy::SetEnumTraitL method.
 * @since Series S60 3.0
 * @param aProxy Proxy instance.
 * @param aElementHandle Element pointer.
 * @param aAttributeTypeId -
 * @param aValue -
 */
LOCAL_C void DoSetEnumTraitL(
    MM2GSVGProxy* aProxy,
    TInt aElementHandle,
    TInt16 aAttributeTypeId,
    TInt16 aValue)
{
    aProxy->SetEnumTraitL(
        STATIC_CAST(TM2GSvgElementHandle, aElementHandle),
        STATIC_CAST(TM2GSvgAttrType, aAttributeTypeId),
        aValue);
}

/**
 * JNI method
 */
JNIEXPORT void JNICALL
Java_com_nokia_microedition_m2g_M2GSVGElement__1setEnumTrait(
    JNIEnv* aJni,
    jclass,
    jint aEventSourceHandle,
    jint aSvgProxyHandle,
    jint aElementHandle,
    jshort aAttributeTypeId,
    jshort aValue,
    jboolean aUiToolKit)
{
    M2G_DEBUG_2("M2G_DEBUG: JNI ( M2GSVGElement ) _setEnumTrait: type=%d, value=%d - begin", aAttributeTypeId, aValue);
    TInt err = KM2GNotOk;
    if (aUiToolKit)  // If eSWT is the current toolkit
    {
        MSwtClient* client = reinterpret_cast< MSwtClient* >(aEventSourceHandle);
        if (client && aSvgProxyHandle)
        {
            typedef void (MM2GSVGProxy::*pSetEnumTraitL)(const TM2GSvgElementHandle&,const TM2GSvgAttrType&,TInt16);
            pSetEnumTraitL SetEnumTraitL = &MM2GSVGProxy::SetEnumTraitL;
            MM2GSVGProxy* aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);
            TM2GSvgElementHandle lElementHandle = STATIC_CAST(TM2GSvgElementHandle, aElementHandle);
            TM2GSvgAttrType lAttrType = STATIC_CAST(TM2GSvgAttrType, aAttributeTypeId);
            TMethodWrapper3<MM2GSVGProxy, const TM2GSvgElementHandle&,const TM2GSvgAttrType&,TInt16  >SetEnumTrait(
                *aProxy,
                SetEnumTraitL,
                lElementHandle,
                lAttrType,
                aValue
            );
            err = client->Execute(SetEnumTrait);
        }
    }
    else
    {
        CM2GEventSource* eventSource = JavaUnhand< CM2GEventSource >(aEventSourceHandle);
        if (eventSource && aSvgProxyHandle)
        {
            err = eventSource->ExecuteTrap(
                      &DoSetEnumTraitL,
                      JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle),
                      aElementHandle,
                      aAttributeTypeId,
                      aValue);
        }
    }
    M2GGeneral::CheckErrorCode(aJni, err);
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGElement ) _setEnumTrait - end");
}

// -----------------------------------------------------------------------------
// Java_com_nokia_microedition_m2g_M2GSVGElement::_setFloatTrait
// -----------------------------------------------------------------------------
/**
 * Calls MM2GSVGProxy::SetFloatTraitL method.
 * @since Series S60 3.0
 * @param aProxy Proxy instance.
 * @param aElementHandle Element pointer.
 * @param aAttributeTypeId -
 * @param aValue -
 */
LOCAL_C void DoSetFloatTraitL(
    MM2GSVGProxy* aProxy,
    TInt aElementHandle,
    TInt16 aAttributeTypeId,
    TReal32* aValue)
{
    aProxy->SetFloatTraitL(
        STATIC_CAST(TM2GSvgElementHandle, aElementHandle),
        STATIC_CAST(TM2GSvgAttrType, aAttributeTypeId),
        *aValue);
}

/**
 * JNI method
 */
JNIEXPORT void JNICALL
Java_com_nokia_microedition_m2g_M2GSVGElement__1setFloatTrait(
    JNIEnv* aJni,
    jclass,
    jint aEventSourceHandle,
    jint aSvgProxyHandle,
    jint aElementHandle,
    jshort aAttributeTypeId,
    jfloat aValue ,
    jboolean aUiToolKit)
{
    M2G_DEBUG_1("M2G_DEBUG: JNI ( M2GSVGElement ) _setFloatTrait: %f - begin", aValue);
    TInt err = KM2GNotOk;
    if (aUiToolKit)  // If eSWT is the current toolkit
    {
        MSwtClient* client = reinterpret_cast< MSwtClient* >(aEventSourceHandle);
        if (client && aSvgProxyHandle)
        {
            typedef void (MM2GSVGProxy::*pSetFloatTraitL)(const TM2GSvgElementHandle&,const TM2GSvgAttrType&,const TReal32&);
            pSetFloatTraitL SetFloatTraitL = &MM2GSVGProxy::SetFloatTraitL;
            MM2GSVGProxy* aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);
            TM2GSvgElementHandle lElementHandle = STATIC_CAST(TM2GSvgElementHandle, aElementHandle);
            TM2GSvgAttrType lAttrType = STATIC_CAST(TM2GSvgAttrType, aAttributeTypeId);
            TMethodWrapper3<MM2GSVGProxy, const TM2GSvgElementHandle&,const TM2GSvgAttrType&,const TReal32&  >SetFloatTrait(
                *aProxy,
                SetFloatTraitL,
                lElementHandle,
                lAttrType,
                aValue
            );
            err = client->Execute(SetFloatTrait);
        }
    }
    else
    {
        CM2GEventSource* eventSource = JavaUnhand< CM2GEventSource >(aEventSourceHandle);
        if (eventSource && aSvgProxyHandle)
        {
            err = eventSource->ExecuteTrap(
                      &DoSetFloatTraitL,
                      JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle),
                      aElementHandle,
                      aAttributeTypeId, REINTERPRET_CAST(TReal32*, &aValue));
        }
    }
    M2GGeneral::CheckErrorCode(aJni, err);
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGElement ) _setFloatTrait: - end");
}

// -----------------------------------------------------------------------------
// Java_com_nokia_microedition_m2g_M2GSVGElement::_setMatrixTrait
// -----------------------------------------------------------------------------
/**
 * Calls MM2GSVGProxy::SetMatrixTraitL method.
 * @since Series S60 3.0
 * @param aProxy Proxy instance.
 * @param aElementHandle Element handle
 * @param aAttributeTypeId Attribute type id
 * @param aMatrixData -
 */
LOCAL_C void DoSetMatrixTraitL(
    MM2GSVGProxy* aProxy,
    TInt aElementHandle,
    TInt16 aAttributeTypeId,
    TM2GMatrixData* aMatrix)
{
    aProxy->SetMatrixTraitL(
        STATIC_CAST(TM2GSvgElementHandle, aElementHandle),
        STATIC_CAST(TM2GSvgAttrType, aAttributeTypeId),
        *aMatrix);
}

/**
 * JNI method
 */
JNIEXPORT void JNICALL
Java_com_nokia_microedition_m2g_M2GSVGElement__1setMatrixTrait(
    JNIEnv* aJni,
    jclass,
    jint aEventSourceHandle,
    jint aSvgProxyHandle,
    jint aElementHandle,
    jshort aAttributeTypeId,
    jfloatArray aMatrixComponents,
    jboolean aUiToolKit)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGElement ) _setMatrixTrait - begin");
    TInt err = KM2GNotOk;
    TM2GMatrixData matrix;
    // Init matrix object
    jfloat* components = aJni->GetFloatArrayElements(aMatrixComponents,0);
    TInt count = aJni->GetArrayLength(aMatrixComponents);
    for (TInt index = 0; index < count; index++)
    {
        matrix[ index ] = components[ index ];
    }
    // Release data source
    aJni->ReleaseFloatArrayElements(aMatrixComponents, components, JNI_ABORT);
    if (aUiToolKit)  // If eSWT is the current toolkit
    {
        MSwtClient* client = reinterpret_cast< MSwtClient* >(aEventSourceHandle);
        if (client && aSvgProxyHandle)
        {
            typedef void (MM2GSVGProxy::*pSetMatrixTraitL)(const TM2GSvgElementHandle&,const TM2GSvgAttrType&,const TM2GMatrixData&);
            pSetMatrixTraitL SetMatrixTraitL = &MM2GSVGProxy::SetMatrixTraitL;
            MM2GSVGProxy* aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);
            TM2GSvgElementHandle lElementHandle = STATIC_CAST(TM2GSvgElementHandle, aElementHandle);
            TM2GSvgAttrType lAttrType = STATIC_CAST(TM2GSvgAttrType, aAttributeTypeId);
            TMethodWrapper3<MM2GSVGProxy, const TM2GSvgElementHandle&,const TM2GSvgAttrType&,const TM2GMatrixData&  >SetMatrixTrait(
                *aProxy,
                SetMatrixTraitL,
                lElementHandle,
                lAttrType,
                matrix
            );
            err = client->Execute(SetMatrixTrait);
        }
    }
    else
    {
        CM2GEventSource* eventSource = JavaUnhand< CM2GEventSource >(aEventSourceHandle);
        if (eventSource && aSvgProxyHandle)
        {
            err = eventSource->ExecuteTrap(
                      &DoSetMatrixTraitL,
                      JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle),
                      aElementHandle,
                      aAttributeTypeId,
                      &matrix);

        }
    }
    M2GGeneral::CheckErrorCode(aJni, err);
    M2G_DEBUG_6("JNI ( M2GSVGElement ) _setMatrixTrait: %f, %f, %f, %f, %f, %f - end", matrix[ 0 ], matrix[ 1 ], matrix[ 2 ], matrix[ 3 ], matrix[ 4 ], matrix[ 5 ]);
}

// -----------------------------------------------------------------------------
// Java_com_nokia_microedition_m2g_M2GSVGElement::_setPathTrait
// -----------------------------------------------------------------------------
/**
 * Calls MM2GSVGProxy::SetPathTraitL method.
 * @since Series S60 3.0
 * @param aProxy Proxy instance.
 * @param aElementHandle Element handle
 * @param aAttributeTypeId Attribute type id
 * @param aMatrixData -
 */
LOCAL_C void DoSetPathTraitL(
    MM2GSVGProxy* aProxy,
    TInt aElementHandle,
    TInt16 aAttributeTypeId,
    TInt aPathHandle)
{
    aProxy->SetPathTraitL(
        STATIC_CAST(TM2GSvgElementHandle, aElementHandle),
        STATIC_CAST(TM2GSvgAttrType, aAttributeTypeId),
        STATIC_CAST(TM2GSvgPathHandle, aPathHandle));
}
/**
 * JNI method
 */
JNIEXPORT void JNICALL
Java_com_nokia_microedition_m2g_M2GSVGElement__1setPathTrait(
    JNIEnv* aJni,
    jclass,
    jint aEventSourceHandle,
    jint aSvgProxyHandle,
    jint aElementHandle,
    jshort aAttributeTypeId,
    jint aPathHandle,
    jboolean aUiToolKit)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGElement ) _setPathTrait - begin");
    TInt err = KM2GNotOk;
    if (aUiToolKit)  // If eSWT is the current toolkit
    {
        MSwtClient* client = reinterpret_cast< MSwtClient* >(aEventSourceHandle);
        if (client && aSvgProxyHandle)
        {
            typedef void (MM2GSVGProxy::*pSetPathTraitL)(const TM2GSvgElementHandle&,const TM2GSvgAttrType&,const TM2GSvgPathHandle&);
            pSetPathTraitL SetPathTraitL = &MM2GSVGProxy::SetPathTraitL;
            MM2GSVGProxy* aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);
            TM2GSvgElementHandle lElementHandle = STATIC_CAST(TM2GSvgElementHandle, aElementHandle);
            TM2GSvgAttrType lAttrType = STATIC_CAST(TM2GSvgAttrType, aAttributeTypeId);
            TMethodWrapper3<MM2GSVGProxy, const TM2GSvgElementHandle&,const TM2GSvgAttrType&,const TM2GSvgPathHandle& >SetPathTrait(
                *aProxy,
                SetPathTraitL,
                lElementHandle,
                lAttrType,
                aPathHandle
            );
            err = client->Execute(SetPathTrait);
        }
    }
    else
    {
        CM2GEventSource* eventSource = JavaUnhand< CM2GEventSource >(aEventSourceHandle);
        if (eventSource && aSvgProxyHandle)
        {
            err = eventSource->ExecuteTrap(
                      &DoSetPathTraitL,
                      JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle),
                      aElementHandle,
                      aAttributeTypeId,
                      aPathHandle);
        }
    }
    M2GGeneral::CheckErrorCode(aJni, err);
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGElement ) _setPathTrait - end");
}

// -----------------------------------------------------------------------------
// Java_com_nokia_microedition_m2g_M2GSVGElement::_setRectTrait
// -----------------------------------------------------------------------------
/**
 * Calls MM2GSVGProxy::SetRectTraitL method.
 * @since Series S60 3.0
 * @param aProxy Proxy instance.
 * @param aElementHandle Element handle
 * @param aAttributeTypeId Attribute type id
 * @param aMatrixData -
 */
LOCAL_C void DoSetRectTraitL(
    MM2GSVGProxy* aProxy,
    TInt aElementHandle,
    TInt16 aAttributeTypeId,
    TM2GRectData* aRect)
{
    aProxy->SetRectTraitL(
        STATIC_CAST(TM2GSvgElementHandle, aElementHandle),
        STATIC_CAST(TM2GSvgAttrType, aAttributeTypeId),
        *aRect);
}
/**
 * JNI method
 */
JNIEXPORT void JNICALL
Java_com_nokia_microedition_m2g_M2GSVGElement__1setRectTrait(
    JNIEnv* aJni,
    jclass,
    jint aEventSourceHandle,
    jint aSvgProxyHandle,
    jint aElementHandle,
    jshort aAttributeTypeId,
    jfloat aX, jfloat aY, jfloat aWidth, jfloat aHeight,
    jboolean aUiToolKit)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGElement ) _setRectTrait - begin");
    TM2GRectData rect;
    rect[ 0 ] = aX;
    rect[ 1 ] = aY;
    rect[ 2 ] = aWidth;
    rect[ 3 ] = aHeight;
    TInt err = KM2GNotOk;
    if (aUiToolKit)  // If eSWT is the current toolkit
    {
        MSwtClient* client = reinterpret_cast< MSwtClient* >(aEventSourceHandle);
        if (client && aSvgProxyHandle)
        {
            typedef void (MM2GSVGProxy::*pSetRectTraitL)(const TM2GSvgElementHandle&,const TM2GSvgAttrType&,const TM2GRectData&);
            pSetRectTraitL SetRectTraitL = &MM2GSVGProxy::SetRectTraitL;
            MM2GSVGProxy* aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);
            TM2GSvgElementHandle lElementHandle = STATIC_CAST(TM2GSvgElementHandle, aElementHandle);
            TM2GSvgAttrType lAttrType = STATIC_CAST(TM2GSvgAttrType, aAttributeTypeId);
            TMethodWrapper3<MM2GSVGProxy, const TM2GSvgElementHandle&,const TM2GSvgAttrType&,const TM2GRectData& >SetRectTrait(
                *aProxy,
                SetRectTraitL,
                lElementHandle,
                lAttrType,
                rect
            );
            err = client->Execute(SetRectTrait);
        }
    }
    else
    {
        CM2GEventSource* eventSource = JavaUnhand< CM2GEventSource >(aEventSourceHandle);
        if (eventSource && aSvgProxyHandle)
        {
            err = eventSource->ExecuteTrap(
                      &DoSetRectTraitL,
                      JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle),
                      aElementHandle,
                      aAttributeTypeId,
                      &rect);
        }
    }
    M2GGeneral::CheckErrorCode(aJni, err);
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGElement ) _getRectTrait - end");
}

// -----------------------------------------------------------------------------
// Java_com_nokia_microedition_m2g_M2GSVGElement::_setStringTrait
// -----------------------------------------------------------------------------
/**
 * Calls MM2GSVGProxy::SetStringTraitL method.
 * @since Series S60 3.0
 * @param aElementHandle Element handle
 * @param aAttributeTypeId Attribute type id
 * @param aString -
 */
LOCAL_C void DoSetStringTraitL(
    MM2GSVGProxy* aProxy,
    TInt aElementHandle,
    TInt16 aAttributeTypeId,
    TPtrC16* aStr)
{
    aProxy->SetStringTraitL(
        STATIC_CAST(TM2GSvgElementHandle, aElementHandle),
        STATIC_CAST(TM2GSvgAttrType, aAttributeTypeId),
        *aStr);
}
/**
 * JNI method
 */
JNIEXPORT void JNICALL
Java_com_nokia_microedition_m2g_M2GSVGElement__1setStringTrait(
    JNIEnv* aJni,
    jclass,
    jint aEventSourceHandle,
    jint aSvgProxyHandle,
    jint aElementHandle,
    jshort aAttributeTypeId,
    jstring aValue,
    jboolean aUiToolKit)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGElement ) _setStringTrait - begin");
    RJString str(*aJni, aValue);
    TInt err = KM2GNotOk;
    if (aUiToolKit)  // If eSWT is the current toolkit
    {
        MSwtClient* client = reinterpret_cast< MSwtClient* >(aEventSourceHandle);
        if (client && aSvgProxyHandle)
        {
            typedef void (MM2GSVGProxy::*pSetStringTraitL)(const TM2GSvgElementHandle&,const TM2GSvgAttrType&,const TPtrC16&);
            pSetStringTraitL SetStringTraitL = &MM2GSVGProxy::SetStringTraitL;
            MM2GSVGProxy* aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);
            TM2GSvgElementHandle lElementHandle = STATIC_CAST(TM2GSvgElementHandle, aElementHandle);
            TM2GSvgAttrType lAttrType = STATIC_CAST(TM2GSvgAttrType, aAttributeTypeId);
            TPtrC16 lValue=STATIC_CAST(TPtrC16, str);
            TMethodWrapper3<MM2GSVGProxy, const TM2GSvgElementHandle&,const TM2GSvgAttrType&,const TPtrC16&  >SetStringTrait(
                *aProxy,
                SetStringTraitL,
                lElementHandle,
                lAttrType,
                lValue
            );
            err = client->Execute(SetStringTrait);
        }
    }
    else
    {
        CM2GEventSource* eventSource = JavaUnhand< CM2GEventSource >(aEventSourceHandle);
        if (eventSource && aSvgProxyHandle)
        {
            err = eventSource->ExecuteTrap(
                      &DoSetStringTraitL,
                      JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle),
                      aElementHandle,
                      aAttributeTypeId,
                      STATIC_CAST(TPtrC16*, &str));

        }
    }
    M2GGeneral::CheckErrorCode(aJni, err);
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGElement ) _setStringTrait - end");
}


M2G_NS_END
