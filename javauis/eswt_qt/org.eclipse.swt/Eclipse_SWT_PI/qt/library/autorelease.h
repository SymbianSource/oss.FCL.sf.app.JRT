/*******************************************************************************
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - initial implementation
 *******************************************************************************/

#ifndef SWTAUTORELEASE_H
#define SWTAUTORELEASE_H

#include "swtapplication.h"
#include "jniutils.h"

namespace Java { namespace eSWT {

/**
 * Helper class for auto_ptr like resource management.
 * Allocate these objects on stack and let them release the resources
 * when the object goes out of scope.
 */
template<class T>
class AutoRelease
{
public:
    AutoRelease( T* aPointer, bool isArray = false )
        : mPointer( aPointer ), mArray( isArray )
        {
        }
    virtual ~AutoRelease()
        {
        release();
        }
    T* get()
        {
        return mPointer;
        }
    void reset(T* aPointer)
        {
        release();
        mPointer = aPointer;
        }
private:
    void release()
        {
        if( mArray )
            {
            delete[] mPointer;
            }
        else
            {
            delete mPointer;
            }
        mPointer = NULL;
        }
private:
    T* mPointer;
    bool mArray;
};


/**
 * Helper class to automatically call ReleaseStringChars for a Java
 * string chars object obtained from a call to GetStringChars.
 */
class AutoReleaseStringChars
{
public:
    AutoReleaseStringChars( JNIEnv* aJniEnv, jstring aString, const jchar* aPointer )
        : mJniEnv( aJniEnv ), mString( aString ), mPointer( aPointer )
        {}
    virtual ~AutoReleaseStringChars()
        {
        mJniEnv->ReleaseStringChars( mString, mPointer );
        }
protected:
    JNIEnv*      mJniEnv;
    jstring      mString;
    const jchar* mPointer;
};


/**
 * Helper class to automatically call DeleteLocalRef to free the
 * given Java class reference.
 */
class AutoDeleteLocalClassRef
{
public:
    AutoDeleteLocalClassRef( JNIEnv* aJniEnv, jclass aClazz )
        : mJniEnv( aJniEnv ), mClazz( aClazz )
        {}
    virtual ~AutoDeleteLocalClassRef()
        {
        mJniEnv->DeleteLocalRef( mClazz );
        }
protected:
    JNIEnv* mJniEnv;
    jclass mClazz;
};

/**
 * Helper class to automatically free ExecExceptionHandlers.
 */
class AutoPopExecStack
{
public:
    AutoPopExecStack(QObject* aObject)
        {
        swtApp->jniUtils().enterExec(aObject);
        }
    virtual ~AutoPopExecStack()
        {
        swtApp->jniUtils().leaveExec();
        }
};

}}

#endif // SWTAUTORELEASE_H

