CC      = /usr/bin/g++
CFLAGS  =  -I -fPIC -DRTI_USES_STD_FSTREAM -fpermissive -std=c++17 -w  -I/usr/local/include/librdkafka -I???/daceDS/CppBaseWrapper/src -Ofast #-g  -Og 
LDFLAGS =  -L???/daceDS/CppBaseWrapper/lib  -lstdc++fs -pthread #-DDSTRAFFIC_CTRL_DBG #-DKDEBUG -DDSTRAFFIC_TRANSFER_DBG 

#use traci?
# SUMO 	= ???/sumo-1.7.0/src
# SUMOBIN	= ???/sumo-1.7.0/build/src
# CFLAGSVAR  = -I$(SUMO)/foreign -I$(SUMO) -I$(SUMOBIN) 
# LDFLAGSVAR = -DUSING_TRACI
# APISRC = src/api/SumoConnectionTraCI.cpp 
# ADDT  = $(SUMOBIN)/utils/traci/libutils_traci.a $(SUMOBIN)/foreign/tcpip/libforeign_tcpip.a

#use libsumo?
SUMOSRC = ???/sumo-1_13_0/src
CFLAGSVAR  =  -I$(SUMOSRC)
LDFLAGSVAR =  -L$(SUMO_HOME)/bin
APISRC = src/api/SumoConnectionLibSUMO.cpp 
ADDT  =  -lsumocpp


all: 
	$(CC) $(CFLAGS)  $(CFLAGSVAR) $(LDFLAGS) $(LDFLAGSVAR) \
	src/PrepareRun.cpp \
	src/SumoWrapper.cpp \
	src/api/InteractionImpl.cpp \
	src/api/OrchestrationImpl.cpp \
	src/api/ProvisionImpl.cpp \
	$(APISRC) \
	src/api/SumoObserver.cpp \
	src/logic/SumoSimulationControl.cpp \
	-o bin/SumoWrapper -lsimwrapper -lpugixml $(ADDT)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf *.o *.so ../bin/SumoWrapper 
	
.PHONY: all clean

