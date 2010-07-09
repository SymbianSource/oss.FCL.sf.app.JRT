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
* Description:  Java Installer component iconconverter.
*                Reads icon and .jar file and stores it a temp file in a
*                format that can be shown by the S60 UI shell.
*
*/


#ifndef ICONCONVERTER_H
#define ICONCONVERTER_H

#include <e32base.h>
#include <fbs.h>
#include <imageconversion.h>


class CBitmapScaler;
class TIconSizes;


namespace java
{

NONSHARABLE_CLASS(CActiveListener) : public CActive
{
public:
    CActiveListener() : CActive(CActive::EPriorityIdle)
    {
        CActiveScheduler::Add(this);
    }
    void InitialiseActiveListener()
    {
        SetActive();
    }
    // From CActive
    virtual void RunL()
    {
        CActiveScheduler::Stop();
    }
    virtual void DoCancel() {};
};


/**
* Reads the icon from specified .jar file, converts it to format
* supported by S60 UI shell and returns it in a temporary file.
*
*/
NONSHARABLE_CLASS(CIconConverter) : public CBase
{
public:

    /**
     * NewL constructor
     * Constructs a new CIconConverter object
     *
     * @param[in] aFs file session
     * @return pointer to new IconConverter
     */
    static CIconConverter* NewL(RFs& aRFs);

    /**
     * Read iconFile in aJarFile, if it is SVG icon
     * use CMifConverter to convert it to MIF,
     * otherwise convert it to MBM,
     * store it to output file
     *
     * @param[in] aJarFile jar file that contains the icon
     * @param[in] aIconFile the path name of icon inside jar
     * @param[in] aOutputFile the name of the file where the converted icon
     *   must be stored to
     * @param[out] apWasMbm The value pointed by this parameter will be set to
     *   ETrue if the icon was converted to MBM, otherwise it is set to EFalse
     * @return Symbian error code in case of an error,
     *   KErrNone is conversion succeeds
     */
    int Convert(
        const TDesC &aJarFile,
        const TDesC &aIconFile,
        const TDesC &aOutputFile,
        TBool *apWasMbm);

    /**
     * Destroy the object and release all internal memory objects.
     * Does not close file session.
     */
    ~CIconConverter();

    /**
     * Logs the names of all MIME types that IconConverter supports
     * in the current device. Also the corresponding file extensions
     * are logged.
     */
    static void LogAllSupportedMimeTypes();

private:
    /**
     * Constructor
     * Constructs a new CIconConverter object
     *
     * @param[in] aFs file session
     * @return pointer to new IconConverter
     */
    CIconConverter(RFs &aRFs);

    /**
     * Reads icon from .jar file and then based on the type of
     * icon calls either ConvertNormalIconL or
     * ConvertScalableIconL
     *
     * @param[in] aJarFile jar file that contains the icon
     * @param[in] aIconFile the path name of icon inside jar
     * @param[in] aOutputFile the name of the file where the converted icon
     *   must be stored to
     * @param[out] apWasMbm The value pointed by this parameter will be set to
     *   ETrue if the icon was converted to MBM, otherwise it is set to EFalse
     * @return Symbian error code in case of an error,
     *   KErrNone is conversion succeeds
     */
    int ConvertL(
        const TDesC &aJarFile,
        const TDesC &aIconFile,
        const TDesC &aOutputFile,
        TBool *apWasMbm);

    /**
     * Real implementation of Convert for normal icons.
     * Uses iImageDecoder to convert icon to mbm.
     *
     * @param[in] apIconBuf
     * @param[in] aptrIconBuf
     * @param[in] aOutputFile the name of the file where the converted icon
     *   must be stored to
     * @return Symbian error code in case of an error,
     *   KErrNone is conversion succeeds
     */
    int ConvertNormalIconL(
        HBufC8*      apIconBuf,
        TPtr8       &aptrIconBuf,
        const TDesC &aOutputFile);

    /**
     * Real implementation of Convert for scalable icons.
     * Uses CMifConverter to convert svg to mif
     *
     * @param[in] apIconBuf
     * @param[in] aOutputFile the name of the file where the converted icon
     *   must be stored to
     * @return Symbian error code in case of an error,
     *   KErrNone is conversion succeeds
     */
    int ConvertScalableIconL(
        HBufC8*      apIconBuf,
        const TDesC &aOutputFile);

    /**
     * Scale the original icon into the current temporary icon.
     */
    void ScaleL(CBitmapScaler& aScaler, const TSize aSize);

    /**
     * Get the ideal icon sizes from UI, trough the ecom plugin notifier.
     */
    TIconSizes GetIdealIconSizes();

private:
    /**
     * File server session. Shared with the caller.
     * Do not close.
     */
    RFs iRFs;

    /**
     * Converts from any format supported by the conversion
     * plugins in the S60 system to bitmap.
     */
    CBufferedImageDecoder* iImageDecoder;

    /**
     * Pointer to bitmap generated from the icon.
     */
    CFbsBitmap* iBitmap;

    /**
     * Pointer to the bitmap mask generated from the icon.
     */
    CFbsBitmap* iBitmapMask;

    /**
     * Scaled copy of the original icon bitmap.
     */
    CFbsBitmap* iBitmapScaledCopy;

    /**
     * Scaled copy of the original icon mask.
     */
    CFbsBitmap* iBitmapMaskScaledCopy;

    CActiveListener *iActiveListener;

};

} // namespace java

#endif // ICONCONVERTER_H
