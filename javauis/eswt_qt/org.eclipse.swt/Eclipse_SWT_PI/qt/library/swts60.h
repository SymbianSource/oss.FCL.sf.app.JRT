/*******************************************************************************
 * Copyright (c) 2009, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - initial implementation
 *******************************************************************************/

#ifndef _SWTS60_H_
#define _SWTS60_H_

#include <QS60MainApplication>
#include <jni.h>
#include <e32cmn.h>

class QObject;
class TWsEvent;
/*
 * Specialisation of a Symbian application framework class. 
 */
class SwtQtS60MainApplication : public QS60MainApplication
    {
public:
    /*
     * This is needed to construct the Symbian application with the correct 
     * application id. 
     */
    TUid AppDllUid() const;
    };

namespace Java { namespace eSWT {

/*
 * SWT specific Symbian utility class. 
 */
class SymbianUtils
    {
    public:

    /*
     * Obtains the UI thread and calls back the given object in it. Also 
     * creates the Symbian application framework in the thread. 
     * @param aJniEnv The JNI environment of the calling thread
     * @param aRunner The Java object to call back in the UI thread
     * @param aUid The id to use for the Symbian application. 
     * @return symbian error code
     */
    static int startUI(JNIEnv* aJniEnv, jobject aRunner, jint aUid);

    /*
     * Called to initialise the calling thread as the UI thread. startUI might
     * or might have not been called to start the thread calling this
     * function. 
     * @param aJniEnv The JNI environment of the calling thread
     * @param aUid The id to use for the Symbian application. 
     * @return symbian error code
     */
    static int initUiThread(JNIEnv* aJniEnv, const TInt& aUid);
    
    /*
     * Called to clean-up the UI resources allocated by a call to initUiThread. 
     */
    static void cleanupUiThread();
    
    /*
     * Event filter for intercepting Symbian window server events. 
     */
    static bool eventFilter(QObject* object, const TWsEvent* aEvent);
    
    /*
     * Event filter for intercepting Symbian Resource change events. 
     */
    static bool eventFilter(QObject* object, const TInt aSymbianType, TInt aSwtType);
        
    /*
     * Temporary workaround to set the application title to the window group. 
     * Qt doesn't set this in QCoreApplication::setApplicationName. 
     */
    static void setAppName(JNIEnv* aJniEnv, jstring aName);
    
    /*
     * Returns Current screen device number 
     */
    static TInt GetScreenDeviceNumber();
    
    /*
     * Returns Color depth of the screen 
     */
    static TInt GetColorDepth();
    
    /*
     * Returns Available hardware inputs 
     */
    static TInt GetHwInputs();
    
    /*
     * Symbian application factory. Qt uses to create the Symbian application 
     * framework. 
     */
    static CApaApplication* NewApplication();

private:

    static void notifyThreadInitStatus(const TInt& aStatus, TThreadId aInitThreadId, TRequestStatus* aStatusPtr);
    static TInt uiThreadEntryPoint(TAny* aTlsData);
    static TInt supportThreadEntryPoint(TAny* aParams);
    static TInt trappedSupportThreadEntryPoint(TAny* aParams);
    static void startSupportThread(TAny* aParams);
    static void notifyUIThreadExit(JNIEnv* aEnv);
};

}}

#endif /* _SWTS60_H_ */
