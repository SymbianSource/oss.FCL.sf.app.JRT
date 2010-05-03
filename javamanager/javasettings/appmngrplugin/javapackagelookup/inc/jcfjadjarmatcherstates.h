/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Matches JAD and JAR attributes.
*
*/



#ifndef JCFJADJARMATCHERSTATES_H
#define JCFJADJARMATCHERSTATES_H

#include <e32base.h>
#include <e32def.h>
#include <e32std.h>
#include <mtclreg.h>

#include "jcfjadjarmatcherobservable.h"
#include "mjavaattribute.h"

enum TStates
{
    EBegin = 0,         // 0
    EOpenSession,       // 1
    EOpenMtmRegistry,   // 2
    EOpenMtmClient,     // 3
    EOpenJarFile,       // 4
    EOpenJadFile,       // 5
    EScanJarFile,       // 6
    EScanJadFile,       // 7
    EEntrySelect,       // 8
    EScanEntries,       // 9
    EScanAttchms,       // 10
    ESeekRootEntries,   // 11
    ESeekEntries,       // 12
    EScanStores,        // 13
    EEnd                // 14
};

enum TMode
{
    EUseNames,
    EUseHandles
};


/**
* Base class for scanner classes.
*  Contains common funcionality of scanner classes
*
*  @lib JcfJadJarMatcher.lib
*  @since S60 2.0
*/
class CJcfJadJarMatcherState :
        public CActive, public MJcfJadJarMatcherObservable
{
public:
    /**
     * To construct new CJcfJadJarMatcherState.
     *
     * Preconditions. The following conditions must be true prior to calling
     * this method; otherwise, its operation is undefined.
     * - None.
     *
     * Postconditions. The following condition is true immediately after
     * returning from this method.
     * - None.
     *
     * @param aObs - an observer of this class.
     */
    CJcfJadJarMatcherState(MJcfJadJarMatcherObserver* aObs);

    /**
     * To destruct CJcfJadJarMatcherState.
     *
     * Preconditions. The following conditions must be true prior to calling
     * this method; otherwise, its operation is undefined.
     * - None.
     *
     * Postconditions. The following condition is true immediately after
     * returning from this method.
     * - None.
     *
     */
    virtual ~CJcfJadJarMatcherState();

    /**
     * To execute this state.
     *
     * Preconditions. The following conditions must be true prior to calling
     * this method; otherwise, its operation is undefined.
     * - None.
     *
     * Postconditions. The following condition is true immediately after
     * returning from this method.
     * - None.
     *
     * @param aJarName - a name of the file.
     * @param aDirName - a name of the directory to be scanned.
     * @param aJadNamePtr - a pointer to the output descriptor.
     */
    virtual void ExecuteL(const TDesC& aJarName,
                          const TDesC& aDirName,
                          TDes* aJadNamePtr) = 0;

protected:
    /**
     * To handle errors.
     *
     * Preconditions. The following conditions must be true prior to calling
     * this method; otherwise, its operation is undefined.
     * - None.
     *
     * Postconditions. The following condition is true immediately after
     * returning from this method.
     * - None.
     *
     * @param aError - The reason of error.
     * @return KErrNone.
     */
    virtual TInt RunError(TInt aError);

    /**
     * To do clean-up for next round.
     *
     * Preconditions. The following conditions must be true prior to calling
     * this method; otherwise, its operation is undefined.
     * - None.
     *
     * Postconditions. The following condition is true immediately after
     * returning from this method.
     * - iState == EBegin.
     *
     */
    virtual void Cleanup() = 0;

    /**
    * To check whether JAD and JAR files match or not.
    *
    * Definition. Operation returns true if following statements are all true:
    *
    * 1) The version of JAD is same as the version of JAR.
    * 2) The vendor of JAD is same as the vendor of JAR.
    * 3) The name of JAD is same as the name of JAR.
    * 4) Suite has at least one MIDlet.
    *
    * Preconditions. The following conditions must be true prior to calling
    * this method; otherwise, its operation is undefined.
    * - None.
    *
    * Postconditions. The following condition is true immediately after
    * returning from this method.
    * - None.
    *
    */
    TBool Match(const RPointerArray<Java::MJavaAttribute>& aJadAttributes,
                const RPointerArray<Java::MJavaAttribute>& aJarAttributes);

    /**
     * Check attributes with given name equals.
     *
     * @param aJadAttributes JAD attributes to look for.
     * @param aJarAttributes JAR attributes to look for.
     * @param aName Attribute name to checked.
     * @return true if attributes equal, false otherwise.
     */
    TBool AttributesEqual(
        const RPointerArray<Java::MJavaAttribute>& aJadAttributes,
        const RPointerArray<Java::MJavaAttribute>& aJarAttributes,
        const TDesC& aName) const;

    /**
     * Get attribute index. Attribute is searched by name.
     *
     * @param aAttributes where to search.
     * @param aName attribute name to look for.
     * @return attribute index, KErrNotFound if not found.
     */
    TInt AttributeIndex(
        const RPointerArray<Java::MJavaAttribute>& aAttributes,
        const TDesC& aName) const;

protected:
    TStates iState;
    TInt iValue;
};

#endif // JCFJADJARMATCHERSTATES_H
