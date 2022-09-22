<!-- 
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
!-->

<template>
  <div fluid id="app" style="margin-top:0; height: 100vh; overflow:hidden"  >
    <!-- <div style="display: none"> -->
    <HelperBBs />
    <MainNavBar />
    <ScenarioMainContent />
    <!-- </div> -->
  </div>
</template>

<script>
import MainNavBar from "./components/MainNavBar.vue";
import ScenarioMainContent from "./components/MainContent.vue";
import HelperBBs from "./components/HelperBBs.vue";
import xml2js from "xml2js";
import PostService from "./PostService.js";
import Vue from "vue";
let colormap = require("colormap");
const numberColors = 10;
// https://stackoverflow.com/questions/7616461/generate-a-hash-from-string-in-javascript
const hashCode = s => s.split('').reduce((a,b)=>{a=((a<<5)-a)+b.charCodeAt(0)+b.charCodeAt(0)*b.charCodeAt(0);return a&a},0)

export default {
  name: "App",
  data() {
    return {
      scee: {},
      defaultSCE: {"scenarioID":"1652363394662","domainReferences":{},"simulationStart":0,"simulationEnd":500,"execution":{"randomSeed":23,"constraints":"","priority":0,"syncedParticipants":0},"buildingBlocks":[],"translators":[],"projectors":[]},
      runningSce : false,
      resourceList: [],
      datamodel: {domains: [], layers: [], components: [], translators: [], projectors: []},
      activeInstance: undefined,
      instances: new Map(),
      numberColors: numberColors,
      colors: colormap({
        colormap: "jet", //phase, jet, hsv, rainbow, earth, rainbow-soft
        nshades: numberColors,
        format: "hex",
        alpha: 1,
      }),
      trafficdata: [],
      trafficmap: new Map(),
      statusdata: [],
      topologies: { traffic : null},
      catalog: [],
      dummyRun: false,
      historyMicroSpeed : []
    };
  },
  components: {
    HelperBBs,
    MainNavBar,
    ScenarioMainContent,
    //Canvas,
    //SceViewer,
  },
  mounted: function () {
    ///this.parseMap(defaultMapFile);
    this.getDatamodel();

     this.$root.$on("promoteInstance", () => {
      console.log("promoteInstance");
      this.promoteInstance(this.activeInstance);
    });
  },

  computed: {
    involvedDomains(){
      let drs = []
      console.log("domainreferecnes:")
      console.dir(Object.keys(this.scee.domainReferences))
      for(let dr of Object.keys(this.scee.domainReferences)){
        drs.push(dr)
      }
      return drs
    }
  },
  methods: {

    alexHash(str){
      let hash = 0
      for( let c of str){
          hash += c.charCodeAt(0)
      }
      return hash
    },

    sceRunFinished(){
       console.log("run is finished")
      this.runningSce = false;

      let copySCE = JSON.parse(JSON.stringify(this.scee));
      let catRef = this.catalog;
      let entr = {}
      const response = PostService.sendQuerySce(this.scee.scenarioID);
      response.then(function (result) {

        console.dir(result.data);
        console.dir(result.data.topics);
        console.dir(result.data.results);
        entr["scenarioID"] = copySCE.scenarioID
        entr["SCE"] = copySCE
        entr["Date"] = new Date().toLocaleString('de-DE', {
              timeZone: 'Europe/Berlin',
            })
        entr["SummaryTopics"] = result.data.topics
        entr["SummaryResults"] = result.data.results
        console.dir(entr);
        catRef.push(entr);
      });


      
    },
    sceRunCancel(){
      const response = PostService.sendCtrlSce(this.scee.scenarioID, "kill");
      response.then(function (result) {
        console.log(result);
      });

      this.runningSce = false;
    },

    sceRun(){
      console.log("running sce");
      this.trafficdata = [];
      this.trafficmap = new Map();
      this.statusdata = [];
      this.scee.scenarioID = String(new Date().getTime()) //lndw
      const response = PostService.sendRunSce(this.scee);
      response.then(function (result) {
        console.log(result);
      });
      this.runningSce = true;
    },


    getDatamodel() {
      console.log("fetching datamodel from backend");
      PostService.getDatamodel().then((result) => {
        console.log("got the following dm:");
        console.dir(result.data);
        this.datamodel = result.data;
      }).catch((error) => {
				console.log('fetching datamodel failed');
				console.log(error);
			});
    },
    uploadDatamodel() {
      console.log("uploading datamodel to backend");
      PostService.setDatamodel( this.datamodel).then((result) => {
        console.log("uploaded the dm:");
        console.dir(result.data);
        this.getDatamodel()
      }).catch((error) => {
				console.log('uploading datamodel failed');
				console.log(error);
			});
    },
    setMapElements: function (lList, nList, y_min, y_max, x_min, x_max) {
      // Vue.prototype.$roadMap = {
      this.topologies.traffic = {
        links: lList,
        nodes: nList,
        y_min: y_min,
        y_max: y_max,
        x_min: x_min,
        x_max: x_max,
      };
    },
    getColor(idStr) {
      console.log("idstr is " + idStr)
      console.log("hashCode is " + hashCode(idStr))
      // let id = (hashCode(idStr) * 0.5) % this.numberColors;


      console.log("alexHash is " + this.alexHash(idStr))
      // alexHash(idStr) 
      let id = this.alexHash(idStr) % this.numberColors;

      if (id<0){id*=-1}
      //console.log("color id is " + id)
      return this.colors[id];
    },
    parseMap: function (content) {

      var parser = new xml2js.Parser();
      let setMapElements = this.setMapElements;
      parser.parseString(content, function (err, result) {
        console.log("received the following map:")
        console.dir(result)
        var lList = new Map();
        var nList = new Map();
        var net = result["DomainReference"];
        var links = net["link"];
        for (var e in links) {
          var link = links[e];
          var name = link["$"]["id"];
          if (name[0] == ":") {
            continue;
          }
          lList.set(name, link["$"]);
        }
        var nodes = net["node"];
        var y_min = undefined;
        var y_max = undefined;
        var x_min = undefined;
        var x_max = undefined;
        for (var n in nodes) {
          var node = nodes[n];
          var nodeName = node["$"]["id"];
          node["$"]["y"] *= -1;
          nList.set(nodeName, node["$"]);
          let x = parseInt(node["$"]["x"]);
          let y = parseInt(node["$"]["y"]);
          if (x_min == undefined || x_min > x) {
            x_min = x;
          }
          if (x_max == undefined || x_max < x) {
            x_max = x;
          }

          if (y_min == undefined || y_min > y) {
            y_min = y;
          }
          if (y_max == undefined || y_max < y) {
            y_max = y;
          }
        }

        console.log("parsed map with "+lList.size+" links and " + nList.size + " nodes")
        setMapElements(lList, nList, y_min, y_max, x_min, x_max);
        //redraw Map
        //drawMap();
      });
    },

    promoteInstance(instanceID){
      let id = this.getBBIndexForId(instanceID);
      //cannot promote
      if (id == -1 || id == 0){
        console.log("cannot promote instance with id " +id);
        return;
      }
      //swap with predecessor
      var cpy = this.scee.buildingBlocks;
      var b = cpy[id-1];
      var a = cpy[id];
      Vue.set(this.scee.buildingBlocks, id-1, a);
      Vue.set(this.scee.buildingBlocks, id, b);
    },


    /************************************************* 
    *******************  DATAMODEL  *******************
    **************************************************/

    getBBDefinitionFromDataModel(domain, layer, type) {
      
      let bbs = this.datamodel.components;
      if(bbs == null){
        console.log("this.datamodel.components is empty")
        return undefined;
      }


      console.log("Looking for:"+domain+layer+type)
      for (const bb of bbs.values()) {
        if (domain == bb.domain && layer ==bb.layer && type == bb.name) {
          return bb;
        } else {
          console.log("mismatch for:"+bb.domain+bb.layer+bb.name)
        }
      }
      return undefined;
    },

    getTranslatorFromDataModel(translatorName){
       for (const translator of this.datamodel.translators) {
         if (translator.name == translatorName){
            return JSON.parse(JSON.stringify(translator));
         }
       }
      console.log("translator " +translatorName +" not found");
      return null;
    },
    


    addTranslator(){
        let t = {
            "name": "NewTranslator"+this.datamodel.translators.length,
            "version" : 1,
            "domain": "",
            "layerA" : "",
            "layerB" : ""
        };
        this.datamodel.translators.push(t);
    },

    addProjector(){
        let t = {
            "name": "NewProjector"+this.datamodel.projectors.length,
            "version" : 1,
            "domainA": "",
            "domainB": "",
            "layerA" : "",
            "layerB" : ""
        };
        this.datamodel.translators.push(t);
    },

    addComponent(){
        let t = {
            "name": "NewSimulator"+this.datamodel.components.length,
            "domain": "",
            "layer": "",
            "version": 1,
            "results": [],
            "resourceTypes" : [],
            "availableStepsizes" : [],
            "continous" : false
        };
        this.datamodel.components.push(t);
    },

    addDomain(){
        let t = {
            "name": "NewDomain"+this.datamodel.domains.length,
            "domain": "",
            "version": 1,
            "description" : "",
            "reference": {
                "description": "",
                "data" : []
            }
        };
        this.datamodel.domains.push(t);
    },

    addLayer(){
        let t = {
              "name": "NewLayer"+this.datamodel.layers.length,
              "domain": "",
              "version": 1,
              "ndm": {
              },
              "compounds": [
              ],
              "methods": [
              ],
              "primitives": [
              ]
          };
        this.datamodel.layers.push(t);
    },


    removeTranslator(translator){
      for(let translatorIdx in this.datamodel.translators){
          let t = this.datamodel.translators[translatorIdx]
          if (translator.domain == t.domain && translator.name == t.name){
            this.datamodel.translators.splice(translatorIdx,1)
            return
          }
      }
    },


    removeProjector(projector){
      for(let projectorIdx in this.datamodel.projectors){
          let t = this.datamodel.projectors[projectorIdx]
          if (projector.domainA == t.domainA && projector.name == t.name){
            this.datamodel.projectors.splice(projectorIdx,1)
            return
          }
      }
    },


    removeComponent(sim){
      for(let idx in this.datamodel.components){
          let t = this.datamodel.components[idx]
          if (sim.domain == t.domain && sim.name == t.name){
            this.datamodel.components.splice(idx,1)
            return
          }
      }
    },

    removeDomain(domain){
      for(let idx in this.datamodel.domains){
          let t = this.datamodel.domains[idx]
          if (domain.name == t.name){
            this.datamodel.domains.splice(idx,1)
            return
          }
      }
    },
    removeLayer(layer){
      for(let idx in this.datamodel.layers){
          let t = this.datamodel.layers[idx]
          if (layer.domain == t.domain && layer.name == t.name){
            this.datamodel.layers.splice(idx,1)
            return
          }
      }
    },


    /************************************************* 
    *******************  BB DEF    *******************
    **************************************************/

    getBBDefinitions(){
      let sce = Vue.prototype.$sce;
      if(sce == null) {
        console.log("getBBDefinitionForInstances: sce not found");
        return null;
      }
      if (Object.keys(sce).indexOf("buildingBlocks") == -1) {
        console.log("field BBs not found");
        return null;
      }
      return Vue.prototype.$sce.buildingBlocks;
    },
    
    getBBIndexForId(instanceID) {
      console.log("getBBIndexForId for " + instanceID);
      // let sce = Vue.prototype.$sce;
      let sce = this.scee;
      if (sce == null || Object.keys(sce).indexOf("buildingBlocks") == -1) {
        console.log("field BBs not found");
        return -1;
      }
      // let sims = Vue.prototype.$sce.buildingBlocks;
      let sims = sce.buildingBlocks;
      // console.dir(sims);
      // console.log(sims[0]);
      for (var i = 0; i < sims.length; i++) {
        if (instanceID == sims[i].instanceID) {

          console.log("simulatorID for " + instanceID+" is "+i);
          return i;
        }
      }

      return -1;
    },

    getBBDefinitionForInstance(instanceID) {
      
      let bbs = this.getBBDefinitions();
      if(bbs == null){
        return undefined;
      }

      for (const bb of bbs.values()) {
        if (instanceID != bb.instanceID) {
          continue;
        }

        return bb;
      }

      return undefined;
    },
    getBBDefinitionForType(domain, layer, type) {
      
      let bbs = this.getBBDefinitions();
      if(bbs == null){
        return undefined;
      }

      for (const bb of bbs.values()) {
        if (domain == bb.domain && layer ==bb.Layer && type == bb.Type) {
          return bb;
        }
      }

      return undefined;
    },
    getResp(instanceID) {
      let sce = this.scee;
      if (Object.keys(sce).indexOf("buildingBlocks") == -1) {
        console.log("field BBs not found");
        return [];
      }
      let sims = sce.buildingBlocks;

      for (const sim of sims.values()) {
        if (instanceID != sim.instanceID) {
          continue;
        }

        if (Object.keys(sim).indexOf("Responsibilities") == -1) {
          // console.log(
          //   "field resp in instanceID in  BBs not found"
          // );
          // console.dir(sim);
          continue;
        }
        console.log("Responsibilities found");
        console.dir(sim.responsibilities);
        return sim.responsibilities;
      }

      return [];
    },
    setResp(instanceID, respList) {
      console.log("setting resp of " + instanceID + " to " + respList);
      let bbIndex = this.getBBIndexForId(instanceID);
      console.log("index is " + bbIndex);

      console.dir(this.scee.buildingBlocks[bbIndex]);
      // console.log(Vue.prototype.$sce.buildingBlocks[simID].resp);
      Vue.set(this.scee.buildingBlocks[bbIndex], "responsibilities", respList);
      console.log("changed $sce");
      return;
    },  
    addObserver(instanceID) {
      let bbIndex = this.getBBIndexForId(instanceID);
      let o = this.scee.buildingBlocks[bbIndex].observers
      console.log("old observerlist:")
      console.dir(o)
      o.push( {"task": "publish",
                    "filter": "",
                    "period": 1000,
                    "element": "vehicle",
                    "trigger": "",
                    "type": "avro"})

      console.log("new observerlist:")
      console.dir(o)
      Vue.set(this.scee.buildingBlocks[bbIndex], "observers", o);
    },

    removeObserver(instanceID, oIdx) {
      let bbIndex = this.getBBIndexForId(instanceID);
      let o = this.scee.buildingBlocks[bbIndex].observers
      o.splice(oIdx,1)
      Vue.set(this.scee.buildingBlocks[bbIndex], "observers", o);
    },
    removeBB(instanceID) {
      let bbIndex = this.getBBIndexForId(instanceID);
      this.scee.buildingBlocks.splice(bbIndex,1)
    },


    /************************************************* 
    *******************  Trans DEF    *******************
    **************************************************/
    getTranslatorIndexForId(tID) {
      
      let sce = this.scee;
      if (Object.keys(sce).indexOf("translators") == -1) {
        console.log("field translators not found");
        return -1;
      }
      let ts = sce.translators;
      for (var i = 0; i < ts.length; i++) {
        if (tID == ts[i].ID) {

          console.log("translatorIndex for " + tID+" is "+i);
          return i;
        }
      }

      return -1;
    },


    updateTranslatorResps(){
    },


    addTranslatorToSce(name){
      let description = this.getTranslatorFromDataModel(name);
      let t = {"translatorID" : description.translatorID,
      "domain" : description.domain,
      "type" : description.type,
      "layerA" : description.layerA,
      "layerB" : description.layerB,
      "responsibilitiesA" : [],
      "responsibilitiesB" : [],
      "resources" : new Map(),
      "parameters" : ""
      }
      this.scee.translators.push(t);
    },


    removeTranslatorFromSce(id){
      let idx = this.getTranslatorIndexForId(id)
      if(idx>0){
        this.scee.translators.splice(idx,1)
      }
    },


    /************************************************* 
    *******************  RESOURCES *******************
    **************************************************/


    getAvailableResources(){
      return this.resourceList
    },

    getAvailableResourcesForLayer(domain, layer){
      domain = domain.toLowerCase()
      layer = layer.toLowerCase()
      // console.log("availableResourcesForLayer: "+domain+"-"+layer)
      var filteredresourceList = []
       for(let resource of this.resourceList){
         let tmpDomain = String(resource.domain).toLowerCase()
         let tmpLayer = String(resource.Layer).toLowerCase()
        if(tmpDomain == domain && tmpLayer == layer ){
          filteredresourceList.push(resource)
        } else {
          // console.log("mismatch for "+resource.ID + "__"+tmpDomain+"-"+tmpLayer)
        }
      }
      // console.dir(filteredresourceList)
      return filteredresourceList
    },


    getAvailableResourcesByType(domain, layer, type){
      console.log("looking for resources for "+domain +","+ layer+"," +type)
      domain = domain.toLowerCase()
      layer = layer.toLowerCase()
      type = type.toLowerCase()
      var filteredresourceList = []
      for(let resource of this.resourceList){
         let tmpDomain = String(resource.Domain).toLowerCase()
         let tmpLayer = String(resource.Layer).toLowerCase()
         let tmpType = String(resource.Type).toLowerCase()
        if(tmpDomain == domain && tmpLayer == layer && tmpType == type){
          filteredresourceList.push(resource)
        } else {
          console.log("mismatch for "+tmpDomain +","+ tmpLayer+"," +tmpType)
        }
      }
      if(filteredresourceList.size ==0){
        console.log("did not find anything for "+domain+","+layer+","+type)
      }
      console.dir(filteredresourceList)
      return filteredresourceList
    },

    getResourceByID(rid){
      rid = rid.toLowerCase()
      for(let resource of this.resourceList){
        if( rid == String(resource.ID).toLowerCase() ){
          return resource
        }
      }
      console.log("did not find resource by id="+rid)
      return null
    },

    removeResourceByID(rid){
      rid = rid.toLowerCase()
      for (var i = this.resourceList.length - 1; i >= 0; i--) {
        if (this.resourceList[i].ID.toLowerCase() == rid) { 
            this.resourceList.splice(i, 1);
            break
        }
      }
      return 
    },

    setResourcesForInstance(instanceID, resourceIDList) {
      console.log("setting resources of " + instanceID + " to ")
      console.dir(resourceIDList);
      let bbIndex = this.getBBIndexForId(instanceID);
      if ( bbIndex >= 0 ){
        var bbResourceMap = new Map()
        for(const resourceID of resourceIDList){
          let r = this.getResourceByID(resourceID)
          if ( r != null){
            bbResourceMap[resourceID] = r.Type
          }
        }

        Vue.set(this.scee.buildingBlocks[bbIndex], "Resources", bbResourceMap);
      } else {
        let tIndex = this.getTranslatorIndexForId(instanceID);
        if ( tIndex >= 0 ){
          var tResourceMap = new Map()
          for(const resourceID of resourceIDList){
            let r = this.getResourceByID(resourceID)
            if ( r != null){
              if(r.Layer == ''){
              tResourceMap[resourceID] = r.Type
              } else{
              tResourceMap[resourceID] = r.Layer+"."+r.Type
              }
            }
          }

          Vue.set(this.scee.translators[tIndex], "Resources", tResourceMap);
        } else {
          console.log("did not find idx for instanceid="+instanceID)
          return
        }
      }
      console.log("changed $sce");
      return;
    },
    
    getResourceIDsForInstance(instanceID) {
      let bbIndex = this.getBBIndexForId(instanceID);
      let resources = []
      if(bbIndex > -1){
        resources = this.scee.buildingBlocks[bbIndex].Resources
        resources = Object.keys(resources);// we only want the resourceid = key
        return resources
      }
      
      let tIndex = this.getTranslatorIndexForId(instanceID);
      if(tIndex > -1){
        resources = this.scee.translators[tIndex].Resources
        resources = Object.keys(resources);// we only want the resourceid = key
        return resources
      }
      return resources
      
    },
    


    /************************************************* 
    *******************  RESULTS *******************
    **************************************************/
    
    setResultsForInstance(instanceID, resultList) {
      let idx = this.getBBIndexForId(instanceID);
      var bbResultMap = new Map()
      for(const resultID of resultList){
        bbResultMap[resultID] = "default"
      }
      Vue.set(this.scee.buildingBlocks[idx], "Results", bbResultMap);
      console.log("changed $sce");
      return;
    },
    getResultsForInstance(instanceID) {
      let idx = this.getBBIndexForId(instanceID);
      if(idx == -1) return []
      console.dir(this.scee.buildingBlocks[idx].Results)
      return this.scee.buildingBlocks[idx].Results;

    },


    /************************************************* 
    *******************      SCE    *******************
    **************************************************/

 
    

    setDomainReferences(drs) {
      console.log("setting domainreferences");
      Vue.set(this.scee, "domainReferences", drs);
      console.log("changed $sce");
      return;
    },   
    
    resetSCE(){
      this.scee = JSON.parse(JSON.stringify(this.defaultSCE)); 
    },
    

    setDomainReferenceByID(domain, domainReferenceID) {
      Vue.set(this.scee.domainReferences, domain, domainReferenceID);
      return;
    },
    getDomainReferenceID(domain) {
      return this.scee.domainReferences[domain];
    },





    refreshNoParticipants(){
      console.log("refreshNoParticipants")
        //adapt the number of synced participants
        var numSyncedParticipants = 0
        for(const sim of this.scee.buildingBlocks){
          console.dir(sim)
          if(sim.Synchronized) {
            numSyncedParticipants += 1
          }
        }
        for(const trans of this.scee.translators){
           console.dir(trans)
            numSyncedParticipants += 1
          
        }
        for(const trans of this.scee.projectors){
           console.dir(trans)
            numSyncedParticipants += 1
          
        }
      Vue.set(this.scee.execution, "SyncedParticipants", numSyncedParticipants);
    },


    parseSce: function () {},
  },

  watch: {
    $roadMap: {
      handler() {
        console.log("app: roadMap changed");
      },
      deep: true,
    },
    $sce: {
      handler() {
        console.log("app: sce changed");


      },
      deep: true,
    },
  },
};
</script>

