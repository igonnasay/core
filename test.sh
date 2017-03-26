
IPath='-I. -Ita-lib/include -Icore_test/googletest/include -Ieasylogging'
LFlag='-L. -L/usr/lib -lpthread -lta_lib -lboost_regex -lboost_regex-mt -lboost_filesystem -lboost_filesystem-mt -std=c++11'

g++ gtest_main.a core_test.cpp $IPath $LFlag -o 'test_exe'

echo 'build finshed.'
echo 'run test case.'

./test_exe
