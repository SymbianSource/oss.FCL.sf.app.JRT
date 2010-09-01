/*******************************************************************************
 * Copyright (c) 2007, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/


#ifndef SWTCLIENTENTRY_H
#define SWTCLIENTENTRY_H


class MSwtClient;


namespace SWT
{
/**
 * Creates new MSwtClient instance. Returned client is attached to
 * MSwtDisplay object existing in TLS. If display does not exist NULL is
 * returned, that can be used to check if this function is called from
 * right thread and if display was created.
 * @return new MSwtClient instance or NULL if not called from eSWT main
 * thread.
 */
IMPORT_C MSwtClient* CreateClientL();
}


#endif // SWTCLIENTENTRY_H
