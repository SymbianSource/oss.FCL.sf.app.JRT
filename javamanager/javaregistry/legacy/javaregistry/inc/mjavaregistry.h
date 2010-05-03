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
* Description:  Interface classes for legacy Java Registry API
*
*/


#ifndef MJAVAREGISTRY_H
#define MJAVAREGISTRY_H

// INCLUDE FILES

#include <e32base.h>

// FORWARD DECLARATIONS

class MJavaRegistrySuiteEntry;
class MJavaRegistryMIDletEntry;
class MInstalledAppsRegistry;
class MInstalledAppsRegistryEntry;
class TAppVersion;

// CLASS DECLARATIONS

/**
* This interface provides a wrapper layer around the InstalledAppsRegistry
* to allow for specific information regarding Java MIDlets to be retrieved
* from the registry.
*/
class MJavaRegistry
{
public:
    /**
    * Creates a JavaRegistry object
    *
    * @return    A pointer to a MJavaRegistry object
    */
    IMPORT_C static MJavaRegistry* CreateL();

    /** Releases the registry created by CreateL */
    virtual void Release() const = 0;

    /**
    * Returns an array of UIDs of the installed MIDlet suites.
    *
    * @param aSuiteUids    The returns UIDs of the installed suites.
    */
    virtual void InstalledSuiteUidsL(RArray<TUid>& aSuiteUids) const = 0;

    /**
    * Returns an array of UIDs of the installed MIDlets.
    *
    * @param aMIDletUids    The returns UIDs of the installed MIDlets.
    */
    virtual void InstalledMIDletUidsL(RArray<TUid>& aMIDletUids) const = 0;

    /**
    * Gets whether or not a UID for a suite exists.
    *
    * @param aSuiteUid The UID to be checked.
    * @return          Boolean indicating whether or not the suite exists.
    */
    virtual TBool SuiteEntryExistsL(TUid aSuiteUid) const = 0;

    /**
    * Given a UID, returns a corresponding suite entry.
    * @leave           KErrNotFound if entry is not found.
    *
    * @param aSuiteUid The UID of the suite to be retrieved.
    * @return          The returned suite entry.
    */
    virtual MJavaRegistrySuiteEntry* SuiteEntryL(TUid aSuiteUid) const = 0;

    /**
    * Given a suite name and vendor, returns a corresponding suite entry.
    * @leave               KErrNotFound if entry is not found.
    *
    * @param aSuiteName    The name of the suite to be retrieved.
    * @param aSuiteVendor    The vendor of the suite to be retrieved.
    * @return             The returned suite entry.
    */
    virtual MJavaRegistrySuiteEntry* SuiteEntryL(const TDesC& aSuiteName,
            const TDesC& aSuiteVendor) const = 0;

    /**
    * Given a UID, returns a corresponding midlet entry.
    * @leave              KErrNotFound if entry is not found.
    *
    * @param aMIDletUid   The UID of the midlet to be retrieved.
    * @return             The returned midlet entry.
    */
    virtual MJavaRegistryMIDletEntry* MIDletEntryL(TUid aMIDletUid) const = 0;

    /**
    * Gets the generic installed apps registry. Note that the calling
    * client is NOT passed ownership; therefore there is no requirement
    * to call release.
    *
    * @return    A MInstalledAppsRegistry object.
    */
    virtual const MInstalledAppsRegistry& InstalledAppsRegistryL() const = 0;

protected:
    /**
    * Destructor.
    */
    virtual ~MJavaRegistry() {}
};

/**
 * This interface provides access to a MIDlet suite entry in the registry.
 * Attributes of the suite may be retrieved through this interface.
 */
class MJavaRegistrySuiteEntry
{
public:
    /**
    * Releases the suite entry.
    */
    virtual void Release() const = 0;

    /**
    * Gets the UID of the suite.
    *
    * @return    The UID of the midlet suite.
    */
    virtual TUid UidL() const = 0;

    /**
    * Gets the suite name.
    *
    * @return    The name of the midlet suite.
    */
    virtual const TDesC& SuiteNameL() const = 0;

    /**
    * Gets the vendor name.
    *
    * @return    The name of the midlet suite's vendor.
    */
    virtual const TDesC& VendorL() const = 0;

    /**
    * Gets the version.
    *
    * @return    The version of the midlet suite.
    */
    virtual TAppVersion VersionL() const = 0;

    /**
    * Gets the full pathname of the installed JAD.
    *
    * @return    Path of midlet suite's JAD.
    */
    virtual const TDesC& JadPathL() const = 0;

    /**
    * Gets the URL of where the JAD was downloaded from.
    *
    * @return    JAD URL.
    */
    virtual const TDesC& JadUrlL() const = 0;

    /**
    * Gets the full pathname of the installed JAR.
    *
    * @return    Path of midlet suite's JAR.
     */
    virtual const TDesC& JarPathL() const = 0;

    /**
    * Gets the URL of where the JAR was downloaded from.
    *
    * @return    JAR URL.
    */
    virtual const TDesC& JarUrlL() const = 0;

    /**
    * Returns an array of midlet UIDs associated with this suite.
    *
    * @param aMIDletUids    The UIDs of the MIDlets associated with this suite.
    */
    virtual void MIDletUidsL(RArray<TUid>& aMIDletUids) const = 0;

    /**
    * Gets the number of MIDlets in this suite.
    *
    * @return    Number of midlets in suite.
    */
    virtual TInt NumberOfMIDletsL() const = 0;

    /**
    * Given a UID, returns a corresponding midlet entry.
    * @leave              KErrNotFound if entry is not found.
    *
    * @param aMIDletUid   The UID of the midlet to be retrieved.
    * @return             The returned midlet entry.
    */
    virtual MJavaRegistryMIDletEntry* MIDletByUidL(TUid aMIDletUid) const = 0;

    /**
    * Given a midlet number, returns a corresponding midlet entry.
    * @leave              KErrNotFound if entry is not found.
    *
    * @param aMIDletNum   The midlet number to be retrieved (NOTE: Starting from
    *                     1, ie 1..number of midlets).
    * @return             Midlet entry.
    */
    virtual MJavaRegistryMIDletEntry* MIDletByNumberL(TInt aMIDletNum)
    const = 0;

    /**
    * Gets the generic installed apps registry entry associated with this
    * MIDlet suite. Note that the caller client is NOT passed ownership;
    * therefore there is no requirement to call release.
    *
    * @return             A MInstalledAppsRegistryEntry object.
    */
    virtual const MInstalledAppsRegistryEntry& InstalledAppsEntryL() const = 0;

protected:
    /**
    * Destructor.
    */
    virtual ~MJavaRegistrySuiteEntry() {}
};

/**
 * This interface provides access to a MIDlet entry. Through this MIDlet
 * entry, the client may retrieve the parent suite which can then provide
 * access to the suite's attributes.
 */
class MJavaRegistryMIDletEntry
{
public:
    /**
    * Releases the MIDlet entry.
    */
    virtual void Release() const = 0;

    /**
    * Gets the UID of this MIDlet.
    *
    * @return    UID of the MIDlet.
    */
    virtual TUid UidL() const = 0;

    /**
    * Gets the name of this MIDlet.
    *
    * @return    Name of the MIDlet.
    */
    virtual const TDesC& MIDletNameL() const = 0;

    /**
    * Gets a reference to the suite entry which parents this MIDlet
    *
    * @return    Parent MIDlet suite entry.
    */
    virtual const MJavaRegistrySuiteEntry& SuiteEntryL() const = 0;

    /**
    * Gets the size of the MIDlet.
    */
    virtual TInt SizeL() const = 0;

protected:
    /**
    * Destructor.
    */
    virtual ~MJavaRegistryMIDletEntry() {}
};

#endif // MJAVAREGISTRY_H
