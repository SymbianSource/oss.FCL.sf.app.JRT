/*******************************************************************************
 * Copyright (c) 2005, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/


#ifndef SWTFUNCTOR_H
#define SWTFUNCTOR_H


/**
 * Interface for a functor
 */
class MSwtFunctor
{
public:
    virtual void operator()() const =0;
};


#endif // SWTFUNCTOR_H
