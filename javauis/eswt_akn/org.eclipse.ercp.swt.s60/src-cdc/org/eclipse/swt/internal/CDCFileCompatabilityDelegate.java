/*******************************************************************************
 * Copyright (c) 2007 Nokia Corporation.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/
package org.eclipse.swt.internal;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;



public class CDCFileCompatabilityDelegate implements FileCompatabilityDelegate
{

    public CDCFileCompatabilityDelegate()
    {
        super();
    }

    public  InputStream getInputStreamFromFile(String path) throws IOException
    {
        return new FileInputStream(path);
    }

    public  String convertPathToSpecific(String path)
    {
        return path;
    }

    public String convertPathFromSpecific(String specificPath)
    {
        return specificPath;
    }

    public  OutputStream getOutStreamFromFile(String path) throws IOException
    {
        // Get the absolute path of the filename.
        File file = new File(path);
        String absolutePath = file.getAbsolutePath();

        return new FileOutputStream(absolutePath);
    }

}
