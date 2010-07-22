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
* Description:  Implementation of CScrBackupUtil
*
*/

#include "javascrbackuputil.h"
#include "midp2backupdataids.h"

#include "logger.h"
#include "javasymbianoslayer.h" // for CleanupResetAndDestroyPushL

#include <scs/nullstream.h>
#include <e32cmn.h> 
#include <apaid.h> //for TApaAppCapability::ENonNative
#include <e32def.h>

using namespace java::backup;
using namespace Usif;

// Properties registered to SCR.
_LIT(KMIDletName, "MIDlet-Name");
_LIT(KUid, "Uid");
_LIT(KMediaId, "Media-Id");
_LIT(KMIDletInfoURL, "MIDlet-Info-URL");
_LIT(KMIDletDescription, "MIDlet-Description");
_LIT(KDownloadURL, "Download-URL");
_LIT(KSettingsPlugin, "SettingsName");
_LIT(KMIDletDeletionConformation, "MIDlet-Delete-Confirm");

_LIT(KDomainCategory,"Domain-Category");


CScrBackupUtil* CScrBackupUtil::NewL()
    {
    LOG(EBackup,EInfo," --->  CScrBackupUtil:  NewL()");
    
    CScrBackupUtil *self = CScrBackupUtil::NewLC();
   
    CleanupStack::Pop(self);
    
    LOG(EBackup,EInfo," <--- CScrBackupUtil: NewL()");
    return self;
    }


CScrBackupUtil* CScrBackupUtil::NewLC()
    {
    LOG(EBackup,EInfo," --->  CScrBackupUtil: NewLC");
    CScrBackupUtil *self = new(ELeave) CScrBackupUtil();
    CleanupStack::PushL(self); 
    self->ConstructL();
    LOG(EBackup,EInfo," <--- CScrBackupUtil: NewLC");
    return self;
    }

CScrBackupUtil::CScrBackupUtil():iPtrToWriteBuffer(NULL,0)
    {
    LOG(EBackup,EInfo," --->  CScrBackupUtil: CScrBackupUtil");
    LOG(EBackup,EInfo," <---  CScrBackupUtil: CScrBackupUtil");
    }


void CScrBackupUtil::ConstructL()
    {
        LOG(EBackup,EInfo," ---> CScrBackupUtil: ConstructL ");
        
        //will be incremented as and when needed
        iRemainingInfo = HBufC8::NewL(4096);//TODO :need to change it to the total size of the the attribute
        //iPtrToWriteBuffer = ((const_cast<TUint8*>(iRemainingInfo->Ptr())));// need to do this coz when fun end and goes to fetch next 4096 bytes i lose it
        iPtrToWriteBuffer.Set(iRemainingInfo->Des());
        iIsFristCall = ETrue;
        
        iState = EIsFetchCompleteInfo;
        iIsToWriteInStream = ETrue;
        LOG(EBackup,EInfo," <--- CScrBackupUtil: ConstructL()");
    }
void CScrBackupUtil::ConnectScrL()
    {
            LOG(EBackup,EInfo," ---> CScrBackupUtil: ConnectScrL()");
            iScr = new RSoftwareComponentRegistry();
            if (NULL == iScr)
            {
                ELOG(EBackup,"Creating RSoftwareComponentRegistry failed");
                User::Leave(KErrGeneral);
            }
            
            TInt err = iScr->Connect();
            if (KErrNone != err)
            {
                ELOG1(EBackup,"Connecting to RSoftwareComponentRegistry failed, error %d",err);
                //TODO need to see can we afford leaving here ;)
                User::Leave(err);
            }
            LOG(EBackup,EInfo," <--- CScrBackupUtil: ConnectScrL()");
    }
void CScrBackupUtil::ExtractUidAndComponentIdL(RArray<Usif::TComponentId>& componentIds,RArray<TInt>& uids,TInt& ComponentsCount)
    {
        LOG(EBackup,EInfo," ---> CScrBackupUtil: ExtractUidAndComponentId()");
        CComponentFilter* filter;
        CPropertyEntry* property;
        filter = CComponentFilter::NewLC();
        //Set the filter for software type Usif::KSoftwareTypeJava and ftech the Ids
        filter->SetSoftwareTypeL(Usif::KSoftwareTypeJava);
        TRAPD(error,iScr->GetComponentIdsL(componentIds,filter));
        if(error)
            {
            ELOG(EBackup,"Failed to get the components ID");
            }
        CleanupStack::PopAndDestroy(filter);
        // fetch the no. of component from the given RArray
        ComponentsCount = componentIds.Count();
        // No use of Component Ids as such as they are used by SCR internally 
        //so exctracted the UID of of each component
        for(TInt i = 0 ;i < ComponentsCount ; i++)
            {
        
                property = iScr->GetComponentPropertyL(componentIds.operator [](i),KUid());
                if (property->PropertyType() == CPropertyEntry::EIntProperty)
                        {
                           TInt uid = ((CIntPropertyEntry*)property)->IntValue();
                           uids.AppendL(uid);
                             
                        }
                        else
                        {
                                 //Error
                        }  
                        
                        delete property;
                        property = NULL;
                    
            }
        LOG(EBackup,EInfo," <--- CScrBackupUtil: ExtractUidAndComponentId()");
    }
void CScrBackupUtil::BackupScrDataL(RDesWriteStream& aStream, TBool& aBackupNotFinished, TInt& aBufferSpaceLeft)
    {
        LOG(EBackup,EInfo," ---> CScrBackupUtil: BackupScrDataL()");
        
        iBufferSpaceLeft = aBufferSpaceLeft ;
        
        if(iIsFristCall)
            {
               
                // to get no. of components need to keep a flag that its called only once
                ConnectScrL();
                
                ExtractUidAndComponentIdL(iComponentIds,iUids,iNumberOfComponents);
                
                //write the no. of components so that it will help us in restoration time
                aStream.WriteInt32L(iNumberOfComponents);
                iBufferSpaceLeft -= sizeof(TInt32);
                              
                //all the ids of the midlets that are to be backedup and the size of the rarray with holds the Ids 
                TPckgBuf<RArray<TInt> > ids(iUids);
                TInt size = ComputeSizeL(ids);
                aStream.WriteInt32L(size);
                iBufferSpaceLeft -= sizeof(TInt32);
                
                aStream.WriteL(ids);
                
                iBufferSpaceLeft = iBufferSpaceLeft - size;
        
                iIsFristCall = EFalse;
            }    
                
            for( i ; i < iComponentIds.Count() ;)
                {
                    if(iState == EIsFetchCompleteInfo)
                        {
                            
                            TInt at = iComponentIds.operator[](i);
                            
                            //Temp stream in which all attributes are backed up 
                            RDesWriteStream writeStream;
                            writeStream.Open(iPtrToWriteBuffer);
                            
                            GetComponentInfoL(at,writeStream);
                            
                            writeStream.CommitL();
                            iLengthOfString = iPtrToWriteBuffer.Length();
                            
                            iWriteToStream = ETrue;
                            
                            // write the size, Used while restoring
                            aStream.WriteInt32L(iLengthOfString);
                            iBufferSpaceLeft = iBufferSpaceLeft - sizeof(TInt);
                            
                            
                        }
                    if(iState == EIsWriteCompleteInfo)
                        {                           
                         
                            //writing the remaining information into the new buffer
                            if(iBufferToWriteNextTime)
                            {
                                aStream.WriteL(iPtrToWriteBuffer,iRemainingBuffer);
                                aStream.CommitL();
                                LOG(EBackup, EInfo, "attributes written fully in the stream fetching for next component");
                                    
                                iBufferSpaceLeft = iBufferSpaceLeft - iRemainingBuffer;
                                
                                iBufferToWriteNextTime = EFalse;
                                iState = EIsFetchCompleteInfo;
                                
                                iLengthOfString = 0; // to maintain the loop
                                
                                iWriteToStream = EFalse;
                                i++;
                            }
                            // if the whole string cannot be writen, write part of it
                            if (iLengthOfString > iBufferSpaceLeft && !iBufferToWriteNextTime)
                            {
                                
                                iRemainingBuffer = iLengthOfString - iBufferSpaceLeft;
                                
                                aStream.WriteL(iPtrToWriteBuffer,iBufferSpaceLeft);
                                aStream.CommitL();
                                
                                iBufferToWriteNextTime = ETrue;
                                break;
                            }
                            // else write the complete string
                            else if(iWriteToStream)
                            {       aStream.WriteL(iPtrToWriteBuffer,iLengthOfString);
                                    
                                    aStream.CommitL();
                                    iBufferSpaceLeft = iBufferSpaceLeft - iLengthOfString;
                                    iState = EIsFetchCompleteInfo;
                                    iLengthOfString = 0; // to maintain the loop 
                                    i++;
                            }
                            
                        }
                }
            // making it false to backup next component next time
            if(i >= iComponentIds.Count())
                aBackupNotFinished = EFalse; 
            
    }

template <typename T>
TInt CScrBackupUtil::ComputeSizeL(const T &aObject)
    {
        LOG(EBackup,EInfo," ---> computeSize() ");
        TInt size(0);
        RNullWriteStream nullstream;
        CleanupClosePushL(nullstream);
        nullstream << aObject;
        nullstream.CommitL();
        size = nullstream.BytesWritten();
     
        CleanupStack::PopAndDestroy(&nullstream);
        LOG(EBackup,EInfo," <--- computeSize() ");
        return size;
       
        
    }
void CScrBackupUtil::GetComponentInfoL(TComponentId aComponentId ,RDesWriteStream& aStream )
    {
        
        LOG(EBackup,EInfo," ---> GetComponentInfo() ");
        
        TInt j = 0 ;
        CPropertyEntry* property;
        CComponentEntry* entries = CComponentEntry::NewL();
        CleanupStack::PushL(entries);
        TLanguage language =KNonLocalized;
        
        TBool result = iScr->GetComponentL(aComponentId,*entries,language);
        entries->ExternalizeL(aStream);
        
        CleanupStack::PopAndDestroy(entries);
        TBool isComponentPresent = iScr->IsComponentPresentL(aComponentId);
        aStream.WriteInt8L(isComponentPresent);
        
        
        // Localizable Component info 
        RPointerArray<CLocalizableComponentInfo> localizableComponentInfo;
        iScr->GetComponentLocalizedInfoL(aComponentId,localizableComponentInfo);
        aStream.WriteInt32L(localizableComponentInfo.Count());
        
        
        for(j ; j < localizableComponentInfo.Count(); j++)
            {
                    (localizableComponentInfo.operator [](j))->ExternalizeL(aStream);
                     
            }
        
        // Properties
        // extracted the UID
        property = iScr->GetComponentPropertyL(aComponentId,KUid());
        if (NULL != property)
            {  
                 property->ExternalizeL(aStream);
                
                  
                delete property;
                property = NULL;
            }
        else 
            {
                // need to return or have to proceed
            }
        // Exctracted Domain - Category
        property = iScr->GetComponentPropertyL(aComponentId,KDomainCategory());
        if (NULL != property)
            {
                property->ExternalizeL(aStream);
                                
                delete property;
                property = NULL;
            }
        // Exctracted Media ID
        property = iScr->GetComponentPropertyL(aComponentId,KMediaId());
        if (NULL != property)
            {
                property->ExternalizeL(aStream);
                
    
                delete property;
                property = NULL;
            }
        // Exctracted Setings plugin name
        property = iScr->GetComponentPropertyL(aComponentId,KSettingsPlugin());
        if (NULL != property)
            {
                property->ExternalizeL(aStream);
                
                
                delete property;
                property = NULL;
            }
        
        // Optional Properties
        // extracted info URL
        property = iScr->GetComponentPropertyL(aComponentId,KMIDletInfoURL());
        if (NULL != property)
            {
                property->ExternalizeL(aStream);
                
                
                delete property;
                property = NULL;
            }
        else // as its optional property so could be NULL 
            {
                aStream.WriteInt8L(NULL);
            }
    
        // extracted midlet description
        property = iScr->GetComponentPropertyL(aComponentId,KMIDletDescription());
        if (NULL != property)
            {
                property->ExternalizeL(aStream);
                
                
                delete property;
                property = NULL;
            }
        else // as its optional property so could be NULL 
           {
               aStream.WriteInt8L(NULL);
           }
        // extracted midlet DeletionConformation
        property = iScr->GetComponentPropertyL(aComponentId,KMIDletDeletionConformation());
        if (NULL != property)
            {
                property->ExternalizeL(aStream);
                
                
                delete property;
                property = NULL;
            }
        else // as its optional property so could be NULL 
           {
               aStream.WriteInt8L(NULL);
           }
        //extracted midlet DN URL
        property = iScr->GetComponentPropertyL(aComponentId,KDownloadURL());
        if (NULL != property)
            {  
              property->ExternalizeL(aStream);
              
              
              delete property;
              property = NULL;
            }
        else // as its optional property so could be NULL 
           {
               aStream.WriteInt8L(NULL);
           }
        
        
        // backing up files corresponds to component
        RSoftwareComponentRegistryFilesList fileList;
        CleanupClosePushL(fileList);
        TInt count = iScr->GetComponentFilesCountL(aComponentId); 
        TInt k = 0;
        RPointerArray<HBufC> files;
        fileList.OpenListL(*iScr,aComponentId);
        fileList.NextFileSetL(count,files);        
        aStream.WriteInt32L(files.Count());
        CleanupStack::PopAndDestroy(&fileList);//filelist
        
        for( k ; k < files.Count(); k++)
            {
               HBufC* temp =  files.operator [](k);
               aStream.WriteInt32L(temp->Length());
               aStream.WriteL(temp->Des());                                    
            }
        
        
        RArray<TUid> noOfApps; 
        HBufC* appIcon;
        TAppRegInfo appInfo;
        iScr->GetAppUidsForComponentL(aComponentId,noOfApps); 
        RRegistrationInfoForApplication regInfoIcon;
        CleanupClosePushL(regInfoIcon);
        RApplicationRegistrationInfo regInfoCaption;
        CleanupClosePushL(regInfoCaption);
       
        
       //backing up the no. of apps
       aStream.WriteInt32L(noOfApps.Count()); 
               
        
        for(j = 0 ; j < noOfApps.Count(); j++ )
            {
                regInfoIcon.OpenL(*iScr,noOfApps.operator [](j));
                regInfoCaption.OpenL(*iScr);
                
               //check if language is imp to give as an argument\
               // Language extracted
               TLanguage language = regInfoIcon.ApplicationLanguageL();
               // Language backed up
               aStream.WriteInt32L(language);
                                     
               // AppUID extracted
               TUid appUid = noOfApps[j];               
               // AppUID backed up
               aStream.WriteInt32L(appUid.iUid);
              
               //App caption extracted 
               regInfoCaption.GetAppInfoL(appUid,appInfo,language);
               TAppCaption caption = appInfo.Caption();
               caption.Trim();// would give the acctual size of content
               //App caption backed up
               aStream.WriteL(caption);
               
               //AppIcon path extracted
               regInfoIcon.GetAppIconL(appIcon);
               //AppIcon path backed up
               aStream.WriteL(appIcon->Des());
                     
            
               //need to backup no. of icons to needed for registrating application
               TInt noOfIcons = regInfoIcon.NumberOfOwnDefinedIconsL();
               aStream.WriteInt32L(noOfIcons);
            }
               CleanupStack::PopAndDestroy();//regInfoIcon
               CleanupStack::PopAndDestroy();//regInfoCaption
               //regInfoIcon.Close();
               //regInfoCaption.Close();

        iState = EIsWriteCompleteInfo;
        
        LOG(EBackup,EInfo," <--- GetComponentInfo() ");
            
    }
void CScrBackupUtil::RestoreScrDataL(RDesReadStream& aStream, TInt& aRestoreState, TInt& aBufferSpaceLeft)
    {
		LOG(EBackup,EInfo," ---> RestoreScrDataL() ");
        if(iIsFristCall)
            {
                ConnectScrL();
                TInt j = 0;
                TInt presentComponentsCount;
                RArray<Usif::TComponentId> presentComponentsIds;
                iBufferSpaceLeft = aBufferSpaceLeft;
                //aStream.r
                //reading components count
                iNumberOfComponents = aStream.ReadInt32L();
                iBufferSpaceLeft = iBufferSpaceLeft - sizeof(TInt32);
                
                //reading the size of the TPckgBuf<RArray<TComponentId>>
                TInt size = aStream.ReadInt32L();
                iBufferSpaceLeft = iBufferSpaceLeft - sizeof(TInt32);
                
                //reading TPckgBuf<RArray<TComponentId>> 
                HBufC* temp = HBufC::NewLC(aStream , size);
                
                TPckgBuf<RArray<TInt> > ids;
                //extracting RArray from TPckgBuf<RArray<TUids>>
                ids.Append(temp->Des());
                CleanupStack::PopAndDestroy(temp);
                iUids = ids.operator ()();// populated iUids
                iBufferSpaceLeft = iBufferSpaceLeft - size;
                iNumberOfComponents = iUids.Count();
                
                
                ExtractUidAndComponentIdL(presentComponentsIds,iPresentUids,presentComponentsCount);
                                
                // calculating the midlets to be deleted from SCR
                for(j ; j <= iNumberOfComponents ; j++ )
                    {
                          TInt id = NULL;
                          TInt index = 0;
                          id = iUids.operator [](j);
                          index = iPresentUids.Find(iUids.operator [](j));
                          if(index != NULL)
                              {
                                  presentComponentsIds.Remove(index);// removeing the app that is present in SCr and backed up too
                                  iPresentUids.Remove(index);// TODO : check if right component Id is removed
                              }       
                    }

                
                   iPresentUids.Compress();  // need to see if this works
                   presentComponentsIds.Compress();// contains the uids that has to be deleted
                   //Updated the count
                   presentComponentsCount = iPresentUids.Count();
                   for(j = 0 ; j <= presentComponentsCount ; j++ )
                       {       
                           TRAPD(err ,iScr->DeleteApplicationEntriesL(presentComponentsIds.operator [](j)));
                           if (KErrNone == err)
                               {
                                   iScr->DeleteComponentL(presentComponentsIds.operator [](j));
                               }
                            
                       }
                   
                   
            }
        RDesReadStream readStream;
        readStream.Open(iPtrToWriteBuffer);
        for( i ; i < iComponentIds.Count() ;)
            {
                if(iState == EIsFetchCompleteInfo)
                    {   
                        TUid uid ;
                        uid.iUid = iUids.operator [](i);
                        SetDataInScrL(uid,readStream);
                        i++;
                    }
                if(iState == EIsWriteCompleteInfo)
                    {                           
                        RDesReadStream readStream;
                        readStream.Open(iPtrToWriteBuffer);
                        iLengthOfString = aStream.ReadInt32L();
                        
                         
                        //Reading the remaining information into the new buffer
                        if(iBufferToWriteNextTime)
                            {
                            //appened it in the remaing
                             HBufC* remainingBuff = HBufC::NewL(iRemainingBuffer);
                             TPtr ptr(remainingBuff->Des());            
                             aStream.ReadL(ptr,iRemainingBuffer);
                             iPtrToWriteBuffer.Append(ptr);
                             iState = EIsFetchCompleteInfo;                     
                             iLengthOfString = 0; // to maintain the loop
                             iWriteToStream = EFalse;
                                                       
                            
                            }
                        if (iLengthOfString > iBufferSpaceLeft && !iBufferToWriteNextTime)
                        {  
                        iRemainingBuffer = iLengthOfString - iBufferSpaceLeft;
                        aStream.ReadL(iPtrToWriteBuffer,iBufferSpaceLeft);
                        iBufferToWriteNextTime = ETrue;
                        break ;
                        }
                        // Read the complete string
                        else if(iWriteToStream)
                        {       
                            aStream.ReadL(iPtrToWriteBuffer,iLengthOfString);
                            iBufferSpaceLeft = iBufferSpaceLeft - iLengthOfString;
                            iState = EIsFetchCompleteInfo;
                            iLengthOfString = 0; // to maintain the loop 
                            
                        }
                                                
                    }
            }
        //take care of restorestate
        // SCR restore is over; Set state to EAppArc
        if(i >= iComponentIds.Count())
            {
                aRestoreState = EAppArc;
                aBufferSpaceLeft = iBufferSpaceLeft;// need to to this coz of last iteration
            }
              
        LOG(EBackup,EInfo," <--- RestoreScrDataL() ");
    }
void CScrBackupUtil::SetDataInScrL(TUid aUid , RDesReadStream& aStream)
    {
		LOG(EBackup,EInfo," ---> SetDataInScrL() ");
        TInt tempSize = NULL;
        TInt j = 0;
        tempSize = aStream.ReadInt32L();
        CComponentEntry* entries;
        entries = CComponentEntry::NewL();
        CleanupStack::PushL( entries);
        entries->InternalizeL(aStream);
        
        
        TBool isComponentPresent = aStream.ReadInt8L();
        
        // Localizable Component info 
        RPointerArray<CLocalizableComponentInfo> localizableComponentInfoArray; // Not owned by me nee to delete in last 
        TInt temp = aStream.ReadInt32L();
        for(j ; j <= temp; j++)
            {
                CLocalizableComponentInfo* componentinfo = CLocalizableComponentInfo::NewL(aStream);
                localizableComponentInfoArray.Append(componentinfo);
            }
 
        // now need to set all the properties
        // extracted the UID
        CIntPropertyEntry* property;
        property = (CIntPropertyEntry*)CPropertyEntry::NewL(aStream);
        TInt64 id = property->Int64Value();
        delete property;
        
        //extracted midlet Domain-Catogary
        CLocalizablePropertyEntry* propertyString;
        propertyString =(CLocalizablePropertyEntry*) CPropertyEntry::NewL(aStream);
        HBufC* domainCategory  = (propertyString->StrValue()).Alloc();
        delete propertyString;
        
        // Exctracted Media ID
        property = (CIntPropertyEntry*)CPropertyEntry::NewL(aStream);
        TInt64 mediaID = property->Int64Value();
        delete property;
        
        //extracted midlet SettingsValue
        propertyString =(CLocalizablePropertyEntry*) CPropertyEntry::NewL(aStream);
        HBufC* SettingsValue  = (propertyString->StrValue()).Alloc();
        delete propertyString;
        
        // Optional Properties
        // extracted info URL
        propertyString = propertyString = (CLocalizablePropertyEntry*)CPropertyEntry::NewL(aStream);  
        HBufC* url = (propertyString->StrValue()).Alloc();
        delete propertyString;
                    
               
        // extracted midlet description
        propertyString = (CLocalizablePropertyEntry*)CPropertyEntry::NewL(aStream);
        HBufC* midletDescription  = (propertyString->StrValue()).Alloc();
        delete propertyString;
        
        // midlet deletion conformation has to be here
        CBinaryPropertyEntry* propertyBinary = (CBinaryPropertyEntry*)CPropertyEntry::NewL(aStream);
        HBufC8* midletDeletionConformation  = (propertyBinary->BinaryValue()).Alloc();
        delete propertyBinary;
        
        //extracted midlet DN URL
        propertyString =(CLocalizablePropertyEntry*) CPropertyEntry::NewL(aStream);
        HBufC* dnUrl  = (propertyString->StrValue()).Alloc();
        delete propertyString;
        
        //Extracting no. of files registered with a component
        //1st the no. of files
        RPointerArray<HBufC> componentFilesArray;
        
        TInt fileCount = aStream.ReadInt32L();
       //now the files 
        for(TInt k = 0 ; k < fileCount ; k++)
            {   
                TInt size = aStream.ReadInt32L();
                HBufC* componentFile = HBufC::NewL(aStream,size);
                CleanupStack::PushL( componentFile);
                componentFilesArray.AppendL(componentFile);
                CleanupStack::Pop( componentFile);
            }
        
        //Extracting no. of apps with a component
        TInt noOfApps = aStream.ReadInt32L();
        RPointerArray<CAppInfo> appinfoArray;
        for(j = 0 ; j < noOfApps ; j++ )
            {
                   CAppInfo* appinfo = CAppInfo::NewL();
                   CleanupStack::PushL( appinfo);// i will not own it after using has to be destroyed  
                   TLanguage language;
                   language = (TLanguage)(aStream.ReadInt32L());
                   TUid uid;
                   uid.Uid(aStream.ReadInt32L());
                   
                   TAppCaption caption ;
                   aStream.ReadL(caption);
                   HBufC* appIcon ;
                   TPtr temp(appIcon->Des());
                   aStream.ReadL(temp);
                   TInt noOfIcons = aStream.ReadInt32L();
                   
                   appinfo->Set(language,uid,caption,temp,noOfIcons);// TODO: check if it works
                   appinfoArray.Append(appinfo);
                   
                   
                   
            }
        
         
        TInt index =  iPresentUids.Find(aUid.iUid);
        if(index != NULL)
            {           CComponentEntry* entry = CComponentEntry::NewLC();
                        TUid uid ;
                        uid.Uid(iPresentUids.operator [](index));
                        TComponentId componentId = iScr->GetComponentIdForAppL(uid);
                        iScr->GetComponentL(componentId,*entry);
                        HBufC* versionPresent = entry->Version().AllocLC();
                        CleanupStack::PopAndDestroy(entry);
                        HBufC* versionBackUpComponent = entries->Version().AllocLC();
                       if((versionPresent->CompareC(versionBackUpComponent->Des())))//check version
                           {
                                   //ignore  Write a log statement as the midlet is same no need to do any thing :)
                           }    
                       else 
                           {
                             //  delete it and thn register it 
                             TRAPD(err ,iScr->DeleteApplicationEntriesL(componentId));
                             if (KErrNone == err)
                                  {
                                      iScr->DeleteComponentL(componentId);
                                  }
                            // call register
                            TComponentId newComponentId = RegisterComponentL(*entries,uid,url->Des(),midletDescription->Des(),midletDeletionConformation->Des(),dnUrl->Des(),mediaID,domainCategory->Des(),SettingsValue->Des(),componentFilesArray,isComponentPresent,localizableComponentInfoArray);
                             for(j=0; j < appinfoArray.Count() ; j++)
                                 {
                                     CAppInfo* appInfo = appinfoArray.operator [](j);
                                     
                                     RegisterApplicationL(newComponentId,appInfo->GetAppUid(),appInfo->GetCaption(),appInfo->GetAppIcon()->Des(),appInfo->NoOfIcons(),appInfo->GetLanguage());
                                     CleanupStack::PopAndDestroy(appInfo);
                                 }
                           }
                       CleanupStack::PopAndDestroy(versionPresent);
                       CleanupStack::PopAndDestroy(versionBackUpComponent);
            }
        else
            {
                //just go register it as its not there in the phone 
                TComponentId newComponentId = RegisterComponentL(*entries,aUid,url->Des(),midletDescription->Des(),midletDeletionConformation->Des(),dnUrl->Des(),mediaID,domainCategory->Des(),SettingsValue->Des(),componentFilesArray,isComponentPresent,localizableComponentInfoArray);
                 for(j=0; j < appinfoArray.Count() ; j++)
                     {
                         CAppInfo* appInfo = appinfoArray.operator [](j);
                         
                         RegisterApplicationL(newComponentId,appInfo->GetAppUid(),appInfo->GetCaption(),appInfo->GetAppIcon()->Des(),appInfo->NoOfIcons(),appInfo->GetLanguage());
                         CleanupStack::PopAndDestroy(appInfo);
                     }
            }
        
        //CleanupStack::Pop();//componentFilesArray
        CleanupStack::PopAndDestroy( entries);
        
        LOG(EBackup,EInfo," <--- SetDataInScrL() ");
    }


TComponentId CScrBackupUtil::RegisterComponentL(CComponentEntry& aEntry ,TUid aUid,TPtr aMidletInfoUrl,
        TPtr aMidletDescription,TPtr8 aMidletDeletionConformation, TPtr aDownloadUrl,TInt64 aMediaId,TPtr aDomainCatogary , TPtr aSettingValue,
        RPointerArray<HBufC>& aComponentFilesArray, TBool aIsComponentPresent,RPointerArray<CLocalizableComponentInfo>& aLocalizableComponentInfoArray)
{
	LOG(EBackup,EInfo," ---> RegisterComponentL() ");
    HBufC* globalId =( aEntry.GlobalId()).AllocLC();
   
    TComponentId componentId = iScr->AddComponentL(aLocalizableComponentInfoArray,Usif::KSoftwareTypeJava,globalId);
   
    
    
    iScr->SetComponentVersionL(componentId, aEntry.Version());
    //LOG1(EJavaInstaller, EInfo, "RegisterComponentL: version set %s",*version);
    
    iScr->SetIsComponentRemovableL(componentId, aEntry.IsRemovable());
    
    
    iScr->SetIsComponentDrmProtectedL(componentId, aEntry.IsDrmProtected());
    
    
    iScr->SetIsComponentKnownRevokedL(componentId,aEntry.IsKnownRevoked());
    
    
    iScr->SetIsComponentOriginVerifiedL(componentId, aEntry.IsOriginVerified());
    
    
    iScr->SetComponentSizeL(componentId, aEntry.ComponentSize());
    
    
    iScr->SetScomoStateL(componentId,aEntry.ScomoState());
    
    
    iScr->SetIsComponentPresentL(componentId,aIsComponentPresent);
    
    
    
    iScr->SetComponentPropertyL(componentId,KUid(), aUid.iUid);
    
    
    iScr->SetComponentPropertyL(componentId,KMIDletInfoURL(),aMidletInfoUrl);
    
    iScr->SetComponentPropertyL(componentId, KMIDletDescription(), aMidletDescription);
    
    iScr->SetComponentPropertyL(componentId,KMIDletDeletionConformation(),aMidletDeletionConformation);
        
    iScr->SetComponentPropertyL(componentId, KDownloadURL(), aDownloadUrl);
    
    iScr->SetComponentPropertyL(componentId,KDomainCategory(),aDomainCatogary);
    
    iScr->SetComponentPropertyL(componentId, KMediaId(), aMediaId);
    
    
    iScr->SetComponentPropertyL(componentId, KSettingsPlugin(), aSettingValue);
    

    CleanupStack::PopAndDestroy(globalId);
  
    TInt fileCount = aComponentFilesArray.Count();
    for (TInt i = 0; i < fileCount; i++)
    {
        HBufC *componentFile = aComponentFilesArray.operator [](i);
        iScr->RegisterComponentFileL(componentId, *componentFile);
        CleanupStack::PopAndDestroy(componentFile);
    }
    LOG(EBackup,EInfo," <--- RegisterComponentL() ");
    return componentId;
}

void CScrBackupUtil::RegisterApplicationL(TInt aComponentId, TUid aAppUid,
        TAppCaption aAppName, TDesC aIconFilename, TInt aNumberOfIcons,
        TLanguage aLanguages)
    {
		LOG(EBackup,EInfo," ---> RegisterApplicationL() ");
        TFileName appName;
        RPointerArray<HBufC> ownedFileArray; // codescanner::resourcenotoncleanupstack
        RPointerArray<Usif::CServiceInfo> serviceArray; // codescanner::resourcenotoncleanupstack
        RPointerArray<Usif::CPropertyEntry> appPropertiesArray; // codescanner::resourcenotoncleanupstack
        RPointerArray<Usif::CAppViewData> viewDataList; // codescanner::resourcenotoncleanupstack

        RPointerArray<Usif::COpaqueData> opaqueDataArray;
        CleanupResetAndDestroyPushL(opaqueDataArray);
        // Write application Uid to opaque data (needed by MIDlet launcher).
        TBuf8<4> opaqueDataBuf; // Opaque data will contain one signed 32-bit int.
        RDesWriteStream writeStream(opaqueDataBuf);
        writeStream.WriteInt32L(aAppUid.iUid);
        writeStream.CommitL();
        
       
        
        COpaqueData *opaqueData = COpaqueData::NewLC(opaqueDataBuf, KUnspecifiedLocale);
        opaqueDataArray.AppendL(opaqueData);
        CleanupStack::Pop(opaqueData);

        RPointerArray<Usif::CLocalizableAppInfo> localizableAppInfoList;
        CleanupResetAndDestroyPushL(localizableAppInfoList);
        // Add non-localized application name (caption) and icon.

        CCaptionAndIconInfo *captionAndIconInfo = CCaptionAndIconInfo::NewLC(aAppName,aIconFilename,aNumberOfIcons);
        
        CLocalizableAppInfo *locAppInfo = CLocalizableAppInfo::NewLC(/*aShortCaption=*/ KNullDesC,KNonLocalized,
            /*aGroupName=*/ KNullDesC,captionAndIconInfo,viewDataList);
        localizableAppInfoList.AppendL(locAppInfo);
        
        CleanupStack::Pop(locAppInfo);
        CleanupStack::Pop(captionAndIconInfo);
               
    CCaptionAndIconInfo *tmpCaptionAndIconInfo =CCaptionAndIconInfo::NewLC(aAppName,/*aIconFileName=*/ KNullDesC,/*aNumOfAppIcons=*/ 0);
    CLocalizableAppInfo *tmpLocAppInfo = CLocalizableAppInfo::NewLC(/*aShortCaption=*/ KNullDesC,aLanguages,/*aGroupName=*/ KNullDesC,
                                                                        tmpCaptionAndIconInfo,/*aViewDataList=*/ viewDataList);
                localizableAppInfoList.AppendL(tmpLocAppInfo);
               
                CleanupStack::Pop(tmpLocAppInfo);
                CleanupStack::Pop(tmpCaptionAndIconInfo);
            
            
        
        // Create application registration data objects.
        TApplicationCharacteristics appCharacteristics;
        appCharacteristics.iAttributes = TApaAppCapability::ENonNative;
        appCharacteristics.iEmbeddability = TApplicationCharacteristics::ENotEmbeddable;
        appCharacteristics.iSupportsNewFile = EFalse;
        appCharacteristics.iAppIsHidden = EFalse;
        appCharacteristics.iLaunchInBackground = EFalse;
        
        CApplicationRegistrationData *appRegData =
        CApplicationRegistrationData::NewLC(ownedFileArray, serviceArray,localizableAppInfoList,appPropertiesArray,opaqueDataArray,
                /*aAppUid=*/ aAppUid, /*aAppFile=*/ appName,/*aCharacteristics=*/ appCharacteristics,/*aDefaultScreenNumber=*/ 0);
        
        LOG(EJavaInstaller, EInfo,"RegisterApplicationL : Called AddApplicationEntryL  >>>>>>>>>>>> ");
        iScr->AddApplicationEntryL(aComponentId, *appRegData);
        CleanupStack::PopAndDestroy(appRegData);

      
        CleanupStack::Pop(&localizableAppInfoList); // deleted in appRegData destructor
        CleanupStack::Pop(&opaqueDataArray); // deleted in appRegData destructor

        LOG(EBackup,EInfo," <--- RegisterApplicationL() ");
    }

CScrBackupUtil::~CScrBackupUtil()
    {
    LOG(EBackup,EInfo," ---> ~CScrBackupUtil() ");
    
    if(iScr)
        {
            iScr->Close();
            delete iScr;
            iScr = NULL;
        }
    if(iRemainingInfo)
        {
            delete iRemainingInfo;
            iRemainingInfo = NULL;
        }
            
    LOG(EBackup,EInfo," <--- ~CScrBackupUtil() ");
    }

CAppInfo* CAppInfo::NewL()
    {
    LOG(EBackup,EInfo," ---> CAppInfo:NewL()");
    CAppInfo *self = CAppInfo::NewLC();
    CleanupStack::Pop(self);
    LOG(EBackup,EInfo," <--- CAppInfo:NewL()");
    return self;
    }


CAppInfo* CAppInfo::NewLC()
    {
    LOG(EBackup,EInfo," ---> CAppInfo:NewLC");
    CAppInfo *self = new(ELeave) CAppInfo();
    CleanupStack::PushL(self); 
    self->ConstructL();
    LOG(EBackup,EInfo," <---CAppInfo:NewLC");
    return self;
    }
CAppInfo::CAppInfo()
    {

    }
void CAppInfo::ConstructL()
    {
        LOG(EBackup,EInfo," ---> CAppInfo:ConstructL ");
        
        
        
        LOG(EBackup,EInfo," <--- CAppInfo:ConstructL()");
    }
CAppInfo::~CAppInfo()
    {
    LOG(EBackup,EInfo," ---> CAppInfo:~CAppInfo() ");
    if(iAppIcon)
        {
            delete iAppIcon;
        }
    LOG(EBackup,EInfo," <--- CAppInfo:~CAppInfo() ");
    }
    
void CAppInfo::Set(TLanguage language,TUid appUid,Usif::TAppCaption caption,TDesC& appIcon,TInt aNoOfIcons)
    {
        LOG(EBackup,EInfo," ---> CAppInfo:Set() ");
        iLanguage = language;
        iAppUid = appUid;
        iCaption = caption;
        iAppIcon->Des().Copy(appIcon);
        iNoOfIcons = aNoOfIcons;
        LOG(EBackup,EInfo," <--- CAppInfo:Set() ");
        
    }
TLanguage CAppInfo::GetLanguage()
    {
    LOG(EBackup,EInfo," ---> CAppInfo:GetLanguage() ");
    LOG(EBackup,EInfo," <--- CAppInfo:GetLanguage() ");
    return iLanguage;
    }
TUid CAppInfo::GetAppUid()
    {
    LOG(EBackup,EInfo," ---> CAppInfo:GetAppUid() ");
    LOG(EBackup,EInfo," <--- CAppInfo:GetAppUid() ");
    return iAppUid;
    }
Usif::TAppCaption CAppInfo::GetCaption()
    {
    LOG(EBackup,EInfo," ---> CAppInfo:GetCaption() ");
    LOG(EBackup,EInfo," <--- CAppInfo:GetCaption() ");
    return iCaption;
   
    }
HBufC* CAppInfo::GetAppIcon()
    {
    LOG(EBackup,EInfo," ---> CAppInfo:GetAppIcon() ");
    LOG(EBackup,EInfo," <--- CAppInfo:GetAppIcon() ");
    return iAppIcon;
    
    }

TInt CAppInfo::NoOfIcons()
    {
    LOG(EBackup,EInfo," ---> CAppInfo:NoOfIcons() ");
    LOG(EBackup,EInfo," <--- CAppInfo:NoOfIcons() ");
    return iNoOfIcons;
    }
