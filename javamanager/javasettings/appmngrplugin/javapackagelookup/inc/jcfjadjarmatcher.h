/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Public interface of JavaPackageLookup
*
*/


#ifndef JCFJADJARMATCHER_H
#define JCFJADJARMATCHER_H

#include <e32base.h>
#include <e32def.h>
#include <e32std.h>
#include <f32file.h>
#include "jcfjadjarmatcherobserver.h"
#include "jcfjadjarmatcherobservable.h"

class CJcfJadJarMatcherScanJadFiles;
class CJcfJadJarMatcherScanJarFiles;
class CJcfJadJarMatcherScanFromInbox;


/**
* Public interface of JavaPackageLookup.
*  Contains seek methods for matching JAD and JAR files.
*
*  @lib JcfJadJarMatcher.lib
*  @since S60 2.0
*/
class CJcfJadJarMatcher : public CBase, MJcfJadJarMatcherObservable, MJcfJadJarMatcherObserver
{
public:
    /**
     * To create a new CJcfJadJarMatcher.
     *
     * Preconditions. The following conditions must be true prior to calling
     * this method; otherwise, its operation is undefined.
     * - None.
     *
     * Postconditions. The following condition is true immediately after
     * returning from this method.
     * - None.
     *
     * @param aObs - an observer of this object.
     * @param RFs - a reference to the file session.
     * @return New matcher object.
     * @leave If creation fails.
     */
    static CJcfJadJarMatcher* NewL(MJcfJadJarMatcherObserver* aObs, RFs& aFileSession);

    /**
     * To find matching JAD file for given JAR file from public folders.
     *
     * Definition. This method seeks JAD files from given directory. If
     * matching JAD file is not found and aDir defines relative path (without
     * driver), method starts to seek the file from same directory in different
     * driver. All available drivers searched in descenting order: 'Y:' -> 'A:'
     * and finally 'Z:'.
     *
     * The JAD file 'jad' and the JAR file 'jar' is said to be matching iff
     *
     *     JcfJadJarMatcher.Match('jad', 'jar') == true
     *
     * Otherwise files are said to be unmatching.
     *
     * Preconditions. The following conditions must be true prior to calling
     * this method; otherwise, its operation is undefined.
     * - aJarFile should be a name of valid JAR file.
     * - aJadFile.Length() >= 256.
     *
     * Postconditions. The following condition is true immediately after
     * returning from this method.
     * - None.
     *
     * @param aJarFile - Name of the JAR file, whose matching JAD file is
     * searched.
     * @param aDir - Path indicating a directory to search on each drive.
     * @param aJadFile - Name of the matching JAD file, or Null.
     */
    void MatchJadL(const TDesC& aJarFile, const TDesC& aDir, TDes* aJadFile);

    /**
     * To find matching JAR file for given JAD file from public folders.
     *
     * Definition. This method seeks JAR files from given directory. If
     * matching JAR file is not found and aDir defines relative path (without
     * driver), method starts to seek the file from same directory in different
     * driver. All available drivers searched in descenting order: 'Y:' -> 'A:'
     * and finally 'Z:'.
     *
     * The JAD file 'jad' and the JAR file 'jar' is said to be matching iff
     *
     *     JcfJadJarMatcher.Match('jad', 'jar') == true
     *
     * Otherwise files are said to be unmatching.
     *
     * Preconditions. The following conditions must be true prior to calling
     * this method; otherwise, its operation is undefined.
     * - aJadFile should be a name of valid JAD file.
     * - aJarFile.Length() >= 256.
     *
     * Postconditions. The following condition is true immediately after
     * returning from this method.
     * - None.
     *
     * @param aJadFile - Name of the JAD file, whose matching JAR file is
     * searched.
     * @param aDir - Path indicating a directory to search on each drive.
     * @param aJarFile - Name of the matching JAR file, or Null.
     */
    void MatchJarL(const TDesC& aJadFile, const TDesC& aDir, TDes* aJarFile);


    /**
     * To destruct CJcfJadJarMatcher.
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
    ~CJcfJadJarMatcher();

    /**
     * This method is called whenever observed object needs to.
     *
     * Preconditions. The following conditions must be true prior to calling this
     * method; otherwise, its operation is undefined.
     * - None.
     *
     * Postconditions. The following condition is true immediately after returning
     * from this method.
     * - None.
     *
     * @param aStatus - a generic argument.
     */
    void Update(TInt aStatus);

private:
    /**
     * To construct new CJcfJadJarMatcher object.
     *
     * Preconditions. The following conditions must be true prior to calling
     * this method; otherwise, its operation is undefined.
     * - None.
     *
     * Postconditions. The following condition is true immediately after
     * returning from this method.
     * - None.
     *
     * @param aObs - an observer of this object.
     * @param RFs - a reference to the file session.
     */
    CJcfJadJarMatcher(MJcfJadJarMatcherObserver* aObs, RFs& aFileSession);

private:
    RFs& iFs;
    RFile iFile;
    CJcfJadJarMatcherScanJadFiles* iScanJad;
    CJcfJadJarMatcherScanJarFiles* iScanJar;
};

#endif // JCFJADJARMATCHER_H
