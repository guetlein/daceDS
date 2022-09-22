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
  <div style="height: 100vh; overflow:auto; padding: 30px" >
    <h4>General</h4>
    <b-container fluid>
      <b-row>
        <b-col sm="3">
          <label for="input-scenarioID">ScenarioID</label>
        </b-col>
        <b-col sm="8">
          <b-form-input
            id="input-scenarioID"
            size="sm"
            v-model="app.scee.scenarioID"
          ></b-form-input>
        </b-col>
      </b-row>

      <b-row>
        <b-col sm="3">
          <label for="select-involved-domains">Involved Domains</label>
        </b-col>
        <b-col sm="8">
            <b-select  v-bind:value="app.involvedDomains"  @change="changeInvolvedDomains" id="'select-involved-domains" multiple :select-size="2">
            <option v-for="d in domains" v-bind:value="d.name" v-bind:key="d.name">
              {{ d.name }}
            </option>
            </b-select>
        </b-col>
      </b-row>      
      
   

      <b-row>
        <b-col sm="3">
          <label for="input-start">Scenario Start:</label>
        </b-col>
        <b-col sm="8">
          <b-form-input
            id="input-start"
            type="time"
            size="sm"
            :value="getHHMM(app.scee.simulationStart)" 
            @input="setSimulationStart($event)"
          ></b-form-input>
        </b-col>
      </b-row>

      <b-row>
        <b-col sm="3">
          <label for="input-end">Scenario End:</label>
        </b-col>
        <b-col sm="8">
          <b-form-input
            id="input-end"
            type="time"
            :value="getHHMM(app.scee.simulationEnd)" 
            @input="setSimulationEnd($event)"
            size="sm"
          ></b-form-input>
        </b-col>
      </b-row>
      <b-row>
        <b-col sm="3">
          <label for="input-end">RandomSeed:</label>
        </b-col>
        <b-col sm="8">
          <b-form-input
            id="input-randSeed"
            type="number"
            v-model.number="app.scee.execution.RandomSeed"
            size="sm"
          ></b-form-input>
        </b-col>
      </b-row>

      <b-row>
        <b-col sm="3">
          <label for="input-end">Synchronized Participants:</label>
        </b-col>
        <b-col sm="8">
          <b-form-input
            id="input-numSync"
            type="number"
            v-model.number="app.scee.execution.SyncedParticipants"
            size="sm"
          ></b-form-input>
        </b-col>
      </b-row>


    <!-- simulators -->
    </b-container>
    <br><br>
    <!-- <h4 v-if="getBBsFromSCE.length>0">Building Blocks</h4> -->
    <h4>Building Blocks</h4>

    <b-container fluid>
    <div
      v-if="getBBsFromSCE.length==0"
      :key="'placeholder_bb'" >
      No involved BB.
    </div>
    <div
      v-for="bb in getBBsFromSCE"
      :key="'baseSetupInstance' + bb.ID"
    >
        <b-row style="margin-top:20px; margin-bottom:10px " >
          <b-col sm="8">
              <b>{{ bb.ID }} </b>
          </b-col>
        </b-row>
        <b-row>
          <b-col cols="3">
            <label style="margin-left:20px" for="input-id">ID:</label>
          </b-col>
          <b-col sm="8">
            <b-form-input
              size="sm"
              :id="'input-id' +bb.ID "
              v-model="bb.ID"
             disabled
              class="mt-1"
            ></b-form-input>
          </b-col>
        </b-row>
        <b-row>
          <b-col cols="3">
            <label style="margin-left:20px" for="input-id">Type:</label>
          </b-col>
          <b-col sm="8">
            <b-form-input
              size="sm"
              :id="'input-type' +bb.ID "
              v-model="bb.Type"
             disabled
              class="mt-1"
            ></b-form-input>
          </b-col>
        </b-row>

        <b-row>
          <b-col cols="3">
            <label style="margin-left:20px" for="input-dom">Domain:</label>
          </b-col>
          <b-col sm="8">
            <b-form-input
              size="sm"
              :id="'input-dom' +bb.ID "
              v-model="bb.domain"
             disabled
              class="mt-1"
            ></b-form-input>
          </b-col>
        </b-row>

        <b-row>
          <b-col cols="3">
            <label style="margin-left:20px" for="input-default">Layer:</label>
          </b-col>
          <b-col sm="8">
            <b-form-input
              size="sm"
              :id="'input-layer' +bb.ID "
             v-model="bb.Layer"
             disabled
              class="mt-1"
            ></b-form-input>
          </b-col>
        </b-row>


      <!-- Todo: choose from bb def -->
        <b-row>
          <b-col cols="3">
            <label style="margin-left:20px" for="input-default">StepLength:</label>
          </b-col>
          <b-col sm="8">
            <b-form-input
              size="sm"
              number
              :id="'input-stepLength' +bb.ID "
              v-model.number="bb.StepLength"
              class="mt-1"
            ></b-form-input>
          </b-col>
        </b-row>



          <!--- leads to warning because of obects != array in binding -->
        <b-row>
          <b-col cols="3">
            <label style="margin-left:20px" for="input-default">Resources: </label>
          </b-col>
          <b-col sm="8">
            <b-select :value="getResourceIDsForInstance(bb.ID)" :id="'select-resource-'+bb.ID" multiple :select-size="4" @change="setResourcesForInstance(bb.ID, $event)">
            <!-- <b-select  multiple :select-size="4" > -->
            <option v-for="availableResource in availableResourcesForInstance(bb.ID)" v-bind:value="availableResource.ID" v-bind:key="availableResource.ID+bb.ID">
              {{ availableResource.Type }}: {{ availableResource.ID }}
            </option>
          </b-select>

          </b-col>
        </b-row> 
 

        <b-row>
          <b-col cols="3">
            <label style="margin-left:20px" >Responsibilities:</label>
          </b-col>
          <b-col sm="8">
            <b-form-input
              size="sm"
              :placeholder="bb.Responsibilities.join()"
              class="mt-1"
             disabled
            ></b-form-input>
          </b-col>
        </b-row>
        
        

        <b-row>
          <b-col cols="3">
            <label style="margin-left:20px" for="input-default">Results:</label>
          </b-col>
          <b-col sm="8">
            <b-form-checkbox-group size="sm" :value="getResultIDsForInstance(bb.ID)" :options="availableResultsForInstance(bb)"  @change="setResultsForInstance(bb.ID, $event)" />
          </b-col>
        </b-row>

        <b-row>
          <b-col cols="3">
            <label style="margin-left:20px" for="input-default">Synchronized:</label>
          </b-col>
          <b-col sm="8">
            <b-form-checkbox
              size="sm"
              :id="'input-timing-r' +bb.ID "
              v-model="bb.Synchronized"
              class="mt-1"
              @change="onTimingChange"
            >Participating</b-form-checkbox>
          </b-col>
        </b-row>


        <b-row>
          <b-col cols="3">
            <label style="margin-left:20px" for="input-default">observers: </label>
          </b-col>
          <b-col sm="8">
            
            <b-button @click="addObserver(bb.ID)" variant="outline-light" style="padding:0;background-color:width:300px;text-align:right"><b-badge variant="success">Add Observer</b-badge></b-button>
           
            <div v-for="(observer,idx) in bb.observers" :id="'input-observer-'+bb.ID+'-'+idx" v-bind:key="'input-observer-'+bb.ID+'-'+idx" style="font-size:12px; margin-bottom:20px; padding-left:5px; padding-top:5px;border-top:1px solid lightgray;" >
              Observer #{{idx}} <b-button @click="removeObserver(bb.ID,idx)" variant="outline-light" style="padding:0;background-color:width:100%;text-align:right"><b-badge variant="danger">Remove</b-badge></b-button>
               
              <b-row>
               <b-col cols="4">Task</b-col>
               <b-col cols="8"><b-form-input  size="xs"  :id="'input-observer-task-'+bb.ID+'-'+idx" style="font-size:12px;" v-model="observer.task" class="mt-1"></b-form-input></b-col>
              </b-row>
              <b-row>
               <b-col cols="4">Filter</b-col>
               <b-col cols="8"><b-form-input  size="xs"  :id="'input-observer-filter-'+bb.ID+'-'+idx" style="font-size:12px;" v-model="observer.filter" class="mt-1"></b-form-input></b-col>
              </b-row>
              <b-row>
               <b-col cols="4">Period</b-col>
               <b-col cols="8"><b-form-input  size="xs" type="number" :id="'input-observer-period-'+bb.ID+'-'+idx" style="font-size:12px;" v-model.number="observer.period" class="mt-1"></b-form-input></b-col>
              </b-row>
              <b-row>
               <b-col cols="4">Element</b-col>
               <b-col cols="8"><b-form-input  size="xs"  :id="'input-observer-element'+bb.ID+'-'+idx" style="font-size:12px;" v-model="observer.element" class="mt-1"></b-form-input></b-col>
              </b-row>

            </div>
          </b-col>
        </b-row> 


        <b-row>
          <b-col cols="3">
            <label style="margin-left:20px">Custom Params:</label>
          </b-col>
          <b-col sm="8">
            <b-form-input
              size="sm"
              :id="'input-custom-params' +bb.ID "
              v-model="bb.CustomParams"
              class="mt-1"
            ></b-form-input>
          </b-col>
        </b-row> 
    </div>
    </b-container>

    <br><br>



    
    <h4>translators</h4>
    <b-container fluid>
    <div
      v-if="gettranslatorsFromSCE.length==0"
      :key="'placeholder_t'" >
      <p>
      No involved translators.
      </p>
     </div>

    <div
      v-for="translator in gettranslatorsFromSCE"
      :key="'baseSetupInstance' + translator.ID"
    >
        <b-row style="margin-top:20px; margin-bottom:10px " >
          <b-col sm="8">
              <b>{{ translator.ID }} </b>
          </b-col>
        </b-row>
        <b-row>
          <b-col cols="3">
            <label style="margin-left:20px" for="input-id">InstanceID:</label>
          </b-col>
          <b-col sm="8">
            <b-form-input
              size="sm"
              :id="'input-id' +translator.ID "
              v-model="translator.ID"
             disabled
              class="mt-1"
            ></b-form-input>
          </b-col>
        </b-row>

        <b-row>
          <b-col cols="3">
            <label style="margin-left:20px" for="input-dom">Domain:</label>
          </b-col>
          <b-col sm="8">
            <b-form-input
              size="sm"
              :id="'input-dom' +translator.ID "
              v-model="translator.domain"
             disabled
              class="mt-1"
            ></b-form-input>
          </b-col>
        </b-row>

        <b-row>
          <b-col cols="3">
            <label style="margin-left:20px" for="input-default">Layer A:</label>
          </b-col>
          <b-col sm="8">
            <b-form-input
              size="sm"
              :id="'input-layera' +translator.ID "
             v-model="translator.layerB"
             disabled
              class="mt-1"
            ></b-form-input>
          </b-col>
        </b-row>

        <b-row>
          <b-col cols="3">
            <label style="margin-left:20px" for="input-default">Responsibilities A:</label>
          </b-col>
          <b-col sm="8">
            <b-form-input
              size="sm"
              :id="'input-respa' +translator.ID "
             :placeholder="translator.responsibilitiesA.join()"
             disabled
              class="mt-1"
            ></b-form-input>
          </b-col>
        </b-row>


        <b-row>
          <b-col cols="3">
            <label style="margin-left:20px" for="input-default">Layer B:</label>
          </b-col>
          <b-col sm="8">
            <b-form-input
              size="sm"
              :id="'input-layerb' +translator.ID "
             v-model="translator.layerB"
             disabled
              class="mt-1"
            ></b-form-input>
          </b-col>
        </b-row>


        <b-row>
          <b-col cols="3">
            <label style="margin-left:20px" for="input-default">Responsibilities B:</label>
          </b-col>
          <b-col sm="8">
            <b-form-input
              size="sm"
              :id="'input-respb' +translator.ID "
             :placeholder="translator.responsibilitiesB.join()"
             disabled
              class="mt-1"
            ></b-form-input>
          </b-col>
        </b-row>

        <b-row>
          <b-col cols="3">
            <label style="margin-left:20px" for="input-default">Resources: </label>
          </b-col>
          <b-col sm="8">
            <b-select :value="getResourceIDsForInstance(translator.ID)" :id="'select-resource-t-'+translator.ID" multiple :select-size="4" @change="setResourcesForInstance(translator.ID, $event)">
            <!-- <b-select :id="'select-resource-'+bb.ID"> sooo gehts/ -->
            <option v-for="(availableResource, index) in availableResourcesForInstance(translator.ID)" v-bind:value="availableResource.ID" v-bind:key="availableResource.ID+index">
              {{ availableResource.Type }}: {{ availableResource.ID }}
            </option>
          </b-select>

          </b-col>
        </b-row> 
 
    </div>

    </b-container>

    <br><br>
    
    <!-- <h4 v-if="getProjectors.length>0">Projectors</h4> -->
    <h4>Projectors</h4>
        <b-container fluid>
    <div
      v-if="getProjectors.length==0"
      :key="'placeholder_t'" >
      <p>
      No involved translators.
      </p>
     </div>

    <div
      v-for="projector in getProjectors"
      :key="'baseSetupInstance' + projector.ID"
    >
        <b-row style="margin-top:20px; margin-bottom:10px " >
          <b-col sm="8">
              <b>{{ projector.ID }} </b>
          </b-col>
        </b-row>
        <b-row>
          <b-col cols="3">
            <label style="margin-left:20px" for="input-id">InstanceID:</label>
          </b-col>
          <b-col sm="8">
            <b-form-input
              size="sm"
              id="'input-id' +id "
              v-model="projector.ID"
             disabled
              class="mt-1"
            ></b-form-input>
          </b-col>
        </b-row>


    </div>

    </b-container>
  </div>
</template>

<script>
export default {
  data() {
    return {
      app: this.$root.$children[0],
      m: new Map(),
    };
  },
   beforeMount: function () {
    // this.app.scee = JSON.parse(JSON.stringify(this.dflt));
    this.app.resetSCE();


  },
  methods: {
    addObserver(bbID){
      this.app.addObserver(bbID);
    }, 
    removeObserver(bbID, OIdx){
      this.app.removeObserver(bbID,OIdx);
    }, 
    getHHMM(seconds){
      return new Date(seconds * 1000).toISOString().substr(11, 8)
    }, 
    setSimulationStart(timestr){
      let h = timestr.split(":")[0]
      let m = timestr.split(":")[1]
      this.app.scee.simulationStart = h*3600+m*60
    },
    setSimulationEnd(timestr){
      let h = timestr.split(":")[0]
      let m = timestr.split(":")[1]
      this.app.scee.simulationEnd = h*3600+m*60
    },

    onTimingChange(){
      this.app.refreshNoParticipants();
    },
    getDomainReferencesFromBackend(){
      //todo: get map file from backend
      console.log("getDomainReferencesFromBackend ")
      for(const [key, value] of Object.entries(this.app.scee.domainReferences)){
        console.log("getDomainReferencesFromBackend ")
        console.dir(key);
        console.dir(value);
      }

    },

    /************************************************* 
    *******************  RESOURCES *******************
    **************************************************/

   //leads to "expects array value" warning
     availableResourcesForInstance(instanceID) {
      console.log("availableResourcesForInstance:"+instanceID)
      let avR = []
      for(const sim of this.getBBsFromSCE){
        if(instanceID==sim.ID){
          avR = this.app.getAvailableResourcesForLayer(sim.domain, sim.layer);
        }
      }
      for(const translator of this.gettranslatorsFromSCE){
        if(instanceID==translator.ID){
          let a = this.app.getAvailableResourcesForLayer(translator.domain, translator.layerB);
          let b = this.app.getAvailableResourcesForLayer(translator.domain, translator.layerB);
          avR = a.concat(b)
          avR = avR.concat(this.app.getAvailableResourcesForLayer(translator.domain, "")) //domain references
        }
      }
      console.log("availableResourcesForInstance for "+ instanceID)
      console.dir(avR)
      return avR
                
    },
    setResourcesForInstance(ID, listOfResources){
      console.log("selectResourceChanged " + ID + " changed. list of selected resources is:");
      console.dir(listOfResources)
      this.app.setResourcesForInstance(ID, listOfResources)
    },

    getResourceIDsForInstance(ID){
      console.log("getResourceIDsForInstance")
      console.dir(this.app.getResourceIDsForInstance(ID))
      return this.app.getResourceIDsForInstance(ID)
    },



    /************************************************* 
    *******************  RESULTS *******************
    **************************************************/
     availableResultsForInstance(b) {
      let bb = this.app.getBBDefinitionFromDataModel(b.domain, b.layer, b.type)
      if (bb == undefined) {
        console.log("could not find bb for:"+b.domain + b.layer + b.type);
        return []
      }
      return bb.results                
    },
    setResultsForInstance(ID, listOfResources){
      console.log("selectResourceChanged " + ID + " changed");
      this.app.setResultsForInstance(ID, listOfResources)
    },

    getResultsForInstance(ID){
      console.log("getresultsforinstance")
      console.dir(this.app.getResultsForInstance(ID))
      return this.app.getResultsForInstance(ID)
    },

    getResultIDsForInstance(ID){
      let v = this.app.getResultsForInstance(ID)
      console.log("getresultIDsforinstance")
      console.dir(v)
      if(Object.keys(v).length ===0){
        return []
      }
      return v.keys()
    },



    changeInvolvedDomains(d){
      console.log("changeInvolvedDomains");
      console.dir(d)
      var oldDomainRefernces = JSON.parse(JSON.stringify(this.app.scee.domainReferences));
      var newDomainReferences = {}
      for(let domain of d){
        console.log("domain: "+domain)
        if (! (domain  in  oldDomainRefernces)){
          newDomainReferences[domain] = "";
        } 
        //and keep values of existing ones
        else {
          newDomainReferences[domain] = oldDomainRefernces[domain];
        }
      }
      this.app.setDomainReferences(newDomainReferences)
    },



  },
  computed: {
    getBBsFromSCE() {
      if (this.app.scee.buildingBlocks != null)
        return this.app.scee.buildingBlocks;
      return []
    },
    gettranslatorsFromSCE() {
      if (this.app.scee.translators != null)
        return this.app.scee.translators;
      return []
    },
    getProjectors() {
      if (this.app.scee.projectors != null)
        return this.app.scee.projectors;
      return []
    },
    roadMaps(){
     var maps = []
     for(let resource of this.app.resourceList){
       if(resource.Type == "RoadMap"){
         maps.push(resource.ID)
       }
     }
     return maps
    },
    resources(){
     var maps = []
     for(let resource of this.app.resourceList){
      maps.push(resource)
     }
     return maps
    },

    domains(){
     var ds = []
     for(let d of this.app.datamodel.domains){
      ds.push(d)
     }
     return ds
    },

  },
  watch: {
    $sce: {
      handler() {
        console.log("basesetup: sce changed");
      },
      deep: true,
    },
    
  },
  mounted: function(){

  }
};
</script>

<style scoped>
h4{
  color: #555; /*#007bff;*/
  font-size:20px;
  border-bottom:1px solid #bbb;
}
</style>