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

#ifndef JAVASOFTINDICATOR_H
#define JAVASOFTINDICATOR_H

#include <hbindicatorinterface.h>

/**
 * Handles client request and showing the indications. 
 */
class JavaSoftIndicator : public HbIndicatorInterface
    {
public:
    /**
     * Constructor
     */
    JavaSoftIndicator(const QString &indicatorType);

    /**
     * Destructor
     */
    virtual ~JavaSoftIndicator();

    /**
     * @see HbIndicatorInterface
     */
    bool handleInteraction(InteractionType type);

    /**
     * @see HbIndicatorInterface
     */
    QVariant indicatorData(int role) const;

protected:
    /**
     * @see HbIndicatorInterface
     */
    bool handleClientRequest(RequestType type, const QVariant &parameter);

private:

    void StartJavaMidletL() const;
    void RemoveSoftNoteFromStorage();

private:

    QVariant iParameter;
    };

#endif // JAVASOFTINDICATOR_H
