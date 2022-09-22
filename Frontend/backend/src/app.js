/*
#
# Copyright 2021 Moritz Gütlein
# 
# Licensed under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License.  You may obtain a copy
# of the License at
# 
#   http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
# License for the specific language governing permissions and limitations under
# the License.
#
*/

const formData = require("express-form-data");
const express = require('express')
const bodyParser = require('body-parser')
const cors = require('cors')
const morgan = require('morgan')
const helpers = require('./helpers');
const app = express()
app.use(cors())
app.use(bodyParser.json()); 
app.use(bodyParser.urlencoded({ extended: true })); 


const BROKER = "192.168.0.209:9092"
const REGISTRY = "http://192.168.0.209:8081"


var path = require('path')
var multer = require('multer');
const storage = multer.diskStorage({
  destination: function(req, file, cb) {
      cb(null, 'data/resources/uploads');
  },
  filename: function(req, file, cb) {
      cb(null, req.body.id+ '_' + path.parse(file.originalname).name + '_' + Date.now() + path.extname(file.originalname));
  }
});
var upload = multer({ storage: storage, fileFilter: helpers.fileExtensionFilter });
app.use(upload.array('Payload')); 
app.use(express.static('public'));


/*
* *********************************************************
*                     READ DEFINITIONS
* *********************************************************
*/
var glob = require("glob")
var fs = require("fs")

var datamodel = {
  domains : [],
  layers : [],
  components : [],
  translators : [],
  projectors : []
}

const readDefinitionsFromDataPool = true


if(readDefinitionsFromDataPool == false) {
  /////////////////////////////////////////////////////////
  // get domains
  console.log('        ')
  console.log(' ~~ domains ~~ ')

  glob("data/domains/*.json", function (er, files) {
    for(i in files){
      var path = files[i]
      console.log("found "+path)
      let rawdata = fs.readFileSync(path);
      let elem = JSON.parse(rawdata);
      //console.log(elem)
      console.log(elem.name)
      datamodel.domains.push(elem)
    }
  })

  //////////////////////////////////////////////////////////
  // get layers
  console.log('        ')
  console.log(' ~~ layers ~~ ')
  glob("data/layers/*.json", function (er, files) {
    for(i in files){
      var path = files[i]
      console.log("found "+path)
      let rawdata = fs.readFileSync(path);
      let layer = JSON.parse(rawdata);
      //console.log(layer)
      console.log(layer.name)
      datamodel.layers.push(layer)
    }
  })

  /////////////////////////////////////////////////////////
  // get sims
  console.log('        ')
  console.log(' ~~ components ~~ ')
  glob("data/components/*.json", function (er, files) {
    for(i in files){
      var path = files[i]
      console.log("found "+path)
      let rawdata = fs.readFileSync(path);
      let elem = JSON.parse(rawdata);
      //console.log(elem)
      console.log(elem.name)
      datamodel.components.push(elem)
    }
  })

  /////////////////////////////////////////////////////////
  // get translators
  console.log('        ')
  console.log(' ~~ translators ~~ ')
  glob("data/translators/*.json", function (er, files) {
    for(i in files){
      var path = files[i]
      console.log("found "+path)
      let rawdata = fs.readFileSync(path);
      let elem = JSON.parse(rawdata);
      //console.log(elem)
      console.log(elem.name)
      datamodel.translators.push(elem)
    } 
  })
}






app.post('/getDomains', (req, res) => {
  console.log("got getDomain request")   
  return res.status(200).send(datamodel.domains);
});
app.post('/getLayers', (req, res) => {
  console.log("got getLayers request")   
  return res.status(200).send(datamodel.layers);
});
app.post('/getSimulators', (req, res) => {
  console.log("got getSimulators request")   
  return res.status(200).send(datamodel.simulators);
});
app.post('/getTranslators', (req, res) => {
  console.log("got getTranslators request")   
  return res.status(200).send(datamodel.translators);
});
app.post('/getDatamodel', (req, res) => {
  console.log("got getDatamodel request")   
  return res.status(200).send(datamodel);
});
app.post('/setDatamodel', (req, res) => {
  console.log("got setDatamodel request") 
  let dm = req.body
  console.log("old dm:")
  console.dir(datamodel)
  datamodel = dm
  console.log("new dm:")
  console.dir(datamodel)
  //todo: save on filedisk
  return res.status(200).send(datamodel);
});

  


/*
* *********************************************************
*                          KAFKA
* *********************************************************
*/


var cid = Date.now();
const { Kafka } = require('kafkajs')
const { SchemaRegistry, SchemaType, readAVSCAsync } = require('@kafkajs/confluent-schema-registry');
const { create } = require("domain");
const { dir } = require("console");

// configure Kafka broker
const kafka = new Kafka({
  clientId: "daceDS-web-backend-client"+cid,
  brokers: [BROKER], 
});
const registry = new SchemaRegistry({
  host: REGISTRY,  
});

// create a producer which will be used for producing messages
const producer = kafka.producer();
const stringProducer  = kafka.producer();

var statusConsumer;
var statusMessages = [];
async function status () {
  console.log("creating status listener")
  statusConsumer = kafka.consumer({ groupId: 'backend'+1000*Math.random() })
  await statusConsumer.connect()
  console.log("connected status listener")

  statusConsumer.run({
    eachMessage: async ({ topic, partition, message }) => {
        console.log({
            key: message.key.toString(),
            value: message.value.toString(),
            headers: message.headers,
        })
    },
  });
}

function getIDX(lst, name){
  var idx = 0
  for(var e of lst){
    if ( e.name == name){
      return idx
    } else {
      console.log(e.name + " vs " + name)
    }
    idx++
  }
  return -1
}

async function readDefFromDataPool(){
  var definitionConsumerDomains = kafka.consumer({ groupId: cid+"_def"+1000*Math.random()});
  await definitionConsumerDomains.connect();
  try{
    var t = 'orchestration\\.simulation\\.definitions\\.(\\S)+'
    r = new RegExp(t)
    await definitionConsumerDomains.subscribe({ topic: r, fromBeginning: true });
  } catch(e) {
    console.log(e)
    console.log("canceling subscription")
    return
  }  
  definitionConsumerDomains.run({
    eachMessage: async ({ topic, partition, message }) => {

      if(topic.includes("resource")){
        console.log("skipping " + topic)
        return;
      }

      dd = message.value.toString();


      // console.log(dd);
      for( var d of dd.split(';') ){
        if(d.length == 0){
          continue;
        }
        try{
          let j = JSON.parse(d)

          // console.log("found:");
          // console.log(j);        
          if(topic.includes("domains")){
            var idx = getIDX(datamodel.domains, j.name)
            if (idx == -1){
              datamodel.domains.push(j);
            } else {
              datamodel.domains.splice(idx, 1, j);
            }
          }       
          else if(topic.includes("layers")){            
            var idx = getIDX(datamodel.layers, j.name)
            if (idx == -1){
              datamodel.layers.push(j);
            } else {
              datamodel.layers.splice(idx, 1, j);
            }
          }    
          else if(topic.includes("components")){
            var idx = getIDX(datamodel.components, j.name)
            if (idx == -1){
              datamodel.components.push(j);
            } else {
              datamodel.components.splice(idx, 1, j);
            }
          }    
          else if(topic.includes("connectors")){
            if (("domainA" in j)==false){
              var idx = getIDX(datamodel.translators, j.name)
              if (idx == -1){
                datamodel.translators.push(j);
              } else {
                datamodel.translators.splice(idx, 1, j);
              }
            } else {
              var idx = getIDX(datamodel.projectors, j.name)
              if (idx == -1){
                datamodel.projectors.push(j);
              } else {
                datamodel.projectors.splice(idx, 1, j);
              }
            }
            
          }
          else {
            console.log("ignoring " + topic)
          }
          console.log("\nfound " + datamodel.domains.length + " domains")
          console.log("found " + datamodel.layers.length + " layers")
          console.log("found " + datamodel.components.length + " components")
          console.log("found " + datamodel.translators.length + " translators")
          console.log("found " + datamodel.projectors.length + " projectors\n")
        }
        catch(e){
          console.log("failed to parse: ")
          console.log(d)
          console.log(e)
        }
      }
    }
  });


}

if(readDefinitionsFromDataPool == true) {
  readDefFromDataPool()
}

async function registerSceSchema () {
  try {
    const schema = await readAVSCAsync("./src/Scenario_flat.avsc");
    ret = await registry.register(schema);
    console.log("registerSchema for SCE succ, id:"+ret.id);
    sceSchemaID = ret.id;
  } catch (e) {
    console.log(e);
  }
};  
async function registerResourceFileSchema () {
  try {
    const schema = await readAVSCAsync("./src/ResourceFile.avsc");
    ret = await registry.register(schema);
    console.log("registerSchema for ResourceFile succ, id:"+ret.id);
    rfSchemaID = ret.id;
  } catch (e) {
    console.log(e);
  }
};
async function registerSyncMsgSchema () {
  try {
    const schema = await readAVSCAsync("./src/SyncMsg.avsc");
    ret = await registry.register(schema);
    console.log("registerSchema for SyncMsg succ, id:"+ret.id);
  } catch (e) {
    console.log(e);
  }
};

async function registerCtrlMsgSchema () {
  try {
    const schema = await readAVSCAsync("./src/CtrlMsg.avsc");
    ret = await registry.register(schema);
    console.log("registerSchema for CtrlMsg succ, id:"+ret.id);
    ctrlSchemaID = ret.id;
  } catch (e) {
    console.log(e);
  }
};

const produceToKafka = async (registryId, topic, message) => {
  message = cpy(message)
  console.log("about to send somethin to "+topic)
  await producer.connect();

  encoded = await registry.encode(registryId, message).catch(error => { console.log('caught when encoding: ', error.message, ' on ', topic); console.dir(error); })
  
  outgoingMessage = {
    key: "",
    value: encoded
  };

  if(outgoingMessage.value != null){
    console.log("encoded message!")
    await producer.send({
      topic: topic,
      messages: [outgoingMessage],
    }).catch(error => { console.log('caught when sending: ', error.message); console.dir(error);  });


    console.log("done sending to "+topic)
  } else {
    console.log("could not send anything to "+topic)
  }
  await producer.disconnect();
};

var sceSchemaID = -1;
registerSceSchema();
var rfSchemaID = -1;
registerResourceFileSchema();
registerSyncMsgSchema();
var ctrlSchemaID = -1;
registerCtrlMsgSchema();
var orchTopic = "orchestration.simulation"
// }



const produceStringToKafka = async (topic, message) => {
  console.log("about to send '"+message+"' to "+topic)
  await stringProducer.connect();

  outgoingMessage = {
    key: "",
    value: message
  };

    await stringProducer.send({
      topic: topic,
      messages: [outgoingMessage],
    }).catch(error => { console.log('caught when sending: ', error.message); console.dir(error);  });


    console.log("done sending to "+topic)

  await stringProducer.disconnect();
};


var scenarioID = ""
var provisionConsumer = null
var mostRecentProvisionTimes = new Map()
var tmpProvisionMessages = new Map()
var availableProvisionMessages = new Map()
var neverGotAnyMsg = true

async function consumeProvision(t){
  console.log("trying to consume: "+t)
  try{
    if (provisionConsumer != null){
      await provisionConsumer.stop()
    } else {
      provisionConsumer  = kafka.consumer({ groupId: cid+"_"+1000*Math.random()})
      await provisionConsumer.connect()
    }
    r = new RegExp(t)
    console.log("subscribe to ["+r+"]")
    await provisionConsumer.subscribe({ topic: r,fromBeginning: true });
  } catch(e) {
    console.log(e)
    console.log("canceling subscription")
    return
  }

  console.log("started subscription")
  await provisionConsumer.run({
    eachMessage: async ({ topic, partition, message }) => {
      // console.log(">>> provision message on "+topic+"@"+message.timestamp)
      try{
        const decodedMessage = {
          timestamp : Number(message.headers.time),
          topic: topic,
          value: await registry.decode(message.value)
        }
        var timestamp =  Number(message.headers.time) //Number(message.timestamp)

        // console.log(">>> provision message on "+topic+"@header="+timestamp)
          //first message on this topic?
          if(!mostRecentProvisionTimes.has(topic)) {
            mostRecentProvisionTimes.set(topic, timestamp);
            tmpProvisionMessages.set(topic, []);
            availableProvisionMessages.set(topic, []);
          }

          //newer timestamp? save old data
          if(mostRecentProvisionTimes.get(topic) < timestamp){
            console.log("finalizing time list for "+mostRecentProvisionTimes.get(topic))
            // availableProvisionMessages.get(topic).set(mostRecentProvisionTimes.get(topic), cpy(tmpProvisionMessages.get(topic)));
            availableProvisionMessages.set(topic, cpy(tmpProvisionMessages.get(topic)));
            tmpProvisionMessages.set(topic, []);
            mostRecentProvisionTimes.set(topic, timestamp)

          }
          else if(mostRecentProvisionTimes.get(topic) == timestamp){
            tmpProvisionMessages.get(topic).push(decodedMessage)
            neverGotAnyMsg = false 
            // dir(decodedMessage)

          //message is older
          } else if(mostRecentProvisionTimes.get(topic) > timestamp){
            console.log("ignoring old message from "+timestamp+", already at "+mostRecentProvisionTimes.get(topic))
          }
      } catch (e) {console.log(e)}
    }, //end of each msg
  });
}




var statusMessages = []
var statusConsumer = null
async function consumeStatusTopic(t){
  if (statusConsumer != null){
    await statusConsumer.stop()
  } else {
    statusConsumer  = kafka.consumer({ groupId: "foo"+cid})
    await statusConsumer.connect()
  }
  
  await statusConsumer.subscribe({ topic: t,fromBeginning: true });
  await statusConsumer.run({
    eachMessage: async ({ topic, partition, message }) => {
        console.log(">>> status on "+t)
        // message.key = message.key.toString()
        message.value = message.value.toString()
        message.topic = topic
        statusMessages.push(message)
    },
  });
}



var syncMessages = []
var syncConsumer = null
async function consumeSyncTopic(t){
  console.log("consuming on sync topic:"+t)
  if (syncConsumer != null){
    await syncConsumer.stop()
  } else {
    syncConsumer  = kafka.consumer({ groupId: "foo2"+cid})
    await syncConsumer.connect()
  }
  
  await syncConsumer.subscribe({ topic: t,fromBeginning: true });
  await syncConsumer.run({
    eachMessage: async ({ topic, partition, message }) => {
        console.log(">>> sync on "+t)

        const decodedMessage = {
          timestamp : Number(message.timestamp),
          topic: topic,
          value: await registry.decode(message.value)
        }

        syncMessages.push(decodedMessage)
    },
  });
}


var resultMessages = []
var resultConsumer = null
async function consumeResultTopic(t){
  console.log("consuming on result topic:"+t)
  if (resultConsumer != null){
    await resultConsumer.stop()
  } else {
    resultConsumer  = kafka.consumer({ groupId: "foo2"+cid})
    await resultConsumer.connect()
  }
  
  await resultConsumer.subscribe({ topic: t,fromBeginning: true });
  await resultConsumer.run({
    eachMessage: async ({ topic, partition, message }) => {
        console.log(">>> result on "+t)

        const decodedMessage = {
          timestamp : Number(message.timestamp),
          topic: topic,
          value: await registry.decode(message.value)
        }

        resultMessages.push(decodedMessage)
    },
  });
}




async function sendSCE(sce){
  console.log("publishing sce")
  console.log("produceToKafka")
  console.log("sceSchemaID="+sceSchemaID)
  console.log("rfSchemaID="+rfSchemaID)
  console.log("orchTopic="+orchTopic)

  if (sceSchemaID == -1){
    console.log("exiting, invalid schema")
    process.exit(1);
  }
  // console.dir(sce)

  await produceToKafka(sceSchemaID,orchTopic,sce);
}

async function sendResources(sce){
  console.log("publishing resources")
  var resTopic = "provision.simulation."+sce.ID+".resource"
  for(const sim of sce.buildingBlocks){
    function* entries(obj) {
      for (let key in obj)
          yield [key, obj[key]];
    }
    var simResources = new Map(entries(sim["Resources"]));
    for(let [resourceID, resourceType] of simResources) {
        console.log("publishing "+resourceID+ "-"+resourceType+" for " + sim.ID+ " on "+resTopic);
        absPath = getAbsPathForResourceID(resourceID);


        var rf = {
            "ID" : resourceID,
            "Type" : resourceType,
            "File" : null,//bts, 
            "FileReference" : "file://"+absPath
          }
        console.log("producing: "+rf.ID+"_"+rf.Type+"_"+rf.FileReference)
        await produceToKafka(rfSchemaID,resTopic,rf);
    } 
  }
}

function getNumberOfObservers(sce){
  counter=0
  for(const sim of sce.buildingBlocks){
    function* entries(obj) {
      for (let key in obj)
          yield [key, obj[key]];
    }
    var observers = new Map(entries(sim["Observers"]));
    counter+=observers.size
  }
  return counter
}

function getNumberOfBBs(sce){
  return sce.buildingBlocks.length
}

async function sendStuff(sce){
  await sendSCE(sce)
  await sendResources(sce)
}

async function getTopics(sceID){
  const admin = kafka.admin()

  // remember to connect and disconnect when you are done
  await admin.connect()
  let alltopics =  await admin.listTopics()
  let topics = []
  for( const topic of alltopics){
    if(topic.includes("orchestration.simulation."+sceID)){
      topics.push(topic)
    }
    else if(topic.includes("interaction.simulation."+sceID)){
      topics.push(topic)
    }
    else if(topic.includes("provision.simulation."+sceID)){
      topics.push(topic)
    } 

  }
  // console.dir(topics)

  //filter all empty topcis
  let nonEmptyTopics = []
  for(let t of topics){
    let o = await admin.fetchTopicOffsets(t)
    for(let partition of o){
      if(partition.offset > 0){
        nonEmptyTopics.push(t)
        break
      }
    }
    
  }
  
  await admin.disconnect()
  return nonEmptyTopics
}

async function respondQuerySce(sceID, res){
  let topics = await getTopics(sceID)
  let m = cpy(resultMessages)
  resultMessages = []
  let r = { "topics" : topics, "results" : m }

  return res.status(200).send(r);
}

app.post('/querySce', (req, res) => {
  let sceID = req.body.sceID
  console.log("\n\n\nreturning info for "+sceID)


  return respondQuerySce(sceID, res)

});

app.post('/ctrlSce', (req, res) => {
  let ctrl = req.body
  let topic = "orchestration.simulation."+ctrl.sceID+".ctrl"

  let c = {"Sender" : "Web", "Command" : ctrl.command}
  produceToKafka(ctrlSchemaID, topic, c)
  res.send(
    [{
      title: "Bassd",
      description: "scho!"
    }]
  )
});

neverGotAnyMsg = true
app.post('/runSce', (req, res) => {
  console.log("\n\n\n\ngot run request")
  // console.dir(req)
  let sce = req.body
  // console.dir(sce)

  //ldnw 
  sendStuff(sce)
 

  numObs = getNumberOfObservers(sce)
  numBBs = getNumberOfBBs(sce)
  console.dir("there are "+ numObs + " observers and "+numBBs+ " BBs")
    
  scenarioID = sce.ID

  consumeStatusTopic('orchestration.simulation.'+scenarioID+'.status').then(() => console.log("done status"))

  consumeResultTopic('provision.simulation.'+scenarioID+'.result')

  //is there a chance to observe something?
  neverGotAnyMsg = true
  if(numObs > 0 || numBBs > 1) {
    consumeProvision('provision\\.simulation\\.'+scenarioID+'\\.traffic\\.(\\S)+').then(() => console.log("consume done"))
  } else {
    //does this make sense? should a single bb use the sync topic?
    consumeSyncTopic('orchestration.simulation.'+scenarioID+'.sync')
  }
  
  res.send(
    [{
      title: "Bassd",
      description: "scho!"
    }]
  )
});


/*
* *********************************************************
*                          Feedback
* *********************************************************
*/

app.post('/pollSyncData', (req, res) => {
  console.log("got poll sce sync request")
  let l = syncMessages.length
  let response = syncMessages.splice(0,l)
  if (l==1){
    l = [l]
  }
  console.log("returning " + response.length + " sync msgs")
  return res.status(200).send(response); //clears and returns list
});

app.post('/pollStatusData', (req, res) => {
  console.log("got poll sce status request")
  let l = statusMessages.length
  let response = statusMessages.splice(0,l)
  if (l==1){
    l = [l]
  }
  return res.status(200).send(response); //clears and returns list
});

provisionErrorCount = 0
provisionErrorCountLimit = 1
app.post('/pollProvisionData', (req, res) => {
  console.log("got pollTrafficDatas request")

  //regex does only eval on old topics, resubscribe from time to time
  if(provisionErrorCount > provisionErrorCountLimit){
    provisionErrorCount = 0
    provisionErrorCountLimit +=5
    consumeProvision('provision\\.simulation\\.'+scenarioID+'\\.traffic\\.(\\S)+').then(() => console.log("consume done"))
  }
  if(neverGotAnyMsg){
    provisionErrorCount += 1
  }

  var ret = getAvailableProvisionMsgs();
  console.log("returning " + ret.length + " provision msgs")
  return res.status(200).send(ret); //clears and returns list
});

function cpy(i){
  return JSON.parse(JSON.stringify(i))
}

function getAvailableProvisionMsgs(){
  var msgs = []


  if (availableProvisionMessages.size == 0 ){
    return []
  }
  console.log("got msgs on: ")
  console.dir(availableProvisionMessages.keys())

  for(const topic of availableProvisionMessages.keys()){
    // console.log("adding msgs for "+topic)
    if(availableProvisionMessages.get(topic).size == 0){
      continue
    }

    msgs = msgs.concat(cpy(availableProvisionMessages.get(topic)))
    availableProvisionMessages.set(topic, [])
  }

  // console.log("av msgs: ")
  // console.dir(msgs)

  return msgs
}

/*
* *********************************************************
*                          RESOURCES
* *********************************************************
*/



console.log('        ')
console.log(' ~~ resources ~~ ')

var resourceCatalogue = {resourceList: []}
const resourceCataloguePath = "data/resources/resourceCatalogue.json"

if(fs.existsSync(resourceCataloguePath)){
  let rawdata = fs.readFileSync(resourceCataloguePath);
  resourceCatalogue = JSON.parse(rawdata);
} 
console.log("resourceCatalogue holds " + resourceCatalogue.resourceList.length + " entries")

function getAbsPathForResourceID(id){
  for(const resource of resourceCatalogue.resourceList){
    if(resource.ID.normalize() == id.normalize()){
      return process.cwd()+"/"+resource.Path;
    }
  }
  console.log("\n\n\ndid not find resource file for id="+id)
  return ""
}

function getBytesForResourceID(id){
  for(const resource of resourceCatalogue.resourceList){
    if(resource.ID.normalize() == id.normalize()){
      // let rawdata = fs.readFileSync(resource.Path,{encoding:'utf8', flag:'r'})
      let rawdata = fs.readFileSync(resource.Path,{flag:'r'})
      return rawdata;
    }
  }
  console.log("\n\n\ndid not find resource file for id="+id)
  return null
}

app.post('/uploadResource', (req, res) => {
  console.log("got uploadResource request")

  // let upload = multer({ storage: storage, fileFilter: helpers.imageFilter }).single('profile_pic');

  //console.dir(req)
  if (!req.files) {
    console.log('No files were uploaded.');
    return res.status(400).send('No files were uploaded.');
  }
  if (Object.keys(req.files).length === 0) {
    console.log('Length is 0.');
    return res.status(400).send('Length is 0.');
  }

  let Payload = req.files[0];
  console.dir(Payload)

  //add to catalog
  resourceCatalogue.resourceList.push({'ID' : req.body.ID, 'Domain' : req.body.domain, 'Layer' : req.body.Layer, 'Type' :  req.body.Type, 'Path' : Payload.path})
  
  fs.writeFileSync(resourceCataloguePath, JSON.stringify(resourceCatalogue));

  return res.status(200).send('Bassd scho.');

});

app.post('/deleteResource', (req, res) => {
  console.log("got deleteResource request")
  //todo: delete resource
  return res.status(200).send('Bassd scho.');
});

app.post('/deleteAllResources', (req, res) => {
  console.log("got deleteAllResources request")
  //todo: delete all resource
  return res.status(200).send('Bassd scho.');
});

app.post('/getResourceList', (req, res) => {
  console.log("got getResourceList request")   
  return res.status(200).send(resourceCatalogue.resourceList);
});

app.post('/getResource', (req, res) => {
  let id = req.body.id
  console.log("got getResource request: ")  
  console.dir(id)
  for(let file of resourceCatalogue.resourceList){
    if(file.ID == id){
      let utf8 = fs.readFileSync(file.Path, "utf8");
      // let j = JSON.parse(rawdata);
      console.log("found resource")
      return res.status(200).send({str : utf8});
    }
  } 
  console.log("did not find resource, only got:")
  for(let file of resourceCatalogue.resourceList){
    console.log(file.ID)
  }
  return res.status(404).send("not found");
});


/*
* *********************************************************
*                          MISC
* *********************************************************
*/
app.post('/connectivityCheck', (req, res) => {
  console.log("got connectivityCheck request")
  return res.status(200).send('Hello!');
});


/*
* *********************************************************
*                          RUN
* *********************************************************
*/
try{
app.listen(process.env.PORT || 8087)
} catch(e){
  console.log("port might be used, "+ e)
}


