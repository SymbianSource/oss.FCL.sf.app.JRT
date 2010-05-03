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
* Description:
*
*/
#include "javaapplicationsettingsview.h"
#include "javaapplicationsettingsview_p.h"

JavaApplicationSettingsView::JavaApplicationSettingsView() :
        ApplicationSettingsView()
{
}

void JavaApplicationSettingsView::initialize(const QString& aApplication)
{
    d_ptr = new JavaApplicationSettingsViewPrivate(aApplication);
    d_ptr->init(this);
}

JavaApplicationSettingsView::~JavaApplicationSettingsView()
{
    delete d_ptr;
}

Q_EXPORT_PLUGIN2(javaapplicationsettingsview, JavaApplicationSettingsView);
