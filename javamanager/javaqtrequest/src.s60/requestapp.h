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
* Description:  QT Service application the makes Qt Highway API
*               request based on the command line of the application
*
*/

#ifndef REQUESTAPP_H
#define REQUESTAPP_H

#include <QApplication>

class RequestApp : public QApplication
{
    Q_OBJECT

public:
    RequestApp(int &argc, char **argv);

public slots:
    void sendRequest();
};


#endif  // REQUESTAPP_H
