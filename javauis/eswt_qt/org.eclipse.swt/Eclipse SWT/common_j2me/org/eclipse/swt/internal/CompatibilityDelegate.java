/*******************************************************************************
 * Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

public interface CompatibilityDelegate {
    public boolean canOpenFile(String path) throws SecurityException;
    public InputStream getInputStreamFromFile(String path) throws IOException;
    public String convertPathToSpecific(String path);
    public OutputStream getOutStreamFromFile(String path) throws IOException;
}
