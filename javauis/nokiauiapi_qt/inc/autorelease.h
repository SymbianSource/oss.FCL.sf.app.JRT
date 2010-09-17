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

#ifndef NOKIAUIAUTORELEASE_H
#define NOKIAUIAUTORELEASE_H

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

#endif // NOKIAUIAUTORELEASE_H

