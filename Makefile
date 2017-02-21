
CFlag= -std=c++11 -I./ -Ieasylogging -Ictp -Ita-lib/include

LFlag= -L./ -L/usr/lib -lpthread -lthostmduserapi -lthosttraderapi -lcurses -lta_lib \
	   -lboost_regex -lboost_regex-mt -lboost_system -lboost_system-mt

core_objects = init.o base.o tutils.o market.o trade.o strategy.o strategydata.o \
			   minutes_strategy.o manager.o easylogging/easylogging.o

start : $(core_objects) start.o
	g++ ${LFlag} $(core_objects) start.o -o start

libcore.a : $(core_objects)
	ar rvs libcore.a $(core_objects)

start.o : start.cpp
	g++ ${CFlag} -c start.cpp -o start.o

init.o : init.h init.cpp
	g++ ${CFlag} -c init.cpp -o init.o

base.o : BaseData.h BaseData.cpp
	g++ ${CFlag} -c BaseData.cpp -o base.o

tutils.o : tutils.h tutils.cpp
	g++ ${CFlag} -c tutils.cpp -o tutils.o

market.o : market.h market.cpp
	g++ ${CFlag} -c market.cpp -o market.o

trade.o : trade.h trade.cpp
	g++ ${CFlag} -c trade.cpp -o trade.o

strategy.o : strategy.h strategy.cpp
	g++ ${CFlag} -c strategy.cpp -o strategy.o

strategydata.o : StrategyData.h StrategyData.cpp
	g++ ${CFlag} -c StrategyData.cpp -o strategydata.o

minutes_strategy.o : minutes_strategy.h minutes_strategy.cpp
	g++ ${CFlag} -c minutes_strategy.cpp -o minutes_strategy.o

manager.o : manager.h manager.cpp
	g++ ${CFlag} -c manager.cpp -o manager.o

easylogging/easylogging.o : easylogging/easylogging++.h easylogging/easylogging++.cc
	g++ $(CFlag) -c easylogging/easylogging++.cc -o easylogging/easylogging.o

clean : 
	rm -rf *.o
	rm -rf start
	rm -rf libcore.a




