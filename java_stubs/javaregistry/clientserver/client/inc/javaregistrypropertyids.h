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
* Description:  javaregistrypropertyids definition
*
*/

#include <e32base.h>

#ifndef JAVAREGISTRYPROPERTYIDS_H
#define JAVAREGISTRYPROPERTYIDS_H

namespace Java
{
namespace Manager
{
namespace Registry
{
/**
 * Common Id's of properties processed by
 * CWriteableJavaRegistryEntry class.
 *
 * @since S60 v3.2
 */
enum TEntryIDs
{
    EPropType = 0,
    EPropName,
    EPropCertificateChains,
    EPropDrive,
    EPropDrmContentId
};

/**
 * General package specific Id's of properties processed by
 * CWriteableJavaRegistryPackageEntry class.
 *
 * @since S60 v3.2
 */
enum TPackageEntryIDs
{
    EPropVersion = 100,
    EPropVendor,
    EPropUninstallable,
    EPropApplicationUids,
    EPropDownloadPlugin,
    EPropInstallPlugin,
    EPropSize,
    EPropSerializedAttributes
};

/**
 * Application specific Id's of properties processed by
 * CWriteableJavaRegistryApplicationEntry class.
 *
 * @since S60 v3.2
 */
enum TApplicationEntryIDs
{
    EPropStartable = 200,
    EPropVisible,
    EPropResident,
    EPropGroupName,
    EPropInstallPackage
};

/*
 * MIDP Registry ID ranges
 */
const TInt KMidletSuiteRangeStart = 300;
const TInt KMidletRangeStart = 400;

/*
 * OSGi Registry ID ranges
 */
const TInt KDPRangeStart = 500;
const TInt KOAMSuiteRangeStart = 600;
const TInt KOAMRangeStart = 700;
const TInt KERCPRangeStart = 800;

}// namespace Registry
}// namespace Manager
}// namespace Java


#endif // JAVAREGISTRYPROPERTYIDS_H
