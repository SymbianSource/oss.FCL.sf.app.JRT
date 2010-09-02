@rem
@rem Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
@rem All rights reserved.
@rem This component and the accompanying materials are made available
@rem under the terms of "Eclipse Public License v1.0"
@rem which accompanies this distribution, and is available
@rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
@rem
@rem Initial Contributors:
@rem Nokia Corporation - initial contribution.
@rem
@rem Contributors:
@rem
@rem Description:
@rem
@rem Script for running CoreUi unit tests from S60 eshell
testcoreui -nomemleaks -g TestCoreUi
javaunittester -conf=cldc -mainclass=com.nokia.mj.impl.coreuitest.CoreUiTests 1
javaunittester -conf=cldc -mainclass=com.nokia.mj.impl.coreuitest.CoreUiTests 01
javaunittester -conf=cldc -mainclass=com.nokia.mj.impl.coreuitest.CoreUiTests 001
