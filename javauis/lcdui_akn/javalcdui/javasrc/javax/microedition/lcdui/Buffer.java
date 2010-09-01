/*
* Copyright (c) 1999 - 2004 Nokia Corporation and/or its subsidiary(-ies).
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

package javax.microedition.lcdui;

import com.nokia.mj.impl.rt.legacy.NativeError;

/**
 * Buffer for LCDUI/LCDGR commands.
 *
 * Commands placed in buffer consist of a header followed by
 * optional data. The header is always a 32bit integer in
 * one of the following two forms.
 *
 * An activation header has the ACTIVATE marker (the top bit) set
 * and contains a component handle in the lower 30 bits.
 *
 * A command header has the top four bits clear, and the remaining
 * 28 bits are divided as follows:
 *
 * bits 0-15 contain the command size including the header as the
 * number of integers (or words) in the buffer occupied by the
 * commadn.
 *
 * bits 16-27 contain an unsigned integer opcode.
 *
 */
final class Buffer
{
    private static final int BUFFER_SIZE = 1024;    // 4KB buffer
    private static final int CHAR_BUFFER_SIZE = 128;

    public static final int NOP      = 0;
    public static final int ACTIVATE = (1<<31);
    public static final int OP_MASK  = 0x0FFF;

    /**
     * JavaUnhand<> mangled pointer to CMIDBuffer
     */
    private int         iHandle;

    /**
     * Java side buffer and write pointer.
     */
    private int[]       iBuffer;
    private int         iCount;

    /**
     * Tracks current buffer processor to minimize ACTIVATE commands.
     */
    private int         iCurrent;
    // index of activation size
    private int         iStart;     // index of activation size
    private char[] iCharBuffer;


    /**
     *
     */
    Buffer(Toolkit aToolkit)
    {
        iBuffer    = new int[BUFFER_SIZE];
        iCharBuffer = new char[CHAR_BUFFER_SIZE];
        iCount     = 0;
        iCurrent   = -1;
        iStart     = 0;
        iHandle    = NativeError.check(_open(aToolkit.getEventServerHandle()));
    }

    /**
     * Sync any pending commands and then shutdown buffer processing
     */
    synchronized void close()
    {
        if (iHandle <= 0)
        {
            return;
        }
        sync();
        _close(iHandle);
        iHandle = 0;
    }

    /**
     *
     */
    int getHandle()
    {
        if (iHandle <= 0)
        {
            throw new RuntimeException("Buffer closed");
        }
        return iHandle;
    }


    /**
     * writes a command with no parameters into the buffer.
     */
    final void write(int aHandle, int aCommand)
    {
        if (iHandle <= 0)
        {
            throw new RuntimeException("Buffer closed");
        }

        if (aHandle != iCurrent)
        {
            deactivate();       // remove existing target
            activate(aHandle);  // set new target - this may cause a flush.
        }

        // size = 1;
        final int opc  = (aCommand << 16) | 1;
        ensureSpace(1);

        iBuffer[iCount++] = opc;
    }

    /**
     *
     */
    final void write(int aHandle, int aCommand, int aArg)
    {
        if (iHandle <= 0)
        {
            throw new RuntimeException("Buffer closed");
        }

        if (aHandle != iCurrent)
        {
            deactivate();       // remove existing target
            activate(aHandle);  // set new target - this may cause a flush.
        }

        // size = 2;
        final int opc  = (aCommand << 16) | 2;
        ensureSpace(2);

        iBuffer[iCount++] = opc;
        iBuffer[iCount++] = aArg;
    }

    /**
     *
     */
    final void write(int aHandle, int aCommand, int aArg1, int aArg2)
    {
        if (iHandle <= 0)
        {
            throw new RuntimeException("Buffer closed");
        }

        if (aHandle != iCurrent)
        {
            deactivate();       // remove existing target
            activate(aHandle);  // set new target - this may cause a flush.
        }

        // size = 3;
        final int opc  = (aCommand << 16) | 3;
        ensureSpace(3);

        final int[] buffer = iBuffer;
        int count = iCount;
        buffer[count++] = opc;
        buffer[count++] = aArg1;
        buffer[count++] = aArg2;
        iCount = count;
    }

    /**
     *
     */
    final void write(int aHandle, int aCommand, int aArg1, int aArg2, int aArg3)
    {
        if (iHandle <= 0)
        {
            throw new RuntimeException("Buffer closed");
        }

        if (aHandle != iCurrent)
        {
            deactivate();       // remove existing target
            activate(aHandle);  // set new target - this may cause a flush.
        }

        // size = 4;
        final int opc  = (aCommand << 16) | 4;
        ensureSpace(4);

        final int[] buffer = iBuffer;
        int count = iCount;
        buffer[count++] = opc;
        buffer[count++] = aArg1;
        buffer[count++] = aArg2;
        buffer[count++] = aArg3;
        iCount = count;
    }

    /**
     *
     */
    final void write(int aHandle, int aCommand, int aArg1, int aArg2,
                     int aArg3, int aArg4)
    {
        if (iHandle <= 0)
        {
            throw new RuntimeException("Buffer closed");
        }

        if (aHandle != iCurrent)
        {
            deactivate();       // remove existing target
            activate(aHandle);  // set new target - this may cause a flush.
        }

        // size = 5;
        final int opc  = (aCommand << 16) | 5;
        ensureSpace(5);

        final int[] buffer = iBuffer;
        int count = iCount;
        buffer[count++] = opc;
        buffer[count++] = aArg1;
        buffer[count++] = aArg2;
        buffer[count++] = aArg3;
        buffer[count++] = aArg4;
        iCount = count;
    }

    /**
     *
     */
    final void write(int aHandle, int aCommand, int aArg1, int aArg2,
                     int aArg3, int aArg4, int aArg5)
    {
        if (iHandle <= 0)
        {
            throw new RuntimeException("Buffer closed");
        }

        if (aHandle != iCurrent)
        {
            deactivate();       // remove existing target
            activate(aHandle);  // set new target - this may cause a flush.
        }

        // size = 6;
        final int opc  = (aCommand << 16) | 6;
        ensureSpace(6);

        final int[] buffer = iBuffer;
        int count = iCount;
        buffer[count++] = opc;
        buffer[count++] = aArg1;
        buffer[count++] = aArg2;
        buffer[count++] = aArg3;
        buffer[count++] = aArg4;
        buffer[count++] = aArg5;
        iCount = count;
    }

    /**
     *
     */
    final void write(int aHandle, int aCommand, int aArg1, int aArg2,
                     int aArg3, int aArg4, int aArg5, int aArg6)
    {
        if (iHandle <= 0)
        {
            throw new RuntimeException("Buffer closed");
        }

        if (aHandle != iCurrent)
        {
            deactivate();       // remove existing target
            activate(aHandle);  // set new target - this may cause a flush.
        }

        // size = 7;
        final int opc  = (aCommand << 16) | 7;
        ensureSpace(7);

        final int[] buffer = iBuffer;
        int count = iCount;
        buffer[count++] = opc;
        buffer[count++] = aArg1;
        buffer[count++] = aArg2;
        buffer[count++] = aArg3;
        buffer[count++] = aArg4;
        buffer[count++] = aArg5;
        buffer[count++] = aArg6;
        iCount = count;
    }

    /**
     *
     */
    final void write(int aHandle, int aCommand, int aArg1, int aArg2,
                     int aArg3, int aArg4, int aArg5, int aArg6, int aArg7)
    {
        if (iHandle <= 0)
        {
            throw new RuntimeException("Buffer closed");
        }

        if (aHandle != iCurrent)
        {
            deactivate();       // remove existing target
            activate(aHandle);  // set new target - this may cause a flush.
        }

        // size = 8;
        final int opc  = (aCommand << 16) | 8;
        ensureSpace(8);

        final int[] buffer = iBuffer;
        int count = iCount;
        buffer[count++] = opc;
        buffer[count++] = aArg1;
        buffer[count++] = aArg2;
        buffer[count++] = aArg3;
        buffer[count++] = aArg4;
        buffer[count++] = aArg5;
        buffer[count++] = aArg6;
        buffer[count++] = aArg7;
        iCount = count;
    }

    /**
     *
     */
    final void write(int aHandle, int aCommand, int aArg1, int aArg2,
                     int aArg3, int aArg4, int aArg5, int aArg6, int aArg7,
                     int aArg8, int aArg9)
    {
        if (iHandle <= 0)
        {
            throw new RuntimeException("Buffer closed");
        }

        if (aHandle != iCurrent)
        {
            deactivate();       // remove existing target
            activate(aHandle);  // set new target - this may cause a flush.
        }

        // size = 10;
        final int opc  = (aCommand << 16) | 10;
        ensureSpace(10);

        final int[] buffer = iBuffer;
        int count = iCount;
        buffer[count++] = opc;
        buffer[count++] = aArg1;
        buffer[count++] = aArg2;
        buffer[count++] = aArg3;
        buffer[count++] = aArg4;
        buffer[count++] = aArg5;
        buffer[count++] = aArg6;
        buffer[count++] = aArg7;
        buffer[count++] = aArg8;
        buffer[count++] = aArg9;
        iCount = count;
    }

    /**
     * Writes a string command into the buffer, intended to support
     * Graphics.drawString(), this method also passes three additional
     * integer parameters.
     *
     */
    final void writeStr(int aHandle, int aCmd, String aString, int aOff,
                        int aLen, int aArg1, int aArg2, int aArg3)
    {
        //
        // Implementation note: We cannot avoid two copies as even a native method
        // would still have to use GetStringChars() which makes a copy. There is
        // no equivalent to GetIntArrayRegion.
        //
        if (aLen > 0)
        {
            final char[] array = (aLen <= CHAR_BUFFER_SIZE ? iCharBuffer : new char[aLen]);
            aString.getChars(aOff, aOff+aLen, array, 0);
            writeChars(aHandle, aCmd, array, 0, aLen, aArg1, aArg2, aArg3);
        }
    }

    /**
     * Writes a character array command into the buffer, intended to support
     * Graphics.drawChars(), this method also passes three additional
     * integer parameters.
     */
    final void writeChars(int aHandle, int aCommand, char[] aChars, int aOff,
                          int aLen, int aArg1, int aArg2, int aArg3)
    {
        if (aLen < 1)
        {
            return;
        }

        //
        // cmd, x, y, anchor, len, char[0..len-1]
        //
        final int cmdLen = 4 + ((aLen+1)>>1);

        if (cmdLen < BUFFER_SIZE)
        {
            if (iHandle <= 0)
            {
                throw new RuntimeException("Buffer closed");
            }

            if (aHandle != iCurrent)
            {
                deactivate();       // remove existing target
                activate(aHandle);  // set new target - this may cause a flush.
            }

            final int size = cmdLen + 1;
            final int opc  = (aCommand << 16) | size;
            ensureSpace(size);

            iBuffer[iCount++] = opc;
            iBuffer[iCount++] = aArg1;
            iBuffer[iCount++] = aArg2;
            iBuffer[iCount++] = aArg3;

            writeCharsInLine(aChars, aOff, aLen);
        }
        else
        {
            // Not enough space in the buffer
            throw new OutOfMemoryError();
            //
            // Possible alternatives:
            //
            // 1. Allocate an HBufC, write the chars into it, stick it in the buffer.
            //
            //      writeCharsOutOfLine();
            //
        }
    }

    /**
     *
     */
    private void writeCharsInLine(char[] aChars, int aOff, int aLen)
    {
        final int[] buffer = iBuffer;
        int count = iCount;

        buffer[count++] = aLen; // | CHAR_DATA_INLINE

        final int wend = aOff + (aLen & ~1);

        while (aOff < wend)
        {
            final int ch1 = aChars[aOff++];
            final int ch2 = aChars[aOff++];

            //
            // Pack two chars into each int.
            //
            // Assumes:
            //
            // ints are little endian.
            // char to int promotion doesn't sign extend
            //

            buffer[count++] = (ch2 << 16) | ch1;
        }

        if ((aLen & 1) == 1)
        {
            buffer[count++] = aChars[aOff];     // last char in first half-word
        }

        iCount = count;
    }

    /**
     * ensures there is at least aSize words of space remaining in the buffer,
     * and flushes the buffer if not.
     */
    final void ensureSpace(int aSize)
    {
        if ((iCount+aSize) > iBuffer.length)
        {
            pump();
            if (aSize > iBuffer.length)
            {
                throw new RuntimeException("Insufficient space in buffer for command: " + aSize);
            }
        }
    }

    /**
     * Flushes buffer and waits for native processing to complete.
     */
    final void sync()
    {
        synchronized (this)
        {
            if (iCount > 0)
            {
                deactivate();
                _flush(iHandle, iBuffer, iCount);
                iCount=0;
                iStart=0;
                iCurrent=-1;
            }
        }
    }

    /**
     *
     */
    private void deactivate()
    {
        if (-1 != iCurrent)
        {
            iBuffer[iStart] = iCount - iStart - 1;
            iCurrent = -1;
        }
    }

    /**
     *
     */
    private void activate(int aHandle)
    {
        if (0 != aHandle)
        {
            // flush if insufficient space for activation
            // command.
            // RECURSION NOTE: we do not use pump() here
            // to avoid any risk of recursion.
            if ((iCount + 2) > iBuffer.length)
            {
                sync();
            }

            iBuffer[ iCount++ ] = (aHandle | ACTIVATE);
            iCurrent = aHandle;
            iStart   = iCount;
            iBuffer[ iCount++ ] = 0;
        }
        else
        {
            throw new RuntimeException("Invalid handle " + aHandle);
        }
    }

    /**
     * Flush the current contents of the buffer and reset the current writer.
     */
    private void pump()
    {
        final int current = iCurrent;
        sync();
        activate(current);
    }

    private native int  _open(int aEventServerHandle);
    private native void _flush(int aHandle, int[] aBuffer, int aCount);
    private native void _close(int aHandle);
}

