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
* Description:  Mifconverter is part of Java Installer component Icon Converter
*                Converts icons from SVG to MIF
*
*/


#ifndef MIFCONVERTER_H
#define MIFCONVERTER_H

#include <e32base.h>
#include <f32file.h>
#include <s32file.h>

/**
 *  The structure for MIF file header
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS(TMifFileHeader)
{
public:
    /**
     * The UID of the MIF file format. It is used for validating the MIF file format.
     */
    TInt32  iUid;
    /**
     * Determines the valid fields in the MIF file header.
     * Must be greater than or equal to 2.
     */
    TInt32  iVersion;
    /**
     * The pointer to the beginning of the array of icon offset elements.
     */
    TInt32  iOffset;
    /**
     * The array length in the items of icon offset elements.
     */
    TInt32  iLength;
};

/**
 *  The structure for MIF icon offset
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS(TMifIconOffset)
{
public:
    /**
     * The offset to the icon from the beginning of the MIF file.
     * Icon = icon header + icon data.
     */
    TInt32  iIconOffset;
    /**
     * The combined length of the icon header and the icon data.
     */
    TInt32  iIconLength;
};

/**
 *  The structure for MIF icon header
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS(TMifIconHeader)
{
public:
    /**
     * The UID of the MIF icon header format.
     * It is used for validating the MIF file format.
     */
    TInt32  iUid;
    /**
     * The version of the MIF icon header format.
     * Determines the valid fields in the MIF icon header.
     */
    TInt32  iVersion;
    /**
     * The offset to icon data (from the beginning of the icon).
     */
    TInt32  iOffset;
    /**
     * The length of icon data in bytes.
     */
    TInt32  iLength;
    /**
     * The type of the MIF icon.
     */
    TInt32  iType;
    /**
     * The depth of the bitmap the content will be rendered to.
     * Corresponds to the Symbian OS enumeration TDisplayMode.
     */
    TInt32  iDepth;
    /**
     * Whether the icon is animated.
     */
    TInt32  iAnimated;
    /**
     * The depth of the mask.
     * Corresponds to the Symbian OS enumeration TDisplayMode.
     */
    TInt32  iMaskDepth;
};

/**
 *  Class to handle the conversion of icons from SVG to MIF file
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS(CMifConverter) : public CBase
{
public:
    /**
     * CMifConverter::NewL constructor
     * Construct a CMifConverter object, using two phase construction
     * and return a pointer to the created object.
     *
     * @since S60 v3.2
     * @param aFs file session
     * @param aMifFile the name of the MIF file to be created
     * @return a pointer to the created instance of CMifConverter
     */
    static CMifConverter* NewL(RFs& aFs, const TDesC& aMifFile);

    /**
     * CMifConverter::~CMifConverter destructor
     * Destroy the object and release all memory objects
     *
     * @since S60 v3.2
     */
    virtual ~CMifConverter();

public:
    /**
     * CMifConverter::WriteMifFileHeaderL method
     * Writes file header to the MIF file
     *
     * @since S60 v3.2
     * @param aMifFileHeader TMifFileHeader structure
     */
    void WriteMifFileHeaderL(const TMifFileHeader& aMifFileHeader);

    /**
     * CMifConverter::WriteMifIconOffsetL method
     * Writes icon offset to the MIF file
     *
     * @since S60 v3.2
     * @param aMifIconOffset TMifIconOffset structure
     */
    void WriteMifIconOffsetL(const TMifIconOffset& aMifIconOffset);

    /**
     * CMifConverter::WriteMifIconHeaderL method
     * Writes icon header to the MIF file
     *
     * @since S60 v3.2
     * @param aMifIconHeader TMifIconHeader structure
     */
    void WriteMifIconHeaderL(const TMifIconHeader& aMifIconHeader);

    /**
     * CMifConverter::WriteMifBodyL method
     * Writes file body to the MIF file
     *
     * @since S60 v3.2
     * @param apIconBuf buffer containing the contents of the svg file
     */
    void WriteMifBodyL(HBufC8* apIconBuf);

private:
    /**
     * CMifConverter::CMifConverter constructor
     * Perform the first phase of two phase construction
     *
     * @since S60 v3.2
     * @param aFs file session
     */
    CMifConverter(RFs& aFs);

    /**
     * CMifConverter::ConstructL constructor
     * Perform the second phase of two phase construction
     *
     * @since S60 v3.2
     * @param aMifFile the name of the MIF file to be created
     */
    void ConstructL(const TDesC& aMifFile);


private:
    /**
     * File session reference.
     */
    RFs& iFs;

    /**
     * Stream for the MIF file
     * Own.
     */
    RFileWriteStream iMifWriteStream;
};

#endif // MIFCONVERTER_H

