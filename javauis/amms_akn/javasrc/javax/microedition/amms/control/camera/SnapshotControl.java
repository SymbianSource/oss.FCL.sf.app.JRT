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
package javax.microedition.amms.control.camera;

import javax.microedition.media.MediaException;
import javax.microedition.media.Control;

/**
 * Please refer to JSR 234 for more details.
 */
public interface SnapshotControl extends Control
{

    /**
     * Please refer to JSR 234 for more details.
     */
    String SHOOTING_STOPPED = "SHOOTING_STOPPED";

    /**
     * Please refer to JSR 234 for more details.
     */
    String STORAGE_ERROR = "STORAGE_ERROR";

    /**
     * Please refer to JSR 234 for more details.
     */
    String WAITING_UNFREEZE = "WAITING_UNFREEZE";

    /**
     * Please refer to JSR 234 for more details.
     */
    public final static int FREEZE = -2;

    /**
     * Please refer to JSR 234 for more details.
     */
    public final static int FREEZE_AND_CONFIRM = -1;



    /**
     * Please refer to JSR 234 for more details.
     */
    void setDirectory(String directory);

    /**
     * Please refer to JSR 234 for more details.
     */
    String getDirectory();

    /**
     * Please refer to JSR 234 for more details.
     */
    void setFilePrefix(String prefix);

    /**
     * Please refer to JSR 234 for more details.
     */
    String getFilePrefix();

    /**
     * Please refer to JSR 234 for more details.
     */
    void setFileSuffix(String suffix);

    /**
     * Please refer to JSR 234 for more details.
     */
    String getFileSuffix();

    /**
     * Please refer to JSR 234 for more details.
     */
    void start(int maxShots) throws SecurityException;


    /**
     * Please refer to JSR 234 for more details.
     */
    void stop();

    /**
     * Please refer to JSR 234 for more details.
     */
    void unfreeze(boolean save);

}

