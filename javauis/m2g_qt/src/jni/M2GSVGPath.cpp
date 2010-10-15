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
#include "com_nokia_microedition_m2g_M2GSVGPath.h"
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
// Java_com_nokia_microedition_m2g_M2GSVGPath::_addClose
// -----------------------------------------------------------------------------
/**
 * Calls MM2GSVGProxy::AddCloseL method.
 * @since Series S60 3.0
 * @param aProxy Proxy instance.
 * @param aPathHandle Svg path handle
 */
JNIEXPORT void JNICALL
Java_com_nokia_microedition_m2g_M2GSVGPath__1addClose(
    JNIEnv* aJni,
    jclass,
    jint aSvgProxyHandle,
    jint aPathHandle)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGPath ) _addClose - begin");
    TInt err = KM2GNotOk;

    M2G_DO_LOCK
    if (aSvgProxyHandle)
        {
        MM2GSVGProxy* aProxy = JavaUnhand< MM2GSVGProxy >(aSvgProxyHandle);
        TRAP(err, aProxy->AddCloseL(STATIC_CAST(TM2GSvgPathHandle, aPathHandle)); );
        }
    M2G_DO_UNLOCK(aJni)

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
JNIEXPORT void JNICALL
Java_com_nokia_microedition_m2g_M2GSVGPath__1addCurveTo(
    JNIEnv* aJni,
    jclass,
    jint aSvgProxyHandle,
    jint aPathHandle,
    jfloat aX1, jfloat aY1, jfloat aX2, jfloat aY2, jfloat aX3, jfloat aY3 )
{
    M2G_DEBUG_6("M2G_DEBUG: JNI ( M2GSVGPath ) _addCurveTo: X1=%f, Y1=%f, X2=%f, Y2=%f, X3=%f, Y3=%f - begin", aX1, aY1, aX2, aY2, aX3, aY3);
    TInt err = KM2GNotOk;

    TM2GPathCurveData curve;
              curve[ 0 ] = STATIC_CAST(TReal32, aX1);
              curve[ 1 ] = STATIC_CAST(TReal32, aY1);
              curve[ 2 ] = STATIC_CAST(TReal32, aX2);
              curve[ 3 ] = STATIC_CAST(TReal32, aY2);
              curve[ 4 ] = STATIC_CAST(TReal32, aX3);
              curve[ 5 ] = STATIC_CAST(TReal32, aY3);

      M2G_DO_LOCK        
      if (aSvgProxyHandle)
          {
          MM2GSVGProxy* aProxy = JavaUnhand< MM2GSVGProxy >(aSvgProxyHandle);
          TRAP(err,    aProxy->AddCurveToL(
                 STATIC_CAST(TM2GSvgPathHandle, aPathHandle),
                 curve); );
          }
    M2G_DO_UNLOCK(aJni)
    
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
JNIEXPORT void JNICALL
Java_com_nokia_microedition_m2g_M2GSVGPath__1addLineTo(
    JNIEnv* aJni,
    jclass,
    jint aSvgProxyHandle,
    jint aPathHandle,
    jfloat aX, jfloat aY )
{
    M2G_DEBUG_2("M2G_DEBUG: JNI ( M2GSVGPath ) _addLineTo: X=%f, Y=%f - begin", aX, aY);
    TInt err = KM2GNotOk;

    M2G_DO_LOCK
    if (aSvgProxyHandle)
        {
        MM2GSVGProxy* aProxy = JavaUnhand< MM2GSVGProxy >(aSvgProxyHandle);
        TRAP(err,     aProxy->AddLineToL(STATIC_CAST(TM2GSvgPathHandle, aPathHandle), *(REINTERPRET_CAST(TReal32*, &aX)), *(REINTERPRET_CAST(TReal32*, &aY))); );
    
        }
    M2G_DO_UNLOCK(aJni)
    
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
JNIEXPORT void JNICALL
Java_com_nokia_microedition_m2g_M2GSVGPath__1addMoveTo(
    JNIEnv* aJni,
    jclass,
    jint aSvgProxyHandle,
    jint aPathHandle,
    jfloat aX,
    jfloat aY)
{
    M2G_DEBUG_2("M2G_DEBUG: JNI ( M2GSVGPath ) _addMoveTo: X=%f, Y=%f - begin", aX, aY);
    TInt err           = KM2GNotOk;
    
    M2G_DO_LOCK
    if (aSvgProxyHandle)
        {
            MM2GSVGProxy *   aProxy = JavaUnhand< MM2GSVGProxy >(aSvgProxyHandle);
            TRAP(err, aProxy->AddMoveToL(STATIC_CAST(TM2GSvgPathHandle, aPathHandle), aX, aY)   ); 
        }
    M2G_DO_UNLOCK(aJni)

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
JNIEXPORT void JNICALL
Java_com_nokia_microedition_m2g_M2GSVGPath__1addQuadTo(
    JNIEnv* aJni,
    jclass,
    jint aSvgProxyHandle,
    jint aPathHandle,
    jfloat aX1, jfloat aY1, jfloat aX2, jfloat aY2)
{
    M2G_DEBUG_4("M2G_DEBUG: JNI ( M2GSVGPath ) _addQuadTo: X1=%f, Y1=%f, X2=%f, Y2=%f - begin", aX1, aY1, aX2, aY2);
    // TInt elementHandle = M2G_INVALID_HANDLE;
    TInt err           = KM2GNotOk;
    
    M2G_DO_LOCK
    if (aSvgProxyHandle)
        {
            MM2GSVGProxy *   aProxy = JavaUnhand< MM2GSVGProxy >(aSvgProxyHandle);
            TRAP(err, aProxy->AddQuadToL(
                    STATIC_CAST(TM2GSvgPathHandle, aPathHandle),aX1, aY1, aX2, aY2); );
        }
    M2G_DO_UNLOCK(aJni)
    
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
JNIEXPORT jint JNICALL
Java_com_nokia_microedition_m2g_M2GSVGPath__1createPath(
    JNIEnv* aJni,
    jclass,
    jint aSvgProxyHandle)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGPath ) _createPath - begin");

    TInt err           = KM2GNotOk;
    TInt pathHandle = M2G_INVALID_HANDLE;
    
    M2G_DO_LOCK
    if (aSvgProxyHandle)
        {
            MM2GSVGProxy *    aProxy = JavaUnhand< MM2GSVGProxy >(aSvgProxyHandle);
            TRAP(err, aProxy->CreatePathL(pathHandle);  );
        }
    M2G_DO_UNLOCK(aJni)

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
JNIEXPORT void JNICALL
Java_com_nokia_microedition_m2g_M2GSVGPath__1destroyPath(
    JNIEnv* aJni,
    jclass,
    jint aSvgProxyHandle,
    jint aPathHandle)
{
    M2G_DEBUG_1("M2G_DEBUG: JNI ( M2GSVGPath ) _destroyPath: %d - begin", aPathHandle);
    TInt err           = KM2GNotOk;
    
    M2G_DO_LOCK
    if (aSvgProxyHandle)
        {
        MM2GSVGProxy *    aProxy = JavaUnhand< MM2GSVGProxy >(aSvgProxyHandle);
        TRAP(err, aProxy->DestroyPathL(
                STATIC_CAST(TM2GSvgPathHandle, STATIC_CAST(TM2GSvgPathHandle, aPathHandle))); );
        }
    M2G_DO_UNLOCK(aJni)

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
JNIEXPORT jint JNICALL
Java_com_nokia_microedition_m2g_M2GSVGPath__1getNumberOfSegments(
    JNIEnv* aJni,
    jclass,
    jint aSvgProxyHandle,
    jint aPathHandle)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGPath ) _getNumberOfSegments - begin");
    TInt err              = KM2GNotOk;
    TInt numberOfSegments = 0;
    
    M2G_DO_LOCK
    if (aSvgProxyHandle)
        {
        MM2GSVGProxy * aProxy = JavaUnhand< MM2GSVGProxy >(aSvgProxyHandle);
        TRAP(err, aProxy->GetNumberOfSegmentsL(
                STATIC_CAST(TM2GSvgPathHandle, aPathHandle),
                numberOfSegments); );
        }
    M2G_DO_UNLOCK(aJni)
    
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
JNIEXPORT jfloat JNICALL
Java_com_nokia_microedition_m2g_M2GSVGPath__1getSegmentParameter(
    JNIEnv* aJni,
    jclass,
    jint aSvgProxyHandle,
    jint aPathHandle, jint aSegmentIndex, jint aParamIndex)
{
    M2G_DEBUG_2("M2G_DEBUG: JNI ( M2GSVGPath ) _getSegmentParameter: seg index=%d, param index=%d - begin", aSegmentIndex, aParamIndex);
    TInt err              = KM2GNotOk;
    TReal32 segmentParam  = 0;
    
    M2G_DO_LOCK
    if (aSvgProxyHandle)
        {
        MM2GSVGProxy *             aProxy = JavaUnhand< MM2GSVGProxy >(aSvgProxyHandle);
        TRAP(err,    aProxy->GetSegmentParameterL(
            STATIC_CAST(TM2GSvgPathHandle, aPathHandle), aSegmentIndex,aParamIndex, segmentParam); );
        }
    M2G_DO_UNLOCK(aJni)

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
JNIEXPORT jshort JNICALL
Java_com_nokia_microedition_m2g_M2GSVGPath__1getSegmentType(
    JNIEnv* aJni,
    jclass,
    jint aSvgProxyHandle,
    TInt aPathHandle,
    jint aIndex)
{
    M2G_DEBUG_1("M2G_DEBUG: JNI ( M2GSVGPath ) _getSegmentType: index=%d - begin", aIndex);
    TInt err              = KM2GNotOk;
    TInt16 aSegmentType  = 0;
    
    M2G_DO_LOCK
    if (aSvgProxyHandle)
        {
        MM2GSVGProxy *             aProxy = JavaUnhand< MM2GSVGProxy >(aSvgProxyHandle);
        TRAP(err, aProxy->GetSegmentTypeL(
                   STATIC_CAST(TM2GSvgPathHandle, aPathHandle), aIndex,aSegmentType); );
        }
    M2G_DO_UNLOCK(aJni)

    M2GGeneral::CheckErrorCode(aJni, err);
    M2G_DEBUG_1("M2G_DEBUG: JNI ( M2GSVGPath ) _getSegmentType: %d - end", aSegmentType);
    return STATIC_CAST(jshort, aSegmentType);
}

M2G_NS_END
