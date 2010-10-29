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

#include <apgtask.h>
#include <apgcli.h>
#include <memory>
#include <javastorageentry.h>
#include <javastorage.h>
#include <javastoragenames.h>
#include <JavaCommonUtils.h>
#include <logger.h>
#include <QFile.h>

#include "javasoftindicator.h"

using namespace std;
using namespace java::storage;
using namespace java::util;

// ----------------------------------------------------------------------------
// JavaSoftIndicator::JavaSoftIndicator
// @see javasoftindicator.h
// ----------------------------------------------------------------------------
JavaSoftIndicator::JavaSoftIndicator(const QString &indicatorType) :
    HbIndicatorInterface(indicatorType,
            HbIndicatorInterface::NotificationCategory, InteractionActivated)
    {
    }

// ----------------------------------------------------------------------------
// JavaSoftIndicator::~JavaSoftIndicator
// @see javasoftindicator.h
// ----------------------------------------------------------------------------
JavaSoftIndicator::~JavaSoftIndicator()
    {
    }

// ----------------------------------------------------------------------------
// JavaSoftIndicator::handleInteraction
// @see javasoftindicator.h
// ----------------------------------------------------------------------------
bool JavaSoftIndicator::handleInteraction(InteractionType type)
    {
    bool handled = false;
    if (type == InteractionActivated)
        {
        // get midlet to foreground or lauch if it is not open
        TRAP_IGNORE(StartJavaMidletL());
        QVariantMap variantMap;
        if (iParameter.isValid() && iParameter.canConvert(QVariant::List))
            {
            variantMap.insert("MidletId",iParameter.toList()[0].toUInt());
            variantMap.insert("NotificationId",iParameter.toList()[1].toInt());
            }
        // send user activated signal to indicator observer
        emit userActivated(variantMap);
        handled = true;
        // remove softnote from database
        RemoveSoftNoteFromStorage();
        // remove indicator from status menu
        emit deactivate();
        // remove image from public directoy
        QFile imgFile(iParameter.toList()[4].toString());
        if(imgFile.exists())
            {
            imgFile.remove();
            }
        }
    return handled;
    }

// ----------------------------------------------------------------------------
// JavaSoftIndicator::indicatorData
// @see javasoftindicator.h
// ----------------------------------------------------------------------------
QVariant JavaSoftIndicator::indicatorData(int role) const
    {
    switch (role)
        {
        case PrimaryTextRole:
            {
            QString text("");
            if (iParameter.isValid() && iParameter.canConvert(QVariant::List))
                {
                if (iParameter.toList()[2].isValid())
                    {
                    text = iParameter.toList()[2].toString();
                    }
                }
            return text;
            }
        case SecondaryTextRole:
            {
            QString text("");
            if (iParameter.isValid() && iParameter.canConvert(QVariant::List))
                {
                if (iParameter.toList()[3].isValid())
                    {
                    text = iParameter.toList()[3].toString();
                    }
                }
            return text;
            }
        case DecorationNameRole:
            {
            QVariant img;
            if (iParameter.isValid() && iParameter.canConvert(QVariant::List))
                {
                if (iParameter.toList()[4].isValid())
                    {
                    img = iParameter.toList()[4].toString(); 
                    }
                }
            return img;
            }
        default:
            return QVariant();
        }
    }

// ----------------------------------------------------------------------------
// JavaSoftIndicator::handleClientRequest
// ----------------------------------------------------------------------------
bool JavaSoftIndicator::handleClientRequest(RequestType type,
        const QVariant &parameter)
    {
    bool handled(false);
    switch (type)
        {
        case RequestActivate:
            {
            if (iParameter != parameter)
                {
                iParameter = parameter;
                }
            emit dataChanged();
            handled = true;
            }
            break;
        case RequestDeactivate:
            {
            RemoveSoftNoteFromStorage();
            emit deactivate();
            QFile imgFile(iParameter.toList()[4].toString());
            if(imgFile.exists())
                {
                imgFile.remove();
                }
            }
            break;
        default:
            break;
        }
    return handled;
    }

// ----------------------------------------------------------
// JavaSoftIndicator::StartJavaMidletL()
// ----------------------------------------------------------
void JavaSoftIndicator::StartJavaMidletL() const
    {
    if (iParameter.isValid() && iParameter.canConvert(QVariant::List))
        {
        if (iParameter.toList()[0].isValid())
            {
            TUint javaMidletUid = iParameter.toList()[0].toUInt();

            RWsSession ws;
            User::LeaveIfError(ws.Connect() == KErrNone);
            TApaTaskList tasklist(ws);
            TApaTask task = tasklist.FindApp(TUid::Uid(javaMidletUid));
            if (task.Exists())
                {
                task.BringToForeground();
                ws.Close();
                }
            else
                {
                ws.Close();
                RApaLsSession appArcSession;
                User::LeaveIfError(appArcSession.Connect());
                TThreadId threadId;
                appArcSession.StartDocument(_L(""),
                TUid::Uid(javaMidletUid), threadId);
                appArcSession.Close();
                }
            }
        }
    }

// ----------------------------------------------------------
// JavaSoftIndicator::RemoveSoftNoteFromStorage()
// ----------------------------------------------------------
void JavaSoftIndicator::RemoveSoftNoteFromStorage()
    {
    std::auto_ptr<JavaStorage> js(JavaStorage::createInstance());
    try
        {
        js->open(JAVA_DATABASE_NAME);

        JavaStorageApplicationEntry_t entries;
        JavaStorageEntry attribute;

        std::wstring midletId = JavaCommonUtils::intToWstring(
                iParameter.toList()[0].toUInt());
        attribute.setEntry(SOFTNOTE_MIDLET_ID, midletId,
                JavaStorageEntry::STRING);
        entries.insert(attribute);

        std::wstring notificationId = JavaCommonUtils::intToWstring(
                iParameter.toList()[1].toInt());
        attribute.setEntry(SOFTNOTE_ID, notificationId, JavaStorageEntry::INT);
        entries.insert(attribute);

        js->remove(JAVA_SOFTNOTE_TABLE, entries);
        js->close();
        entries.clear();
        }
    catch (JavaStorageException& ex)
        {
        LOG1(EJavaStorage, EInfo," JavaSoftNote Exception %S", ex.toString().c_str());
        }
    }

