/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Header file for CScrBackupUtil class
*
*/

#ifndef JAVASCRBACKUPUTIL_H_
#define JAVASCRBACKUPUTIL_H_


#include <e32base.h>
#include <s32strm.h>
#include <f32file.h> 
#include <e32cmn.h>
#include <s32mem.h>
#include <S32BUF.H>  

#include <mw/usif/usifcommon.h>
#include <mw/usif/scr/scr.h>
#include <mw/usif/scr/screntries.h>
#include <mw/usif/scr/appreginfo.h> //for TAppCaption
#include <e32lang.h>

#include <string>
#include <vector>

namespace java
    {
namespace  backup
    {

class CScrBackupUtil : public CBase
    {
public:
    static CScrBackupUtil* NewL();

    ~CScrBackupUtil();
    
    static CScrBackupUtil* NewLC();
    
    /*
     * filling the data with the SCR value one by one 
     */
    void BackupScrDataL(RDesWriteStream& aStream, TBool& aBackupNotFinished, TInt& aBufferSpaceLeft);
    
    void GetComponentInfoL(Usif::TComponentId aComponentId,RDesWriteStream& aStream);
    void WriteToActualStream(RDesWriteStream& aStream);
    
    void RestoreScrDataL(RDesReadStream& aStream, TInt& aRestoreState, TInt& aBufferSpaceLeft);
    void SetDataInScrL(TUid aUid ,RDesReadStream& aStream);
    
private: //fun
    void ConstructL();
    void ConnectScrL();
    void writeRemainingData();
    void RegisterApplicationL(TInt aComponentId, TUid aAppUid,
            Usif::TAppCaption aAppName, TDesC aIconFilename, TInt aNumberOfIcons,
            TLanguage aLanguages);
    
    template <typename T>
    TInt ComputeSizeL(const T &aObject);
    void ExtractUidAndComponentIdL(RArray<Usif::TComponentId>& componentIds,RArray<TInt>& uids,TInt& ComponentsCount);   
    Usif::TComponentId RegisterComponentL(Usif::CComponentEntry& aEntry ,TUid aUid,TPtr aMidletInfoUrl, TPtr aMidletDescription, TPtr8 aMidletDeletionConformation,TPtr aDownloadUrl,TInt64 aMediaId,TPtr aDomainCatogary , TPtr aSettingValue,RPointerArray<HBufC>& aComponentFilesArray, TBool aIsComponentPresent,RPointerArray<Usif::CLocalizableComponentInfo>& aLocalizableComponentInfoArray);
    CScrBackupUtil();
private://members
      
     enum TState
         {
         EIsFetchCompleteInfo,
         EIsWriteCompleteInfo
         };
     TState iState;
     HBufC8* iRemainingInfo;
     // to check if the previous attribte claims that he cant 
     //write in stream because of space so all other attribute 
     //down the lane should be in the remaining info buffer to maintain the sequence
     TBool iIsToWriteInStream;
     TBool iBufferToWriteNextTime;
     TBool iWriteToStream;
    TInt i ;
    TInt iLengthOfString;
    Usif::RSoftwareComponentRegistry* iScr;
    RArray<Usif::TComponentId> iComponentIds;
    RArray<TInt> iUids;
    RArray<TInt> iPresentUids;
    
    TInt iNumberOfComponents;
    TInt iBufferSpaceLeft;
    
    TInt iRemainingBuffer;
    TPtr8 iPtrToWriteBuffer;
    
    TBool iIsFristCall;
  
    };

// Helper class pass the info of midlet with in midletSuite 
class CAppInfo : public CBase
    {
public:
    static CAppInfo* NewL();

    ~CAppInfo();

    static CAppInfo* NewLC();
    void ConstructL();
    void Set(TLanguage language,TUid appUid,Usif::TAppCaption caption,TDesC& appIcon,TInt aNoOfIcons);
    TLanguage GetLanguage();
    TUid GetAppUid();
    Usif::TAppCaption GetCaption();
    HBufC* GetAppIcon();
    TInt NoOfIcons();
protected:
        
    CAppInfo();
    
    
private:
    TLanguage iLanguage;
    TUid iAppUid;
    Usif::TAppCaption iCaption;
    HBufC* iAppIcon;
    TInt iNoOfIcons;
    };




























































class CJavaBackupEntries : public CBase
    {
public:
    ~CJavaBackupEntries();
    static CJavaBackupEntries* NewL(Usif::TComponentId aId); 
    
    static CJavaBackupEntries* NewLC(Usif::TComponentId aId);
    
    
    //used to get the below info
    /*
     * will get all the entries in this API
     * called when restoring
     */
    void GetEntries();
    
    /*
     * to set all the entries
     * 
     */
    void SetEntries();
private :
    
    void ConstructL(Usif::TComponentId aId);
    
private:
            //check if this duplication can be removed as the same variable is in ccomponentEntry
    Usif::TComponentId iComponentId;   ///< The unique id of the software component.
            //HBufC* iName;                ///< Name of the software component.
            //HBufC* iVendor;              ///< Vendor of the software component.
            //HBufC* iSwType;              ///< Software type name of the component.
            //HBufC* iGlobalId;            ///< Globally unique component identifier.
            //TBool iRemovable;            ///< Whether the component is removable.
            //TInt64 iSize;                ///< The total size of the component at installation time in bytes.
            //TScomoState iScomoState;     ///< SCOMO state of the component.
            //TDriveList iInstalledDrives; ///< The set of drives that files for the software component were installed to.
            //HBufC* iVersion;             ///< Version identifier of the software component
                //HBufC* iInstallTime;         ///< The install time of the component. 
            //TBool iDrmProtected;         ///< Whether the component is DRM protected.
                //TBool iHidden;               ///< Whether the component is hidden in the list of installed components.
            //TBool iKnownRevoked;         ///< Whether the component has been revoked.
            //TBool iOriginVerified;       ///< Whether the installer has verified the originator of the component.
            
            
            // TODO Can we keep a pointer here 
    Usif::CComponentEntry iEntries;
            TBool iIsComponentPresent;
            // component property
            TInt  iMidletSuitId;
            HBufC *iMidletInfoUrl;
            HBufC *iMidletDescription;
            TBool iMidletDeletetionConfirmation; // Didnt got any thing for it 
            HBufC *iMidletDownloadUrl;
            HBufC *iDomainCatagory;// didnt got any thing for it 
            TInt iMediaId;
            //addtion to CcomponentEntry Class
            RPointerArray<Usif::CLocalizableComponentInfo> &iCompLocalizedInfoArray;
            
            
            Usif::RSoftwareComponentRegistry *iScr;
            
            
            
    };
   }

   }



#endif /* JAVASCRBACKUPUTIL_H_ */
