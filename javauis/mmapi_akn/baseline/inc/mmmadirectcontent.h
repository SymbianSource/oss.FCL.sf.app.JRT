/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef MMMADIRECTCONTENT_H
#define MMMADIRECTCONTENT_H

class MMMAContainer;

/**
 * Content, such as a video clip or camera view finder, which is displayed on
 * a container.
 */
class MMMADirectContent
{
public:
    /**
     * Notify content that container visiblity has changed. The content must not
     * be displayed when it's container is invisible.
     * @param "aVisible" ETrue if the container is visible, EFalse if it is invisible.
     */
    virtual void MdcContainerVisibilityChanged(TBool aVisible) = 0;

    /**
     * Notify content that content rect is changed.
     * @param aContentRect Maximum area the content can occupy in screen
     * co-ordinates.
     * @param aParentRect area that can be used to draw content in screen
     * co-ordinates.
     */
    virtual void MdcContentRectChanged(const TRect& aContentRect,
                                       const TRect& aParentRect) = 0;

    /**
     * Notify content that container is about to be destoryed.
     */
    virtual void MdcContainerDestroyed() = 0;

    /**
     * Sets new container for content. Content may not support changing
     * container.
     * @parma aContainer New container for content
     */
    virtual void MdcSetContainer(MMMAContainer* aContainer) = 0;

    /**
     * This is used in bitmap mode. DSA mode content return allways NULL.
     * @return Bitmap to draw or NULL if not available.
     */
    virtual CFbsBitmap* MdcFrameBuffer() const = 0;

    /**
     * This is used get preferred size when container need content size
     * @return source size which preferred size for container
     */
    virtual TSize MdcSourceSize() = 0;

    /**
     * Notify content that the rect of a window has changed.
     *
     * @param aRect New rect of a window.
     */
    virtual void MdcContainerWindowRectChanged(const TRect& aRect) = 0;
};

#endif // MMMADIRECTCONTENT_H
