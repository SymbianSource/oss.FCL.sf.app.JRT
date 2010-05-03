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


package com.nokia.mj.impl.fileutils;

public interface DriveListener
{
    // keep in sync with driveutilities.h!!

    // Drive observer operation types.
    public final static int REMOVABLE_MEDIA_INSERTED   = 1;
    public final static int REMOVABLE_MEDIA_REMOVED    = 2;

    public abstract void driveChanged(final int aOperation, final DriveInfo aDriveInfo);
}
