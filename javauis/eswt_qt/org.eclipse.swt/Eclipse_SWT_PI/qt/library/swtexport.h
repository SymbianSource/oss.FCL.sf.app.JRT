/*******************************************************************************
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - initial implementation
 *******************************************************************************/

#ifndef SWTEXPORT_H_
#define SWTEXPORT_H_

// Macros for exporting/importing symbols using the correct Qt-macro. 
// SWTQT_LIBRARY is defined in the Qt project file of the exporting library. 

#if defined(SWTQT_LIBRARY)
#  define SWTQT_EXPORT Q_DECL_EXPORT
#else
#  define SWTQT_EXPORT Q_DECL_IMPORT
#endif

#endif /* SWTEXPORT_H_ */
