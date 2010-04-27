/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Definition of CInstalledAppsRegistryFiles class.
*
*/


#ifndef INSTALLEDAPPSREGISTRYFILES_H
#define INSTALLEDAPPSREGISTRYFILES_H

// INCLUDE FILES

#include <e32base.h>

// FORWARD DECLARATIONS

class RWriteStream;
class RReadStream;
class RFs;

/**
* This class is used by CInstalledAppsRegistryEntry and encapsulates
* all the file names contained in a package. To save memory and
* disk space, it is not stored directly as an array of filenames,
* but rather as an array of directories, and each directory containing
* an array of pathless filenames. This is so that directory paths
* are only stored once.
*/
class CInstalledAppsRegistryFiles : public CBase
{
public:
    // construction, destruction
    static CInstalledAppsRegistryFiles* NewL();
    static CInstalledAppsRegistryFiles* NewL
    (const CInstalledAppsRegistryFiles&  aOther);
    static CInstalledAppsRegistryFiles* NewL(RReadStream& aStream);
    virtual ~CInstalledAppsRegistryFiles();

    // For streaming/serialisation
    void ExternalizeL(RWriteStream& aStream) const;

    /**
    * Returns the number of bytes required be present in a stream wishing to
    * store names of files, directories and apparc identifiers contained by
    * this object.
    * @return The number of bytes that will be externalized to a stream.
    */
    TInt SizeForStreamingL() const;

public: // APIs reflected from MInstalledAppsRegistryEntry

    /**
    * Adds a file to the list of files.
    */
    void AddFileL(const TDesC& aFileName);

    /**
    * Appends all the files (and full paths) into the array.
    */
    void ListFilesL(RPointerArray<HBufC>& aFileArray) const;

    /**
    * Returns TRUE if this package uses the input file.
    */
    TBool UsesFileL(const TDesC& aFileName) const;

    /**
    * Returns a filename in the entry with a given extension.
    */
    void FindFileWithExtL(const TDesC&  aExtension,
                          TFileName& aFileName) const;

    /**
    * Returns the number of files added in this object.
    */
    TInt FileCount() const;

    /**
    * Adds a directory owned by the application.
    */
    void AddDirectoryL(const TDesC& aDirName);

    /**
    * Appends all the directories owned into the array.
    */
    void ListDirectoriesL(RPointerArray<HBufC>& aDirArray) const;

    /**
    * Adds an Apparc-identifier (Apparc-identifiers are used for uninstalling
    * Apparc registration files).
    */
    void AddApparcIdentifierL(const TDesC& aApparcIdentifier);

    /**
    * Appends into the array all the Apparc-identifiers (Apparc-identifiers
    * are used for uninstalling Apparc registration files).
    */
    void ListApparcIdentifiersL(RPointerArray<HBufC>&  aApparcIdentifiersArray) const;

    /**
    * Scans all the installed files and returns the total size.
    */
    TInt Size() const;

private:
    // Internal class storing pathnames and filenames under that path
    class CDirFiles : public CBase
    {
    public:
        // construction, destruction
        static CDirFiles*  NewLC(const TDesC& aDirName);
        static CDirFiles*  NewLC(const CDirFiles& aOther);
        static CDirFiles*  NewLC(RReadStream& aStream);
        ~CDirFiles();

        /**
        * Gets the directory name.
        */
        const TDesC&  DirectoryName() const;

        /**
        * Adds a new file hanging off a directory - must be a filename,
        * not a full path.
        */
        void AddFileL(const TDesC& aFileName);

        /**
        * Returns TRUE if the file exists in this directory.
        */
        TBool Exists(const TDesC& aFileName) const;

        /**
        * Appends all the files (including full path) into the input array.
        */
        void ListFilesL(RPointerArray<HBufC>& aFileArray) const;

        /**
        * Sets/Gets the IsOwned attribute.
        */
        void SetOwned(TBool aIsOwned);
        TBool IsOwned() const;

        /** Finds a filename in the directory with a given extension. Returns
        * ETrue if a file could be found with that extension.
        */
        TBool FindFileWithExtL(const TDesC&  aExtension,
                               TFileName&  aFileName) const;

        /**
        * Returns the number of files in this directory.
        */
        TInt FileCount() const;

        /**
        * Returns the size of all files used by this directory.
        */
        TInt Size(RFs& aFs) const;

        /**
        * For streaming/serialisation.
        */
        void ExternalizeL(RWriteStream& aStream) const;

        /**
        * Returns the number of bytes required be present in a stream wishing
        * to store names of files, directories and apparc identifiers
        * contained by this object.
        * @return The number of bytes that will be externalized to a stream.
        */
        TInt SizeForStreamingL() const;

    private:
        // Constructors (1st- and 2nd-phase)
        CDirFiles();
        void ConstructL(const TDesC& aDirName);
        void ConstructL(const CDirFiles& aOther);
        void ConstructL(RReadStream& aStream);

        /**
        * Returns the size of a file which exists in this directory.
        * The filename is RELATIVE to this directory.
        */
        TInt SizeOfFile(RFs& aFs, const TDesC& aFileName) const;

    private:    // data members

        HBufC*  iDirName;               // Owned.
        RPointerArray<HBufC> iFiles;    // Owned.

        // IsOwned is TRUE if the entry "owns" the directory - This is set
        // from the AddDirectory API call. An owned directory will be deleted
        // when the app is removed.
        TBool iIsOwned;
    };

private:
    // Constructors (1st- and 2nd-phase)
    CInstalledAppsRegistryFiles();
    void ConstructL(RReadStream& aStream);
    void ConstructL(const CInstalledAppsRegistryFiles& aOther);

    // Disallow assignment
    const CInstalledAppsRegistryFiles&
    operator=(const CInstalledAppsRegistryFiles&);

    /**
     * Gets a CDirFiles object which corresponds to the directory
     * specified. If none exists in the array, it returns NULL
     */
    CDirFiles*  GetDirectory(const TDesC& aDirectory) const;

private:

    RPointerArray<CDirFiles> iDirectories;          // Owned.
    RPointerArray<HBufC> iApparcIdentifiersArray;   // Owned.
};

#endif // INSTALLEDAPPSREGISTRYFILES_H
