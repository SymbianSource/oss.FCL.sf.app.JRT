/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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

package javax.microedition.io.file;

import com.nokia.mj.impl.file.FileSystemRegistryImpl;

public class FileSystemRegistry
{
    private FileSystemRegistry()
    {
    }

    public static boolean addFileSystemListener(FileSystemListener aListener)
    {
        return FileSystemRegistryImpl.addFileSystemListener(aListener);
    }

    public static boolean removeFileSystemListener(FileSystemListener aListener)
    {
        return FileSystemRegistryImpl.removeFileSystemListener(aListener);
    }

    public static java.util.Enumeration listRoots()
    {
        return FileSystemRegistryImpl.listRoots();
    }
}
