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
* Description:
*
*/
#ifndef APPLICATIONSETTINGSVIEW_H
#define APPLICATIONSETTINGSVIEW_H

#include <hbview.h>
#include <QtPlugin>

class ApplicationSettingsView: public HbView
{
public:

    /*
     * Base constructor
     */
    explicit ApplicationSettingsView() : HbView() {}

    /*
     * Destructor
     */
    virtual ~ApplicationSettingsView() {}

public slots:
    /*
     * Initializes the view with application specific info
     *
     * @param aAppUid Identifier for the application
     * @param aAppName Name for the application
     */
    void initialize(const QString& aAppUid, const QString& aAppName = QString());

};

Q_DECLARE_INTERFACE(ApplicationSettingsView,
                    "com.nokia.ApplicationLibrary.ApplicationSettingsView/1.0");

#endif // APPLICATIONSETTINGSVIEW_H
