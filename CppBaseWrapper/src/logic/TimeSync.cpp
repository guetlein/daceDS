/*******************************************************************************
 * Copyright 2021 Moritz Gütlein
 * 
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License.  You may obtain a copy
 * of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations under
 * the License.
 ******************************************************************************/

#include "TimeSync.h"
namespace daceDS {
// using namespace daceDS;

/*
* todo: should lbts ignore messages -> and represent advance wishes?
* or do we want another field "localTime" that combines lbts and messageCount
*/
// void TimeSync::init( std::shared_ptr<Producer> p, std::shared_ptr<Consumer> c) {
//     DS_SYNC_DBG("Initializing TimeSync");
//     AvroHelper::getInstance()->initSchema(SCHEMA_NAME_SYNCMSG, SCHEMA_DEF_SYNCMSG, topic);
//     consumer = c;
//     consumer->subscribe(Config::getInstance()->get(CONFIG_KAFKABROKER), {topic}, participantID, "");
//     producer = p;
//     producer->init(Config::getInstance()->get(CONFIG_KAFKABROKER), {topic}, participantID, "");
// }

TimeSync::TimeSync(std::string t, int64_t s) : syncTopic(t), syncedParticipants(s) {
    //AvroHelper::getInstance()->initSchema(SCHEMA_NAME_SYNCMSG, SCHEMA_DEF_SYNCMSG, topic); //no need, we have a recordBasedNaming-strategy
    handler = std::make_shared<TimeSyncHandler>(std::shared_ptr<TimeSync>(this));
}

void TimeSync::prepare() {
    std::vector<std::string> topics;
    topics.push_back(syncTopic);

    participantID = Config::getInstance()->getScenarioID()+"_"+Config::getInstance()->getSimulatorID()+"_"+Constants::STR_TIMESYNC_ID;

    producer->init(Config::getInstance()->get(CONFIG_KAFKABROKER), topics, participantID, "");
    consumer->subscribe(Config::getInstance()->get(CONFIG_KAFKABROKER), topics, participantID, "");

}
void TimeSync::calcLBTS() {
    //find current lower bound time stamp
    auto it = std::min_element(timeRequests.begin(), timeRequests.end(),
                               [](decltype(timeRequests)::value_type& l, decltype(timeRequests)::value_type& r) -> bool { return l.second < r.second; });

    if (it->second > lbtsInMS && timeRequests.size() == syncedParticipants) {
        lbtsInMS = it->second;
        DS_SYNC_DBG("participantID=" << participantID << " lbts is now: " << lbtsInMS);
    } else {
        DS_SYNC_DBG("participantID=" << participantID << " SyncedParticipants: " << timeRequests.size() << "/" << syncedParticipants);
    }
}

// new version
void TimeSync::handleTopicSyncMsgRequest(datamodel::SyncMsg s) {
    DS_SYNC_DBG("participantID=" << participantID << " processRequest " << s.Sender << "|" << s.Time);
    if (timeRequests.count(s.Sender) == 0 || s.Time > timeRequests[s.Sender]) {
        timeRequests[s.Sender] = s.Time;
    }

    //iterative approach
    for (auto entry : s.Messages) {
        std::string stopic = entry.first;
        int64_t count = entry.second;

        //continue if topic is not of interest
        // std::string expectation = Consumer::getExpectation(stopic);
        // if (expectation.size() > 0) {
        if (Consumer::isExpectedTopic(stopic)) {
            expectedMessages[stopic] += count;
            DS_SYNC_DBG("topic is of interest = " << stopic << " increasing expected count to " << expectedMessages[stopic]);
        } else {
            DS_SYNC_DBG("topic is not of interest = " << stopic);
        }
    }

    calcLBTS();
    tick();
}

void TimeSync::handleTopicSyncMsgJoin(datamodel::SyncMsg syncMsg) {
    handleTopicSyncMsgRequest(syncMsg);  //todo
}

void TimeSync::handleTopicSyncMsgLeave(datamodel::SyncMsg syncMsg) {
    syncedParticipants--;
    if (timeRequests.count(syncMsg.Sender) != 0) {
        timeRequests.erase(syncMsg.Sender);
    }
    calcLBTS();
}

//use async wait
void TimeSync::tick() {
    cv.notify_all();
}

//is my new time equal or greater than the lbts
bool TimeSync::timeOK(int64_t newkafkatime) {
    DS_SYNC_DBG("TimeOK? " << newkafkatime << " vs " << getLBTSInMS());
    return getLBTSInMS() >= newkafkatime;
}

void TimeSync::timeAdvance(int64_t stepInMS) {
    auto oldTime = getLBTSInMS();
    int64_t newkafkatime = oldTime +  stepInMS;
    datamodel::SyncMsg s;
    // s.Action = datamodel::ActionType::REQUEST;
    s.Action = "request";  //currently, some implementation do not support enums...
    s.Epoch = 0;           //todo
    s.Sender = participantID;
    s.Time = newkafkatime;

    std::map<std::string, int64_t> diff;
    for (auto entry : Producer::countingSentMessages) {
        if (lastAccouncedSentMessages.count(entry.first) > 0) {
            int64_t d = entry.second - lastAccouncedSentMessages[entry.first];
            if (d > 0) {
                diff[entry.first] = d;
            }
        } else {
            diff[entry.first] = entry.second;
        }
    }

    lastAccouncedSentMessages = Producer::countingSentMessages;
    s.Messages = diff;

    std::vector<char> out = AvroHelper::getInstance()->encodeSyncMsg(s);
    DS_SYNC_DBG("Sending wish to proceed to " << newkafkatime);
    while (!producer->publish(syncTopic, out, oldTime)) {
    }
    timeRequests[participantID] = newkafkatime;
    calcLBTS();
    tick();

    {
        std::unique_lock<std::mutex> lk(cv_m);
        DS_SYNC_DBG("Waiting for time grant...");
        bool finished = false;
        do {
            finished = cv.wait_for(lk, std::chrono::milliseconds(1), [this, newkafkatime] { return timeOK(newkafkatime); });
        } while (!finished);

        DS_SYNC_DBG("...finished waiting for t == " << newkafkatime);
    }
    //wait for msgs
    waitForMessages();

    DS_SYNC_DBG("Can proceed to " << newkafkatime);
}

void TimeSync::timeAdvanceToAbsolutePoint(int64_t newkafkatime) {
    datamodel::SyncMsg s;
    // s.Action = datamodel::ActionType::REQUEST;
    s.Action = "request";  //currently, some implementation do not support enums...
    s.Epoch = 0;           //todo
    s.Sender = participantID;
    s.Time = newkafkatime;

    std::map<std::string, int64_t> diff;
    for (auto entry : Producer::countingSentMessages) {
        if (lastAccouncedSentMessages.count(entry.first) > 0) {
            int64_t d = entry.second - lastAccouncedSentMessages[entry.first];
            if (d > 0) {
                diff[entry.first] = d;
            }
        } else {
            diff[entry.first] = entry.second;
        }
    }

    lastAccouncedSentMessages = Producer::countingSentMessages;
    s.Messages = diff;

    std::vector<char> out = AvroHelper::getInstance()->encodeSyncMsg(s);
    DS_SYNC_DBG("Sending timeAdvanceToAbsolutePoint wish to proceed to " << newkafkatime);
    while (!producer->publish(syncTopic, out, getLBTSInMS())) {
    }
    timeRequests[participantID] = newkafkatime;

    {
        std::unique_lock<std::mutex> lk(cv_m);
        DS_SYNC_DBG("Waiting for time grant...");
        bool finished = false;
        do {
            finished = cv.wait_for(lk, std::chrono::milliseconds(1), [this, newkafkatime] { return timeOK(newkafkatime); });
        } while (!finished);

        DS_SYNC_DBG("...finished waiting for t == " << newkafkatime);
    }
    //wait for msgs
    waitForMessages();

    DS_SYNC_DBG("Can proceed to " << newkafkatime);
}

bool TimeSync::msgsOK() {
    for (auto entry : expectedMessages) {
        std::string topic = entry.first;
        int64_t expectedNum = entry.second;

        // FIXME: we ignore own messages --> do we have to consider that in such a way:?
        //          this was enabled before, but now we probably need to ignore it
        // if (Producer::countingSentMessages.count(topic)) {
        //     expectedNum -= Producer::countingSentMessages[topic];
        // }

        //check if messages are received on this topic --> if not, keep waiting
        if (expectedNum > 0 && (Consumer::countingReceiveCounter.count(topic) == 0 || Consumer::countingReceiveCounter[topic] < expectedNum)) {  //todo: have a seperate buffer for msgs with future timestamps
            DS_SYNC_DBG("MsgsOK? topic=" << topic << ", expecting=" << expectedNum << ", received=" << Consumer::countingReceiveCounter[topic]);
            return false;
        }
    }
    DS_SYNC_DBG("MsgsOK? yep!");
    return true;
}

void TimeSync::waitForMessages() {
    DS_SYNC_DBG("Waiting for msgs...");
    std::unique_lock<std::mutex> lk(cv_m);
    bool finished = false;
    do {
        finished = cv.wait_for(lk, std::chrono::milliseconds(1), [this] { return msgsOK(); });
    } while (!finished);
    DS_SYNC_DBG("...finished waiting for messages");
}

//todo: allow "late"-joining
void TimeSync::joinTiming(int64_t time) {
    DS_SYNC_DBG("sending out join msg for id " << participantID);
    std::map<std::string, int64_t> foo;
    datamodel::SyncMsg s;
    //s.Action = datamodel::ActionType::JOIN;
    s.Action = "join";  //currently, some implementation do not support enums...
    s.Sender = participantID;
    s.Time = time;
    s.Epoch = 0;

    std::vector<char> out = AvroHelper::getInstance()->encodeSyncMsg(s);
    producer->publish(syncTopic, out, 0L);
    timeRequests[participantID] = time;
    calcLBTS();
    tick();

    std::unique_lock<std::mutex> lk(cv_m);
    cv.wait(lk, [this] { return getLBTSInMS() >= 0; });

    //try this variant?
    // std::unique_lock<std::mutex> lk(cv_m);
    // bool finished = false;
    // do {
    //     finished = cv.wait_for(lk, std::chrono::milliseconds(1), [this, time] { return timeOK(time); });
    // } while (!finished);


    DS_SYNC_DBG(participantID << " joined successfully");
}

void TimeSync::leaveTiming() {
    datamodel::SyncMsg s;
    //s.Action = datamodel::ActionType::LEAVE;
    s.Action = "leave";  //currently, some implementation do not support enums...
    s.Sender = participantID;
    s.Time = 0L;
    s.Epoch = 0;

    std::vector<char> out = AvroHelper::getInstance()->encodeSyncMsg(s);
    producer->publish(syncTopic, out, getLBTSInMS());
}

}  // namespace daceDS
