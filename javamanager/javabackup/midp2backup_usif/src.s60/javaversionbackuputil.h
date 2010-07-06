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
* Description:  Header file for JavaVersionBackupUtil class
*
*/


#ifndef JAVAVERSIONBACKUPUTIL_H
#define JAVAVERSIONBACKUPUTIL_H

#include <e32base.h>
#include <driveinfo.h>
#include <javaenvinfo.h>

class RDesReadStream;
class RDesWriteStream;

namespace java
{
namespace backup
{
/**
 *  JavaVersionBackupUtil class is used for checking the java version
 *  during a B&R operation. This class writes the current java version
 *  while backing up and then checks whether the backed up version
 *  is compatible with the currnet version while doing a restore.
 *  If it is not compatible, then the backup process is gracefully exited.
 */

class JavaVersionBackupUtil
{

public:

    /**
     * This method writes the current java version into the SBE buffer.
     * This functions is called when a backup process is started.
     *
     * @param aBuffer the buffer which is sent by the SBE
     */
    static void WriteJavaVersionL(RDesWriteStream& aStream);

    /**
     * This method reads the java version from the stream. If there is no
     * Java version or if the Java version is not compatible with the
     * current version, then the restore process is exited gracefully.
     *
     * @param aStream an RDesReadStream on the buffer which is sent
     *        by the SBE
     * @param aDrive current drive to be backed up
     * @return status
     */
    static TUint CheckJavaVersionL(RDesReadStream& aStream, TDriveNumber aDrive, TBool versionCheck);

private:

    /**
     * This method is used to compare the current java version with the
     * one read from the stream.
     * This method is not used presently. Logic may be added in the future
     *
     * @param aVersion the java version read from the stream
     * @param aCurVersion the current java version
     */
    void CompareL(TVersion aVersion, TVersion aCurVersion);

};

} // namespace backup
} // namespace java

#endif // JAVAVERSIONBACKUPUTIL_H
