/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: JNI implementation of CanvasGraphicsItem class
*
*/


// INTERNAL INCLUDES
#include "com_nokia_mid_ui_CanvasGraphicsItem.h"
#include "CMIDCanvas.h" // userinclude in nokialcdui.component

// EXTERNAL INCLUDES
#include <MMIDCanvasGraphicsItem.h>
#include <MMIDCanvasGraphicsItemPainter.h>
#include <CMIDToolkit.h>
#include <jutils.h>
#include <jdebug.h>

/**
 * Local helper function for creating the native side peer object for
 * CanvasGraphicsItem.
 *
 * @param aNativePeerHandle On return, contains the native peer's handle
 */
LOCAL_C void CreateNativePeerL(
    CMIDToolkit* aToolkit,
    jobject aPeer,
    TInt* aNativePeerHandle,
    MMIDCanvasGraphicsItemPainter* aItemPainter)
{
    // Get LCDUI component factory.
    MMIDComponentFactory* factory = aToolkit->ComponentFactory();

    // Create new graphics item component.
    MMIDCanvasGraphicsItem* graphicsItem = factory->CreateCanvasGraphicsItemL(
                                               aItemPainter);

    // Put the component to cleanup stack during the register operation.
    CleanupDisposePushL(graphicsItem);

    // Register component to the LCDUI.
    *aNativePeerHandle = aToolkit->RegisterComponentL(graphicsItem, aPeer);

    // Component can be popped from the cleanup stack.
    CleanupPopComponent(graphicsItem);
}

/*
 * Class:     com_nokia_mid_ui_CanvasGraphicsItem
 * Method:    _createNativePeer
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_com_nokia_mid_ui_CanvasGraphicsItem__1createNativePeer(
    JNIEnv* aJniEnv,
    jobject aPeer,
    jint aToolkitHandle,
    jint aPainterHandle,
    jint /* aWidth */,
    jint /* aHeight */)
{
    DEBUG("CanvasGraphicsItem.cpp - createNativePeer +");

    CMIDToolkit* toolkit = JavaUnhand< CMIDToolkit >(aToolkitHandle);

    MMIDCanvasGraphicsItemPainter* itemPainter =
        MIDUnhandObject<MMIDCanvasGraphicsItemPainter>(aPainterHandle);

    jobject peer = aJniEnv->NewWeakGlobalRef(aPeer);
    TInt handle = -1;
    TSize size;

    TInt error = toolkit->ExecuteTrap(
                     &CreateNativePeerL,
                     toolkit,
                     peer,
                     &handle,
                     itemPainter);

    if (error == KErrNone)
    {
        DEBUG(
            "CanvasGraphicsItem.cpp - createNativePeer");
    }
    else
    {
        // Global reference must be removed at this point if construction
        // failed for some reason.
        aJniEnv->DeleteWeakGlobalRef(static_cast< jweak >(peer));
    }

    DEBUG_INT("CanvasGraphicsItem.cpp - createNativePeer, error = %d", error);

    return (error != KErrNone ? error : handle);
}


/**
 * Local helper function for setting the size of a graphics item
 *
 * @param aGraphicsItem The graphics item object to be modified.
 * @param aWidth The width of the graphics item.
 * @param aHeight The height of the graphics item.
 * @param aHeightInRows Indicates if the height is presented in rows.
 */
LOCAL_C void SetSizeL(
    MMIDCanvasGraphicsItem* aGraphicsItem,
    TInt aWidth,
    TInt aHeight)
{
    aGraphicsItem->SetSizeL(aWidth, aHeight);
}

/*
 * Class:     com_nokia_mid_ui_CanvasGraphicsItem
 * Method:    _setSize
 * Signature: (IIII)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mid_ui_CanvasGraphicsItem__1setSize(
    JNIEnv* /* aJniEnv */,
    jobject /* aPeer */,
    jint aToolkitHandle,
    jint aNativePeerHandle,
    jint aWidth,
    jint aHeight)
{
    DEBUG("CanvasGraphicsItem.cpp - setSize +");

    CMIDToolkit* toolkit =
        JavaUnhand< CMIDToolkit >(aToolkitHandle);

    MMIDCanvasGraphicsItem* item =
        MIDUnhandObject< MMIDCanvasGraphicsItem >(aNativePeerHandle);

    TInt error = toolkit->ExecuteTrap(
                     &SetSizeL,
                     item,
                     aWidth,
                     aHeight);

    DEBUG("CanvasGraphicsItem.cpp - setSize -");

    return error;
}

/**
 * Local helper function for setting the parent of a graphics item
 *
 * @param aGraphicsItem The graphics item object to be modified.
 * @param aParent The parent to be set.
 */
LOCAL_C void SetParentL(
    MMIDCanvasGraphicsItem* aGraphicsItem, MMIDComponent* aParent)
{
    MMIDCustomComponentContainer* container = NULL;
    MDirectContainer* directContainer = NULL;

    if (aParent)
    {
        MMIDComponent::TType type(aParent->Type());

        __ASSERT_DEBUG(type == MMIDComponent::ECanvas ||
                       type == MMIDComponent::ECustomItem,
                       User::Invariant());

        // Use static cast instead of reinterpret_cast because
        // reinterpret_cast does not preform the conversion correctly.
        // static_cast is OK eventhough CMIDCanvas is non-sharable class.
        // We don't use its methods.
        container = static_cast< CMIDCanvas* >(aParent);
        directContainer = static_cast< CMIDCanvas* >(aParent);
    }

    aGraphicsItem->SetParentL(container);
    aGraphicsItem->SetDirectContainerL(directContainer);
}

/*
 * Class:     com_nokia_mid_ui_CanvasGraphicsItem
 * Method:    _setParent
 * Signature: (III)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mid_ui_CanvasGraphicsItem__1setParent(
    JNIEnv* /* aJniEnv */,
    jobject /* aPeer */,
    jint aToolkitHandle,
    jint aNativePeerHandle,
    jint aParentHandle)
{
    CMIDToolkit* toolkit = JavaUnhand< CMIDToolkit >(aToolkitHandle);
    MMIDComponent* parent = NULL;

    MMIDCanvasGraphicsItem* item =
        MIDUnhandObject<MMIDCanvasGraphicsItem>(aNativePeerHandle);

    if (aParentHandle)
    {
        parent = MIDUnhandObject< MMIDComponent >(aParentHandle);
    }

    TInt error = toolkit->ExecuteTrap(
                     &SetParentL,
                     item,
                     parent);

    DEBUG_INT("CanvasGraphicsItem.cpp - setParent, error = %d", error);

    return error;
}

/**
 * Local helper function for setting a graphics item visible.
 *
 * @param aItem The graphics item to set visible.
 */
LOCAL_C void SetVisibleL(MMIDCanvasGraphicsItem* aItem, TBool aVisible)
{
    aItem->SetVisibleL(aVisible);
}

/*
 * Class:     com_nokia_mid_ui_CanvasGraphicsItem
 * Method:    _show
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mid_ui_CanvasGraphicsItem__1setVisible(
    JNIEnv* /* aJniEnv */,
    jobject /* aPeer */,
    jint aToolkitHandle,
    jint aNativePeerHandle,
    jboolean aVisible)
{
    DEBUG_INT("CanvasGraphicsItem.cpp - setVisible +, aVisible=%d", aVisible);

    CMIDToolkit* toolkit = JavaUnhand< CMIDToolkit >(aToolkitHandle);

    MMIDCanvasGraphicsItem* item =
        MIDUnhandObject<MMIDCanvasGraphicsItem>(aNativePeerHandle);

    TInt error = toolkit->ExecuteTrap(
                     &SetVisibleL,
                     item,
                     (TBool)aVisible);

    DEBUG_INT("CanvasGraphicsItem.cpp - setVisible -, error=%d", error);

    return error;
}

/**
 * Local helper function for setting a graphics item's position.
 *
 * @param aItem The graphics item.
 * @param aX The x coordinate of the anchor point.
 * @param aY The y coordinate of the anchor point.
 */
LOCAL_C void SetPosition(MMIDCanvasGraphicsItem* aItem, TInt aX, TInt aY)
{
    aItem->SetPosition(aX, aY);
}

/*
 * Class:     com_nokia_mid_ui_CanvasGraphicsItem
 * Method:    _setPosition
 * Signature: (IIII)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mid_ui_CanvasGraphicsItem__1setPosition(
    JNIEnv* /* aJniEnv */,
    jobject /* aPeer */,
    jint aToolkitHandle,
    jint aNativePeerHandle,
    jint aX,
    jint aY)
{
    DEBUG("CanvasGraphicsItem.cpp - setPosition +");

    CMIDToolkit* toolkit = JavaUnhand< CMIDToolkit >(aToolkitHandle);

    MMIDCanvasGraphicsItem* item =
        MIDUnhandObject<MMIDCanvasGraphicsItem>(aNativePeerHandle);

    toolkit->ExecuteV(&SetPosition, item, aX, aY);

    DEBUG("CanvasGraphicsItem.cpp - setPosition -");

    return KErrNone;
}


/**
 * Local helper function for setting the elevation of a graphics item.
 *
 * @param aGraphicsItem The graphics item object to be modified.
 * @param aZ The z-position defining the elevation.
 */
LOCAL_C void SetElevationL(MMIDCanvasGraphicsItem* aGraphicsItem, TInt aZ)
{
    aGraphicsItem->SetElevationL(aZ);
}

/*
 * Class:     com_nokia_mid_ui_CanvasGraphicsItem
 * Method:    _setZPosition
 * Signature: (III)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mid_ui_CanvasGraphicsItem__1setZPosition(
    JNIEnv* /* aJniEnv */,
    jobject /* aPeer */,
    jint aToolkitHandle,
    jint aNativePeerHandle,
    jint aZ)
{
    DEBUG("CanvasGraphicsItem.cpp - setZPosition +");

    CMIDToolkit* toolkit = JavaUnhand< CMIDToolkit >(aToolkitHandle);

    MMIDCanvasGraphicsItem* item =
        MIDUnhandObject<MMIDCanvasGraphicsItem>(aNativePeerHandle);

    TInt error = toolkit->ExecuteTrap(
                     &SetElevationL,
                     item,
                     aZ);

    DEBUG_INT("CanvasGraphicsItem.cpp - setZPosition -, error=%d", error);

    return error;
}

/**
 * Local helper function for getting the elevation of a graphics item.
 *
 * @param aItem The graphics item.
 * @return The elevation of the graphics item's content.
 */
LOCAL_C TInt Elevation(MMIDCanvasGraphicsItem* aItem)
{
    return aItem->Elevation();
}

/*
 * Class:     com_nokia_mid_ui_CanvasGraphicsItem
 * Method:    _getZPosition
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mid_ui_CanvasGraphicsItem__1getZPosition(
    JNIEnv* /* aJniEnv */,
    jobject /* aPeer */,
    jint aToolkitHandle,
    jint aNativePeerHandle)
{
    DEBUG("CanvasGraphicsItem.cpp - getZPosition +");

    CMIDToolkit* toolkit = JavaUnhand< CMIDToolkit >(aToolkitHandle);

    MMIDCanvasGraphicsItem* item =
        MIDUnhandObject<MMIDCanvasGraphicsItem>(aNativePeerHandle);

    TInt position = toolkit->Execute(&Elevation, item);

    DEBUG_INT("CanvasGraphicsItem.cpp - getZPosition -, position=%d",
              position);

    return position;
}


/**
 * Local helper function for dispoisng graphics item native side component.
 *
 * @param aItem The graphics item to be destroyed.
 */
LOCAL_C void Dispose(CMIDToolkit* aToolkit, MMIDCanvasGraphicsItem* aItem)
{
    aToolkit->DisposeObject(aItem);
}

/*
 * Class:     com_nokia_mid_ui_CanvasGraphicsItem
 * Method:    _dispose
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_com_nokia_mid_ui_CanvasGraphicsItem__1dispose(
    JNIEnv* /* aJniEnv */,
    jobject /* aPeer */,
    jint aToolkitHandle,
    jint aNativePeerHandle)
{
    DEBUG("CanvasGraphicsItem.cpp - dispose +");

    CMIDToolkit* toolkit = JavaUnhand< CMIDToolkit >(aToolkitHandle);

    MMIDCanvasGraphicsItem* item =
        MIDUnhandObject<MMIDCanvasGraphicsItem>(aNativePeerHandle);

    toolkit->ExecuteV(&Dispose, toolkit, item);

    DEBUG("CanvasGraphicsItem.cpp - dispose -");
}

