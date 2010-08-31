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

import javax.microedition.lcdui.Image;
import javax.microedition.amms.control.*;
import javax.microedition.media.*;

/**
 * Please refer to JSR 234 for more details.
 */
public interface OverlayControl extends EffectControl
{

    /**
     * Please refer to JSR 234 for more details.
     */
    int insertImage(Object image, int x, int y, int order) throws IllegalArgumentException;

    /**
     * Please refer to JSR 234 for more details.
     */
    int insertImage(Object image, int x, int y, int order, int transparentColor) throws IllegalArgumentException;

    /**
     * Please refer to JSR 234 for more details.
     */
    void removeImage(Object image);


    /**
     * Please refer to JSR 234 for more details.
     */
    Object getImage(int order);

    /**
     * Please refer to JSR 234 for more details.
     */
    int numberOfImages();



    /**
     * Please refer to JSR 234 for more details.
     *
     */
    void clear();

}
