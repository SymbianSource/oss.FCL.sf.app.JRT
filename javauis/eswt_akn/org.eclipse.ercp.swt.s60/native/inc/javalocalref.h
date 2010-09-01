/*******************************************************************************
 * Copyright (c) 2005, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/


#ifndef JAVALOCALREF_H
#define JAVALOCALREF_H



#include <e32base.h>
#include <jni.h>




/**
 * Helper class for dealing with Java local references
 *
 * The local reference held by the object will be deleted when the object is
 * itself destroyed or when its Close method is called.
 *
 * The purpose of this object is easing deletion of local references when the
 * code path is not straightforward (the function may exit from several places
 * or leaves may occur). In order to protect against leaves, PushToCleanupStackL()
 * ought ot be called.
 */
template<class T>
class RJavaLocalRef
{
public:
    inline RJavaLocalRef(JNIEnv* aJniEnv, T aLocalRef);

    inline bool operator!() const;
    inline operator T();

    inline void Close();
    inline void Release();

private:
    JNIEnv& iJniEnv;
    T       iLocalRef;
};



/**
 * Constructs an object holding a Java local reference.
 */
template<class T>
inline RJavaLocalRef<T>::RJavaLocalRef(JNIEnv* aJniEnv, T aLocalRef)
        : iJniEnv(*aJniEnv)
        , iLocalRef(aLocalRef)
{
    ASSERT(aJniEnv!=NULL);
}


template<class T>
inline bool RJavaLocalRef<T>::operator!() const
{
    return (iLocalRef==NULL);
}


/**
 * Returns the held local reference
 */
template<class T>
inline RJavaLocalRef<T>::operator T()
{
    return iLocalRef;
}


/**
 * Deletes and releases the held local reference.
 */
template<class T>
inline void RJavaLocalRef<T>::Close()
{
    iJniEnv.DeleteLocalRef(iLocalRef);
    iLocalRef = NULL;
}


/**
 * Releases the held local reference without deleting it.
 *
 * Subsequent calls to Close() will have no effect.
 */
template<class T>
inline void RJavaLocalRef<T>::Release()
{
    iLocalRef = NULL;
}


#endif // JAVALOCALREF_H



