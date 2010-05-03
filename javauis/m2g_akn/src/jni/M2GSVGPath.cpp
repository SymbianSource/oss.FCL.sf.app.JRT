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
#include "com_nokia_microedition_m2g_M2GSVGPath.h"
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
// Java_com_nokia_microedition_m2g_M2GSVGPath::_addClose
// -----------------------------------------------------------------------------
/**
 * Calls MM2GSVGProxy::AddCloseL method.
 * @since Series S60 3.0
 * @param aProxy Proxy instance.
 * @param aPathHandle Svg path handle
 */
LOCAL_C void DoAddCloseL(
    MM2GSVGProxy* aProxy,
    TInt aPathHandle)
{
    aProxy->AddCloseL(STATIC_CAST(TM2GSvgPathHandle, aPathHandle));
}

/**
 * JNI method
 */
JNIEXPORT void JNICALL
Java_com_nokia_microedition_m2g_M2GSVGPath__1addClose(
    JNIEnv* aJni,
    jclass,
    jint aEventSourceHandle,
    jint aSvgProxyHandle,
    jint aPathHandle,
    jboolean aUiToolKit)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGPath ) _addClose - begin");
    TInt err = KM2GNotOk;
    if (aUiToolKit)  // If eSWT is the current toolkit
    {
        MSwtClient* client = reinterpret_cast< MSwtClient* >(aEventSourceHandle);
        if (client && aSvgProxyHandle)
        {
            typedef void (MM2GSVGProxy::*pAddCloseL)(const TM2GSvgPathHandle&);
            pAddCloseL AddCloseL = &MM2GSVGProxy::AddCloseL;
            MM2GSVGProxy* aProxy = JavaUnhand< MM2GSVGProxy >(aSvgProxyHandle);
            TM2GSvgPathHandle lPathHandle = STATIC_CAST(TM2GSvgPathHandle, aPathHandle);
            TMethodWrapper1<MM2GSVGProxy, const TM2GSvgPathHandle& > AddClose(
                *aProxy,
                AddCloseL,
                lPathHandle);
            err = client->Execute(AddClose);
        }
    }
    else
    {
        CM2GEventSource* eventSource = JavaUnhand< CM2GEventSource >(aEventSourceHandle);
        if (eventSource && aSvgProxyHandle)
        {
            err = eventSource->ExecuteTrap(
                      &DoAddCloseL,
                      JavaUnhand< MM2GSVGProxy >(aSvgProxyHandle),
                      aPathHandle);
        }
    }
    M2GGeneral::CheckErrorCode(aJni, err);

    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGPath ) _addClose - end");
}

// -----------------------------------------------------------------------------
// Java_com_nokia_microedition_m2g_M2GSVGPath::_addCurve
// -----------------------------------------------------------------------------
/**
 * Calls MM2GSVGProxy::AddCurveTo method.
 * @since Series S60 3.0
 * @param aProxy Proxy instance.
 * @param aPathHandle Svg path handle
 * @param aPathCurveData Curve parameters
 */
LOCAL_C void DoAddCurveToL(
    MM2GSVGProxy* aProxy,
    TInt aPathHandle,
    TM2GPathCurveData* aPathCurveData)
{
    aProxy->AddCurveToL(
        STATIC_CAST(TM2GSvgPathHandle, aPathHandle),
        *aPathCurveData);
}

/**
 * JNI method
 */
JNIEXPORT void JNICALL
Java_com_nokia_microedition_m2g_M2GSVGPath__1addCurveTo(
    JNIEnv* aJni,
    jclass,
    jint aEventSourceHandle,
    jint aSvgProxyHandle,
    jint aPathHandle,
    jfloat aX1, jfloat aY1, jfloat aX2, jfloat aY2, jfloat aX3, jfloat aY3 ,
    jboolean aUiToolKit)
{
    M2G_DEBUG_6("M2G_DEBUG: JNI ( M2GSVGPath ) _addCurveTo: X1=%f, Y1=%f, X2=%f, Y2=%f, X3=%f, Y3=%f - begin", aX1, aY1, aX2, aY2, aX3, aY3);
    TInt err = KM2GNotOk;
    if (aUiToolKit)  // If eSWT is the current toolkit
    {
        MSwtClient* client = reinterpret_cast< MSwtClient* >(aEventSourceHandle);
        if (client && aSvgProxyHandle)
        {
            typedef void (MM2GSVGProxy::*pAddCurveToL)(const TM2GSvgPathHandle& ,const TM2GPathCurveData&);
            pAddCurveToL AddCurveToL = &MM2GSVGProxy::AddCurveToL;
            MM2GSVGProxy* aProxy = JavaUnhand< MM2GSVGProxy >(aSvgProxyHandle);
            TM2GSvgPathHandle lPathHandle = STATIC_CAST(TM2GSvgPathHandle, aPathHandle);
            TM2GPathCurveData curve;
            curve[ 0 ] = STATIC_CAST(TReal32, aX1);
            curve[ 1 ] = STATIC_CAST(TReal32, aY1);
            curve[ 2 ] = STATIC_CAST(TReal32, aX2);
            curve[ 3 ] = STATIC_CAST(TReal32, aY2);
            curve[ 4 ] = STATIC_CAST(TReal32, aX3);
            curve[ 5 ] = STATIC_CAST(TReal32, aY3);
            TMethodWrapper2<MM2GSVGProxy, const TM2GSvgPathHandle& ,const TM2GPathCurveData& > AddCurveTo(
                *aProxy,
                AddCurveToL,
                lPathHandle,
                curve);
            err = client->Execute(AddCurveTo);
        }
    }
    else
    {
        CM2GEventSource* eventSource = JavaUnhand< CM2GEventSource >(aEventSourceHandle);
        if (eventSource && aSvgProxyHandle)
        {
            TM2GPathCurveData curve;
            curve[ 0 ] = STATIC_CAST(TReal32, aX1);
            curve[ 1 ] = STATIC_CAST(TReal32, aY1);
            curve[ 2 ] = STATIC_CAST(TReal32, aX2);
            curve[ 3 ] = STATIC_CAST(TReal32, aY2);
            curve[ 4 ] = STATIC_CAST(TReal32, aX3);
            curve[ 5 ] = STATIC_CAST(TReal32, aY3);

            err = eventSource->ExecuteTrap(
                      &DoAddCurveToL,
                      JavaUnhand< MM2GSVGProxy >(aSvgProxyHandle),
                      aPathHandle,
                      &curve);

        }
    }
    M2GGeneral::CheckErrorCode(aJni, err);
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGPath ) _addCurveTo - end");
}

// -----------------------------------------------------------------------------
// Java_com_nokia_microedition_m2g_M2GSVGPath::_addLineTo
// -----------------------------------------------------------------------------
/**
 * Calls MM2GSVGProxy::AddLineToL method.
 * @since Series S60 3.0
 * @param aProxy Proxy instance.
 * @param aPathHandle Svg path handle
 * @param aX -
 * @param aY -
 */
LOCAL_C void DoAddLineToL(
    MM2GSVGProxy* aProxy,
    TInt aPathHandle,
    TReal32* aX, TReal32* aY)
{
    aProxy->AddLineToL(STATIC_CAST(TM2GSvgPathHandle, aPathHandle), *aX, *aY);
}

/**
 * JNI method
 */
JNIEXPORT void JNICALL
Java_com_nokia_microedition_m2g_M2GSVGPath__1addLineTo(
    JNIEnv* aJni,
    jclass,
    jint aEventSourceHandle,
    jint aSvgProxyHandle,
    jint aPathHandle,
    jfloat aX, jfloat aY ,jboolean aUiToolKit)
{
    M2G_DEBUG_2("M2G_DEBUG: JNI ( M2GSVGPath ) _addLineTo: X=%f, Y=%f - begin", aX, aY);
    TInt err = KM2GNotOk;
    if (aUiToolKit)  // If eSWT is the current toolkit
    {
        MSwtClient* client = reinterpret_cast< MSwtClient* >(aEventSourceHandle);
        if (client && aSvgProxyHandle)
        {
            typedef void (MM2GSVGProxy::*pAddLineToL)(const TM2GSvgPathHandle& ,const TReal32& , const TReal32&);
            pAddLineToL AddLineToL = &MM2GSVGProxy::AddLineToL;
            MM2GSVGProxy* aProxy = JavaUnhand< MM2GSVGProxy >(aSvgProxyHandle);
            TM2GSvgPathHandle lPathHandle = STATIC_CAST(TM2GSvgPathHandle, aPathHandle);
            TMethodWrapper3<MM2GSVGProxy, const TM2GSvgPathHandle& ,const TReal32& , const TReal32& > AddLineTo(
                *aProxy,
                AddLineToL,
                lPathHandle,
                *(REINTERPRET_CAST(TReal32*, &aX)),
                *(REINTERPRET_CAST(TReal32*, &aY)));
            err = client->Execute(AddLineTo);
        }
    }
    else
    {
        CM2GEventSource* eventSource = JavaUnhand< CM2GEventSource >(aEventSourceHandle);
        if (eventSource && aSvgProxyHandle)
        {
            err = eventSource->ExecuteTrap(
                      &DoAddLineToL,
                      JavaUnhand< MM2GSVGProxy >(aSvgProxyHandle),
                      aPathHandle,
                      REINTERPRET_CAST(TReal32*, &aX),
                      REINTERPRET_CAST(TReal32*, &aY));
        }
    }
    M2GGeneral::CheckErrorCode(aJni, err);

    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGPath ) _addLineTo - end");
}

// -----------------------------------------------------------------------------
// Java_com_nokia_microedition_m2g_M2GSVGPath::_addMoveTo
// -----------------------------------------------------------------------------
/**
 * Calls MM2GSVGProxy::AddMoveToL method.
 * @since Series S60 3.0
 * @param aProxy Proxy instance.
 * @param aPathHandle Svg path handle
 * @param aX -
 * @param aY -
 */
LOCAL_C void DoAddMoveToL(
    MM2GSVGProxy* aProxy,
    TInt aPathHandle,
    TReal32* aX, TReal32* aY)
{
    aProxy->AddMoveToL(STATIC_CAST(TM2GSvgPathHandle, aPathHandle), *aX, *aY);
}

/**
 * JNI method
 */
JNIEXPORT void JNICALL
Java_com_nokia_microedition_m2g_M2GSVGPath__1addMoveTo(
    JNIEnv* aJni,
    jclass,
    jint aEventSourceHandle,
    jint aSvgProxyHandle,
    jint aPathHandle,
    jfloat aX,
    jfloat aY,
    jboolean aUiToolkit)
{
    M2G_DEBUG_2("M2G_DEBUG: JNI ( M2GSVGPath ) _addMoveTo: X=%f, Y=%f - begin", aX, aY);
    TInt err           = KM2GNotOk;
    if (aUiToolkit)
    {
        MSwtClient* client = reinterpret_cast< MSwtClient* >(aEventSourceHandle);
        if (client && aSvgProxyHandle)
        {
            typedef void (MM2GSVGProxy::*pAddMoveToL)(
                const TM2GSvgPathHandle&,
                const TReal32&,
                const TReal32&);
            pAddMoveToL AddMoveToL = &MM2GSVGProxy::AddMoveToL;
            MM2GSVGProxy *             aProxy = JavaUnhand< MM2GSVGProxy >(aSvgProxyHandle);
            TM2GSvgDocumentHandle lPathHandle = STATIC_CAST(TM2GSvgPathHandle, aPathHandle);
            TMethodWrapper3<MM2GSVGProxy,
            const TM2GSvgPathHandle&,
            const TReal32&,
            const TReal32&
            > AddMoveTo(
                *aProxy,
                AddMoveToL,
                lPathHandle,
                aX,
                aY
            );
            err = client->Execute(AddMoveTo);
        }
    }
    else
    {
        CM2GEventSource* eventSource = JavaUnhand< CM2GEventSource >(aEventSourceHandle);
        if (eventSource && aSvgProxyHandle)
        {
            err = eventSource->ExecuteTrap(
                      &DoAddMoveToL,
                      JavaUnhand< MM2GSVGProxy >(aSvgProxyHandle),
                      aPathHandle,
                      REINTERPRET_CAST(TReal32*, &aX),
                      REINTERPRET_CAST(TReal32*, &aY));

        }
    }
    M2GGeneral::CheckErrorCode(aJni, err);
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGPath ) _addMoveTo - end");
}

// -----------------------------------------------------------------------------
// Java_com_nokia_microedition_m2g_M2GSVGPath::_addQuadTo
// -----------------------------------------------------------------------------
/**
 * Calls MM2GSVGProxy::AddQuadToL method.
 * @since Series S60 3.0
 * @param aProxy Proxy instance.
 * @param aPathHandle Svg path handle
 * @param aX1 -
 * @param aY1 -
 * @param aX2 -
 * @param aY2 -
 */
LOCAL_C void DoAddQuadToL(
    MM2GSVGProxy* aProxy,
    TInt aPathHandle,
    TReal32* aX1, TReal32* aY1, TReal32* aX2, TReal32* aY2)
{
    aProxy->AddQuadToL(
        STATIC_CAST(TM2GSvgPathHandle, aPathHandle),
        *aX1, *aY1, *aX2, *aY2);
}

/**
 * JNI method
 */
JNIEXPORT void JNICALL
Java_com_nokia_microedition_m2g_M2GSVGPath__1addQuadTo(
    JNIEnv* aJni,
    jclass,
    jint aEventSourceHandle,
    jint aSvgProxyHandle,
    jint aPathHandle,
    jfloat aX1, jfloat aY1, jfloat aX2, jfloat aY2,jboolean aUiToolkit)
{
    M2G_DEBUG_4("M2G_DEBUG: JNI ( M2GSVGPath ) _addQuadTo: X1=%f, Y1=%f, X2=%f, Y2=%f - begin", aX1, aY1, aX2, aY2);
    // TInt elementHandle = M2G_INVALID_HANDLE;
    TInt err           = KM2GNotOk;
    if (aUiToolkit)
    {
        MSwtClient* client = reinterpret_cast< MSwtClient* >(aEventSourceHandle);
        if (client && aSvgProxyHandle)
        {
            typedef void (MM2GSVGProxy::*pAddQuadToL)(
                const TM2GSvgPathHandle&,
                const TReal32&,
                const TReal32&,
                const TReal32&,
                const TReal32&);
            pAddQuadToL AddQuadToL = &MM2GSVGProxy::AddQuadToL;
            MM2GSVGProxy *             aProxy = JavaUnhand< MM2GSVGProxy >(aSvgProxyHandle);
            TM2GSvgDocumentHandle lPathHandle = STATIC_CAST(TM2GSvgPathHandle, aPathHandle);
            TMethodWrapper5<MM2GSVGProxy,
            const TM2GSvgPathHandle&,
            const TReal32&,
            const TReal32&,
            const TReal32&,
            const TReal32&
            > AddQuadTo(
                *aProxy,
                AddQuadToL,
                lPathHandle,
                aX1,
                aY1,
                aX2,
                aY2
            );
            err = client->Execute(AddQuadTo);
        }
    }
    else
    {
        CM2GEventSource* eventSource = JavaUnhand< CM2GEventSource >(aEventSourceHandle);
        if (eventSource && aSvgProxyHandle)
        {
            err = eventSource->ExecuteTrap(
                      &DoAddQuadToL,
                      JavaUnhand< MM2GSVGProxy >(aSvgProxyHandle),
                      aPathHandle,
                      REINTERPRET_CAST(TReal32*, &aX1),
                      REINTERPRET_CAST(TReal32*, &aY1),
                      REINTERPRET_CAST(TReal32*, &aX2),
                      REINTERPRET_CAST(TReal32*, &aY2));

        }
    }
    M2GGeneral::CheckErrorCode(aJni, err);
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGPath ) _addQuadTo - end");
}

// -----------------------------------------------------------------------------
// Java_com_nokia_microedition_m2g_M2GSVGPath::_createPath
// -----------------------------------------------------------------------------
/**
 * Calls MM2GSVGProxy::CreatePathL method.
 * @since Series S60 3.0
 * @param aProxy Proxy instance.
 * @param aPathHandle Path handle
 */
LOCAL_C void DoCreatePathL(
    MM2GSVGProxy* aProxy, TInt* aPathHandle)
{
    aProxy->CreatePathL(*aPathHandle);
}

/**
 * JNI method
 */
JNIEXPORT jint JNICALL
Java_com_nokia_microedition_m2g_M2GSVGPath__1createPath(
    JNIEnv* aJni,
    jclass,
    jint aEventSourceHandle,
    jint aSvgProxyHandle,
    jboolean aUiToolkit)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGPath ) _createPath - begin");

    TInt err           = KM2GNotOk;
    TInt pathHandle = M2G_INVALID_HANDLE;
    if (aUiToolkit)
    {
        MSwtClient* client = reinterpret_cast< MSwtClient* >(aEventSourceHandle);
        if (client && aSvgProxyHandle)
        {
            typedef void (MM2GSVGProxy::*pCreatePathL)(TM2GSvgPathHandle&);
            pCreatePathL CreatePathL = &MM2GSVGProxy::CreatePathL;
            MM2GSVGProxy *    aProxy = JavaUnhand< MM2GSVGProxy >(aSvgProxyHandle);
            TMethodWrapper1<MM2GSVGProxy,TM2GSvgPathHandle&> CreatePath(
                *aProxy,
                CreatePathL,
                pathHandle
            );
            err = client->Execute(CreatePath);
        }
    }
    else
    {
        CM2GEventSource* eventSource = JavaUnhand< CM2GEventSource >(aEventSourceHandle);
        if (eventSource && aSvgProxyHandle)
        {
            err = eventSource->ExecuteTrap(
                      &DoCreatePathL,
                      JavaUnhand< MM2GSVGProxy >(aSvgProxyHandle),
                      &pathHandle);

        }
    }
    M2GGeneral::CheckErrorCode(aJni, err);

    M2G_DEBUG_1("M2G_DEBUG: JNI ( M2GSVGPath ) _createPath: %d - end", pathHandle);
    return STATIC_CAST(jint, pathHandle);
}

// -----------------------------------------------------------------------------
// Java_com_nokia_microedition_m2g_M2GSVGPath::_destroyPath
// -----------------------------------------------------------------------------
/**
 * Calls MM2GSVGProxy::DestroyPathL method.
 * @since Series S60 3.0
 * @param aProxy Proxy instance.
 * @param aPathHandle Svg path handle
 */
LOCAL_C void DoDestroyPathL(
    MM2GSVGProxy* aProxy,
    TInt aPathHandle)
{
    aProxy->DestroyPathL(
        STATIC_CAST(TM2GSvgPathHandle, aPathHandle));
}

/**
 * JNI method
 */
JNIEXPORT void JNICALL
Java_com_nokia_microedition_m2g_M2GSVGPath__1destroyPath(
    JNIEnv* aJni,
    jclass,
    jint aEventSourceHandle,
    jint aSvgProxyHandle,
    jint aPathHandle,
    jboolean aUiToolkit)
{
    M2G_DEBUG_1("M2G_DEBUG: JNI ( M2GSVGPath ) _destroyPath: %d - begin", aPathHandle);
    TInt err           = KM2GNotOk;
    if (aUiToolkit)
    {
        MSwtClient* client = reinterpret_cast< MSwtClient* >(aEventSourceHandle);
        if (client && aSvgProxyHandle)
        {
            typedef void (MM2GSVGProxy::*pDestroyPathL)(const TM2GSvgPathHandle&);
            pDestroyPathL DestroyPathL = &MM2GSVGProxy::DestroyPathL;
            MM2GSVGProxy *             aProxy = JavaUnhand< MM2GSVGProxy >(aSvgProxyHandle);
            TM2GSvgDocumentHandle lPathHandle = STATIC_CAST(TM2GSvgPathHandle, aPathHandle);
            TMethodWrapper1<MM2GSVGProxy,const TM2GSvgPathHandle& > DestroyPath(
                *aProxy,
                DestroyPathL,
                lPathHandle
            );
            err = client->Execute(DestroyPath);
        }
    }
    else
    {
        CM2GEventSource* eventSource = JavaUnhand< CM2GEventSource >(aEventSourceHandle);
        if (eventSource && aSvgProxyHandle)
        {
            err = eventSource->ExecuteTrap(
                      &DoDestroyPathL,
                      JavaUnhand< MM2GSVGProxy >(aSvgProxyHandle),
                      aPathHandle);
        }
    }
    M2GGeneral::CheckErrorCode(aJni, err);
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGPath ) _destroyPath - end");
}

// -----------------------------------------------------------------------------
// Java_com_nokia_microedition_m2g_M2GSVGPath::_getNumberOfSegments
// -----------------------------------------------------------------------------
/**
 * Calls MM2GSVGProxy::GetNumberOfSegments method.
 * @since Series S60 3.0
 * @param aProxy Proxy instance.
 * @param aPathHandle Svg path handle
 * @return Number of segments
 */
LOCAL_C void DoGetNumberOfSegmentsL(
    MM2GSVGProxy* aProxy,
    TInt aPathHandle,
    TInt* aNumberOfSegments)
{
    aProxy->GetNumberOfSegmentsL(
        STATIC_CAST(TM2GSvgPathHandle, aPathHandle),
        *aNumberOfSegments);
}

/**
 * JNI method
 */
JNIEXPORT jint JNICALL
Java_com_nokia_microedition_m2g_M2GSVGPath__1getNumberOfSegments(
    JNIEnv* aJni,
    jclass,
    jint aEventSourceHandle,
    jint aSvgProxyHandle,
    jint aPathHandle,
    jboolean aUiToolkit)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGPath ) _getNumberOfSegments - begin");
    TInt err              = KM2GNotOk;
    TInt numberOfSegments = 0;
    if (aUiToolkit)
    {
        MSwtClient* client = reinterpret_cast< MSwtClient* >(aEventSourceHandle);
        if (client && aSvgProxyHandle)
        {
            typedef void (MM2GSVGProxy::*pGetNumberOfSegmentsL)(const TM2GSvgPathHandle&, TInt&);
            pGetNumberOfSegmentsL GetNumberOfSegmentsL = &MM2GSVGProxy::GetNumberOfSegmentsL;
            MM2GSVGProxy *             aProxy = JavaUnhand< MM2GSVGProxy >(aSvgProxyHandle);
            TM2GSvgDocumentHandle lPathHandle = STATIC_CAST(TM2GSvgPathHandle, aPathHandle);
            TMethodWrapper2<MM2GSVGProxy,const TM2GSvgPathHandle&, TInt& > GetNumberOfSegments(
                *aProxy,
                GetNumberOfSegmentsL,
                lPathHandle,
                numberOfSegments
            );
            err = client->Execute(GetNumberOfSegments);
        }
    }
    else
    {
        CM2GEventSource* eventSource = JavaUnhand< CM2GEventSource >(aEventSourceHandle);

        if (eventSource && aSvgProxyHandle)
        {
            err = eventSource->ExecuteTrap(
                      &DoGetNumberOfSegmentsL,
                      JavaUnhand< MM2GSVGProxy >(aSvgProxyHandle),
                      aPathHandle, &numberOfSegments);

        }
    }
    M2GGeneral::CheckErrorCode(aJni, err);
    M2G_DEBUG_1("M2G_DEBUG: JNI ( M2GSVGPath ) _getNumberOfSegments: %d - end", numberOfSegments);
    return STATIC_CAST(jint, numberOfSegments);
}

// -----------------------------------------------------------------------------
// Java_com_nokia_microedition_m2g_M2GSVGPath::_getSegmentParameter
// -----------------------------------------------------------------------------
/**
 * Calls MM2GSVGProxy::GetSegmentParameterL method.
 * @since Series S60 3.0
 * @param aProxy Proxy instance.
 * @param aPathHandle Svg path handle
 * @param aSegmentIndex -
 * @param aParamIndex -
 * @param aSegmentParam Segment parameter
 */
LOCAL_C void DoGetSegmentParameterL(
    MM2GSVGProxy* aProxy,
    TInt aPathHandle, TInt aSegmentIndex, TInt aParamIndex,
    TReal32* aSegmentParam)
{
    aProxy->GetSegmentParameterL(
        STATIC_CAST(TM2GSvgPathHandle, aPathHandle), aSegmentIndex,
        aParamIndex, *aSegmentParam);
}

/**
 * JNI method
 */
JNIEXPORT jfloat JNICALL
Java_com_nokia_microedition_m2g_M2GSVGPath__1getSegmentParameter(
    JNIEnv* aJni,
    jclass,
    jint aEventSourceHandle,
    jint aSvgProxyHandle,
    jint aPathHandle, jint aSegmentIndex, jint aParamIndex, jboolean aUiToolkit)
{
    M2G_DEBUG_2("M2G_DEBUG: JNI ( M2GSVGPath ) _getSegmentParameter: seg index=%d, param index=%d - begin", aSegmentIndex, aParamIndex);
    TInt err              = KM2GNotOk;
    TReal32 segmentParam  = 0;
    if (aUiToolkit)
    {
        MSwtClient* client = reinterpret_cast< MSwtClient* >(aEventSourceHandle);
        if (client && aSvgProxyHandle)
        {
            typedef void (MM2GSVGProxy::*pGetSegmentParameterL)(
                const TM2GSvgPathHandle&,
                TInt,
                TInt,
                TReal32&);
            pGetSegmentParameterL GetSegmentParameterL = &MM2GSVGProxy::GetSegmentParameterL;
            MM2GSVGProxy *             aProxy = JavaUnhand< MM2GSVGProxy >(aSvgProxyHandle);
            TM2GSvgDocumentHandle lPathHandle = STATIC_CAST(TM2GSvgPathHandle, aPathHandle);
            TMethodWrapper4<MM2GSVGProxy,const TM2GSvgPathHandle&, TInt, TInt, TReal32& > GetSegmentParameter
            (
                *aProxy,
                GetSegmentParameterL,
                lPathHandle,
                aSegmentIndex,
                aParamIndex,
                segmentParam
            );
            err = client->Execute(GetSegmentParameter);
        }
    }
    else
    {
        CM2GEventSource* eventSource = JavaUnhand< CM2GEventSource >(aEventSourceHandle);

        if (eventSource && aSvgProxyHandle)
        {
            err = eventSource->ExecuteTrap(
                      &DoGetSegmentParameterL,
                      JavaUnhand< MM2GSVGProxy >(aSvgProxyHandle),
                      aPathHandle, aSegmentIndex, aParamIndex, &segmentParam);
        }
    }
    M2GGeneral::CheckErrorCode(aJni, err);

    M2G_DEBUG_1("M2G_DEBUG: JNI ( M2GSVGPath ) _getSegmentParameter: %f - end", segmentParam);
    return STATIC_CAST(jfloat, segmentParam);
}


// -----------------------------------------------------------------------------
// Java_com_nokia_microedition_m2g_M2GSVGPath::_getSegmentType
// -----------------------------------------------------------------------------
/**
 * Calls MM2GSVGProxy::GetSegmentTypeL method.
 * @since Series S60 3.0
 * @param aProxy Proxy instance.
 * @param aPathHandle Svg path handle
 * @param aSegmentIndex -
 * @param aSegmentType Segment type to be returned
 */
LOCAL_C void DoGetSegmentTypeL(
    MM2GSVGProxy* aProxy,
    TInt aPathHandle, TInt aSegmentIndex, TInt16* aSegmentType)
{
    return aProxy->GetSegmentTypeL(
               STATIC_CAST(TM2GSvgPathHandle, aPathHandle), aSegmentIndex,
               *aSegmentType);
}

/**
 * JNI method
 */
JNIEXPORT jshort JNICALL
Java_com_nokia_microedition_m2g_M2GSVGPath__1getSegmentType(
    JNIEnv* aJni,
    jclass,
    jint aEventSourceHandle,
    jint aSvgProxyHandle,
    TInt aPathHandle,
    jint aIndex,
    jboolean aUiToolkit)
{
    M2G_DEBUG_1("M2G_DEBUG: JNI ( M2GSVGPath ) _getSegmentType: index=%d - begin", aIndex);
    TInt err              = KM2GNotOk;
    TInt16 aSegmentType  = 0;
    if (aUiToolkit)
    {
        MSwtClient* client = reinterpret_cast< MSwtClient* >(aEventSourceHandle);
        if (client && aSvgProxyHandle)
        {
            typedef void (MM2GSVGProxy::*pGetSegmentTypeL)(
                const TM2GSvgPathHandle&,
                TInt,
                TInt16&);
            pGetSegmentTypeL GetSegmentTypeL = &MM2GSVGProxy::GetSegmentTypeL;
            MM2GSVGProxy *             aProxy = JavaUnhand< MM2GSVGProxy >(aSvgProxyHandle);
            TM2GSvgDocumentHandle lPathHandle = STATIC_CAST(TM2GSvgPathHandle, aPathHandle);
            TMethodWrapper3<MM2GSVGProxy,const TM2GSvgPathHandle&, TInt, TInt16& > GetSegmentType
            (
                *aProxy,
                GetSegmentTypeL,
                lPathHandle,
                aIndex,
                aSegmentType
            );
            err = client->Execute(GetSegmentType);
        }
    }
    else
    {
        CM2GEventSource* eventSource = JavaUnhand< CM2GEventSource >(aEventSourceHandle);

        if (eventSource && aSvgProxyHandle)
        {
            err = eventSource->ExecuteTrap(
                      &DoGetSegmentTypeL,
                      JavaUnhand< MM2GSVGProxy >(aSvgProxyHandle),
                      aPathHandle, aIndex, &aSegmentType);

        }
    }
    M2GGeneral::CheckErrorCode(aJni, err);
    M2G_DEBUG_1("M2G_DEBUG: JNI ( M2GSVGPath ) _getSegmentType: %d - end", aSegmentType);
    return STATIC_CAST(jshort, aSegmentType);
}

M2G_NS_END
