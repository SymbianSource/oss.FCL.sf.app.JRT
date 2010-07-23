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
* Description:
*
*/
package org.w3c.dom.svg;


/**
 * This interface represents an Animation element, which contains methods to control the timing of
 * animations.
 */
public interface SVGAnimationElement extends SVGElement
{

    /**
     * Creates a begin instance time for the current time plus or minus the passed offset.
     * The new instance time is added to the <a href="http://www.w3.org/TR/2001/REC-smil20-20010807/smil20.html#smil-timing-Timing-BeginEnd-InstanceTimesLists">
     * begin instance times list</a>.

     *
     * @param offset The offset in seconds at which to begin the element.
     */
    public void beginElementAt(float offset);

    /**
     * Creates an end instance time for the current time plus or minus the passed offset.
     * The new instance time is added to the <a href="http://www.w3.org/TR/2001/REC-smil20-20010807/smil20.html#smil-timing-Timing-BeginEnd-InstanceTimesLists">
     * end instance times list</a>.

     *
     * @param offset The offset in seconds at which to end the element.
     */
    public void endElementAt(float offset);

}
