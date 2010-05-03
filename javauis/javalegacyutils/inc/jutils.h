/*
* Copyright (c) 1999-2004 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef JUTILS_H
#define JUTILS_H

#include <e32base.h>
#include <badesca.h>
#include "jni.h"

//----------------------------------------------------------------------------
// RJString takes a Java JNI string and converts it to an
// Epoc string. It retains the JNI environment and the string
// in order to release the string resources during destruction
class RJString : public TPtrC16
{
public:
    IMPORT_C RJString(JNIEnv& aJni, jstring aString);
    IMPORT_C ~RJString();

private:
    // Prevent accidental copying because of the shared underlying Java
    // string
    RJString(const RJString&);
    RJString& operator=(const RJString&);

private:
    JNIEnv& iJni;
    jstring iString;
};

//----------------------------------------------------------------------------

IMPORT_C jstring CreateJavaString(JNIEnv& aJni, const TDesC16& aString);

void AddToJavaStringArrayL(JNIEnv& aJni, jobjectArray& aContainer, TInt aPosition, const TDesC& aString);

IMPORT_C jobjectArray CopyToNewJavaStringArrayL(JNIEnv& aJni, const CDesCArray& aNativeArray);



class RJArray
{
public:
    RJArray(JNIEnv& aJni);
    ~RJArray();
    void* GetPrimitiveArrayCriticalLC(jarray aJavaArray, TBool aMutable = EFalse);
    static void CleanupArrayAccess(TAny* aRJArray);

private:
    void ReleasePrimitiveArrayCritical();

private:
    JNIEnv& iJni;
    jarray iJavaArray;
    TUint8* iArrayPtr;
    TBool iMutable;
};



class ArrayUtils
{
public:
    static TInt CopyToNative(JNIEnv& aJni, jbyteArray aJavaBuffer,
                             TInt aOffset, TInt aLength, TDes8& aNativeBuffer);
    IMPORT_C static TInt CopyToJava(JNIEnv& aJni, const TDesC8& aNativeBuffer,
                                    jbyteArray aJavaBuffer, TInt aOffset, TInt aLength);
    static jobjectArray CopyToNewJavaStringArray(JNIEnv& aJni,
            const RPointerArray<HBufC>& aNativeArray);

};



//----------------------------------------------------------------------------
// Constants that define the date/time '00:00, 1 Jan 1970' when used to create a TTime object
const TUint JavaUpperTimeFor1970 = 14474675;
const TUint JavaLowerTimeFor1970 = 254771200;

//----------------------------------------------------------------------------
// Used for converting between a Java jlong value and an Epoc
// TTime and vice-versa. The jlong represents the number of
// milliseconds since 00:00 1st Jan 1970.
class JavaEpocTime
{
public:
    IMPORT_C static TTime CreateEpocTTime(jlong aJavaTime);
    IMPORT_C static jlong CreateJavaTime(TTime aEpocTime);
};

//-----------------------------------------------------------------------------
// Creating integer 'handles' from C++ objects for referencing them inside Java
// The shift garauntees a positive integer, so object creation native methods
// can overload the return value to be a handle or an error code
//
// Unhanding the integer requires the destination type to be known, so is
// implemented as a template function, it should be invoked as
//
//      CXyz* xyz=JavaUnhand(aHandle);
//
const TInt KJavaHandleShift=2;

inline TInt JavaMakeHandle(const TAny* aObject)
{
    return reinterpret_cast<TUint>(aObject)>>KJavaHandleShift;
}

template <typename T>
inline T* JavaUnhand(TInt aHandle)
{
    return reinterpret_cast<T*>(aHandle<<KJavaHandleShift);
}


//-----------------------------------------------------------------------------
// Provide the handle/unhand pattern for CBase derived classes, to gain the
// Handle(), Unhand(), New() and TConstructor members, you should derive from
// this class using the following pattern:
//
//  class CXyz : public CJavaPeer<CXyz>
//      {...};
//
// The TConstructor member allows for more complex factory functions,
// providing automatic use of the cleanup stack. Supposing the derived
// class has a second phase constructor ConstructL(), the factory function
// could be:
//
//  TInt CXyz::New(...)
//      {
//      TRAPD(h,TConstructor c;c->ConstructL(...);h=c.GetHandle();)
//      return h;
//      }
//
template <class T>
class CJavaPeer : public CBase
{
protected:
    class TConstructor
    {
    public:
        inline TConstructor(T* aObject)
                :iObject(aObject)
        {
            CleanupStack::PushL(aObject);
        }
        inline TConstructor()
                :iObject(new(ELeave) T)
        {
            CleanupStack::PushL(iObject);
        }
        inline T* operator->() const
        {
            return static_cast<T*>(iObject);
        }
        inline TInt GetHandle()
        {
            CleanupStack::Pop();
            return JavaMakeHandle(iObject);
        }
    private:
        CBase* iObject;
    };
public:
    inline TInt Handle() const
    {
        return JavaMakeHandle(this);
    }
    inline static T& Unhand(TInt aHandle)
    {
        return *JavaUnhand<T>(aHandle);
    }
    static TInt New()
    {
        T* self=new T;
        return self ? self->Handle() : KErrNoMemory;
    }
};

#endif // JUTILS_H
