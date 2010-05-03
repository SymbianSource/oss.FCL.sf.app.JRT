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


package javax.microedition.io;

import javax.microedition.io.*;
import java.io.IOException;
import java.lang.String;
import java.lang.IllegalArgumentException;

public interface SocketConnection extends StreamConnection
{

    // Please refer to JSR 118.
    public final byte DELAY = 0;

    // A Socket option which specifies what should happen when the socket of a type that promises reliable delivery
    // still has untransmitted messages when it is closed
    public final byte LINGER = 1;

    // A Socket option controls the whether the underlying protocol should periodically transmit messages
    // on a connected socket. If the peer fails to respond to these messages, the connection is considered broken.
    public final byte KEEPALIVE = 2;

    // A Socket option which gets or sets the size of Input buffer
    public final byte RCVBUF = 3;

    // A Socket option which gets or sets the size of ouput buffer
    public final byte SNDBUF = 4;

    // Please refer to JSR 118.
    // The address can be either the remote host name or the IP address(if available).
    public String getAddress() throws IOException;

    // Please refer to JSR 118.
    public String getLocalAddress() throws IOException;

    // Please refer to JSR 118.
    public int getLocalPort() throws IOException;

    // Please refer to JSR 118.
    public int getPort() throws IOException;

    // Please refer to JSR 118.
    public int getSocketOption(byte option) throws IllegalArgumentException,
                IOException;

    public void setSocketOption(byte option, int value)
    throws IllegalArgumentException, IOException;

    public java.io.DataInputStream openDataInputStream()
    throws java.io.IOException;

    public java.io.InputStream openInputStream() throws java.io.IOException;

    public java.io.DataOutputStream openDataOutputStream()
    throws java.io.IOException;

    public java.io.OutputStream openOutputStream() throws java.io.IOException;

}