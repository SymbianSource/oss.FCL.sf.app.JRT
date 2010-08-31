/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class for reading data from Java SourceStream to native side
*
*/


#ifndef CMMASOURCESTREAM_H
#define CMMASOURCESTREAM_H

//#include <jutils.h>
#include "mmafunctionserver.h"

class CMMASourceStreamEvent;
class MMMASourceStreamListener;
class MMMAEventPoster;

// CONSTANTS
const TInt KMMAReadAllData = -1;

//  CLASS DECLARATION
/**
*   Class for reading data from Java SourceStream to native side
*
*
*/
NONSHARABLE_CLASS(CMMASourceStream): public CBase
{
public:
    /**
     * Status of the source stream reader
     */
    enum TStatus
    {
        EReadAll = 1,
        ERead = 2,
        EMoreData = 3,
        ECompleted = 4,
        EClosed = 5
    };

public:     // Constructors and destructors
    /**
     * Object creation. Static method.
     *
     * @param aJNIEnv           handle to used jni environment
     * @param aEventPoster      handle to event poster
     * @param aJavaSourceStream handle to java SourceStream object
     * @param aListener         Handle to source stream listener
     *
     * @return CMMASourceStream object
     */
    static CMMASourceStream* NewL(JNIEnv* aJNIEnv,
                                  MMMAEventPoster* aEventPoster,
                                  jobject aJavaSourceStream,
                                  MMMASourceStreamListener* aListener);

    /**
     * The same as NewL with cleanup stack
     * @see NewL
     */
    static CMMASourceStream* NewLC(JNIEnv* aJNIEnv,
                                   MMMAEventPoster* aEventPoster,
                                   jobject aJavaSourceStream,
                                   MMMASourceStreamListener* aListener);

    /**
     * Deletes owned objects
     */
    ~CMMASourceStream(); // Destructor

protected:  // Constructors and destructors

    /**
     * Set variables
     * @param aEventPoster handle to event poster
     * @param aListener    handle to source stream listener
     */
    CMMASourceStream(MMMAEventPoster* aEventPoster,
                     MMMASourceStreamListener* aListener);

    /**
     * 2nd phase constructor
     *
     * @param aJNIEnv           used jni environment
     * @param aJavaSourceStream handle to java source stream object
     */
    void ConstructL(JNIEnv* aJNIEnv, jobject aJavaSourceStream);

public:     // New methods
    /**
     * Writes data to from java buffer to native side
     *
     * @param aData   data passed by Java side
     * @param aLength size of passed data
     * @param state of this class
     */
    virtual void WriteL(const TUint8* aData,
                        TInt aLength, TInt aState);
    /**
     * Read aLength bytes from Java source stream
     *
     * @param aLength size of read
     */
    void ReadL(TInt aLength);

    /**
     * Read all data from Java source stream, clears buffer
     */
    void ReadAllL();

    /**
     * Reset all data
     */
    virtual void ResetData();

    /**
     * Creates data buffer
     *
     * @param aBufferSize data buffer size
     */
    void CreateDataBufferL(TInt aBufferSize);


protected:  // New methods

    /**
     * Prepare whatever is necessary for reading
     */
    virtual void PrepareReadL();

protected:
    CMMASourceStreamEvent* iReadEvent;
    MMMAEventPoster* iEventPoster;
    MMMASourceStreamListener* iListener;

protected:
    /**
     * Buffer where data from java side will be read, Owned
     */
    HBufC8* iData;

    /**
     * number of bytes coming from java stream, KMMAReadAllData is whole stream
     */
    TInt iReadLength;

    /**
     * current position
     */
    TInt iBufferPosition;
};

#endif // CMMASOURCESTREAM_H
