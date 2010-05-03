/*
* Copyright (c) 1999-2002 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef RMIDARRAY_H
#define RMIDARRAY_H

#include <e32std.h>

const TInt KRMIDArrayIndexNegative=-1;
const TInt KRMIDArrayIndexGreaterThanSize=-2;

template <class T>
class RMIDArray : public RArrayBase
{
public:
    inline RMIDArray();
    inline RMIDArray(TInt aGranularity);
    inline void Close();
    inline TInt Count() const;
    inline const T& operator[](TInt anIndex) const;
    inline T& operator[](TInt anIndex);
    inline void Remove(TInt anIndex);
    inline void Compress();
    inline void Reset();
    inline TInt Find(const T& anEntry) const;
    //
    void Delete(TInt aIndex);
    void DeleteAll();
    void InsertL(const T& anEntry, TInt aPos);
    void AppendL(const T& anEntry);
};


template <class T>
inline RMIDArray<T>::RMIDArray()
        : RArrayBase(sizeof(T))
{}
template <class T>
inline RMIDArray<T>::RMIDArray(TInt aGranularity)
        : RArrayBase(sizeof(T),aGranularity)
{}
template <class T>
inline void RMIDArray<T>::Close()
{
    RArrayBase::Close();
}
template <class T>
inline TInt RMIDArray<T>::Count() const
{
    return RArrayBase::Count();
}
template <class T>
inline const T& RMIDArray<T>::operator[](TInt anIndex) const
{
    return *(const T*)At(anIndex);
}
template <class T>
inline T& RMIDArray<T>::operator[](TInt anIndex)
{
    return *(T*)At(anIndex);
}
template <class T>
inline void RMIDArray<T>::Remove(TInt anIndex)
{
    RArrayBase::Remove(anIndex);
}
template <class T>
inline void RMIDArray<T>::Compress()
{
    RArrayBase::Compress();
}
template <class T>
inline void RMIDArray<T>::Reset()
{
    RArrayBase::Reset();
}
template <class T>
inline TInt RMIDArray<T>::Find(const T& anEntry) const
{
    return RArrayBase::Find(&anEntry);
}


/**
 *Delete deletes the actual object and so can only be used if we are
 *storing pointers in the array
 */
template <class T>
void RMIDArray<T>::Delete(TInt aIndex)
{
    T pointer = *(T*)At(aIndex);
    Remove(aIndex);
    delete pointer;
}

template <class T>
void RMIDArray<T>::InsertL(const T& anEntry, TInt aPos)
{
    User::LeaveIfError(RArrayBase::Insert(&anEntry,aPos));
}

template <class T>
void RMIDArray<T>::AppendL(const T& anEntry)
{
    User::LeaveIfError(RArrayBase::Append(&anEntry));
}

/**
 *Only use this if the RMIDArray stores pointers
 */
template <class T>
void RMIDArray<T>::DeleteAll()
{
    TInt c=Count();
    TInt i;
    for (i=c-1; i>=0; --i)
    {
        Delete(i);
    }
    Reset();
}

#endif // RMIDARRAY_H
