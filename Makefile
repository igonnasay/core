
CFlag= -fPIC -std=c++11 -I./ -Ieasylogging -Ictp -Ita-lib/include -Irapidjson -I/usr/include/python2.7

LFlag= -L./ -L/usr/lib -lpthread -lthostmduserapi -lthosttraderapi -lcurses -lta_lib \
	   -lboost_regex -lboost_regex-mt -lboost_system -lboost_system-mt -lboost_python -lpython2.7

core_objects = bfunc.o base_data.o tutils.o market.o trade.o strategy.o strategy_data.o \
			   minutes_strategy.o manager.o easylogging/easylogging.o

start : $(core_objects) start.o
	g++ ${LFlag} $(core_objects) start.o -o start

api.so : libcore.a api.o
	g++ ${LFlag} libcore.a api.o -shared -o api.so

libcore.a : $(core_objects)
	ar rvs libcore.a $(core_objects)

start.o : start.cpp
	g++ ${CFlag} -c start.cpp -o start.o

bfunc.o : bfunc.h bfunc.cpp
	g++ ${CFlag} -c bfunc.cpp -o bfunc.o

base_data.o : base_data.h base_data.cpp
	g++ ${CFlag} -c base_data.cpp -o base_data.o

tutils.o : tutils.h tutils.cpp
	g++ ${CFlag} -c tutils.cpp -o tutils.o

market.o : market.h market.cpp
	g++ ${CFlag} -c market.cpp -o market.o

trade.o : trade.h trade.cpp
	g++ ${CFlag} -c trade.cpp -o trade.o

strategy.o : strategy.h strategy.cpp
	g++ ${CFlag} -c strategy.cpp -o strategy.o

strategy_data.o : strategy_data.h strategy_data.cpp
	g++ ${CFlag} -c strategy_data.cpp -o strategy_data.o

minutes_strategy.o : minutes_strategy.h minutes_strategy.cpp
	g++ ${CFlag} -c minutes_strategy.cpp -o minutes_strategy.o

manager.o : manager.h manager.cpp
	g++ ${CFlag} -c manager.cpp -o manager.o

easylogging/easylogging.o : easylogging/easylogging++.h easylogging/easylogging++.cc
	g++ $(CFlag) -c easylogging/easylogging++.cc -o easylogging/easylogging.o

api.o : api.cpp
	g++ $(CFlag) -c api.cpp

clean : 
	rm -rf *.o
	rm -rf start
	rm -rf libcore.a
	rm -rf api.so




