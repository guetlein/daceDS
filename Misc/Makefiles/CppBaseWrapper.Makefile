CC=/usr/bin/g++
SRC=???/daceDS/CppBaseWrapper/src
LIB=???/daceDS/CppBaseWrapper/lib

CFLAGS  =  -I $(SRC)  -fPIC -fpermissive -std=c++17 -w  -I/usr/local/include/librdkafka -Ofast #-g  -ggdb -Og 
LDFLAGS =  -rdynamic -lavrocpp -lserdes++ -pthread -lrdkafka++ -lcppkafka -lserdes -lrdkafka #-DKDBGCB -DKDEBUG -DDS_SYNC_DBG

all:
	$(CC) -o $(LIB)/libsimwrapper.so  -shared $(CFLAGS) \
$(SRC)/api/Traffic/Micro/ProvisionHandlerTrafficMicro.cpp \
$(SRC)/communication/AvroHelper.cpp \
$(SRC)/communication/Consumer.cpp \
$(SRC)/communication/MessageHandler.cpp \
$(SRC)/communication/InteractionHandler.cpp \
$(SRC)/communication/kafka/KafkaConsumer.cpp \
$(SRC)/communication/kafka/KafkaProducer.cpp  \
$(SRC)/communication/LogHandler.cpp  \
$(SRC)/communication/OrchestrationHandler.cpp \
$(SRC)/communication/Producer.cpp \
$(SRC)/communication/ProvisionHandler.cpp \
$(SRC)/communication/TimeSyncHandler.cpp \
$(SRC)/logic/SimulationControl.cpp \
$(SRC)/logic/TimeSync.cpp \
$(SRC)/main/SimulationWrapper.cpp \
$(SRC)/util/Config.cpp \
$(SRC)/util/Utils.cpp \
$(LDFLAGS)

clean:
	rm -rf *.o *.so $(LIB)/libsimwrapper.so
	
.PHONY: all clean
