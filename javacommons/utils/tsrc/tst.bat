@rem
@rem Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
rem Script for running JavaCommonUtils unit tests from S60 eshell
javaunittester -mainclass=com.nokia.mj.impl.utils.LoggerTests
javaunittester -mainclass=com.nokia.mj.impl.utils.BufferedReaderTests
javaunittester -mainclass=com.nokia.mj.impl.utils.concurrent.ConcurrentTests
javaunittester -mainclass=com.nokia.mj.impl.rt.SystemPropertyTests
javaunittester -conf=cdc -mainclass=com.nokia.mj.impl.rt.SystemPropertyTests
javaunittester -conf=foundation -mainclass=com.nokia.mj.impl.rt.SystemPropertyTests
javaunittester -mainclass=com.nokia.mj.impl.utils.DebugUtilsTests
testjavautils -nomemleaks
