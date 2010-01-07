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
* Description:  CJavaSuperDParser definition.
*
*/


#ifndef C_JAVASUPERDPARSER_H
#define C_JAVASUPERDPARSER_H

#include <e32base.h>
#include <f32file.h>
#include <s32strm.h>
#include <e32std.h>

class CZipFile;
class CZipFileMember;

namespace Java
{

/**
 * CJavaSuperDParser includes methods for parsing SuperD packages.
 *
 * This class includes methods for creating a superdistributable JAR
 * package and also methods for parsing a superdistributable JAR
 * package when installing it.
 *
 * A superdistributable JAR package is in uncompressed ZIP format
 * containing:
 *     -# a manifest file (META-INF/MANIFEST.MF),
 *     -# a JAD file and
 *     -# either a plaintext JAR file or a JAR file in DCF.
 *
 * The manifest file must have the following attributes:
 *     -# Distribution-Package: &lt;SuperD version number&gt;
 *     -# Distribution-Descriptor-Filename: &lt;JAD file name&gt;
 *     -# Distribution-Content-Filename: &lt;JAR file name&gt;
 *
 * To create a superdistributable jar package use the static method
 * CreateSuperDPackageL:
 * @code
 * using namespace Java;
 * CJavaSuperDParser::CreateSuperDPackageL( RFs& aFileSession,
 *                                          const TDesC& aDestinationFileName,
 *                                          const TDesC& aJadFileName,
 *                                          const TDesC& aJarFileName );
 * @endcode
 *
 * To parse a superdistributable jar package:
 * @code
 * using namespace Java;
 * CJavaSuperDParser* parser( NULL );
 * TRAPD( err, parser = CJavaSuperDParser::NewL( aFileSession,
 *                                               KSuperDFile,
 *                                               KDestinationDir ) );
 * if ( !err )
 *     {
 *     // Now you know that KSuperDFile is a superdistributable package.
 *     // JAD and JAR file have been written in KDestinationDir.
 *     }
 * delete parser;
 * @endcode
 *
 * CJavaSuperDParser was renamed from CJcfSuperDParser.
 *
 * @lib   javadrmutils.lib
 * @since S60 v3.2
 */
NONSHARABLE_CLASS(CJavaSuperDParser) : public CBase
{

public:

    /**
     * First phase constructor. Creates a CJavaSuperDParser object, parses
     * the SuperD package and extracts its contents to the specified
     * folder.
     *
     * @since S60 v3.2
     * @param aFileSession File session handle to be used by this object
     * @param aJarFileName Filename of the file containing SuperD
     * package
     * @param aDestinationFolder Folder where descriptor and content
     * files will be saved
     * @return The newly created CJavaSuperDParser object handle
     * @leave KErrCorrupt If supplied file is not a SuperD package
     */
    IMPORT_C static CJavaSuperDParser* NewL(RFs& aFileSession,
    const TDesC& aJarFileName,
    const TDesC& aDestinationFolder);

    /**
     * First phase constructor. Creates a CJavaSuperDParser object, parses
     * the SuperD package and extracts its contents to the specified
     * folder.
     *
     * @since S60 v3.2
     * @param aFileSession File session handle to be used by this object
     * @param aJarFileHandle File handle of the file containing SuperD
     * package
     * @param aDestinationFolder Folder where descriptor and content
     * files will be saved
     * @return The newly created CJavaSuperDParser object handle
     * @leave KErrCorrupt If supplied file is not a SuperD package
     */
    IMPORT_C static CJavaSuperDParser* NewL(RFs& aFileSession,
                                            RFile& aJarFileHandle,
                                            const TDesC& aDestinationFolder);

    /**
     * Destructor. Frees allocated resources.
     *
     * @since S60 v3.2
     */
    virtual ~CJavaSuperDParser();

    /**
     * Creates a superdistribution package from the given JAD and JAR file.
     *
     * @since S60 v3.2
     * @param aFileSession File session handle to be used by this object
     * @param aDestinationFileName Filename of the destination SuperD
     * package.
     * @param aDescriptorFileName JAD file name
     * @param aContentFileName JAR file name
     */
    IMPORT_C static void CreateSuperDPackageL(RFs& aFileSession,
            const TDesC& aDestinationFileName,
            const TDesC& aDescriptorFileName,
            const TDesC& aContentFileName);

    /**
     * Returns the name of the JAD file in the SuperD package parsed by
     * this object.
     *
     * @since S60 v3.2
     * @return Descriptor file name
     */
    IMPORT_C TFileName DescriptorFileName() const;

    /**
     * Returns the name of the JAR file in the SuperD package parsed by
     * this object.
     *
     * @since S60 v3.2
     * @return Content file name
     */
    IMPORT_C TFileName ContentFileName() const;

private:

    /**
     * Private C++ constructor.
     *
     * @since S60 v3.2
     * @param aFileSession File session handle to be used by this object
     */
    CJavaSuperDParser(RFs& aFileSession);

    /**
     * Second phase Symbian constructor. Extracts the contents of the
     * SuperD package into the destination folder.
     *
     * @since S60 v3.2
     * @param aJarFileName File name of SuperD package in the files system
     * @param aDestinationFolder Files from the package will be extracted
     * here.
     * @leave KErrCorrupt If supplied file is not a SuperD package
     */
    void ConstructL(const TDesC& aJarFileName,
                    const TDesC& aDestinationFolder);

    /**
     * Second phase Symbian constructor. Checks and extracts the contents
     * of the SuperD package into the destination folder with
     * ParseZipFileL.
     *
     * @since S60 v3.2
     * @param aJarFileHandle File handle to the SuperD package
     * @param aDestinationFolder Files from the package will be extracted
     * here.
     * @leave KErrCorrupt If supplied file is not a SuperD package
     */
    void ConstructL(RFile& aJarFileHandle,
                    const TDesC& aDestinationFolder);

    /**
     * Calculates the CRC of a given file according to zip specification.
     *
     * @since S60 v3.2
     * @param aFile File to calculate the CRC for
     * @return CRC value
     */
    static TUint32 CalculateCRC(RFile& aFile);

    /**
     * Creates and writes the MANIFEST.MF file for the SuperD package.
     * The manifest file contains the following three lines:
     *     -# Distribution-Package: &lt;SuperD version number&gt;
     *     -# Distribution-Descriptor-Filename: &lt;JAD file name&gt;
     *     -# Distribution-Content-Filename: &lt;JAR file name&gt;
     *
     * This function creates a META-INF/MANIFEST.MF (in the META-INF
     * directory inside the SuperD JAR file with the structure above,
     * and writes it to a RWriteStream with WriteEntryAndUpdateIndexL.
     *
     * @since S60 v3.2
     * @param aFileSession File session handle to be used by this object
     * @param aStream File write stream where the manifest file will be
     * written
     * @param aCurrentOffset Offset where to start to write in the write
     * stream
     * @param aZipIndex The zip file index
     * @param aRelativeJadFileName Name of the JAD file with relative path
     * @param aRelativeJarFileName Name of the JAR file with relative path
     */
    static void CreateAndWriteManifestL(RFs& aFileSession,
                                        RWriteStream& aStream,
                                        TInt& aCurrentOffset,
                                        HBufC8*& aZipIndex,
                                        const TDesC8& aRelativeJadFileName,
                                        const TDesC8& aRelativeJarFileName);
    /**
     * Writes an entry (file or directory) to the zip file stream and
     * updates the zip file index.
     *
     * @since S60 v3.2
     * @param aFileSession File session handle to be used by this object
     * @param aStream ZIP file write stream where the entry will be written
     * @param aCurrentOffset Offset where to start to write
     * @param aZipIndex The zip file index
     * @param aRelativeFileName Relative name of the entry inside the ZIP
     * file
     * @param aFileName Name of the file in the file system which will be
     * written in the ZIP file. Use NULL to create a directory.
     */
    static void WriteEntryAndUpdateIndexL(RFs& aFileSession,
                                          RWriteStream& aStream,
                                          TInt& aCurrentOffset,
                                          HBufC8*& aZipIndex,
                                          const TDesC8& aRelativeFileName,
                                          const TDesC* aFileName);
    /**
     * Writes the ZIP index entry to the ZIP output stream. The ZIP index
     * should be the last entry in the ZIP file.
     *
     * @since S60 v3.2
     * @param aStream ZIP file write stream where the entry will be written
     * @param aCurrentOffset Offset where to start to write
     * @param aZipIndex The zip file index which will be written in the stream
     */
    static void WriteIndexL(RWriteStream& aStream,
                            TInt aCurrentOffset,
                            const HBufC8* aZipIndex);

    /**
     * Checks and parses the contents of the SuperD package.
     *
     * @since S60 v3.2
     * @param aDestinationFolder Files from the package will be extracted
     * here.
     */
    void ParseZipFileL(const TDesC& aDestinationFolder);

    /**
     * Checks from the manifest file that this is a SuperD package.
     * Extracts the JAD and JAR file name from the manifest file in the
     * SuperD package and checks wether the package version is compatible
     * with the parser.
     *
     * @since S60 v3.2
     * @param aManifest The manifest file entry in the ZIP file.
     */
    void ParseManifestL(CZipFileMember* aManifest);

    /**
     * Extracts the JAD and the JAR file from the SuperD package to the
     * destination folder.
     *
     * @since S60 v3.2
     * @param aDestinationFolder Destination folder name.
     */
    void WriteFileL(const TDesC& aDestinationFolder);

private: // data

    /**
     * File session handle to be used by this object.
     * Not own.
     */
    RFs& iFileSession;

    /**
     * Zip file object.
     * Own.
     */
    CZipFile*       iZipFile;

    /**
     * JAD file entry in the SuperD package.
     * Own.
     */
    CZipFileMember* iJadMember;

    /**
     * JAR file entry in the SuperD package.
     * Own.
     */
    CZipFileMember* iJarMember;

    /**
     * Shows wether there is a valid manifest file in the package.
     */
    TBool           iManifestFound;

    /**
     * Name of the JAD file in the SuperD package.
     */
    TFileName       iDescriptorFileName;

    /**
     * Name of the JAR file in the SuperD package.
     */
    TFileName       iContentFileName;

};

}

#endif // C_JAVASUPERDPARSER_H

// End of File
