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
* Description:  Header file of the StringBuffer class
 *
*/


///////////////////////////////////////////////////////////////////////////////
//
// StringBuffer class is a simple helper utility which provides the following
// functions:
//
// * concatenate HBufC strings
// * concatenate HBufC strings with TInt
// * create java string (jstring) object from HBufC
// * create HBufC object from jstring java object
//
///////////////////////////////////////////////////////////////////////////////
#include <jni.h>
#include <e32cons.h>

#define KInitialSize 10

class StringBuffer
{
public:
    const static TInt KBUFMAX = 10;
    //Method to create a new StringBuffer object
    static StringBuffer* NewL();
    //Method to append an HBufC to the buffer
    void Append(HBufC* what);
    //Method to append a TDesC to the buffer
    void Append(const TDesC &what);
    //Method to append an integer-TInt to the buffer
    void Append(TInt aWhat);
    //Method to create a java string from the c++ buffer
    jstring ToJavaString(JNIEnv* aJNI);
    //Method to get the string object stored in the buffer
    HBufC* GetString();
    //Method to create a java string from an HBufC object
    static jstring CreateJavaString(JNIEnv* aJNI, const HBufC* aString);
    //Method to create an HBufC string from a java string object
    static HBufC* CreateHBufCFromJavaStringLC(JNIEnv* aJNI, jstring aString);
    //Destructor
    ~StringBuffer();
private:
    //Constructor
    StringBuffer();
    //method to construct the objects in buffer
    void ConstructL();
    //variable which stores the string in the buffer
    HBufC* String;
};
