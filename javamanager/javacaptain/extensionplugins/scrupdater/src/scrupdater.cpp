/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ScrUpdater is Java Captain Symbian plugin that updates
*               presence information of Java Applications in USIF SCR
*               when removable drive is added or removed to the device.
*
*/

#include <apgcli.h>
#include <e32base.h>
#include <f32file.h>

#include "javaprocessconstants.h"
#include "javasymbianoslayer.h"
#include "javauids.h"
#include "logger.h"
#include "coreinterface.h"
#include "booteventprovidermessages.h"
#include "mmceventprovidermessages.h"

#include "scrupdater.h"

using namespace Usif;

/**
 * Return pointer to ExtensionPluginInterface implementation for this
 * extension dll
 */
java::captain::ExtensionPluginInterface* getExtensionPlugin()
{
    return new java::captain::ScrUpdater();
}

namespace java
{
namespace captain
{

using java::fileutils::driveInfo;
using java::fileutils::DriveListenerInterface;

/**
 * Empty contructor
 */
ScrUpdater::ScrUpdater() : mCore(0)
{
}

/**
 * Empty destructor
 */
ScrUpdater::~ScrUpdater()
{
}

/**
 * Implement PluginInterface method
 */
void ScrUpdater::startPlugin(CoreInterface* core)
{
    LOG(EJavaCaptain, EInfo, "ScrUpdater plugin started");

    mCore = core;
}

/**
 * Implement PluginInterface method
 */
void ScrUpdater::stopPlugin()
{
    mCore = 0;
}

/**
 * Implement ExtensionPluginInterface method
 */
EventConsumerInterface* ScrUpdater::getEventConsumer()
{
    return this;
}

/**
 * Handle Java Captain events sent by Boot event provider or
 * MMC event provider.
 *
 * Implement EventConsumerInterface method
 */
void ScrUpdater::event(const std::string& eventProvider,
                       java::comms::CommsMessage& aMsg)
{
    if (eventProvider == BOOT_EVENT_PROVIDER)
    {
        int bootType = NORMAL_BOOT_C;
        getBootMessageParams(aMsg, bootType);
        LOG1(
            EJavaCaptain,
            EInfo,
            "ScrUpdater::event() boot event received (type=%d)",
            bootType);
        switch (bootType)
        {
            case IAD_BOOT_C:
            case FIRST_DEVICE_BOOT_C:
            case NORMAL_BOOT_C:
            {
                // Update presence information
                TRAPD(err, initializeScrPresenceInfoL())
                if (KErrNone != err)
                {
                    ELOG1(EJavaCaptain, "initializeScrPresenceInfoL: leaved (%d)", err);
                }
            }
            break;

            default:
            {
                WLOG1(EJavaCaptain,
                    "DriveListenerInterface: event() unknown boot event (type=%d)", bootType);
            }
            break;
        }
    }
    else if (eventProvider == MMC_EVENT_PROVIDER)
    {
        int operation = 0;
        driveInfo di;
        getMmcChangedMessageParams(aMsg, operation, di);
        LOG1(
            EJavaCaptain,
            EInfo,
            "ScrUpdater::event() mmc event received (operation=%d)",
            operation);

        switch (operation)
        {
            case DriveListenerInterface::REMOVABLE_MEDIA_REMOVED_C:
            {
                // All Java applications in the removed drive are set
                // to 'not present' state
                TRAPD(err, removeScrPresencesL(&di));
                if (KErrNone != err)
                {
                    ELOG1(EJavaCaptain, "removeScrPresencesL leaved (%d)", err);
                }
            }
            break;

            case DriveListenerInterface::REMOVABLE_MEDIA_INSERTED_C:
            {
                // Those Java applications in the drive to where the media
                // (e.g. memory card) was added are set to 'present' state
                // IF the media id is correct (in other words if the same
                // memory card that they have been installed to is added
                // to the drive).
                TRAPD(err, addScrPresencesL(&di));
                if (KErrNone != err)
                {
                    ELOG1(EJavaCaptain, "addScrPresencesL leaved (%d)", err);
                }
            }
            break;
        }
    }
}

/**
 * Set the presence state of all Java applications installed
 * to the removable drive specified in aInfo to not present
 */
void ScrUpdater::removeScrPresencesL(driveInfo *aInfo)
{
    __UHEAP_MARK;
    LOG1WSTR(EJavaCaptain, EInfo,
        "removeScrPresencesL: driveInfo root path is %s", aInfo->iRootPath);

    RSoftwareComponentRegistry *pScr = createScrL();
    CleanupStack::PushL(pScr);

    // Get ids of all Java components in scr
    RArray<TComponentId> componentIdList;
    CComponentFilter *pJavaSwTypeFilter = CComponentFilter::NewLC();
    pJavaSwTypeFilter->SetSoftwareTypeL(Usif::KSoftwareTypeJava);

    pScr->GetComponentIdsL(componentIdList, pJavaSwTypeFilter);
    CleanupStack::PopAndDestroy(pJavaSwTypeFilter);
    CleanupClosePushL(componentIdList);

    // For each component check whether it has been installed
    // to the removed drive
    TInt  nComponents = componentIdList.Count();
    TUint removedDrive = (TUint)(aInfo->iRootPath[0]);
    // Now removedDrive contains the drive letter, convert it to drive number 0-25
    if ((removedDrive > 64) && (removedDrive < 91))
    {
        // 'A' - 'Z'
        removedDrive -= 65;
    }
    else if ((removedDrive > 96) && (removedDrive < 123))
    {
        // 'a' - 'z'
        removedDrive -= 97;
    }
    else
    {
        ELOG1WSTR(EJavaCaptain,
            "removeScrPresencesL: Unexpected root path in remove drive info %s",
            aInfo->iRootPath);
        CleanupStack::PopAndDestroy(pScr);
        return;
    }

    LOG2(EJavaCaptain, EInfo, "Number of Java components is %d, removed drive is %d",
        nComponents, removedDrive);

    TBool fPresenceChange = EFalse;
    RArray<TApaAppUpdateInfo> removedApps;
    CleanupClosePushL(removedApps);

    for (TInt nInd = 0; nInd < nComponents; nInd++)
    {
        CComponentEntry *pEntry = CComponentEntry::NewLC();
        if (!(pScr->GetComponentL(componentIdList[nInd], *pEntry)))
        {
            ELOG1(EJavaCaptain,
                "removeScrPresencesL: SCR GetComponentIdsL returned id %d "
                "but GetComponentL does not find it", componentIdList[nInd]);
            CleanupStack::PopAndDestroy(pEntry);
            continue;
        }

        TInt nInstalledDrives = pEntry->InstalledDrives().Length();
        if (nInstalledDrives <= removedDrive)
        {
            // SCR InstalledDrives should be array of 26 elements (value 0 or 1)
            ELOG2(EJavaCaptain,
                "removeScrPresencesL: The length of InstalledDrives array (%d) "
                "is smaller than removedDrive (%d)", nInstalledDrives, removedDrive);
            CleanupStack::PopAndDestroy(pEntry);
            continue;
        }

        LOG1(EJavaCaptain, EInfo, "Java component id %d", componentIdList[nInd]);

        if (pEntry->InstalledDrives()[removedDrive])
        {
            // This component has been installed to the drive
            // that has just been removed
            pScr->SetIsComponentPresentL(componentIdList[nInd], EFalse);

            LOG1(EJavaCaptain, EInfo,
                "removeScrPresencesL: set component %d to not present",
                componentIdList[nInd]);

            fPresenceChange = ETrue;

            // Gather the Uids of all applications that are no longer present
            RArray<TUid> appsInComponent;
            CleanupClosePushL(appsInComponent);
            pScr->GetAppUidsForComponentL(
                componentIdList[nInd], appsInComponent);
            for (TInt nInd2 = 0; nInd2 < appsInComponent.Count(); nInd2++)
            {
                TApaAppUpdateInfo appInfo;
                appInfo.iAppUid = appsInComponent[nInd2];
                appInfo.iAction = TApaAppUpdateInfo::EAppNotPresent;
                (void)removedApps.Append(appInfo);
            }
            CleanupStack::PopAndDestroy(&appsInComponent);
        }

        CleanupStack::PopAndDestroy(pEntry);
    }

    // Tell AppArc which applications are no longer present
    while (fPresenceChange)
    {
        if (removedApps.Count() == 0)
        {
            ELOG(EJavaCaptain, "removeScrPresencesL: Uids of the removed apps are not known");
            break;
        }

        RApaLsSession apaSession;
        TInt err = apaSession.Connect();
        if (KErrNone != err)
        {
            ELOG1(EJavaCaptain, "removeScrPresencesL: Error %d when connecting AppArc", err);
            break;
        }
        else
        {
            CleanupClosePushL(apaSession);
            apaSession.UpdateAppListL(removedApps);
            CleanupStack::PopAndDestroy(); // closes apaSession
            fPresenceChange = EFalse;
        }
    }

    CleanupStack::PopAndDestroy(); // Close removedApps
    CleanupStack::PopAndDestroy(); // Close componentIdList
    CleanupStack::PopAndDestroy(pScr);
    __UHEAP_MARKEND;
}


/**
 * Set the presence state of all Java applications installed
 * to the removable drive specified in aInfo to present
 */
void ScrUpdater::addScrPresencesL(driveInfo *aInfo)
{
    __UHEAP_MARK;
    LOG1WSTR(EJavaCaptain, EInfo,
        "addScrPresencesL: driveInfo root path is %s", aInfo->iRootPath);

    RSoftwareComponentRegistry *pScr = createScrL();
    CleanupStack::PushL(pScr);

    // Get ids of all Java components in scr
    RArray<TComponentId> componentIdList;
    CComponentFilter *pJavaSwTypeFilter = CComponentFilter::NewLC();
    pJavaSwTypeFilter->SetSoftwareTypeL(Usif::KSoftwareTypeJava);

    pScr->GetComponentIdsL(componentIdList, pJavaSwTypeFilter);
    CleanupStack::PopAndDestroy(pJavaSwTypeFilter);
    CleanupClosePushL(componentIdList);

    // For each component check whether it has been installed
    // to the added drive AND whether the media id is correct
    // (in other words if the actual memory card where the component
    // has been installed to is added to the drive).
    TInt  nComponents  = componentIdList.Count();
    TUint addedMediaId = (TUint)(aInfo->iId);
    TUint addedDrive   = (TUint)(aInfo->iRootPath[0]);
    // Now addedDrive contains the drive letter, convert it to drive number 0-25
    if ((addedDrive > 64) && (addedDrive < 91))
    {
        // 'A' - 'Z'
        addedDrive -= 65;
    }
    else if ((addedDrive > 96) && (addedDrive < 123))
    {
        // 'a' - 'z'
        addedDrive -= 97;
    }
    else
    {
        ELOG1WSTR(EJavaCaptain,
            "addScrPresencesL: Unexpected root path in add drive info %s",
            aInfo->iRootPath);
        CleanupStack::PopAndDestroy(pScr);
        return;
    }

    LOG2(EJavaCaptain, EInfo, "Number of Java components is %d, added drive is %d",
        nComponents, addedDrive);

    TBool fPresenceChange = EFalse;
    RArray<TApaAppUpdateInfo> addedApps;
    CleanupClosePushL(addedApps);

    for (TInt nInd = 0; nInd < nComponents; nInd++)
    {
        CComponentEntry *pEntry = CComponentEntry::NewLC();
        if (!(pScr->GetComponentL(componentIdList[nInd], *pEntry)))
        {
            ELOG1(EJavaCaptain,
                "addScrPresencesL: SCR GetComponentIdsL returned id %d "
                "but GetComponentL does not find it", componentIdList[nInd]);
            CleanupStack::PopAndDestroy(pEntry);
            continue;
        }

        // When Java Installer registers Java app to SCR it stores also
        // the media id using SetComponentPropertyL(TComponentId aComponentId,
        // _L("Media-Id")), TInt64 aValue);  (aValue is actually 32 bit int)
        CIntPropertyEntry* pMediaIdProperty = (CIntPropertyEntry *)
            pScr->GetComponentPropertyL(componentIdList[nInd],_L("Media-Id"));
        if (NULL == pMediaIdProperty)
        {
            ELOG1(EJavaCaptain,
                "addScrPresencesL: media_id property not found for component %d",
                componentIdList[nInd]);
            CleanupStack::PopAndDestroy(pEntry);
            continue;
        }
        CleanupStack::PushL(pMediaIdProperty);

        TInt nInstalledDrives = pEntry->InstalledDrives().Length();
        if (nInstalledDrives <= addedDrive)
        {
            // SCR InstalledDrives should be array of 26 elements (value 0 or 1)
            ELOG2(EJavaCaptain,
                "addScrPresencesL: The length of InstalledDrives array (%d) "
                "is smaller than addedDrive (%d)", nInstalledDrives, addedDrive);
            CleanupStack::PopAndDestroy(pEntry);
            continue;
        }

        LOG1(EJavaCaptain, EInfo, "Java component id %d", componentIdList[nInd]);

        if (pEntry->InstalledDrives()[addedDrive])
        {
            // This component has been installed to the drive
            // that has just been added.
            // Now check whether the media id of the added media
            // is OK for this component.
            if (addedMediaId == pMediaIdProperty->IntValue())
            {
                pScr->SetIsComponentPresentL(componentIdList[nInd], ETrue);

                LOG1(EJavaCaptain, EInfo,
                    "addScrPresencesL: set component %d to present",
                    componentIdList[nInd]);

                fPresenceChange = ETrue;

                // Gather the Uids of all 'new' applications that are now present
                RArray<TUid> appsInComponent;
                CleanupClosePushL(appsInComponent);
                pScr->GetAppUidsForComponentL(
                    componentIdList[nInd], appsInComponent);
                for (TInt nInd2 = 0; nInd2 < appsInComponent.Count(); nInd2++)
                {
                    TApaAppUpdateInfo appInfo;
                    appInfo.iAppUid = appsInComponent[nInd2];
                    appInfo.iAction = TApaAppUpdateInfo::EAppPresent;
                    (void)addedApps.Append(appInfo);
                }
                CleanupStack::PopAndDestroy(&appsInComponent);
            }
        }

        CleanupStack::PopAndDestroy(pMediaIdProperty);
        CleanupStack::PopAndDestroy(pEntry);
    }

    // Tell AppArc which 'new' applications are now present
    while (fPresenceChange)
    {
        if (addedApps.Count() == 0)
        {
            ELOG(EJavaCaptain, "addScrPresencesL: Uids of the 'new' apps are not known");
            break;
        }

        RApaLsSession apaSession;
        TInt err = apaSession.Connect();
        if (KErrNone != err)
        {
            ELOG1(EJavaCaptain, "addScrPresencesL: Error %d when connecting AppArc", err);
            break;
        }
        else
        {
            CleanupClosePushL(apaSession);
            apaSession.UpdateAppListL(addedApps);
            CleanupStack::PopAndDestroy(); // closes apaSession
            fPresenceChange = EFalse;
        }
    }

    CleanupStack::PopAndDestroy(); // Close addedApps
    CleanupStack::PopAndDestroy(); // Close componentIdList
    CleanupStack::PopAndDestroy(pScr);
    __UHEAP_MARKEND;
}


/**
 * Loop through all removable drives and get the media id of
 * the memory card or other removable media in the drive and update
 * presence information of all Java applications installed
 * to removable drives accordingly.
 */
void ScrUpdater::initializeScrPresenceInfoL()
{
    __UHEAP_MARK;
    RFs fs;
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);

    // Which drives are present and what is the media id of
    // each removable volume
    TInt  err = KErrNone;
    TInt  err2 = KErrNone;
    TBool drivePresent[EDriveZ + 1];
    TUint driveMediaId[EDriveZ + 1];
    TVolumeInfo volumeInfo;
    TDriveInfo  driveInfo;

    for (TInt nInd = EDriveA; nInd < EDriveZ; nInd++)
    {
        err = fs.Volume(volumeInfo, nInd);
        if (KErrNone == err)
        {
            drivePresent[nInd] = ETrue;
            driveMediaId[nInd] = volumeInfo.iUniqueID;
            // If the media is not removable, media id is not checked
            err2 = fs.Drive(driveInfo, nInd);
            if (KErrNone != err2)
            {
                ELOG1(EJavaCaptain,
                    "initializeScrPresenceInfoL: error (%d) when trying to get drive info",
                    err2);
                User::Leave(err2);
            }
            else
            {
                if (!(driveInfo.iDriveAtt & KDriveAttRemovable))
                {
                    driveMediaId[nInd] = 0;
                }
            }
        }
        else if (KErrNotReady == err)
        {
            // no volume in this drive
            drivePresent[nInd] = EFalse;
            driveMediaId[nInd] = 0;
        }
        else
        {
            ELOG1(EJavaCaptain,
                "initializeScrPresenceInfoL: error (%d) when trying to get volume info",
                err);
            User::Leave(err);
        }
    }
    CleanupStack::PopAndDestroy(); // close RFs


    RSoftwareComponentRegistry *pScr = createScrL();
    CleanupStack::PushL(pScr);

    // Get ids of all Java components in scr
    RArray<TComponentId> componentIdList;
    CComponentFilter *pJavaSwTypeFilter = CComponentFilter::NewLC();
    pJavaSwTypeFilter->SetSoftwareTypeL(Usif::KSoftwareTypeJava);

    pScr->GetComponentIdsL(componentIdList, pJavaSwTypeFilter);
    CleanupStack::PopAndDestroy(pJavaSwTypeFilter);
    CleanupClosePushL(componentIdList);

    // For each component check whether the drive it has been installed
    // to is present AND whether the media id is correct
    TInt  nComponents  = componentIdList.Count();

    LOG1(EJavaCaptain, EInfo, "initializeScrPresenceInfoL: Number of Java components is %d",
        nComponents);

    for (TInt nInd = 0; nInd < nComponents; nInd++)
    {
        CComponentEntry *pEntry = CComponentEntry::NewLC();
        if (!(pScr->GetComponentL(componentIdList[nInd], *pEntry)))
        {
            ELOG1(EJavaCaptain,
                "initializeScrPresenceInfoL: SCR GetComponentIdsL returned id %d "
                "but GetComponentL does not find it", componentIdList[nInd]);
            CleanupStack::PopAndDestroy(pEntry);
            continue;
        }

        CIntPropertyEntry* pMediaIdProperty = (CIntPropertyEntry *)
            pScr->GetComponentPropertyL(componentIdList[nInd],_L("Media-Id"));
        if (NULL == pMediaIdProperty)
        {
            ELOG1(EJavaCaptain,
                "initializeScrPresenceInfoL: media_id property not found for component %d",
                componentIdList[nInd]);
            CleanupStack::PopAndDestroy(pEntry);
            continue;
        }
        CleanupStack::PushL(pMediaIdProperty);

        TInt nInstalledDrives = pEntry->InstalledDrives().Length();
        if (nInstalledDrives > (EDriveZ + 1))
        {
            WLOG2(EJavaCaptain,
                "initializeScrPresenceInfoL: too big (%d) installed drives array for "
                "component %d", nInstalledDrives, componentIdList[nInd]);
            nInstalledDrives = EDriveZ;
        }
        // When Java components are installed, only one installed drive
        // and corresponding media id are registered.
        TInt installationDrive = -1;

        for (TInt driveNumber = EDriveA; driveNumber < nInstalledDrives; driveNumber++)
        {
            if (pEntry->InstalledDrives()[driveNumber])
            {
                installationDrive = driveNumber;
                break;
            }
        }

        if (installationDrive == -1)
        {
            ELOG1(EJavaCaptain,
                "initializeScrPresenceInfoL: component (id %d) did not have installed drive info",
                componentIdList[nInd]);
            CleanupStack::PopAndDestroy(pMediaIdProperty);
            CleanupStack::PopAndDestroy(pEntry);
            continue;
        }

        if (drivePresent[installationDrive])
        {
            // Check also the media id
            if (driveMediaId[installationDrive] == pMediaIdProperty->IntValue())
            {
                LOG1(EJavaCaptain, EInfo,
                    "initializeScrPresenceInfoL: set component %d to present",
                    componentIdList[nInd]);

                pScr->SetIsComponentPresentL(componentIdList[nInd], ETrue);
            }
            else
            {
                LOG1(EJavaCaptain, EInfo,
                    "initializeScrPresenceInfoL: set component %d to NOT present",
                    componentIdList[nInd]);

                pScr->SetIsComponentPresentL(componentIdList[nInd], EFalse);
            }
        }
        else
        {
            LOG1(EJavaCaptain, EInfo,
                "initializeScrPresenceInfoL: set component %d to NOT present",
                componentIdList[nInd]);

            // drive is not present -> Java component installed to that
            // drive is not present
            pScr->SetIsComponentPresentL(componentIdList[nInd], EFalse);
        }

        CleanupStack::PopAndDestroy(pMediaIdProperty);
        CleanupStack::PopAndDestroy(pEntry);
    }

    CleanupStack::PopAndDestroy(); // Close componentIdList
    CleanupStack::PopAndDestroy(pScr); // Also closes RSoftwareComponentRegistry

    __UHEAP_MARKEND;
}


/**
 * Creates an instance of RSoftwareComponentRegistry and connects to it.
 */
RSoftwareComponentRegistry *ScrUpdater::createScrL()
{
    RSoftwareComponentRegistry *pScr = new RSoftwareComponentRegistry;
    if (NULL == pScr)
    {
        ELOG(EJavaInstaller,
            "CreateScrL: Creating RSoftwareComponentRegistry failed");
        User::Leave(KErrGeneral);
    }
    TInt err = pScr->Connect();
    if (KErrNone != err)
    {
        ELOG1(EJavaInstaller,
            "CreateScrL: Connecting to RSoftwareComponentRegistry failed, error %d",
            err);
        delete pScr;
        User::Leave(err);
    }

    return pScr;
}


} // namespace captain
} // namespace java
