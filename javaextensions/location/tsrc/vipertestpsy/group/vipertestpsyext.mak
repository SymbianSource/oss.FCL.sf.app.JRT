# runs after 'abld makefile'
MAKMAKE:
#	@echo *** MAKMAKE ${PLATFORM} ${CFG} ${TO_ROOT}

# runs after 'abld library'
LIB:
#	@echo *** LIB ${PLATFORM} ${CFG} ${TO_ROOT}

# runs after 'abld resource'
RESOURCE:
#	@echo *** RESOURCE ${PLATFORM} ${CFG} ${TO_ROOT}

# runs after 'abld target'
BLD:
#	@echo *** BLD ${PLATFORM} ${CFG} ${TO_ROOT}

# runs after 'abld final'
FINAL:
	copy /y ..\resource\01010176.txt %EPOCROOT%epoc32\release\${PLATFORM}\${CFG}\z\private\10202BE9
#	@echo *** FINAL ${PLATFORM} ${CFG} ${TO_ROOT}

# runs after clean operations
CLEAN:
	del %EPOCROOT%epoc32\release\${PLATFORM}\${CFG}\z\private\10202BE9\01010176.txt
#	@echo *** CLEAN ${PLATFORM} ${CFG} ${TO_ROOT}

# End of File