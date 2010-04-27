/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CManifestReader header file
*
*/


#ifndef APPMNGR2MIDLETMANIFESTREADER_H
#define APPMNGR2MIDLETMANIFESTREADER_H

#include <f32file.h>
#include <e32base.h>
#include <caf/data.h>
#include <memory>
#include <mjavaattribute.h>

using namespace Java;

/**
 * CManifestReader is a simple utility class to read manifest content.
 * It validates only attribute name parameter and parses all attributes
 * from the manifest file.
 */
class AppMngr2MidletManifestReader : public CBase
{
public:

    AppMngr2MidletManifestReader(RFs& aFs);
    ~AppMngr2MidletManifestReader();

    /**
     * Read manifest file from the JAR file or read plain manifest file.
     *
     * @param aManifestFileName Manifest file name inside JAR package or
     *        path to plain manifest file. If ManifestParser is created
     *        with CJavaFileHandle, then file handle is used and manifest
     *        file name is ignored.
     * @param aAttributes Array is populated with manifest attributes.
     */
    void ReadManifestL(const TDesC& aManifestFileName,
                       RPointerArray<MJavaAttribute>& aAttributes);

private:

    /**
     * Read manifest file to buffer.
     *
     * @param aManifestFileName Manifest file name inside JAR package or
     *        path to plain manifest file.
     */
    void ContentL(const TDesC& aManifestFileName);

    /**
     * Read manifest file content to buffer.
     *
     * @param  aManifestFile manifest file
     * @return A pointer to content. Ownership is transferred.
     */
    HBufC8* ReadRawManifestFileL(ContentAccess::CData& aManifestFile);

    /**
     * Read content from plain manifest file.
     *
     * @param aManifestFile Reference to manifest file handle.
     */
    void ReadManifestContentL(RFile& aManifestFile);

    /**
     * Read manifest file content from package.
     *
     * @param aPackageFile Reference to package file handle.
     */
    void ReadManifestContentFromPackageL(RFile& aPackageFile);

    /**
     * Read all attributes from internal buffer.
     *
     * @param aAttributes Attribute array to be populated.
     */
    void ReadAttributesL(RPointerArray<MJavaAttribute>& aAttributes);

    /**
     * Read one line from the internal buffer.
     *
     * @param aContinuation if line is continuation it can be shorter
     *        than attribute name line.
     * @return TInt index to internal buffer where line ends.
     */
    TInt ReadLineIndexL(TBool aContinuation);

    /**
     * Detect continuational line and append it to attribute value.
     *
     * @param aValue      Reference to attribute to be appended if continuational line.
     * @param aContent    Reference to manifest content
     * @param aAttributes Attribute array where attribute is stored if continuational line.
     */
    void ReadContinuationLineL(TDes16& aValue, TDes16& aContent, TInt& aIndex);

    /**
     * Validate Attribute name. Name can contain only
     * {A-Z} | {a-z} | {0-9} | - | _ characters.
     *
     * @param aName Attribute name
     * @exception   if invalid characters this will leave with code
     *              KJavaErrInvalidManifest
     */
    void ValidateAttributeNameL(TDesC16& aName);

private:
    RFs mFs;
    bool mSessionOpen;
    std::auto_ptr<HBufC16>mManifestContent;
};

#endif // APPMNGR2MIDLETMANIFESTREADER_H

