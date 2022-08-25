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
 <div>
         <b-modal
          id="modal-raw-edit-sim"
          title="Edit Simulator Raw"
          size="lg"
          @ok="handleRawEditOk"> 
          <form ref="form" @submit.stop.prevent="handleRawEditSubmit">
            <p class="my-4" style="padding: 20px; padding-top: 0px">
                <b-row style="height:3em"> <b-col><b>Raw Edit Mode</b><hr> </b-col>          </b-row>
                <b-row>
              <b-form-textarea
      id="textarea"
      v-model="currentSim"
      placeholder="Enter something..."
      rows="3"
      max-rows="16"
    ></b-form-textarea>
                </b-row>
            </p>
          </form>
        </b-modal>
            

          <b-table
            striped
            hover
            
            bordered
            :items="simulatorsShort"
          >
                  <template #cell(name)="name">
              {{name.value}}
                <!-- <col style="width:100px;" /> -->
            </template>

            <template #cell(actions)="sim">
                <!-- <col style="width:100px;" /> -->

              <b-button variant="light-outline"  style="padding:0px;margin-bottom:0px" size="sm" @click="rawEditSim(sim.value.simulator)" ><img src="./../assets/edit_image.svg" width="25px"   /></b-button>
            
              <b-button variant="light-outline"  style="padding:0px;margin-bottom:0px" size="sm" @click="delSim(sim.value.simulator)" ><img src="./../assets/empty_trash.svg" width="25px"   /></b-button>
            
            </template>
          </b-table
        >
  </div>
</template>

<script>
export default {
  data() {
    return {
      app: this.$root.$children[0],
      simulatorsShort: [],
      dummySim: {
        name: "",
        domain: "",
        layer: "",
        results: [],
        resourceTypes: [],
        Resources : {}
      },
      referenceSim: null,
      currentSim: null,
    };
  },
  components: {},
  methods: {


    getSim(name, domain, layer){
      for(let sim of this.simulators){
        if (sim.domain == domain && sim.name == name && sim.layer == layer){
          return sim
        }
      }
    console.log(domain+"."+layer+"."+name+" was not found")
    return null
    },    
    
    setSim(refname, refdomain, reflayer, newContent){
      for(let simID in this.simulators){
        let sim = this.simulators[simID]
        if (sim.domain == refdomain && sim.name == refname && sim.layer == reflayer){
          this.simulators[simID] = newContent

          console.log("set contents of "+refdomain+"."+reflayer+"."+refname+" to ")
          console.dir(newContent)
          return
        }
      }
      console.log(refdomain+"."+reflayer+"."+refname+" was not found")
    },

    rawEditSim(sim){
      this.currentSim = JSON.parse(JSON.stringify(sim))
      this.referenceSim = JSON.parse(JSON.stringify(sim))
      console.log("editing following sim")
      console.dir(this.currentSim)
      this.$bvModal.show('modal-raw-edit-sim')
    },


    delSim(sim){
      this.app.removeComponent(sim)
      this.app.uploadDatamodel();
    },

    resetModal() {
      this.currentSim = JSON.parse(JSON.stringify(this.dummySim));
      this.referenceSim = JSON.parse(JSON.stringify(this.dummySim));
    },
      handleRawEditOk(bvModalEvt) {
      bvModalEvt.preventDefault();
      this.handleRawEditSubmit(bvModalEvt);
    },
    handleRawEditSubmit() {
      // Don't validate anything.....
      // Hide the modal manually
      this.$nextTick(() => {
        this.$bvModal.hide('modal-raw-edit-sim');
      });

      this.currentSim = JSON.parse(this.currentSim)
      
      //save layer
      this.setSim(this.referenceSim.name, this.referenceSim.domain, this.referenceSim.layer, this.currentSim)
      this.app.uploadDatamodel();
      this.resetModal();
  },
  },
  computed: {
      simulators(){
        return this.app.datamodel.components
      },
    },
  watch: {
    simulators : function(){
      this.simulatorsShort = []
      for(let simulator of this.simulators){
          var simulatorShort = {
            name : simulator.name,
            domain : simulator.domain,
            layer : simulator.layer,
            version : simulator.version,
            results : "",
            resourceTypes : "",
            actions : {simulator : simulator}
          }

          if (simulator.results == undefined || simulator.results.length==0){
            simulatorShort.results = "--"
          } else {
            for(let i=0;i<simulator.results.length;i++){
              simulatorShort.results += simulatorShort.results +simulator.results[i]
              if(i+1<simulator.results.length){
                simulatorShort.results += ", "
              }
            }
          }

          if (simulator.resourceTypes == undefined || simulator.resourceTypes.length==0){
            simulatorShort.resourceTypes = "--"
          } else {
            for(let i=0;i<simulator.resourceTypes.length;i++){
              simulatorShort.resourceTypes += simulatorShort.resourceTypes +simulator.resourceTypes[i]
              if(i+1<simulator.resourceTypes.length){
                simulatorShort.resourceTypes += ", "
              }
            }
          }
          
          this.simulatorsShort.push(simulatorShort)
        }
    }
  },   
  beforeMount: function(){
    this.currentSim = JSON.parse(JSON.stringify(this.dummySim))
   },
  mounted() {
  },
};
</script>