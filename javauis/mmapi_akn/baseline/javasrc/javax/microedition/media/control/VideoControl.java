/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Please refer JSR 135 for more details.
*
*/


package javax.microedition.media.control;

import javax.microedition.media.MediaException;

public interface VideoControl extends GUIControl
{
    int USE_DIRECT_VIDEO = 1;

    Object initDisplayMode(int aMode, Object aArg);

    void setDisplayLocation(int aX, int aY);

    int getDisplayX();

    int getDisplayY();

    void setVisible(boolean aVisible);

    void setDisplaySize(int aWidth, int aHeight)
    throws MediaException;

    void setDisplayFullScreen(boolean aFullScreenMode) throws MediaException;

    int getSourceWidth();

    int getSourceHeight();

    int getDisplayWidth();

    int getDisplayHeight();

    byte[] getSnapshot(String aImageType) throws MediaException;
}

