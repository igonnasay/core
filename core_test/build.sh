IPath='-I.. -I../ta-lib/include -Igoogletest/include -I../easylogging'
LFlag='-L.. -L/usr/lib -lcore -lpthread -lta_lib -lboost_regex -lboost_regex-mt -lboost_filesystem -lboost_filesystem-mt -std=c++11'

g++ gtest_main.a core_test.cpp $IPath $LFlag -o 'test'

echo 'build finshed.'






