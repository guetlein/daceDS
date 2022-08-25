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
 <div style="background-color: lightgrey">
          <b-modal
          id="modal-add"
          title="Add Instance"
          @ok="handleAddOk">
          <form ref="form" @submit.stop.prevent="handleAddSubmit">


              <b-row style="height:3em">
                <b-col cols=4>
                <label
                  for="selectSimulator">Type:</label>
              </b-col>
               <b-col cols=8>
                <b-form-select
                  v-model="newInstance.Type"
                  required
                  id="selectSimulator"
                  ref="selectSimulator"
                  :options="simulatorNames"
                  @change="typeChange"
                ></b-form-select>
                 </b-col>
              </b-row>


              <b-row style="height:3em">
                <b-col cols=4>
                <label
                  for="inputID"
                  invalid-feedback="name is required"
                  :state="inputIDState"
                  >ID:</label
                >
              </b-col>
               <b-col cols=8>
                <b-form-input
                  v-model="newInstance.ID"
                  required
                  id="inputID"
                  ref="inputID"
                  :state="inputIDState"
                ></b-form-input>
                 </b-col>
              </b-row>



              <b-row style="height:3em">
                <b-col cols=4>
                <label
                  for="inputDomain"
                  >Domain:</label
                >
              </b-col>
               <b-col cols=8>
                <b-form-input
                  v-model="newInstance.Domain"
                  disabled
                  id="inputDomain"
                  ref="inputDomain"
                ></b-form-input>
                 </b-col>
              </b-row>         


              <b-row style="height:3em">
                <b-col cols=4>
                <label
                  for="inputLayer"
                  >Layer:</label
                >
              </b-col>
               <b-col cols=8>
                <b-form-input
                  v-model="newInstance.Layer"
                  disabled
                  id="inputLayer"
                  ref="inputLayer"
                ></b-form-input>
                 </b-col>
              </b-row>                      


               <b-row style="height:3em">
                <b-col cols=4>
                <label
                  for="inputStepLength"
                  >Step Length:</label
                >
              </b-col>
               <b-col cols=8>
                <b-form-input
                  v-model="newInstance.StepLength"
                  id="inputStepLength"
                  ref="inputStepLength"
                  type="number"
                  min="1"
                  max="100000"
                ></b-form-input>
                 </b-col>
              </b-row>      
               <b-row style="height:3em">                <b-col cols=4>
                <label
                  for="inputStepLength"
                  >Timing:</label
                >
              </b-col>
                <b-col cols=8>
                <b-form-checkbox
                  v-model="newInstance.Synchronized"
                  id="checkNdmIsSynchronized"
                >Synchronized</b-form-checkbox>
                </b-col>   
              </b-row>


          </form>
        </b-modal>
  </div>
</template>

<script>


export default {
  data() {
    return {
      app: this.$root.$children[0],
      instanceCounter: 1,
      currentID: 0,
      inputIDState: null,
      simulatorNames: [],
      newInstance: {
        Synchronized : true,
        Resources : {}},
    };
  },
  components: {},
  methods: {


    addInstance() {
      
      if(this.components.length==0){
        alert("No simulators registered!")
        return;
      }
      this.newInstance = {
        instanceID : "",
        type : "",
        layer: "",
        domain : "",
        stepLength: 1000,
        parameters: {},
        resources: {},
        results: new Map(),
        synchronized : true,
        isExternal : false,
        responsibilities: [],
        observers: [          
            ]
      }
      this.$bvModal.show('modal-add')
    },


    /////////////////////////////////
    //for modal
    getSim(name){
      for(let sim of this.components){
        if (sim.name == name){
          return sim
        }
      }
    console.log(name+" was not found")
    return null
    },    

    typeChange(){
        let sim = this.getSim(this.newInstance.Type)
        this.$refs.inputID.value = sim.name+String(this.instanceCounter)
        this.$refs.inputLayer.value = sim.layer
        this.$refs.inputDomain.value = sim.domain
        this.newInstance.instanceID = sim.name+String(this.instanceCounter)
        this.newInstance.Layer = sim.layer
        this.newInstance.Domain = sim.domain
    },
    checkFormValidity() {
      const IDValid = this.$refs.inputID.checkValidity();
      this.inputIDState = IDValid;
      //todo: add more
      return IDValid;
    },
    resetModal() {
      this.newInstance = {}
    },
    handleAddOk(bvModalEvt) {
      // Prevent modal from closing
      bvModalEvt.preventDefault();
      // Trigger submit handler
      this.handleAddSubmit(bvModalEvt);
    },
    handleAddSubmit(e) {

      // Exit when the form isn't valid
      if (!this.checkFormValidity()) {
        console.log("Form is not valid");
        return;
      }
      console.log("Form is valid");
      console.dir(e);

      // Hide the modal manually
      this.$nextTick(() => {
        this.$bvModal.hide('modal-add');
      });

      
      this.instanceCounter++;
      this.app.scee.buildingBlocks.push(this.newInstance);
      this.app.refreshNoParticipants();
    },
  },
   computed: {
      components(){
        return this.app.datamodel.components
      },
    },
  watch: {
    simulators : function(){
      this.simulatorNames = []
      for(let simulator of this.components){
          this.simulatorNames.push({ text: simulator.name, value: simulator.name, disabled: !simulator.enabled })
          // this.simulatorNames.push(simulator.name)
        }
    },
  },
  mounted() {
    this.$root.$on("addInstance", () => {
      console.log("addInstance");
      this.addInstance();

    });
  },
};
</script>