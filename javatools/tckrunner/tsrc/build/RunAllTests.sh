#/bin/bash
#put any pre-test execution commands here.
echo Running all tests
valgrind --leak-check=full --show-reachable=yes ./tck_tester.sh $1 
