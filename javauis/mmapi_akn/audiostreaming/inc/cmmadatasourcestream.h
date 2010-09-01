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


#ifndef CMMADATASOURCESTREAM_H
#define CMMADATASOURCESTREAM_H

#include <jutils.h>
#include "cmmasourcestream.h"

class CMMADataSourceStreamEvent;
class MMMASourceStreamListener;
class MMMAEventPoster;
class CMMAStreamRequest;

// CONSTANTS

//  CLASS DECLARATION
/**
*   Class for reading data from Java SourceStream to native side
*
*/
NONSHARABLE_CLASS(CMMADataSourceStream): public CMMASourceStream
{
public:     // Constructors and destructors
    /**
     * Object creation. Static method.
     *
     * @param aJNIEnv           handle to used jni environment
     * @param aEventPoster      handle to event poster
     * @param aJavaSourceStream handle to java SourceStream object
     * @param aListener         Handle to source stream listener
     *
     * @return CMMADataSourceStream object
     */
    static CMMADataSourceStream* NewLC(JNIEnv* aJNIEnv,
    MMMAEventPoster* aEventPoster,
    jobject aJavaSourceStream,
    MMMASourceStreamListener* aListener);

    /**
     * Deletes owned objects
     */
    ~CMMADataSourceStream(); // Destructor

public: // New methods
    /**
     * Reads data from Java
     */
    void Read(CMMAStreamRequest* aRequest);

    /**
     * @return readed request
     */
    CMMAStreamRequest* Request();

public: // From CMMASourceStream

    /**
     * Overwrites CMMASourceStream's ResetData
     * so iData will not be NULL, does nothing
     */
    void ResetData();

protected:  // Constructors and destructors

    /**
     * Set variables
     * @param aEventPoster handle to event poster
     * @param aListener    handle to source stream listener
     */
    CMMADataSourceStream(MMMAEventPoster* aEventPoster,
                         MMMASourceStreamListener* aListener);

    /**
     * 2nd phase constructor
     *
     * @param aJNIEnv           used jni environment
     * @param aJavaSourceStream handle to java source stream object
     */
    void ConstructL(JNIEnv* aJNIEnv, jobject aJavaSourceStream);


protected:  // From CMMASourceStream

    void WriteL(const TUint8* aData,
                TInt aLength, TInt aState);

protected:
    // not owned request
    CMMAStreamRequest* iRequest;
};

#endif // CMMADATASOURCESTREAM_H
