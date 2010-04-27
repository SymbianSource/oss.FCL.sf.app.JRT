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
package javax.microedition.amms.control.imageeffect;

import javax.microedition.media.Control;
import javax.microedition.amms.control.*;

/**
 *
 * Please refer to JSR 234 for more details.
 */
public interface ImageTransformControl extends EffectControl
{

    /**
     * Please refer to JSR 234 for more details.
     */
    public int getSourceWidth();

    /**
     * Please refer to JSR 234 for more details.
     */
    public int getSourceHeight();

    /**
     * Please refer to JSR 234 for more details.
     */
    public void setSourceRect(int x, int y, int width, int height);


    /**
     * Please refer to JSR 234 for more details.
     */
    public void setTargetSize(int width, int height, int rotation);
}
