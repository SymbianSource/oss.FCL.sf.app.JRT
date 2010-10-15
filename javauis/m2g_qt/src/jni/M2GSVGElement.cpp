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
#include "com_nokia_microedition_m2g_M2GSVGElement.h"
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
// Java_com_nokia_microedition_m2g_M2GSVGElement::_appendChild
// -----------------------------------------------------------------------------
/**
 * Calls MM2GSVGProxy::AppendChildL method.
 * @since Series S60 3.0
 * @param aProxy Proxy instance.
 * @param aElementHandle Element handle
 * @param aChildElementHandle Child handle
 */
JNIEXPORT void JNICALL
Java_com_nokia_microedition_m2g_M2GSVGElement__1appendChild(
    JNIEnv* aJni,
    jclass,
    jint aSvgProxyHandle,
    jint aElementHandle,
    jint aChildElementHandle)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGElement ) _appendChild - begin");
    TInt err = KM2GNotOk;

    M2G_DO_LOCK
    if (aSvgProxyHandle)
        {
        MM2GSVGProxy* aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);
        TRAP(err,   aProxy->AppendChildL(
            STATIC_CAST(TM2GSvgElementHandle, aElementHandle),
            STATIC_CAST(TM2GSvgElementHandle, aChildElementHandle)); );
        }
    M2G_DO_UNLOCK(aJni)
    
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
JNIEXPORT jint JNICALL Java_com_nokia_microedition_m2g_M2GSVGElement__1checkRemoveable(
    JNIEnv* aJni,
    jclass,
    jint aSvgProxyHandle,
    jint aElementHandle)
{
    TInt removeable = -1;
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGElement ) _checkRemoveable - begin");
    TInt err = KM2GNotOk;
    
    
    M2G_DO_LOCK
    if (aSvgProxyHandle)
        {
        MM2GSVGProxy* aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);
    
        TRAP(err, aProxy->CheckRemoveableL(aElementHandle, removeable););
        }
    M2G_DO_UNLOCK(aJni)

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
JNIEXPORT jint JNICALL
Java_com_nokia_microedition_m2g_M2GSVGElement__1getBBox(
    JNIEnv* aJni,
    jclass,
    jint aSvgProxyHandle,
    jint aElementHandle,
    jshort aAttributeTypeId,
    jfloatArray aRectComponents)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGElement ) _getBBox - begin");

    TM2GRectData rect;
    TInt result = 0;
    TInt err = KM2GNotOk;
    
    M2G_DO_LOCK
    if (aSvgProxyHandle)
        {
        MM2GSVGProxy* aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);

        TRAP(err, aProxy->GetBBoxL(
           STATIC_CAST(TM2GSvgElementHandle, aElementHandle),
           STATIC_CAST(TM2GSvgAttrType, aAttributeTypeId),
           rect, result); );
        }
    M2G_DO_UNLOCK(aJni)
  
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
JNIEXPORT jint JNICALL
Java_com_nokia_microedition_m2g_M2GSVGElement__1getColorTrait(
    JNIEnv* aJni,
    jclass,
    jint aSvgProxyHandle,
    jint aElementHandle,
    jshort aAttributeTypeId,
    jintArray aColorComponents)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGElement ) _getColorTrait - begin");
    TM2GColorData color;
    TInt result = -1;
    TInt err = KM2GNotOk;
    
    M2G_DO_LOCK
    if (aSvgProxyHandle)
        {
        MM2GSVGProxy* aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);
        TRAP(err, aProxy->GetColorTraitL(
            aElementHandle,
            aAttributeTypeId,
            color,
            result); );
        }
    M2G_DO_UNLOCK(aJni)

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
JNIEXPORT jshort JNICALL
Java_com_nokia_microedition_m2g_M2GSVGElement__1getElementTypeId(
    JNIEnv* aJni,
    jclass,
    jint aSvgProxyHandle,
    jint aElementHandle)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGElement ) _getElementTypeId - begin");
    TInt16 id = 0;
    TInt err = KM2GNotOk;
    
    M2G_DO_LOCK
    if (aSvgProxyHandle)
        {
        MM2GSVGProxy* aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);
        TRAP(err,  aProxy->GetElementTypeIdL(aElementHandle, id); );
        }
    M2G_DO_UNLOCK(aJni)
    
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
JNIEXPORT jshort JNICALL
Java_com_nokia_microedition_m2g_M2GSVGElement__1getEnumTrait(
    JNIEnv* aJni,
    jclass,
    jint aSvgProxyHandle,
    jint aElementHandle,
    jshort aAttributeTypeId)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGElement ) _getEnumTrait - begin");
    TInt16 enumTrait = 0;
    TInt err = KM2GNotOk;
    
    M2G_DO_LOCK
    if (aSvgProxyHandle)
        {
        MM2GSVGProxy* aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);

        TRAP(err, aProxy->GetEnumTraitL(
            STATIC_CAST(TM2GSvgElementHandle, aElementHandle),
            STATIC_CAST(TM2GSvgAttrType, aAttributeTypeId),
            enumTrait); );
        }
    M2G_DO_UNLOCK(aJni)
        
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
JNIEXPORT jint JNICALL
Java_com_nokia_microedition_m2g_M2GSVGElement__1getFirstElementChild(
    JNIEnv* aJni,
    jclass,
    jint aSvgProxyHandle,
    jint aElementHandle)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGElement ) _getFirstElementChild - begin");

    TInt childHandle = M2G_INVALID_HANDLE;
    TInt err = KM2GNotOk;

    M2G_DO_LOCK
    if (aSvgProxyHandle)
        {
        MM2GSVGProxy* aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);
        TRAP(err, aProxy->GetFirstElementChildL(aElementHandle, childHandle); );
        }
    M2G_DO_UNLOCK(aJni)

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
JNIEXPORT jfloat JNICALL
Java_com_nokia_microedition_m2g_M2GSVGElement__1getFloatTrait(
    JNIEnv* aJni,
    jclass,
    jint aSvgProxyHandle,
    jint aElementHandle,
    jshort aAttributeTypeId)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGElement ) _getFloatTrait - begin");
    TInt err = KM2GNotOk;
    TReal32 floatTrait = 0;
    
    M2G_DO_LOCK
    if (aSvgProxyHandle)
        {
       MM2GSVGProxy* aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);

       TRAP(err ,aProxy->GetFloatTraitL(
           STATIC_CAST(TM2GSvgElementHandle, aElementHandle),
           STATIC_CAST(TM2GSvgAttrType, aAttributeTypeId),
           floatTrait););
        }
    M2G_DO_UNLOCK(aJni)

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
 s*/
JNIEXPORT jint JNICALL
Java_com_nokia_microedition_m2g_M2GSVGElement__1getMatrixTrait(
    JNIEnv* aJni,
    jclass,
    jint aSvgProxyHandle,
    jint aElementHandle,
    jshort aAttributeTypeId,
    jfloatArray aMatrixComponents)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGElement ) _getMatrixTrait - begin");
    TM2GMatrixData matrix;
    TInt result = -1;
    TInt err = KM2GNotOk;
    
    M2G_DO_LOCK
    if (aSvgProxyHandle)
        {
        MM2GSVGProxy* aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);

        TRAP(err, aProxy->GetMatrixTraitL(
            STATIC_CAST(TM2GSvgElementHandle, aElementHandle),
            STATIC_CAST(TM2GSvgAttrType, aAttributeTypeId),
             matrix, result); );
        }
    M2G_DO_UNLOCK(aJni)

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
JNIEXPORT jint JNICALL
Java_com_nokia_microedition_m2g_M2GSVGElement__1getNextElementSibling(
    JNIEnv* aJni,
    jclass,
    jint aSvgProxyHandle,
    jint aElementHandle)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGElement ) _getNextElementSibling - begin");
    TInt siblingHandle = M2G_INVALID_HANDLE;
    TInt err = KM2GNotOk;
    
    M2G_DO_LOCK
    if (aSvgProxyHandle)
        {
           MM2GSVGProxy* aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);
           TRAP(err, aProxy->GetNextElementSiblingL(aElementHandle, siblingHandle); );
        }
    M2G_DO_UNLOCK(aJni)
   
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
JNIEXPORT jint JNICALL
Java_com_nokia_microedition_m2g_M2GSVGElement__1getParent(
    JNIEnv* aJni,
    jclass,
    jint aSvgProxyHandle,
    jint aElementHandle)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGElement ) _getParent - begin");
    TInt parentHandle = M2G_INVALID_HANDLE;
    TInt err = KM2GNotOk;
    
    M2G_DO_LOCK
    
    if (aSvgProxyHandle)
        {
        MM2GSVGProxy* aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);
        TRAP(err,    aProxy->GetParentL(aElementHandle, parentHandle););
        }
    M2G_DO_UNLOCK(aJni)

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
JNIEXPORT jint JNICALL
Java_com_nokia_microedition_m2g_M2GSVGElement__1getPathTrait(
    JNIEnv* aJni,
    jclass,
    jint aSvgProxyHandle,
    jint aElementHandle,
    jshort aAttributeTypeId)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGElement ) _getPathTrait - begin");
    TInt aPathTrait = 0;
    TInt err = KM2GNotOk;
    
    M2G_DO_LOCK
    if (aSvgProxyHandle)
        {
        MM2GSVGProxy* aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);

        TRAP(err, aProxy->GetPathTraitL(
           STATIC_CAST(TM2GSvgElementHandle, aElementHandle),
           STATIC_CAST(TM2GSvgAttrType, aAttributeTypeId),
           aPathTrait); );
        }
    M2G_DO_UNLOCK(aJni)
 
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
JNIEXPORT jint JNICALL
Java_com_nokia_microedition_m2g_M2GSVGElement__1getRectTrait(
    JNIEnv* aJni,
    jclass,
    jint aSvgProxyHandle,
    jint aElementHandle,
    jshort aAttributeTypeId,
    jfloatArray aRectComponents)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGElement ) _getRectTrait - begin");
    TM2GRectData rect;
    TInt result = 0;
    TInt err = KM2GNotOk;
    
    M2G_DO_LOCK
    if (aSvgProxyHandle)
        {
        MM2GSVGProxy* aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);

        TRAP(err , aProxy->GetRectTraitL(
            STATIC_CAST(TM2GSvgElementHandle, aElementHandle),
            STATIC_CAST(TM2GSvgAttrType, aAttributeTypeId),
            rect, result); );
        }
    M2G_DO_UNLOCK(aJni)
    
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
JNIEXPORT jint JNICALL
Java_com_nokia_microedition_m2g_M2GSVGElement__1getRootElement(
    JNIEnv* aJni,
    jclass,
    jint aSvgProxyHandle,
    jint aDocumentHandle)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGElement ) _getRootElement - begin");
    TInt rootElementHandle = M2G_INVALID_HANDLE;
    TInt err = KM2GNotOk;
    M2G_DO_LOCK
    if (aSvgProxyHandle)
        {
        MM2GSVGProxy* aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);

        TRAP(err, aProxy->GetRootElementL(
           STATIC_CAST(TM2GSvgDocumentHandle, aDocumentHandle),
           rootElementHandle); );
        }
    M2G_DO_UNLOCK(aJni)
 
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
JNIEXPORT void JNICALL
Java_com_nokia_microedition_m2g_M2GSVGElement__1getScreenBBox(
    JNIEnv* aJni,
    jclass,
    jint aSvgProxyHandle,
    jint aElementHandle,
    jfloatArray aScreenBBoxComponents)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGElement ) _getScreenBBox - begin");
    TM2GScreenBBoxData screenBBox;
    TInt err = KM2GNotOk;
    
    M2G_DO_LOCK
    if (aSvgProxyHandle)
        {
        MM2GSVGProxy* aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);

        TRAP(err, aProxy->GetScreenBBoxL(
            STATIC_CAST(TM2GSvgElementHandle, aElementHandle),
            screenBBox); );
        }
    M2G_DO_UNLOCK(aJni)
 
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
JNIEXPORT jstring JNICALL
Java_com_nokia_microedition_m2g_M2GSVGElement__1getStringTrait(
    JNIEnv* aJni,
    jclass,
    jint aSvgProxyHandle,
    jint aElementHandle,
    jshort aAttributeTypeId)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGElement ) _getStringTrait - begin");

    TPtrC16 attribute;
    TInt err = KM2GNotOk;
    
    M2G_DO_LOCK
    if (aSvgProxyHandle)
        {
        MM2GSVGProxy* aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);

        TRAP(err, aProxy->GetStringTraitL(
           STATIC_CAST(TM2GSvgElementHandle, aElementHandle),
           STATIC_CAST(TM2GSvgAttrType, aAttributeTypeId),
           attribute); );
        }
    M2G_DO_UNLOCK(aJni)
   
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
JNIEXPORT jint JNICALL
Java_com_nokia_microedition_m2g_M2GSVGElement__1getUsedFromElement(
    JNIEnv* aJni,
    jclass,
    jint aSvgProxyHandle,
    jint aElementHandle)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGElement ) _getUsedFromElement - begin");
    TInt handle = M2G_INVALID_HANDLE;
    TInt err = KM2GNotOk;
    
    M2G_DO_LOCK
    if (aSvgProxyHandle)
        {
        MM2GSVGProxy* aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);

        TRAP(err, aProxy->GetUsedFromElementL(
        STATIC_CAST(TM2GSvgElementHandle, aElementHandle),
        handle); );
        }
    M2G_DO_UNLOCK(aJni)
 
    M2GGeneral::CheckErrorCode(aJni, err);
    M2G_DEBUG_1("M2G_DEBUG: JNI ( M2GSVGElement ) _getUsedFromElement: %d - end", handle);
    return handle;
}


// -----------------------------------------------------------------------------
// Java_com_nokia_microedition_m2g_M2GSVGElement::_insertBefore
// -----------------------------------------------------------------------------
/**
 * Class: com_nokia_microedition_m2g_M2GSVGElement
 * Method: insertBefore
 * Signature:
 */
JNIEXPORT void JNICALL
Java_com_nokia_microedition_m2g_M2GSVGElement__1insertBefore(
    JNIEnv* aJni,
    jclass,
    jint aSvgProxyHandle,
    jint aElementHandle,
    jint aNewChildElementHandle,
    jint aReferenceElementHandle)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGElement ) _insertBefore - begin");
    TInt err = KM2GNotOk;
    
    M2G_DO_LOCK
    
    if (aSvgProxyHandle)
        {
        MM2GSVGProxy* aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);
    
        TRAP(err, aProxy->InsertBeforeL(
          STATIC_CAST(TM2GSvgElementHandle, aElementHandle),
          STATIC_CAST(TM2GSvgElementHandle, aNewChildElementHandle),
          STATIC_CAST(TM2GSvgElementHandle, aReferenceElementHandle)); );
        }
    M2G_DO_UNLOCK(aJni)

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
JNIEXPORT jint JNICALL
Java_com_nokia_microedition_m2g_M2GSVGElement__1isUsed(
    JNIEnv* aJni,
    jclass,
    jint aSvgProxyHandle,
    jint aElementHandle )
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGElement ) _isUsed - begin");
    TInt result = KM2GNotOk;
    TInt err = KM2GNotOk;
    
    M2G_DO_LOCK
    if (aSvgProxyHandle)
        {
        MM2GSVGProxy* aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);
        TRAP(err, aProxy->IsUsedL(STATIC_CAST(TM2GSvgElementHandle, aElementHandle), result); );
        }
    M2G_DO_UNLOCK(aJni)

    
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
JNIEXPORT jint JNICALL
Java_com_nokia_microedition_m2g_M2GSVGElement__1removeChild(
    JNIEnv* aJni,
    jclass,
    jint aSvgProxyHandle,
    jint aElementHandle,
    jint aChildElementHandle)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGElement ) _removeChild - begin");
    TInt handle = M2G_INVALID_HANDLE;
    TInt err = KM2GNotOk;

    M2G_DO_LOCK
    if (aSvgProxyHandle)
        {
        MM2GSVGProxy* aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);

         TRAP(err ,aProxy->RemoveChildL(
           STATIC_CAST(TM2GSvgElementHandle, aElementHandle),
           STATIC_CAST(TM2GSvgElementHandle, aChildElementHandle), handle); );
        }
    M2G_DO_UNLOCK(aJni)
  
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
JNIEXPORT void JNICALL
Java_com_nokia_microedition_m2g_M2GSVGElement__1setColorTrait(
    JNIEnv* aJni,
    jclass,
    jint aSvgProxyHandle,
    jint aElementHandle,
    jshort aAttributeTypeId,
    jint aRed, jint aGreen, jint aBlue)
{
    M2G_DEBUG_3("M2G_DEBUG: JNI ( M2GSVGElement ) _setColorTrait: R=%d, G=%d, B=%d - begin", aRed, aGreen, aBlue);
    TM2GColorData color;
    color[ 0 ] = aRed;
    color[ 1 ] = aGreen;
    color[ 2 ] = aBlue;
    TInt err = KM2GNotOk;

    M2G_DO_LOCK
    if (aSvgProxyHandle)
        {
           MM2GSVGProxy* aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);

           TRAP(err, aProxy->SetColorTraitL(
            STATIC_CAST(TM2GSvgElementHandle, aElementHandle),
            STATIC_CAST(TM2GSvgAttrType, aAttributeTypeId),
            color); );
        }
    M2G_DO_UNLOCK(aJni)
 
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
 * @param aValue 
 */
JNIEXPORT void JNICALL
Java_com_nokia_microedition_m2g_M2GSVGElement__1setEnumTrait(
    JNIEnv* aJni,
    jclass,
    jint aSvgProxyHandle,
    jint aElementHandle,
    jshort aAttributeTypeId,
    jshort aValue)
{
    M2G_DEBUG_2("M2G_DEBUG: JNI ( M2GSVGElement ) _setEnumTrait: type=%d, value=%d - begin", aAttributeTypeId, aValue);
    TInt err = KM2GNotOk;
    
    M2G_DO_LOCK
    if (aSvgProxyHandle)
        {
        MM2GSVGProxy* aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);
    
        TRAP(err, aProxy->SetEnumTraitL(
        STATIC_CAST(TM2GSvgElementHandle, aElementHandle),
        STATIC_CAST(TM2GSvgAttrType, aAttributeTypeId),
        aValue); );
        }
    M2G_DO_UNLOCK(aJni)

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
JNIEXPORT void JNICALL
Java_com_nokia_microedition_m2g_M2GSVGElement__1setFloatTrait(
    JNIEnv* aJni,
    jclass,
    jint aSvgProxyHandle,
    jint aElementHandle,
    jshort aAttributeTypeId,
    jfloat aValue)
{
    M2G_DEBUG_1("M2G_DEBUG: JNI ( M2GSVGElement ) _setFloatTrait: %f - begin", aValue);
    TInt err = KM2GNotOk;
    
    M2G_DO_LOCK
    if (aSvgProxyHandle)
        {
        MM2GSVGProxy* aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);

        TRAP(err, aProxy->SetFloatTraitL(
        STATIC_CAST(TM2GSvgElementHandle, aElementHandle),
        STATIC_CAST(TM2GSvgAttrType, aAttributeTypeId),
        aValue); );
        }
    M2G_DO_UNLOCK(aJni)

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
JNIEXPORT void JNICALL
Java_com_nokia_microedition_m2g_M2GSVGElement__1setMatrixTrait(
    JNIEnv* aJni,
    jclass,
    jint aSvgProxyHandle,
    jint aElementHandle,
    jshort aAttributeTypeId,
    jfloatArray aMatrixComponents)
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
    
    M2G_DO_LOCK
    if (aSvgProxyHandle)
        {
        MM2GSVGProxy* aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);

        TRAP(err, aProxy->SetMatrixTraitL(
            STATIC_CAST(TM2GSvgElementHandle, aElementHandle),
            STATIC_CAST(TM2GSvgAttrType, aAttributeTypeId),
            matrix); );
        }
    M2G_DO_UNLOCK(aJni)
   
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
JNIEXPORT void JNICALL
Java_com_nokia_microedition_m2g_M2GSVGElement__1setPathTrait(
    JNIEnv* aJni,
    jclass,
    jint aSvgProxyHandle,
    jint aElementHandle,
    jshort aAttributeTypeId,
    jint aPathHandle)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGElement ) _setPathTrait - begin");
    TInt err = KM2GNotOk;
    
    M2G_DO_LOCK
    if (aSvgProxyHandle)
        {
        MM2GSVGProxy* aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);

        TRAP(err,aProxy->SetPathTraitL(
           STATIC_CAST(TM2GSvgElementHandle, aElementHandle),
           STATIC_CAST(TM2GSvgAttrType, aAttributeTypeId),
           STATIC_CAST(TM2GSvgPathHandle, aPathHandle)); );
        }
    M2G_DO_UNLOCK(aJni)

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
JNIEXPORT void JNICALL
Java_com_nokia_microedition_m2g_M2GSVGElement__1setRectTrait(
    JNIEnv* aJni,
    jclass,
    jint aSvgProxyHandle,
    jint aElementHandle,
    jshort aAttributeTypeId,
    jfloat aX, jfloat aY, jfloat aWidth, jfloat aHeight)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGElement ) _setRectTrait - begin");
    TM2GRectData rect;
    rect[ 0 ] = aX;
    rect[ 1 ] = aY;
    rect[ 2 ] = aWidth;
    rect[ 3 ] = aHeight;
    TInt err = KM2GNotOk;

    M2G_DO_LOCK
    if (aSvgProxyHandle)
        {
        MM2GSVGProxy* aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);
    
        TRAP(err,  aProxy->SetRectTraitL(
                STATIC_CAST(TM2GSvgElementHandle, aElementHandle),
                STATIC_CAST(TM2GSvgAttrType, aAttributeTypeId),
                rect); );
        }
    M2G_DO_UNLOCK(aJni)

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
JNIEXPORT void JNICALL
Java_com_nokia_microedition_m2g_M2GSVGElement__1setStringTrait(
    JNIEnv* aJni,
    jclass,
    jint aSvgProxyHandle,
    jint aElementHandle,
    jshort aAttributeTypeId,
    jstring aValue)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGElement ) _setStringTrait - begin");
    //RJString str(*aJni, aValue);
    JStringUtils str(*aJni, aValue);
    TInt err = KM2GNotOk;
    
    M2G_DO_LOCK
    if (aSvgProxyHandle)
        {
        MM2GSVGProxy* aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);
        TPtrC16 lValue=STATIC_CAST(TPtrC16, str);
        
        TRAP(err, aProxy->SetStringTraitL(
            STATIC_CAST(TM2GSvgElementHandle, aElementHandle),
            STATIC_CAST(TM2GSvgAttrType, aAttributeTypeId),
            lValue); );
        }
    M2G_DO_UNLOCK(aJni)
   
    M2GGeneral::CheckErrorCode(aJni, err);
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGElement ) _setStringTrait - end");
}

M2G_NS_END
