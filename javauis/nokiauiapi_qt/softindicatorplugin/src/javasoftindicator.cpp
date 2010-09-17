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

#include "javasoftindicator.h" 

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
        TRAP_IGNORE(StartJavaMidletL());
        QVariantMap variantMap;
        if (iParameter.isValid() && iParameter.canConvert(QVariant::List))
            {
            variantMap.insert("MidletId",iParameter.toList()[0].toUInt());
            variantMap.insert("NotificationId",iParameter.toList()[1]);
            }
        emit userActivated(variantMap);
        handled = true;
        emit deactivate();
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
            emit deactivate();
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
            TUint KJavaMidletUid = iParameter.toList()[0].toUInt();

            RWsSession ws;
            User::LeaveIfError(ws.Connect() == KErrNone);
            TApaTaskList tasklist(ws);
            TApaTask task = tasklist.FindApp(TUid::Uid(KJavaMidletUid));
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
                TUid::Uid(KJavaMidletUid), threadId);
                appArcSession.Close();
                }
            }
        }
    }

