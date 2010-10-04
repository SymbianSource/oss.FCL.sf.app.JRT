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

import javax.microedition.io.Connector;
import javax.microedition.io.file.FileConnection;

public class CLDCCompatibilityDelegate implements CompatibilityDelegate {
    
    public CLDCCompatibilityDelegate(){
        super();
    }
    
    public InputStream getInputStreamFromFile(String path) throws IOException {
        String jsr75Path = convertPathToSpecific(path);

        InputStream inputStream = null;
        try{
            FileConnection fileConct = (FileConnection)Connector.open(jsr75Path);
            inputStream = fileConct.openInputStream();
            fileConct.close();
        }
        catch (IOException e) {
           throw e;
        }
        catch(Exception e)
        {
            throw new IOException(e.getMessage());
        }
        return inputStream;
    }

    public String convertPathToSpecific(String path) {        
        String res = path;
        
        // In JSR 75 the file separator is '/' and not '\'
        res = res.replace('\\', '/');
        
        if (res.startsWith("file:///")) {
            res = res.substring(8);
        }
        
        boolean hasDrive = false;
        if (res.length() >= 2) {
            char drive = res.charAt(0);
            hasDrive = ((drive >= 'A' && drive <= 'Z') || (drive >= 'a' && drive <= 'z')) 
                && (res.charAt(1) == ':');
        }

        if (!hasDrive) {
            res = System.getProperty("fileconn.dir.private") + res; // may start now as file:///
        }

        if (!res.startsWith("file:///")) {
            res = "file:///" + res;
        }
        
        // The format now should be: file:///<drive>:/<path>
        
        return res;
    }
    
    public boolean canOpenFile(String path) throws SecurityException {
        String jsr75Path = convertPathToSpecific(path);
        try {
            FileConnection fileConct = (FileConnection) Connector
                    .open(jsr75Path);
            fileConct.close();
        } catch (IOException e) {
            throw new SecurityException(e.getMessage());
        }
        return true;
    }

    public OutputStream getOutStreamFromFile(String path) throws IOException {
        String jsr75Path = convertPathToSpecific(path);

        OutputStream outStream = null; 
        try{
            FileConnection fileConct = (FileConnection)Connector.open(jsr75Path);
            if (!fileConct.exists()) {
                fileConct.create();
            }
            outStream = fileConct.openOutputStream();
            fileConct.close();
        }
        catch(IOException e)
        {
            throw e;
        }
        catch (Exception e) {
            throw new IOException(e.getMessage());
        }
        return outStream;
    }    

}
