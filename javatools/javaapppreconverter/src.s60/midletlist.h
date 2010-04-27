/*
* Copyright (c) 2008 - 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Java platform 2.0 javaapppreconverter process
*
*/


#ifndef MIDLETLIST_H
#define MIDLETLIST_H

#include <e32base.h>
#include <f32file.h>
#include <s32strm.h>
#include <s32file.h>

_LIT(KMidletExportDataFileName, "omjconvertedapps.dat");


/*!
  @class CMidletInfo

  @discussion Contains necessary information about one MIDlet.
    Information enables reregistering the MIDlet to AppArc
  */
class CMidletInfo : public CBase
{
public:
    /*!
      @function CMidletInfo

      @discussion Construct object. Construction cannot leave so can be done in one phase.
      */
    CMidletInfo();

    /*!
      @function CMidletInfo

      @discussion Construct object and initialize all values to values read from
        aMidletInfo
        Construction cannot leave so can be done in one phase.
      @param aMidletInfo the contents of this midlet info will be copied to
        the new midlet info
      */
    CMidletInfo(CMidletInfo* aMidletInfo);

    /*!
      @function CMidletInfo

      @discussion Destroy the object and release all memory objects
      */
    ~CMidletInfo();


    /*!
      @function GetMidletUid

      @discussion get midlet uid
      @result midlet uid
      */
    TUid GetMidletUid() const;

    /*!
      @function GetMidletId

      @discussion get midlet id
      @result midlet id
      */
    TUint32 GetMidletId() const;

    /*!
      @function GetSuiteId

      @discussion get suite id
      @result suite id
      */
    TUint32 GetSuiteId() const;

    /*!
      @function GetMidletName

      @discussion get midlet name
      @result Midlet name
      */
    const TDesC& GetMidletName();

    /*!
      @function GetGroupName

      @discussion get midlet group name
      @result group name
      */
    const TDesC& GetGroupName();

    /*!
      @function GetDrive

      @discussion get drive
      @result drive
      */
    TDriveNumber GetDrive();

    /*!
      @function GetIconFileName

      @discussion get full path name of the icon file
      @result path name to icon file
      */
    const TFileName& GetIconFileName();




    /*!
      @function SetMidletUid

      @discussion set midlet uid
      @param midlet uid
      */
    void SetMidletUid(const TUid aUid);

    /*!
      @function SetMidletId

      @discussion set midlet id
      @param midlet id
      */
    void SetMidletId(const TUint32 aId);

    /*!
      @function SetSuiteId

      @discussion set suite id
      @param suite id
      */
    void SetSuiteId(const TUint32 aId);

    /*!
      @function SetMidletName

      @discussion Set midlet name
      @param aName Midlet name
      */
    void SetMidletName(const TDesC& aName);

    /*!
      @function SetGroupName

      @discussion Set group name
      @param aName Group name
      */
    void SetGroupName(const TDesC& aName);

    /*!
      @function SetDrive

      @discussion Set drive where midlet suite is
      @param aDrive
      */
    void SetDrive(const TDriveNumber aDrive);

    /*!
      @function SetIconFileName

      @discussion setIconFileName
      @param aIconFileName
      */
    void SetIconFileName(HBufC16 *aIconFileName);

    /*!
      @function SetIconFileName

      @discussion setIconFileName
      @param aIconFileName
      */
    void SetIconFileName(const TFileName &aIconFileName);

    /*!
      @function ExternalizeL

      @discussion ExternalizeL
      @param aStream
      */
    void ExternalizeL(RWriteStream& aStream) const;

    /*!
      @function InternalizeL

      @discussion InternalizeL
      @param aStream
      */
    void InternalizeL(RReadStream& aStream);

    /*!
      @function ToString8

      @discussion Store the printable representation to aDescription
      @param aDescription the length of the descriptor should be at least 512
      */
    void ToString8(TDes8& aDescription);



private:
    /*! @var iMidletUid Midlet Uid */
    TUid iMidletUid;

    /*! @var iMidletId Midlet System AMS Id */
    TUint32 iMidletId;

    /*! @var iSuiteId Midlet suite System AMS Id */
    TUint32 iSuiteId;

    /*!
      @var iMidletName
      */
    HBufC16 *iMidletName;

    /*!
      @var iGroupName
      */
    HBufC16 *iGroupName;

    /*!
      @var iDrive the drive where the Midlet files are located, usually E:
      */
    TDriveNumber iDrive;

    /*!
      @var iIconFileName full icon file path name
      */
    TFileName iIconFileName;

};





/* ---------------------------------------------------------------------------- */






/*!
  @class CMidletList

  @discussion List of MIDlets. Contains all MIDlets to be converted.
  */
class CMidletList : public CBase
{
public:

    /*!
      @function CMidletList

      @discussion Construct object. Construction cannot leave so can be done in one phase.
      */
    CMidletList();


    /*!
      @function ~CMidletList

      @discussion Destroy the object but do not destroy objects in list
      */
    ~CMidletList();


    /*!
      @function ResetAndDestroy

      @discussion Empty the list and destroy objects in it.
      */
    void ResetAndDestroy();

    /*!
      @function Append

      @discussion append midlet info object to list. Note that each midlet can be
        in list only once. Adding already existing midlet does nothing (return KErrNone).
        Midlets are identified by midlet Uids.
      @param aMidletInfo   pointer to suite info to be added
      @result KErrNone, if the insertion is successful, otherwise one of the system wide error codes
      */
    TInt Append(const CMidletInfo* aMidletInfo);

    /*!
      @function Remove

      @discussion remove midletInfo object from list if the object is there
      Note that the function does not delete the object whose pointer is removed.
      @param aSuiteInfo   pointer to midlet info to be removed.
      */
    void Remove(const CMidletInfo* aMidletInfo);

    /*!
      @function count

      @discussion count return the number of midlets in the list
      @result the number of midlets in the list
      */
    TInt Count() const;

    /*!
      @function getFirst

      @discussion get the first midlet info in list
      @result pointer to the first object or NULL if list is empty
      */
    CMidletInfo* GetFirst();

    /*!
      @function GetNext

      @discussion get the next midlet info in list  (works also when remove is called)
      @result pointer to the next object in list or NULL if no more objects
      */
    CMidletInfo* GetNext();

    /*!
      @function Find

      @discussion find midletInfo object in list based on midlet Uid
      @param aMidletUid  the Uid of the midlet to be returned
      @result Pointer to the midlet with the specififed Uid or NULL if none was found
      */
    CMidletInfo* Find(const TUid aMidletUid);

    /*!
      @function Find

      @discussion find midletInfo object in list based on midlet name
      @param aName  the name of the midlet to be returned
      @result Pointer to the midlet with the specififed Uid or NULL if none was found
      */
    CMidletInfo* Find(const TPtrC &aName);

    /*!
      @function SetIds

      @discussion If list contains midlet specified with aUid, sets the midlet suite id of
        midlet to aMidletSuiteId and midlet id to aMidletId.
        If the list does not contain midlet with specified Uid, does nothing.
      @param aUid  the Uid (in TUint32 form) of the midlet
      @param aMidletSuiteId   the System AMS id of midlet suite
      @param aMidletId the System AMS id of the midlet
        (== the ordinal of the midlet inside midlet suite)
      */
    void SetIds(const TUint32 aUid, const TUint32 aMidletSuiteId, const TUint32 aMidletId);

    /*!
      @function SetIds

      @discussion If list contains midlet specified with aName, sets the midlet suite id of
        midlet to aMidletSuiteId and midlet id to aMidletId.
        If the list does not contain midlet with specified name, does nothing.
      @param aName  the name of the midlet
      @param aMidletSuiteId   the System AMS id of midlet suite
      @param aMidletId the System AMS id of the midlet
        (== the ordinal of the midlet inside midlet suite)
      */
    void SetIds(const TPtrC &aName, const TUint32 aMidletSuiteId, const TUint32 aMidletId);

    /*!
      @function ExportList

      @discussion Stores all information in the list to the directory given in aDirectory.
        First copies all icon files specified in the midlet entries to aDirectory and
        changes each midlet entry so that midlet entry .iIconFileName points to the copy
        of the icon file in aDirectory.
        Then stores all midlet entries to aDirectory (serializes information to a file).
        In case of fatal error leaves with standard Symbian error code.
      @param aFs file server handle
      @param aDirectory full path name to an empty directory
      */
    void ExportListL(RFs& aFs, const TFileName &aDirectory);


    /*!
      @function ImportList

      @discussion Read all information in the list from the directory given in aDirectory.
        The list should be empty when this function is called.
        Reads all midlet entries from a data file in aDirectory.
        The information to be read has been created earlier by calling ExportList().
        In case of fatal error leaves with standard Symbian error code.
      @param aFs file server handle
      @param aDirectory full path name to a directory filled by calling ExportList().
      */
    void ImportListL(RFs& aFs, const TFileName &aDirectory);




private:
    /*! @var iMidletArray dynamical array of midlets */
    RPointerArray<CMidletInfo> iMidletArray;

    /*! @var iGetNextIndex index of the next object to be returned by getNext() */
    TInt iGetNextIndex;

    /*! @var iMidletArrayDestroyed ETrue after resetAndDestroy has been called */
    TBool iMidletArrayDestroyed;

};


#endif // MIDLETLIST_H

