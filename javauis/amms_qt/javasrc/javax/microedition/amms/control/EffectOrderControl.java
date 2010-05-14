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
package javax.microedition.amms.control;

import javax.microedition.media.MediaException;
import javax.microedition.media.Control;

/**
 * Please refer JSR 234 for more details.
 */
public interface EffectOrderControl extends Control
{

    /**
     Please refer JSR 234 for more details.
     */
    public int setEffectOrder(EffectControl effect, int order);

    /**
     Please refer JSR 234 for more details.
     */
    public int getEffectOrder(EffectControl effect);

    /**
     * Please refer JSR 234 for more details.
     */
    public EffectControl[] getEffectOrders();
}
