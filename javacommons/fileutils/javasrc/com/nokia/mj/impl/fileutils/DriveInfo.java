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

public final class DriveInfo
{
    public String   iRootPath;
    public boolean  iIsPresent;
    public boolean  iIsRemovable;
    public boolean  iIsLocal;
    public boolean  iIsReadOnly;
    public boolean  iIsExternallyMountable;
    public long     iId;
}

