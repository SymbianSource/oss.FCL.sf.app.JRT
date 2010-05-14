/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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

package com.nokia.microedition.media.protocol;

import javax.microedition.media.protocol.SourceStream;
import com.nokia.microedition.media.HttpDataSource;
import javax.microedition.media.MediaException;
import com.nokia.microedition.media.Locator;
import com.nokia.microedition.media.BufferDataSource;
import java.io.IOException;

public class EMCSourceInfo
{

    // media file header
    byte[] header;

    public EMCSourceInfo(BufferDataSource datasource) throws IOException, MediaException
    {
        // datasource.getLocator().getMiddlePart() check if it is 3gp/mp4/rm
        Locator locator = new Locator(datasource.getLocator());
        String extension = getExtension(locator.getMiddlePart());


        if (extension.equals("rm"))
        {

            // read first 5120 bytes of data and pass to native side*/
            header = new byte[ 5120 ];
            int bytesRead = datasource.readAndBuffer(header, 0, 5120);
            if (bytesRead != 5120)
            {
                throw new IOException("Reading from HTTP server failed");
            }

        }
        else if (extension.equals("3gp") || extension.equals("mp4"))   // 3gp/mp4
        {

            byte[] moovbox = new byte[ 512 ];
            int bytesRead = datasource.readAndBuffer(moovbox, 0, 512);
            if (bytesRead != 512)
            {
                throw new IOException("Reading from HTTP server failed");
            }
            String str = new String(moovbox);
            int pos = str.indexOf("moov");

            if (pos == -1)
            {

                return; // moov box not in first 512 bytes, not a streaming profile

            }
            else
            {

                // **************   extract filetype length
                int byte1 = moovbox[0]<0?(256 + moovbox[0]):moovbox[0];
                int byte2 = moovbox[1]<0?(256 + moovbox[1]):moovbox[1];
                int byte3 = moovbox[2]<0?(256 + moovbox[2]):moovbox[2];
                int byte4 = moovbox[3]<0?(256 + moovbox[3]):moovbox[3];

                byte1 = byte1 * 16777216;
                byte2 = byte2 * 65536;
                byte3 = byte3 * 256;
                byte4 = byte4 * 1;

                int filetype = byte1 + byte2 + byte3 + byte4;
                if (filetype < 0)
                {

                    return;
                }

                // **************  extract moovbox length
                byte1 = moovbox[pos-4]<0?(256 + moovbox[pos-4]):moovbox[pos-4];
                byte2 = moovbox[pos-3]<0?(256 + moovbox[pos-3]):moovbox[pos-3];
                byte3 = moovbox[pos-2]<0?(256 + moovbox[pos-2]):moovbox[pos-2];
                byte4 = moovbox[pos-1]<0?(256 + moovbox[pos-1]):moovbox[pos-1];

                byte1 = byte1 * 16777216;
                byte2 = byte2 * 65536;
                byte3 = byte3 * 256;
                byte4 = byte4 * 1;

                int moovboxlen = byte1 + byte2 + byte3 + byte4;
                if (moovboxlen < 0)
                {

                    return;
                }

                // **************

                // no. of bytes: total header length
                int nob = filetype + moovboxlen;
                if (nob < 0)
                {

                    return;
                }

                header = new byte[nob];
                System.arraycopy(moovbox, 0, header, 0, 512);
                moovbox = null;
                bytesRead = datasource.readAndBuffer(header, 512, nob-512);
                if (bytesRead != nob-512)
                {
                    throw new IOException();
                }
            }
        }
        else
        {

            return;
        }
    }

    private String getExtension(String locator)
    {
        int index = locator.lastIndexOf('.');
        if (index == -1) return "";

        try
        {

            return locator.substring(index + 1);

        }
        catch (IndexOutOfBoundsException ex)
        {
            //ignore
        }

        return "";
    }

    public void writeInfo(int eventsourcehandle, int managerhandle) throws MediaException
    {
        int err = 0;

        if (header != null)
        {

            err = _writeHeaderData(eventsourcehandle, managerhandle, header);

        }

        if (err != 0)
        {

            throw new MediaException(
                "Could not create player, Symbian OS error: " + err);
        }
    }

    private native int _writeHeaderData(int aEventsourcehandle, int aManagerHandle, byte[] header);

}

// END OF FILE
