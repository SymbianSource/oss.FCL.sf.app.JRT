/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of various helper functions used for
*                serialization.
*
*/


/////////////////////////////////////////////////////////////////////
// RPointerArray utilities
/////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// CopyPointerArrayL
// -----------------------------------------------------------------------------
//
template <class T>
void CopyPointerArrayL(RPointerArray<T>& aNew, const RPointerArray<T>& aOld)
{
    for (TInt i = 0; i < aOld.Count(); ++i)
    {
        T* item = TTraits<T>::CopyLC(*aOld[i]);
        User::LeaveIfError(aNew.Append(item));
        CleanupStack::Pop(item);
    }
}

// -----------------------------------------------------------------------------
// ExternalizePointerArrayL
// -----------------------------------------------------------------------------
//
template <class T>
void ExternalizePointerArrayL(const RPointerArray<T>& aArray,
                              RWriteStream& aStream)
{
    aStream.WriteInt32L(aArray.Count());
    for (TInt i = 0; i < aArray.Count(); ++i)
    {
        TTraits<T>::WriteToStreamL(*aArray[i], aStream);
    }
}

// -----------------------------------------------------------------------------
// InternalizePointerArrayL
// -----------------------------------------------------------------------------
//
template <class T>
void InternalizePointerArrayL(RPointerArray<T>& aArray, RReadStream& aStream)
{
    TInt count = aStream.ReadInt32L();
    for (TInt i = 0; i < count; ++i)
    {
        T* item = TTraits<T>::ReadFromStreamLC(aStream);
        User::LeaveIfError(aArray.Append(item));
        CleanupStack::Pop(item);
    }
}

// -----------------------------------------------------------------------------
// PointerArraySizeForStreamingL
// -----------------------------------------------------------------------------
//
template <class T>
TInt PointerArraySizeForStreamingL(const RPointerArray<T>& aArray)
{
    TInt size = sizeof(TInt32);
    for (TInt i = aArray.Count(); 0 < i;)
    {
        size += TTraits<T>::SizeForStreamingL(*aArray[--i]);
    }
    return size;
}

/////////////////////////////////////////////////////////////////////
// RArray utilities
/////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// CopyArrayL
// -----------------------------------------------------------------------------
//
template <class T>
void CopyArrayL(RArray<T>& aNew, const RArray<T>& aOld)
{
    for (TInt i = 0; i < aOld.Count(); ++i)
    {
        User::LeaveIfError(aNew.Append(aOld[i]));
    }
}

// -----------------------------------------------------------------------------
// ExternalizeArrayL
// -----------------------------------------------------------------------------
//
template <class T>
void ExternalizeArrayL(const RArray<T>& aArray, RWriteStream& aStream)
{
    aStream.WriteInt32L(aArray.Count());
    for (TInt i = 0; i < aArray.Count(); ++i)
    {
        aStream.WriteL(TPckgC<T>(aArray[i]));
    }
}

// -----------------------------------------------------------------------------
// InternalizeArrayL
// -----------------------------------------------------------------------------
//
template <class T>
void InternalizeArrayL(RArray<T>& aArray, RReadStream& aStream)
{
    TInt count = aStream.ReadInt32L();
    for (TInt i = 0; i < count; ++i)
    {
        T item;
        TPckg<T> itemPckg(item);
        aStream.ReadL(itemPckg);
        User::LeaveIfError(aArray.Append(item));
    }
}

// -----------------------------------------------------------------------------
// ArraySizeForStreamingL
// -----------------------------------------------------------------------------
//
template <class T>
TInt ArraySizeForStreamingL(const RArray<T>& aArray)
{
    return sizeof(TInt32) + aArray.Count() * sizeof(T);
}

