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
* Description:  Class to handle OutputStream operations.
*
*/


#ifndef CMMAOUTPUTSTREAM_H
#define CMMAOUTPUTSTREAM_H

//  EXTERNAL INCLUDES
#include <jutils.h>
#include <s32mem.h>

//  FORWARD DECLARATIONS
class CMMAOutputStreamEvent;
class MMMAEventPoster;

//  CLASS DECLARATION
/**
*   Class to handle OutputStream operations.
*   MMF writes to the stream and Java-class reads the stream.
*
*
*/

NONSHARABLE_CLASS(CMMAOutputStream): public CBase
{
private:
    /**
     * Status code to be delivered to Java
     */
    enum TStatus
    {
        EMoreData = 1,
        ECompleted = 2
    };

public:     // Constructors and destructors
    /**
     * Object creation. Static method.
     *
     * @param aJNIEnv           handle to used jni environment
     * @param aEventPoster      handle to event poster
     * @param aJavaOutputStreamWriter handle to java Outputstream object
     *
     * @return CMMAOutputStream object
     */
    static CMMAOutputStream* NewL(JNIEnv* aJNIEnv,
                                  MMMAEventPoster* aEventPoster,
                                  jobject aJavaOutputStreamWriter);

    /**
     * The same as NewL with cleanup stack
     * @see NewL
     */
    static CMMAOutputStream* NewLC(JNIEnv* aJNIEnv,
                                   MMMAEventPoster* aEventPoster,
                                   jobject aJavaOutputStreamWriter);


    /**
     * Object creation. Static method.
     *
     * @param aOutputStream handle to used output stream
     * @param aJNIEnv       handle to used jni environment
     * @param aEventPoster  handle to event poster
     * @param aJavaOutputStreamWriter handle to java Outputstream object
     */
    static void CreateL(CMMAOutputStream** aOutputStream,
                        JNIEnv* aJNIEnv,
                        MMMAEventPoster* aEventPoster,
                        jobject aJavaOutputStreamWriter);

    /**
     * Deletes owned objects
     */
    ~CMMAOutputStream();    // Destructor

private:    // Constructors and destructors
    /**
     * Default constructor, initializes variables
     */
    CMMAOutputStream(MMMAEventPoster* aEventPoster);

    /**
     * Finishes construction
     *
     * @param aJNIEnv       handle to used jni environment
     * @param aJavaOutputStreamWriter handle to java Outputstream object
     */
    void ConstructL(JNIEnv* aJNIEnv,
                    jobject aJavaOutputStreamWriter);

public: // New methods
    /**
     * Read data from c++ to aOutputData (passed from java side)
     */
    virtual void ReadDataL(TUint8* aOutputData,
                           TInt* aBufferSize,
                           TInt* aReadStatus);

public: // New methods
    /**
     * Write data to a stream.
     */
    void WriteL(const TDesC8& aData);

    /**
     * Commit Java stream
     */
    void Commit();

private: // Data
    /**
     * Owned.
     * Event which notifies Java when data is available or read continues.
     */
    CMMAOutputStreamEvent* iWriteEvent;

    /**
     * Pointer to event source
     */
    MMMAEventPoster* iEventSource;

    /**
     * Ptr to iData member. Moved when data is read.
     */
    TPtr8 iPtr;

    /**
     * Owned.
     * Pointer to data to be written to Java side.
     */
    HBufC8* iData;
};


#endif // CMMAOUTPUTSTREAM_H
