/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Delivers editor input actions to the Java side.
*
*/


#ifndef CMIDTEXTEDITOROBSERVER_H
#define CMIDTEXTEDITOROBSERVER_H

// EXTERNAL INCLUDES
#include <MMIDTextEditor.h>
#include <CMIDToolkit.h>
#include <jni.h>

// CLASS DESCRIPTION
/**
 * Delivers editor input actions to the Java side.
 */
NONSHARABLE_CLASS(CMIDTextEditorObserver) :  public MMIDTextEditorObserver
{
private: // Classes

    // CLASS DECLARATION
    /**
     * Encapsulates data posted from native to Java side.
     */
    NONSHARABLE_CLASS(CMIDTextEditorEvent) :
            public CJavaEvent< CMIDToolkit >
    {
public: // Constructor and destructor

        /**
         * C++ constructor.
         *
         * @param aPeer Java object to be notified.
         * @param aMethod Java function to be called.
         * @param aParam Parameter for the event.
         */
        CMIDTextEditorEvent(
            jobject aPeer,
            jmethodID aMethod,
            TUint aParam);

        /**
         * Destructor.
         */
        virtual ~CMIDTextEditorEvent();

private: // from CJavaEvent

        /**
         * Sends the event to the Java side.
         *
         * @param aJni JNI used for dispatching
         */
        void Dispatch(JNIEnv& aJni);

private: // Data

        // Peer object
        jobject iPeer;
        // Method identifier
        jmethodID iMethod;
        // Parameter
        TUint iParam;
    };

public: // Constructors and destructor

    /**
     * Two-phased constructor. Creates an instance from this class
     * The caller takes the ownership of the returned instance
     *
     * @return New instance from this class
     */
    static CMIDTextEditorObserver* NewL(
        CMIDToolkit& aToolkit,
        JNIEnv& aJniEnv,
        jobject aPeer,
        jmethodID aMethod);

    /**
     * Destructor.
     */
    virtual ~CMIDTextEditorObserver();

public: // From MMIDComponent

    /**
     * Invoked by the framework to clean up any  resources used by the
     * implementation.
     *
     * Typically, the implementation will do "delete this" but is clearly not
     * restricted to
     */
    void Dispose();

public: // From MMIDTextEditorObserver

    /**
     * Called when an input action occurs in the editor.
     *
     * Some events can be controlled by the observer. In this case the
     * return value is checked when the editor performs the operation.
     * Note that there are some actions that are performed regardless of
     * the return value of this method call.
     *
     * @param aActions contains the set of input actions that occured.
     */
    void NotifyInputAction(TUint aActions);

private: // Construtors

    /**
     * C++ constructor.
     */
    CMIDTextEditorObserver(
        CMIDToolkit& aToolkit,
        JNIEnv& aJniEnv,
        jobject aPeer,
        jmethodID aMethod);

private: // Data

    // MID toolkit.
    CMIDToolkit& iToolkit;
    // A valid reference to JNI environment
    JNIEnv& iJniEnv;
    // Peer object
    jobject iPeer;
    // Method identifier
    jmethodID iMethod;
};

#endif // CMIDTEXTEDITOROBSERVER_H

// End of file
