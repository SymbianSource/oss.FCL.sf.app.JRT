@rem
@rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
javainstaller test -mainclass=com.nokia.mj.impl.utils.BufferedReaderTests
javainstaller test -mainclass=com.nokia.mj.impl.utils.concurrent.ConcurrentTests
javaunittester -conf=cldc -mainclass=com.nokia.mj.impl.rt.SystemPropertyTests
javaunittester -conf=cdc -mainclass=com.nokia.mj.impl.rt.SystemPropertyTests
javaunittester -conf=foundation -mainclass=com.nokia.mj.impl.rt.SystemPropertyTests

