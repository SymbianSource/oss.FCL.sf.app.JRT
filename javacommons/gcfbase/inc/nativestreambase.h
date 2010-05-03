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
* Description:  Base class for all native implementation of stream types
*
*/


#ifndef NATIVESTREAMBASE_H
#define NATIVESTREAMBASE_H

#include <jni.h>

namespace java
{

/**
* This is the base class for Native Peer of streams.
* StreamConnectionBase in will make calls to the native side to get/send data.
* It will have stored a handle to the Object and uses it to call these methods.
*
* Usage:
* Please ensure that this class comes first in the line of inheritance as shown.
*
* @code
* class StreamConnectionX : public NativeStreamBase, public AllOtherClasses
* @endcode
*
*/
class NativeStreamBase
{
public:
    /**
    * ReadBytes is called once the buffer at the InputStream in Java runs
    * out of data.
    * How this function should be called.
    * @param[in]  aJavaBuffer: A byte array of length 512 bytes
    * @return return should be the number of bytes read. If error, then the
    *         value of errno should be negated and returned. (return -errno)
    */
    virtual int readBytes(JNIEnv& aJni, jbyteArray aJavaBuffer) = 0;

    /**
    * WriteBytes is called once buffer at the OutputStream in Java overflows.
    * It is also called if flush() is called at the Java side.
    * How this function should be called.
    * @param[in]  aJavaBuffer: A byte array of length aLength
    * @return return should be the number of bytes written. If error, then the
    *         value of errno should be negated and returned. (return -errno)
    */
    virtual int writeBytes(JNIEnv& aJni, jbyteArray aJavaBuffer,
                           int aOffset, int aLength) = 0;

    /**
    * StopReading is called once InputStream is closed from the Application.
    */
    virtual void stopReading() = 0;

    /**
    * StopWriting is called once OutputStream is closed from the Application.
    */
    virtual void stopWriting() = 0;

    /**
    * Defines the size of the buffer to be held on the native side. Should be
    * same as the buffer in the InputStream
    */
    int mBufferSize;

    /**
     * SetBufferSize is called during StreamConnectionBase construction.
     * This is to keep both Java and Native Buffers in sync.
     */
    void setBufferSize(int aSize)
    {
        mBufferSize = aSize;
    }

    virtual int available()
    {
        return 0;
    }
};

}  //end namespace java


#endif // NATIVESTREAMBASE_H
