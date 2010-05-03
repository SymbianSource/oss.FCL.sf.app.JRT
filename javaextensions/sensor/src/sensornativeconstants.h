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
* Description:  Constants for Sensor API
 *
*/

#ifndef SENSORNATIVECONSTANTS_H
#define SENSORNATIVECONSTANTS_H

const int ERROR_NONE = 0;
const int ERROR_GENERAL = -2;
const int ERROR_NOMEMORY = -4;

/**
 * Same order as in S60 sensor API
 * enum TSensrvConditionOperator
 *   {
 *   ESensrvOperatorEquals = 0,
 *   ESensrvOperatorGreaterThan,
 *   ESensrvOperatorGreaterThanOrEquals,
 *   ESensrvOperatorLessThan,
 *   ESensrvOperatorLessThanOrEquals,
 *   ESensrvOperatorBinaryAnd,
 *   ESensrvOperatorBinaryAll
 *   };
 */
const int INT_EQUALS = 0;
const int INT_GREATER_THAN = 1;
const int INT_GREATER_THAN_OR_EQUALS = 2;
const int INT_LESS_THAN = 3;
const int INT_LESS_THAN_OR_EQUALS = 4;
const int INT_NO_OPERATION = -1;

#endif // SENSORNATIVECONSTANTS_H
