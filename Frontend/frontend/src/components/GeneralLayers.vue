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
          id="modal-edit-layer"
          title="Edit Layer"
          size="lg"
          @ok="handleEditOk"> 
          <form ref="form" @submit.stop.prevent="handleEditSubmit">
            <p class="my-4" style="padding: 20px; padding-top: 0px">
                <b-row style="height:3em"> <b-col><b>Basics</b><hr> </b-col>          </b-row>
                         
              <b-row style="height:3em">
                <b-col cols=3>
                <label
                  for="selectDomain"
                  invalid-feedback="domain is required"
                  :state="selectDomainState"
                  >Domain:</label
                >
              </b-col>
               <b-col cols=8>
                <b-form-select
                  v-model="currentLayer.domain"
                  required
                  id="selectDomain"
                  ref="selectDomain"
                  :state="selectDomainState"
                  :options="domainNames"
                  selected="currentLayer.domain"

                ></b-form-select>
                 </b-col>
              </b-row>

                            <b-row style="height:3em">
                <b-col cols=3>
                <label
                  for="inputName"
                  invalid-feedback="name is required"
                  :state="inputNameState"
                  >Name:</label
                >
              </b-col>
               <b-col cols=8>
                <b-form-input
                  v-model="currentLayer.name"
                  required
                  placeholder="Name"
                  id="inputName"
                  ref="inputName"
                  :state="inputNameState"
                ></b-form-input>
                 </b-col>
              </b-row>


                <b-row style="height:3em">
                <b-col cols=3>
                <label
                  for="inputVersion"
                  invalid-feedback="version is required"
                  :state="inputVersionState"
                  >Version:</label
                >
              </b-col>
               <b-col cols=8>
                <b-form-input
                  v-model="currentLayer.version"
                  required
                  placeholder="Enter a Version"
                  id="inputVersion"
                  ref="inputVersion"
                  :state="inputVersionState"
                ></b-form-input>
                 </b-col>
              </b-row>

              <b-row style="height:3em"> <b-col> <br> </b-col><hr>  </b-row>
                <b-row style="height:3em"> <b-col><b>Native Data Model</b><hr> </b-col>
              </b-row>
                              
              <b-row style="height:3em">
                <b-col cols=3>
                <label
                  for="inputNdmName"
                  invalid-feedback="ndm name is required"
                  :state="inputNdmNameState"
                  >Name:</label
                >
              </b-col>
               <b-col cols=8>
                <b-form-input
                  v-model="currentLayer.ndm.name"
                  required
                  placeholder="Enter a NdmName"
                  id="inputNdmName"
                  ref="inputNdmName"
                  :state="inputNdmNameState"
                ></b-form-input>
                 </b-col>
              </b-row>   



               <b-row style="height:3em">
                <b-col cols=3>
                Access:
                
              </b-col>
                <b-col cols=2>
                <b-form-checkbox
                  v-model="currentLayer.ndm.isPersistent"
                  id="checkNdmIsPersistent"
                >Persistent</b-form-checkbox>
                </b-col>
                <b-col cols=2>
                <b-form-checkbox
                  v-model="currentLayer.ndm.isGettable"
                  id="checkNdmIsGettable"
                >Gettable</b-form-checkbox>
                 </b-col>

          
                <b-col cols=2>
                <b-form-checkbox
                  v-model="currentLayer.ndm.isSettable"
                  id="checkNdmIsSettable"
                >Settable</b-form-checkbox>
                 </b-col>
                <b-col cols=2>
                <b-form-checkbox
                  v-model="currentLayer.ndm.isObservable"
                  id="checkNdmIsObservable"
                >Observable</b-form-checkbox>
                 </b-col>
              </b-row>


                 <b-row style="height:3em">
                <b-col cols=3>
                <label
                  for="inputNdmKeyName"
                  invalid-feedback="ndm key name is required"
                  :state="inputNdmKeyNameState"
                  >Key Name:</label
                >
              </b-col>
               <b-col cols=8>
                <b-form-input
                  v-model="currentLayer.ndm.key.name"
                  required
                  placeholder="Enter a NdmKeyName"
                  id="inputNdmKeyName"
                  ref="inputNdmKeyName"
                  :state="inputNdmKeyNameState"
                ></b-form-input>
                 </b-col>
              </b-row>     
              
              <b-row style="height:3em">
                <b-col cols=3>
                <label
                  for="inputNdmKeyType"
                  invalid-feedback="ndm key name is required"
                  >Key Type:</label
                >
              </b-col>
               <b-col cols=8>
                <b-form-select
                  v-model="currentLayer.ndm.key.type"
                  id="selectNdmKeyType"
                  ref="selectNdmKeyType"
                  :options="dataTypes"
                  selected="currentLayer.ndm.key.type"
                ></b-form-select>
                 </b-col>
              </b-row>
              <b-row style="height:3em">
                <b-col cols=3>
                Key Access:
              </b-col>
                <b-col cols=2>
                <b-form-checkbox
                  v-model="currentLayer.ndm.key.isGettable"
                  id="checkNdmKeyIsGettable"
                >Gettable</b-form-checkbox>
                 </b-col>

          
                <b-col cols=2>
                <b-form-checkbox
                  v-model="currentLayer.ndm.key.isSettable"
                  id="checkNdmKeyIsSettable"
                >Settable</b-form-checkbox>
                 </b-col>
                <b-col cols=2>
                <b-form-checkbox
                  v-model="currentLayer.ndm.key.isObservable"
                  id="checkNdmKeyIsObservable"
                >Observable</b-form-checkbox>
                 </b-col>
              </b-row>





            </p>
          </form>
        </b-modal>

         <b-modal
          id="modal-raw-edit-layer"
          title="Edit Layer Raw"
          size="lg"
          @ok="handleRawEditOk"> 
          <form ref="form" @submit.stop.prevent="handleRawEditSubmit">
            <p class="my-4" style="padding: 20px; padding-top: 0px">
                <b-row style="height:3em"> <b-col><b>Raw Edit Mode</b><hr> </b-col>          </b-row>
                <b-row>
              <b-form-textarea
      id="textarea"
      v-model="currentLayer"
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
            :items="layersShort"
          >
                      <template #cell(id)="id">
              {{id.value}}
                <!-- <col style="width:1000px;" /> -->
            </template>
            <template #cell(actions)="layer" >
                <!-- <div style="width:100px;" /> -->
            <!-- <b-button
                  style="width: 36px; background-color:#ddd; margin-left:3px"
              size="sm" 
              @click="editLayer(layer.value.domain, layer.value.name)"
            >               <img
                  src="../assets/outline_mode_edit_black_18dp.png"
                /></b-button> 
            <b-button
                  style="width: 36px; background-color:#ddd; margin-left:3px"
              size="sm"  
              @click="rawEditLayer(layer.value.domain, layer.value.name)"
            >               <img
                  src="../assets/baseline_translate_black_18dp.png"
                /></b-button> 
            <b-button
              size="sm"  
                  style="width: 36px; background-color:#ddd; margin-left:3px"
              @click="delLayer(layer.value.domain, layer.value.name)"
            >                <img
                  src="../assets/baseline_delete_forever_black_18dp.png"
                /></b-button>  -->

                
                <b-button variant="light-outline"  style="padding:0px;margin-bottom:0px" size="sm" @click="editLayer(layer.value.layer)"  ><img src="./../assets/edit_image.svg" width="25px" /></b-button>
                <b-button variant="light-outline"  style="padding:0px;padding-left:6px;margin-bottom:0px;text-align:center;" size="sm" @click="rawEditLayer(layer.value.layer)"   ><img src="./../assets/fine_print.svg" width="25px"  /></b-button>
                <b-button variant="light-outline"  style="padding:0px;margin-bottom:0px" size="sm" @click="delLayer(layer.value.layer)" ><img src="./../assets/empty_trash.svg" width="25px"   /></b-button>
            </template>
          </b-table>               
          
    <!-- <div style="text-align:center">
           <b-button style="margin-right:5px"
      size="sm"
      variant="light"
      ><img src="./../assets/plus.png"  width="25px" /> Add Layer</b-button>


<b-button v-b-modal.modal-clear-resources  size="sm" variant="light"> <img src="./../assets/filing_cabinet.png"  width="25px" /> Clear Layers </b-button>
    </div> -->
  </div>
</template>
<script>

export default {
  data() {
    return {
      app: this.$root.$children[0],
      dataTypes: ['string', 'integer', 'double', 'boolean'],
      layersShort: [],
      domainNames: [],
      //modal
      dummyLayer : {
        domain :"",
        name : "",
        version : -1,
        ndm : {
         "name": "",
          "isPersistent": false,
          "isGettable": true,
          "isSettable": false,
          "isObservable": true,
          "compounds": [""],
          "key": {
            "name": "",
            "type": "string",
            "isGettable": false,
            "isSettable": false,
            "isObservable": false
          },
          "methods": [""],
          "primitives": [""]
        }
      },
      currentLayer: null,
      referenceLayer: null,
      inputNameState: null,
      inputVersionState: null,
      selectDomainState: null,


      inputNdmNameState: null,
      inputNdmKeyNameState: null,

    };
  },
  methods: {
    
    getLayer(domain, name){
      for(let layer of this.layers){
        if (layer.domain == domain && layer.name == name){
          return layer
        }
      }
    console.log(domain+"."+name+" was not found")
    return null
    },    
    
    setLayer(refdomain, refname, newContent){
      for(let layerID in this.layers){
        let layer = this.layers[layerID]
        if (layer.domain == refdomain && layer.name == refname){
          this.layers[layerID] = newContent

          console.log("set contents of "+refdomain+"."+refname+" to ")
          console.dir(newContent)
          return
        }
      }
      console.log(refdomain+"."+refname+" was not found")
    },
    
    editLayer(layer){
      this.currentLayer = JSON.parse(JSON.stringify(layer))
      this.referenceLayer = JSON.parse(JSON.stringify(layer))
      console.log("editing following layer")
      console.dir(this.currentLayer)
      this.$bvModal.show('modal-edit-layer')
    },    
    rawEditLayer(layer){
      this.currentLayer = JSON.parse(JSON.stringify(layer))
      this.referenceLayer = JSON.parse(JSON.stringify(layer))
      console.log("editing following layer")
      console.dir(this.currentLayer)
      this.$bvModal.show('modal-raw-edit-layer')
    },
 
    delLayer(layer){
      this.app.removeLayer(layer);
            this.app.uploadDatamodel();
    },

    //for modal
    checkFormValidity() {
      const nameValid = this.$refs.inputName.checkValidity();
      this.inputNameState = nameValid;
      const versionValid = this.$refs.inputVersion.checkValidity();
      this.inputVersionState = versionValid;
      // const domainValid = this.$refs.selectDomain.checkValidity();
      // this.selectDomainState = domainValid;

      return nameValid && versionValid;
    },
    resetModal() {
      this.currentLayer = JSON.parse(JSON.stringify(this.dummyLayer));
      this.referenceLayer = JSON.parse(JSON.stringify(this.dummyLayer));
    },
    handleEditOk(bvModalEvt) {
      // Prevent modal from closing
      bvModalEvt.preventDefault();
      // Trigger submit handler
      this.handleEditSubmit(bvModalEvt);
    },
    handleEditSubmit(e) {

      // Exit when the form isn't valid
      if (!this.checkFormValidity()) {
        console.log("Form is not valid");
        return;
      }
      console.log("Form is valid");
      console.dir(e);

      // Hide the modal manually
      this.$nextTick(() => {
        this.$bvModal.hide('modal-edit-layer');
      });

      //save layer
      this.setLayer(this.referenceLayer.domain, this.referenceLayer.name, this.currentLayer)
      this.app.uploadDatamodel();
      this.resetModal();
    },

    handleRawEditOk(bvModalEvt) {
      bvModalEvt.preventDefault();
      this.handleRawEditSubmit(bvModalEvt);
    },
    handleRawEditSubmit() {
      // Don't validate anything.....
      // Hide the modal manually
      this.$nextTick(() => {
        this.$bvModal.hide('modal-raw-edit-layer');
      });

      this.currentLayer = JSON.parse(this.currentLayer)
      
      //save layer
      this.setLayer(this.referenceLayer.domain, this.referenceLayer.name, this.currentLayer)
      this.app.uploadDatamodel();
      this.resetModal();
  }
  },
  computed: {
      layers(){
        return this.app.datamodel.layers
      },
      domains(){
        return this.app.datamodel.domains
      }
    },
  watch: {
    layers : function(){
      //alert("layers did change")
      //didn't work as computed property
      this.layersShort = []
      for(let layer of this.layers){
          var layerShort = {
            id : layer.name,
            domain : layer.domain,
            version : layer.version,
            actions : {layer : layer}
          }
          this.layersShort.push(layerShort)
        }
      
      //alert(this.layersShort)

    },
    domains : function (){
      this.domainNames = []
        //this.domainNames.push("")
        for(let domain of this.domains){
          this.domainNames.push(domain.name)
        }
      console.log("domainNames:")
      console.dir(this.domainNames)
    }
  },
   beforeMount: function(){
    this.currentLayer = JSON.parse(JSON.stringify(this.dummyLayer))
   },
  mounted: function () {
    this.layersShort = []
      for(let layer of this.layers){
          var layerShort = {
            id : layer.name,
            domain : layer.domain,
            version : layer.version,
            actions : {layer : layer}
          }
          this.layersShort.push(layerShort)
        }

        this.domainNames = []
        //this.domainNames.push("")
        for(let domain of this.domains){
          this.domainNames.push(domain.name)
        }
      // console.log("domainNames:")
      // console.dir(this.domainNames)

  },
};
</script>