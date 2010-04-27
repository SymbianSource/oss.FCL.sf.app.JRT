/*******************************************************************************
 * Copyright (c) 2007 Nokia Corporation.
 * All rights reserved. This program and the accompanying materials 
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 * 
 * Contributors:
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/

// Note that makmake will not set either _DEBUG- or NDEBUG-macros,
// therefore it will not see any version of jxe.c. This is intentional,
// no udeb/urel-specific jxe.c-files must be recorded as dependencies
// in the generated makefile or building the other of the two targets
// would fail.

#if defined(__WINS__) || defined(__WINSCW__) 

#if defined(_DEBUG)
#include "winscw/udeb/eswtdc_jxe.c"
#elif defined(NDEBUG)
#include "winscw/urel/eswtdc_jxe.c"
#endif

#else

#if defined(_DEBUG)
#include "armv5/udeb/eswtdc_jxe.c"
#elif defined(NDEBUG)
#include "armv5/urel/eswtdc_jxe.c"
#endif

#endif
