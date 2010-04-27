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
* Description:  Definition of various helper functions used for
*                serialization.
*
*/


#ifndef ARRAYUTILS_H
#define ARRAYUTILS_H

// INCLUDE FILES

#include <e32std.h>
#include <s32strm.h>

/////////////////////////////////////////////////////////////////////
// RPointerArray utilities
/////////////////////////////////////////////////////////////////////

template <class T>
void CopyPointerArrayL(RPointerArray<T>& aNew,
                       const RPointerArray<T>& aOld);

template <class T>
void ExternalizePointerArrayL(const RPointerArray<T>& aArray,
                              RWriteStream& aStream);

template <class T>
void InternalizePointerArrayL(RPointerArray<T>& aArray,
                              RReadStream& aStream);

template <class T>
TInt PointerArraySizeForStreamingL(const RPointerArray<T>& aArray);

/////////////////////////////////////////////////////////////////////
// RArray utilities
/////////////////////////////////////////////////////////////////////

template <class T>
void CopyArrayL(RArray<T>& aNew, const RArray<T>& aOld);

template <class T>
void ExternalizeArrayL(const RArray<T>& aArray, RWriteStream& aStream);

template <class T>
void InternalizeArrayL(RArray<T>& aArray, RReadStream& aStream);

template <class T>
TInt ArraySizeForStreamingL(const RArray<T>& aArray);

/////////////////////////////////////////////////////////////////////
// Traits classes - internally required by RPointerArray functions
/////////////////////////////////////////////////////////////////////

template <class T>
class TTraits
{
public:
    static T* CopyLC(const T& aOther)
    {
        return T::NewLC(aOther);
    }

    static T* ReadFromStreamLC(RReadStream& aStream)
    {
        return T::NewLC(aStream);
    }

    static void WriteToStreamL(const T& aItem,
                               RWriteStream& aStream)
    {
        aStream << aItem;
    }

    static TInt SizeForStreamingL(const T& aItem)
    {
        // Note: every possible 'T' must implement SizeForStreamingL method
        // in order for the next line to work.
        return aItem.SizeForStreamingL();
    }
};

// Specialisation for HBufs
template <>
class TTraits<HBufC16>
{
public:
    static HBufC16* CopyLC(const HBufC16& aOther)
    {
        return aOther.AllocLC();
    }

    static HBufC16* ReadFromStreamLC(RReadStream& aStream)
    {
        return HBufC16::NewLC(aStream, KMaxTInt);
    }

    static void WriteToStreamL(const HBufC16& aItem, RWriteStream& aStream)
    {
        aStream << aItem;
    }

    static TInt SizeForStreamingL(const HBufC16& aItem)
    {
        // Streaming a descriptor works as follows: first write out the length
        // of the descriptor. The size of this integer may vary depending on
        // the size of data stored, but it's at most == sizeof(TInt32).
        // Second, storing a Unicode string requires at most double space
        // compared to the 8-bit version, hence the multiplier of 2. Note that
        // a Unicode string may be compressed, too, (at least it's always
        // tried to be compressed), thus a '2*itemlength' seems like a valid
        // estimate for the size.
        return sizeof(TInt32) + 2 * aItem.Length();
    }
};

template <>
class TTraits<HBufC8>
{
public:
    static HBufC8* CopyLC(const HBufC8& aOther)
    {
        return aOther.AllocLC();
    }

    static HBufC8* ReadFromStreamLC(RReadStream& aStream)
    {
        return HBufC8::NewLC(aStream, KMaxTInt);
    }

    static void WriteToStreamL(const HBufC8& aItem, RWriteStream& aStream)
    {
        aStream << aItem;
    }

    static TInt SizeForStreamingL(const HBufC8& aItem)
    {
        // Streaming a descriptor works as follows: first write out the length
        // of the descriptor. The size of this integer may vary depending on
        // the size of data stored, but it's at most == sizeof(TInt32).
        // Second, storing an 8-bit string requires exactly the same size of
        // the length of the string.
        return sizeof(TInt32) + aItem.Length();
    }
};

#include "ArrayUtils.inl"

#endif // ARRAYUTILS_H
