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
* Description:  Cleanup Stack "reset and destroy" push operation.
 *
*/


#ifndef CLEANUPRESETANDDESTROY_H
#define CLEANUPRESETANDDESTROY_H

//  INCLUDE FILES
#include <e32base.h>

/**
 * An operation for pushing objects to cleanup stack with \c TCleanupItems
 * that will perform a \c ResetAndDestroy() operation on the pushed object
 * if a leave occurs.
 *
 * Note that the object itself will not be deleted.
 *
 * @par Example:
 * Here is an example of using \c CleanupResetAndDestroy with a dynamically
 * allocated \c RPointerArray. \c RPointerArray clears its contents with a
 * \cResetAndDestroy() operation.
 * @code
 *
 *  // A function which creates a pointer array with couple of initialized
 *  // CThings. The function must return a pointer to the pointer array,
 *  // because the array has to be allocated dynamically. CThing is some
 *  // simple CBase-derived class.
 *
 *  RPointerArray< CThing >* CreateThingArrayL( )
 *      {
 *      // Create an array of pointers to CThings with granularity of 4
 *
 *      RPointerArray< CThing >* things =
 *          new( ELeave ) RPointerArray< CThing >( 4 );
 *
 *      // Push pointer to the array to the cleanup stack; then push reference
 *      // to the array and a ResetAndDestroy operation to the cleanup stack.
 *
 *      // (Note that order of these operations matters: the ResetAndDestroy
 *      // operation must be performed before the array itself is deleted.)
 *
 *      CleanupStack::PushL( things );
 *      CleanupResetAndDestroyPushL( *things );
 *
 *      // Add couple of CThings with magic numbers to the array.
 *      // If any of the NewL() operations leaves, the array will be cleared
 *      // with ResetAndDestroy() and the array itself will destroyed.
 *
 *      User::LeaveIfError( things->Append( CThing::NewL( 7 ) ) );
 *      User::LeaveIfError( things->Append( CThing::NewL( 96 ) ) );
 *      User::LeaveIfError( things->Append( CThing::NewL( 999 ) ) );
 *
 *      // Pop the array reference with ResetAndDestroy from cleanup stack
 *      // then pop the pointer to the array itself.
 *
 *      CleanupStack::Pop(); // *things
 *      CleanupStack::Pop(); // things
 *
 *      // Now we're ready to return the results (a pointer to the array)
 *      return things;
 *      }
 *
 * @endcode
 */
template<class T>
inline void CleanupResetAndDestroyPushL(T& aRef);

/**
 *  <em>See \ref CleanupResetAndDestroyPushL() documentation.</em>
 */
template<class T>
class CleanupResetAndDestroy
{
public:
    inline static void PushL(T& aRef);

private:
    static void ResetAndDestroy(TAny *aPtr);
};

template<class T>
inline void CleanupResetAndDestroy<T>::PushL(T& aRef)
{
    CleanupStack::PushL(TCleanupItem(&ResetAndDestroy, &aRef));
}

template<class T>
void CleanupResetAndDestroy<T>::ResetAndDestroy(TAny *aPtr)
{
    static_cast<T*>(aPtr)->ResetAndDestroy();
}

template<class T>
inline void CleanupResetAndDestroyPushL(T& aRef)
{
    CleanupResetAndDestroy<T>::PushL(aRef);
}

#endif // CLEANUPRESETANDDESTROY_H
// End of File
