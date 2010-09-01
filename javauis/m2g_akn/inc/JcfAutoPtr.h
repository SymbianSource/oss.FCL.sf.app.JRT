/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef JCFAUTOPTR_H
#define JCFAUTOPTR_H

#include <e32base.h>

/**
 * Functionality of this auto ptr class is same as stl's auto_ptr class
 * except this works with cleanup stack.
 * Implementation of the example operation with "traditional" way:
 * void bar()
 *   {
 *   CFoo* myFoo = CFoo::NewL();
 *   CleanupStack::PushL(myFoo);
 *   myFoo->DoSomeThingL();
 *   CleanupStack::PopAndDestroy(1); //myFoo
 *   }
 * Implementation of the same function with auto pointer class:
 * void bar()
 *   {
 *   jcfcommon::auto_ptr<CFoo> myFoo(CFoo::NewL());
 *   myFoo->DoSomeThingL();
 *   }
 *
 */
namespace jcfcommon
{

template<class X> class auto_ptr
{
public:
    typedef X element_type;

    auto_ptr(X* aPtr = 0): iPtr(aPtr),iCloseCalledFlag(EFalse)
    {
        CleanupStack::PushL(TCleanupItem(Close, (void*)this));
    }

    /**
     * Copy constructor was commented out because current
     * implementation doesn't support situation where auto_ptr
     * is returned as return value of the function. Problem is that
     * orig. auto_ptr is deleted when getBar() operation has been
     * executed and this deleted local reference has been stored to the
     * cleanup stack.
     * E.g:
     * void foo()
     *   {
     *   auto_ptr<bar> barObj = getBar();
     *       .
     *       .
     *       .
     *   User::Leave(KErrNotFound);
     *   }
     */
    /*auto_ptr(auto_ptr& aPtr): iPtr(aPtr.release())
    {
      CleanupStack::PushL(TCleanupItem(Close, (void*)this));
    }*/

    auto_ptr<X>& operator=(auto_ptr<X>& aRhs)
    {
        if (&aRhs != this)
        {
            delete iPtr;
            iPtr = aRhs.release();
        }
        return (*this);
    }

    ~auto_ptr()
    {
        if (!iCloseCalledFlag)
        {
            CleanupStack::Pop();
            delete iPtr;
        }
    }

    X& operator *() const
    {
        return *iPtr;
    }
    X* operator ->() const
    {
        return iPtr;
    }

    X* get() const
    {
        return iPtr;
    }

    X* release()
    {
        X* result = iPtr;
        iPtr = 0;
        return result;
    }

    void reset(X* aPtr = 0)
    {
        if (aPtr != iPtr)
        {
            delete iPtr;
            iPtr = aPtr;
        }
    }

private:
    static void Close(void* aPtr)
    {
        auto_ptr<X>* self = (auto_ptr<X>*)aPtr;
        delete self->iPtr;
        self->iPtr = 0;
        self->iCloseCalledFlag = ETrue;
    }

    //Not implemented.
    auto_ptr(auto_ptr& aPtr);

private:
    X* iPtr;
    TBool iCloseCalledFlag;
};

}//end jcfcommon

#endif // JCFAUTOPTR_H


