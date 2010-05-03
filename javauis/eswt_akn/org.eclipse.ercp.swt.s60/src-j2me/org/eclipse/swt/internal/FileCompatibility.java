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

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

public class FileCompatibility
{

    private static final String CLDC_COMPATABILITY_DELEGATE = "org.eclipse.swt.internal.CLDCFileCompatabilityDelegate";
    private static final String CDC_COMPATABILITY_DELEGATE = "org.eclipse.swt.internal.CDCFileCompatabilityDelegate";

    private static FileCompatabilityDelegate fDelegate;

    private static FileCompatabilityDelegate getDelegate()
    {

        if (fDelegate == null)
        {
            Class clazz;
            try
            {
                clazz = Class.forName(CLDC_COMPATABILITY_DELEGATE);
                fDelegate = (FileCompatabilityDelegate)clazz.newInstance();
            }
            catch (Exception e)
            {

            }
            if (fDelegate == null)
            {
                try
                {
                    clazz = Class.forName(CDC_COMPATABILITY_DELEGATE);
                    fDelegate = (FileCompatabilityDelegate)clazz.newInstance();
                }
                catch (Exception e)
                {
                    new RuntimeException("Can not instantinate File Compatability Delegate");
                }
            }
        }
        return fDelegate;
    }

    public static InputStream getInputStreamFromFile(String path) throws IOException
    {
        return getDelegate().getInputStreamFromFile(path);
    }

    public static String convertPathToSpecific(String path)
    {
        return getDelegate().convertPathToSpecific(path);
    }

    public static String convertPathFromSpecific(String path)
    {
        return getDelegate().convertPathFromSpecific(path);
    }

    public static OutputStream getOutStreamFromFile(String path) throws IOException
    {
        return getDelegate().getOutStreamFromFile(path);
    }
}
