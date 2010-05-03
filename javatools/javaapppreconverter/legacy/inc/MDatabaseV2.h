/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef MDATABASEV2_H
#define MDATABASEV2_H

namespace MIDP
{
namespace DB
{
class MCustomAttribTable;
} // end of namespace DB
} // end of namespace MIDP

namespace MIDP
{
namespace DBv2
{
class MAlarmTable;
class MCHLocalActionNameTable;
class MCHPropertyTable;
class MComponentPropertyTable;
class MConnectionTable;
class MContentHandlerTable;
class MMIDletPropertyTable;
class MMIDletSuitePropertyTable;
class MMIDletSuiteSecurityAttributeTable;
class MMIDletSuiteTable;
class MMIDletTable;
class MUserSecurityPreferencesTable;

class MDatabase
{
public:

    virtual ~MDatabase() = 0;

    virtual MAlarmTable* AlarmTableL(void)  = 0;

    virtual MCHPropertyTable* CHAccessTableL(void)  = 0;

    virtual MCHPropertyTable* CHActionsTableL(void)  = 0;

    virtual MCHLocalActionNameTable* CHLocalActionNameTableL(void)  = 0;

    virtual MCHPropertyTable* CHSuffixesTableL(void)    = 0;

    virtual MCHPropertyTable* CHTypesTableL(void)   = 0;

    virtual MComponentPropertyTable* ComponentPropertyTableL(void) = 0;

    virtual MConnectionTable* DynamicConnectionTableL(void)    = 0;

    virtual MConnectionTable* StaticConnectionTableL(void)    = 0;

    virtual MContentHandlerTable* ContentHandlerTableL(void)    = 0;

    virtual MMIDletPropertyTable* MIDletPropertyTableL(void)    = 0;

    virtual MMIDletSuitePropertyTable* MIDletSuitePropertyTableL(void)  = 0;

    virtual MMIDletSuiteTable* MIDletSuiteTableL(void)  = 0;

    virtual MMIDletTable* MIDletTableL(void)    = 0;

    virtual MMIDletSuiteSecurityAttributeTable* SecurityAttributeTableL(void)  = 0;

    virtual MUserSecurityPreferencesTable* UserSecurityPreferencesTableL(void) = 0;

    virtual DB::MCustomAttribTable* CustomAttribTableL(void) = 0;

};

inline MDatabase::~MDatabase()
{
}

IMPORT_C class MDatabase* GetDatabaseL(TUint32 aVersion);

} // end of namespace DBv2
} // end of namespace MIDP

#endif // MDATABASEV2_H
