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

import javax.microedition.io.*;

public interface FileConnection extends StreamConnection
{

    public void create() throws java.io.IOException;

    public void rename(String name) throws java.io.IOException;

    public void setFileConnection(String name) throws java.io.IOException;

    public void mkdir() throws java.io.IOException;

    public void delete() throws java.io.IOException;

    public boolean exists();

    public String getName();

    public String getPath();

    public String getURL();

    public boolean canRead();

    public boolean canWrite();

    public void setReadable(boolean readable) throws java.io.IOException;

    public void setWritable(boolean writable) throws java.io.IOException;

    public void setHidden(boolean hidden) throws java.io.IOException;

    public boolean isDirectory();

    public boolean isOpen();

    public boolean isHidden();

    public long lastModified();

    public long fileSize() throws java.io.IOException;

    public long availableSize();

    public long usedSize();

    public long totalSize();

    public long directorySize(boolean includeSubDirs)
    throws java.io.IOException;

    public java.util.Enumeration list() throws java.io.IOException;

    public java.util.Enumeration list(java.lang.String filter,
                                      boolean includeHidden) throws java.io.IOException;

    public void truncate(long byteOffset) throws java.io.IOException;

    public java.io.DataInputStream openDataInputStream()
    throws java.io.IOException;

    public java.io.InputStream openInputStream() throws java.io.IOException;

    public java.io.DataOutputStream openDataOutputStream()
    throws java.io.IOException;

    public java.io.OutputStream openOutputStream() throws java.io.IOException;

    public java.io.OutputStream openOutputStream(long offset)
    throws java.io.IOException;

}
