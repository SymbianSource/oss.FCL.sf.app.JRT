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
* Description:
 *
*/


#ifndef STSCOMMON_H
#define STSCOMMON_H

// Enables pushing a call to ResetAndDestroy into cleanup stack

template<class T>
class CleanupResetAndDestroy
{
public:
    inline static void PushL(T& aRef);
private:
    static void ResetAndDestroy(TAny *aPtr);
};
template<class T>
inline void CleanupResetAndDestroyPushL(T& aRef);
template<class T>
inline void CleanupResetAndDestroy<T>::PushL(T& aRef)
{
    CleanupStack::PushL(TCleanupItem(&ResetAndDestroy, &aRef));
}
template<class T>
void CleanupResetAndDestroy<T>::ResetAndDestroy(TAny *aPtr)
{
    (STATIC_CAST(T*,aPtr))->ResetAndDestroy();
}
template<class T>
inline void CleanupResetAndDestroyPushL(T& aRef)
{
    CleanupResetAndDestroy<T>::PushL(aRef);
}

#endif // STSCOMMON_H
