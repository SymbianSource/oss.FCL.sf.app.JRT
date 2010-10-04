/*******************************************************************************
 * Copyright (c) 2009, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - initial implementation
 *******************************************************************************/

#include <memory>
#include <badesca.h>
#include <hbsymbianvariant.h>
#include <HbIndicatorsymbian.h>

#include <javastorageentry.h>
#include <javastorage.h>
#include <javastoragenames.h>
#include <JavaCommonUtils.h>
#include <JavaUid.h>
#include <javasymbianoslayer.h>
#include <booteventprovidermessages.h>
#include <logger.h>

#include "javasoftnotehandler.h"

using namespace std;
using namespace java::storage;
using namespace java::util;
_LIT(KIndicatorTypeJavaSoftNote,"com.nokia.javasoftnotification.indicatorplugin/1.0_%u_%i");
const TInt KIndicatorTypeStringLength = 250;

// ----------------------------------------------------------------------------
// javasoftnotehandler::getExtensionPlugin
// ----------------------------------------------------------------------------
#ifdef __SYMBIAN32__
ExtensionPluginInterface* getExtensionPlugin()
    {
#else
    extern "C" ExtensionPluginInterface* getExtensionPlugin()
        {
#endif
    return new javasoftnotehandler();
    }

// ----------------------------------------------------------------------------
// javasoftnotehandler::javasoftnotehandler
// ----------------------------------------------------------------------------
OS_EXPORT javasoftnotehandler::javasoftnotehandler() 
    {
    JELOG2(EJavaCaptain);
    }

// ----------------------------------------------------------------------------
// javasoftnotehandler::~javasoftnotehandler
// ----------------------------------------------------------------------------
OS_EXPORT javasoftnotehandler::~javasoftnotehandler()
    {
    JELOG2(EJavaCaptain);
    }

// ----------------------------------------------------------------------------
// javasoftnotehandler::startPlugin
// ----------------------------------------------------------------------------
void javasoftnotehandler::startPlugin(CoreInterface* /*core*/)
    {
    JELOG2(EJavaCaptain);
    }

// ----------------------------------------------------------------------------
// javasoftnotehandler::stopPlugin
// ----------------------------------------------------------------------------
 void javasoftnotehandler::stopPlugin()
    {
    JELOG2(EJavaCaptain);
    }

// ----------------------------------------------------------------------------
// javasoftnotehandler::getEventConsumer
// ----------------------------------------------------------------------------
EventConsumerInterface* javasoftnotehandler::getEventConsumer()
    {
    JELOG2(EJavaCaptain);
    return this;
    }

// ----------------------------------------------------------------------------
// javasoftnotehandler::getApplicationManagementListener
// ----------------------------------------------------------------------------
ApplicationManagementEventsInterface* javasoftnotehandler::getApplicationManagementListener()
    {
    JELOG2(EJavaCaptain);
    return this;
    }

// ----------------------------------------------------------------------------
// javasoftnotehandler::event
// ----------------------------------------------------------------------------
void javasoftnotehandler::event(const std::string& eventProvider,
        java::comms::CommsMessage& /*aMsg*/)
    {
    LOG1(EJavaCaptain, EInfo, "javasoftnotehandler::event(), aEvent.event = %s", eventProvider.c_str());
    if (eventProvider == BOOT_EVENT_PROVIDER)
        {
        TRAP_IGNORE(createIndicatorsL());
        }
    }

// ----------------------------------------------------------------------------
// javasoftnotehandler::amAdded
// ----------------------------------------------------------------------------
void javasoftnotehandler::amAdded(const uids_t& aUids)
    {
    JELOG2(EJavaCaptain);
    LOG1(EJavaCaptain, EInfo, "javasoftnotehandler::amAdded, %d uids", aUids.size());
    }

// ----------------------------------------------------------------------------
// javasoftnotehandler::amUpdated
// ----------------------------------------------------------------------------
void javasoftnotehandler::amUpdated(const uids_t& aUids)
    {
    JELOG2(EJavaCaptain);
    LOG1(EJavaCaptain, EInfo, "javasoftnotehandler::amUpdated, %d uids", aUids.size());
    TRAP_IGNORE(deleteIndicatorsL(aUids));
    }

// ----------------------------------------------------------------------------
// javasoftnotehandler::amDeleted
// ----------------------------------------------------------------------------
void javasoftnotehandler::amDeleted(const uids_t& aUids)
    {
    JELOG2(EJavaCaptain);
    LOG1(EJavaCaptain, EInfo, "javasoftnotehandler::amDeleted, %d uids", aUids.size());
    TRAP_IGNORE(deleteIndicatorsL(aUids));
    }

// ----------------------------------------------------------------------------
// javasoftnotehandler::createIndicatorsL
// ----------------------------------------------------------------------------
void javasoftnotehandler::createIndicatorsL()
    {
    std::auto_ptr<JavaStorage> js(JavaStorage::createInstance());
    try
        {
        js->open(JAVA_DATABASE_NAME);
        JavaStorageApplicationEntry_t findPattern;
        JavaStorageApplicationList_t foundEntries;
        // Reading all java based softnotes from database 
        js->search(JAVA_SOFTNOTE_TABLE, findPattern, foundEntries);
        js->close();

        if (foundEntries.size() > 0)
            {
            JavaStorageApplicationList_t::const_iterator applications;
            JavaStorageApplicationEntry_t::const_iterator findIterator;
            JavaStorageEntry attribute;
            CDesC16ArrayFlat* array = NULL;
            // Reading all sfotNotes data  from database and 
            // adding to status menu.
            for (applications = foundEntries.begin(); applications
                    != foundEntries.end(); applications++)
                {
                // Only 5 columns in the softNote table. So, create array with size 5.
                array = new CDesC16ArrayFlat(5);
                CleanupStack::PushL(array);
                TUid midletId;
                TInt notificationId;
                attribute.setEntry(SOFTNOTE_MIDLET_ID, L"");
                findIterator = (*applications).find(attribute);
                // no need to check null midlet id, as it is a primary key in the table.
                        midletId = TUid::Uid(JavaCommonUtils::wstringToInt(
                                        (*findIterator).entryValue()));
                array->AppendL(wstringToBuf((*findIterator).entryValue())->Des());

                attribute.setEntry(SOFTNOTE_ID, L"");
                findIterator = (*applications).find(attribute);
                // no ned to check for null notification id, as it is a primary key in the table.
                notificationId = JavaCommonUtils::wstringToInt(
                        (*findIterator).entryValue());
                array->AppendL(wstringToBuf((*findIterator).entryValue())->Des());

                // if no entry is found for primary text, appends Null desc to array 
                // to keep all variable positions correctly.
                attribute.setEntry(SOFTNOTE_PRIMARY_TEXT, L"");
                findIterator = (*applications).find(attribute);
                if (findIterator != (*applications).end())
                    {
                    array->AppendL(wstringToBuf((*findIterator).entryValue())->Des());
                    }
                else
                    {
                    array->AppendL(KNullDesC);
                    }

                attribute.setEntry(SOFTNOTE_SECONDARY_TEXT, L"");
                findIterator = (*applications).find(attribute);
                if (findIterator != (*applications).end())
                    {
                    array->AppendL(wstringToBuf((*findIterator).entryValue())->Des());
                    }
                else
                    {
                    array->AppendL(KNullDesC);
                    }

                attribute.setEntry(SOFTNOTE_IMAGE_PATH, L"");
                findIterator = (*applications).find(attribute);
                if (findIterator != (*applications).end())
                    {
                    array->AppendL(wstringToBuf((*findIterator).entryValue())->Des());
                    }
                else
                    {
                    array->AppendL(KNullDesC);
                    }

                MDesCArray* marray = static_cast<MDesCArray*>(array);
                CHbSymbianVariant* varValues = CHbSymbianVariant::NewL( marray, CHbSymbianVariant::EDesArray);
                CleanupStack::PushL(varValues);

                TBuf<KIndicatorTypeStringLength> indicatorType;
                indicatorType.Format(KIndicatorTypeJavaSoftNote, midletId, notificationId);
                // creating indicator and adding to status menu
                CHbIndicatorSymbian* indicator = CHbIndicatorSymbian::NewL();
                CleanupStack::PushL(indicator);
                indicator->Activate(indicatorType, varValues);
                CleanupStack::PopAndDestroy(indicator);

                CleanupStack::PopAndDestroy(varValues);
                CleanupStack::PopAndDestroy(array);
                array = NULL;
                }
            }
        }
    catch(JavaStorageException& ex)
        {
        LOG1(EJavaStorage, EInfo," JavaSoftNote Exception %s", ex.toString());
        }   
    }

// ----------------------------------------------------------------------------
// javasoftnotehandler::deleteIndicatorsL
// ----------------------------------------------------------------------------
void javasoftnotehandler::deleteIndicatorsL(const uids_t& aUids)
    {

    std::auto_ptr<JavaStorage> js(JavaStorage::createInstance());
    try        
        {
        js->open(JAVA_DATABASE_NAME);
        JavaStorageEntry attribute;
        JavaStorageApplicationEntry_t findPattern;
        JavaStorageApplicationList_t foundEntries;
        // Search all softnotes for each given midlet UID in the softNote table 
        // and notify to the indicator in status mentu to delete/clear them.
        for (uids_t::const_iterator iter = aUids.begin(); iter != aUids.end(); ++iter)
            {
            TUid aMidletId;
            uidToTUid(*iter, aMidletId);
            attribute.setEntry(SOFTNOTE_MIDLET_ID, JavaCommonUtils::intToWstring(aMidletId.iUid));
            findPattern.insert(attribute);
            // search for all softnotes for single midlet UID 
            js->search(JAVA_SOFTNOTE_TABLE, findPattern, foundEntries);
            findPattern.clear();

            if (foundEntries.size() > 0)
                {
                attribute.setEntry(SOFTNOTE_ID, L"");
                JavaStorageApplicationList_t::const_iterator applications;
                // Deletes all softnotes for single midlet UID
                for(applications = foundEntries.begin(); applications !=foundEntries.end(); applications++)
                    {
                    // Get softnote ID.
                    JavaStorageApplicationEntry_t::const_iterator findIterator = (*applications).find(attribute);
                    if (findIterator != (*applications).end())
                        {
                        TBuf<KIndicatorTypeStringLength> type;
                        type.Format(KIndicatorTypeJavaSoftNote, aMidletId,
                                JavaCommonUtils::wstringToInt((*findIterator).entryValue()));
                        // create reference to Indicator
                        CHbIndicatorSymbian* indicator = CHbIndicatorSymbian::NewL();
                        CleanupStack::PushL(indicator);
                        //Inform to clear from status menu and database.
                        indicator->Deactivate(type);
                        CleanupStack::PopAndDestroy(indicator);
                        }
                    }
                }
            foundEntries.clear();
            }
        js->close();
        }
    catch(JavaStorageException& ex)
        {
        LOG1(EJavaStorage, EInfo," JavaSoftNote Exception %s", ex.toString());
        }
    }

