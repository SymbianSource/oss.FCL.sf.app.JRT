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
package javax.microedition.amms.control.audio3d;

import javax.microedition.media.*;

/**
 *
 Please refer to JSR 234 for more details.
 */

public interface LocationControl extends javax.microedition.media.Control
{

    /**
     Please refer to JSR 234 for more details.
     */
    public void setCartesian(int x, int y, int z);


    /**
     * Please refer to JSR 234 for more details.
     */
    public void setSpherical(int azimuth, int elevation, int radius);


    /**
     * Please refer to JSR 234 for more details.
     */
    public int[] getCartesian();

}
