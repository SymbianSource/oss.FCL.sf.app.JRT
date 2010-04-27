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
* Description:  stringbuffer class implementation
 *
*/


#include "stringbuffer.h"

//////////////////////////////////////////////////////////////////////////////
//
// NewL Method to create a new StringBuffer object
// @return StringBuffer*
//
//////////////////////////////////////////////////////////////////////////////

StringBuffer* StringBuffer::NewL()
{
    StringBuffer* self = new(ELeave) StringBuffer;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

//////////////////////////////////////////////////////////////////////////////
//
// Constructor Method
//
//////////////////////////////////////////////////////////////////////////////
StringBuffer::StringBuffer()
{

}

//////////////////////////////////////////////////////////////////////////////
//
// ConstructL to create the String object which stores the data
//
//////////////////////////////////////////////////////////////////////////////
void StringBuffer::ConstructL()
{
    String = HBufC::NewL(KInitialSize);
}

//////////////////////////////////////////////////////////////////////////////
//
// AppendText Method to append the content of an HBufC object to the
// StringBuffer's content
// @param HBufC*
//
//////////////////////////////////////////////////////////////////////////////

void StringBuffer::Append(HBufC* what)
{
    TInt _size = what->Length();
    if ((String->Des().MaxLength() - String->Size()) < _size)
    {
        String = String->ReAlloc(String->Des().MaxLength() + 2 * _size);
    }
    String->Des().Append(what->Des());
}

//////////////////////////////////////////////////////////////////////////////
//
// GetString Method to get the content of the StringBuffer
// @return HBufC *
//
//////////////////////////////////////////////////////////////////////////////

HBufC* StringBuffer::GetString()
{
    return String;
}
//////////////////////////////////////////////////////////////////////////////
//
// Append Method to append the content of a TDesC object to the
//  StringBuffer's content
// @param TDesC
//
//////////////////////////////////////////////////////////////////////////////

void StringBuffer::Append(const TDesC &what)
{
    TInt _size = what.Length();
    if ((String->Des().MaxLength() - String->Size()) < _size)
    {
        String = String->ReAlloc(String->Des().MaxLength() + 2 * _size);
    }
    String->Des().Append(what);
}

//////////////////////////////////////////////////////////////////////////////
//
// Append Method to append a TInt object to the StringBuffer's content
// @param TInt
//
//////////////////////////////////////////////////////////////////////////////
void StringBuffer::Append(TInt aWhat)
{
    TBufC<KBUFMAX> Temp;
    Temp.Des().AppendNum(aWhat);
    TInt _size = Temp.Des().Length();
    if ((String->Des().MaxLength() - String->Size()) < _size)
    {
        String = String->ReAlloc(String->Des().MaxLength() + 2 * _size);
    }
    String->Des().Append(Temp);
}
//////////////////////////////////////////////////////////////////////////////
//
// CreateJavaString Method to create java string from an HBufC
// @param aJNI JNIEnv*
// @param aString HBufC
// @return jstring
//
//////////////////////////////////////////////////////////////////////////////

jstring StringBuffer::CreateJavaString(JNIEnv* aJNI, const HBufC* aString)
{
    TPtrC16 buf;
    jstring str = aJNI->NewString(aString->Ptr(), aString->Length());
    return str;
}
//////////////////////////////////////////////////////////////////////////////
//
// CreateHBufCFromJavaStringLC Method to cretae HBufC from a java string
// @param JNIEnv*
// @param jstring aString
// @return HBufC*
//
//////////////////////////////////////////////////////////////////////////////

HBufC* StringBuffer::CreateHBufCFromJavaStringLC(JNIEnv* aJNI, jstring aString)
{
    HBufC16* Str = HBufC16::NewL(aJNI->GetStringLength(aString));
    CleanupStack::PushL(Str);
    const TUint16* temp = aJNI->GetStringChars(aString, 0);
    Str->Des() = temp;
    aJNI->ReleaseStringChars(aString, temp);
    return Str;
}

//////////////////////////////////////////////////////////////////////////////
//
// ToJavaString Method to create java string from the content of the
// StringBuffer.
// @param JNIEnv
// @return jstring
//
//////////////////////////////////////////////////////////////////////////////

jstring StringBuffer::ToJavaString(JNIEnv* aJNI)
{
    jstring str = aJNI->NewString(String->Ptr(), String->Length());
    return str;
}

//////////////////////////////////////////////////////////////////////////////
//
// Destructor
//
//////////////////////////////////////////////////////////////////////////////

StringBuffer::~StringBuffer()
{
    delete String;
    String = NULL;
}
