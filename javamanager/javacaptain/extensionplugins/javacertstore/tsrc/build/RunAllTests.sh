#/bin/bash
#put any pre-test execution commands here.
echo Running all tests
#valgrind --leak-check=full --show-reachable=yes ./AllTests -v $1
./AllTests.sh $1

