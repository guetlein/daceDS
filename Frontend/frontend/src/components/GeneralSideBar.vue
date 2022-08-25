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
  <div style="width:200px; padding: 15px 10px 0px 10px; background-color:#fafafa; height:100%">
    <!-- Resources<br> -->
    <b-button block size="sm"  v-b-modal.modal-add-resource variant="light"  style=" background-color:#eee;text-align: left;" v-b-tooltip.hover title="Add a resource from your local computer in order to use it when designing a new scenario.">
      <img src="./../assets/list.svg" width="25px" /> 
      Upload Resource
    </b-button>

    <!-- <b-button block size="sm" style="text-align: left; " variant="light"  >
      <img src="./../assets/radar_plot.svg" width="25px" class="gray" /> 
      Clear Resources
      </b-button> -->

    <!-- <br><br>Domains<br> -->
    <b-button block size="sm" style="background-color:#eee; text-align: left; " variant="light"   @click="addDomain"  v-b-tooltip.hover title="Add a new domain to the taxonomy. Modify the new domain via the 'Domains' tab.">
      <img src="./../assets/mind_map.svg" width="25px" />
      Add Domain
    </b-button>
    <!-- <b-button block v-b-modal.modal-clear-resources size="sm" variant="light"  style="text-align: left; " >
      <img src="./../assets/mind_map.svg" width="25px" class="gray" />
      Clear Domains
    </b-button> -->

    <!-- <br><br>Layers<br> -->
    <b-button  block size="sm" variant="light"   style="background-color:#eee; text-align: left; "  @click="addLayer"  v-b-tooltip.hover title="Add a new layer to the taxonomy. Modify the new layer via the 'Layers' tab.">
      <img src="./../assets/flow_chart.svg" width="25px" /> 
      Add Layer
    </b-button>

    <!-- <b-button block v-b-modal.modal-clear-resources size="sm" variant="light"  style="text-align: left; " >
      <img src="./../assets/flow_chart.svg" width="25px" class="gray" /> 
      Clear Layers
    </b-button> -->

    <!-- <br><br>Simulators<br> -->
    <b-button block size="sm" variant="light" style="background-color:#eee; text-align: left; "  @click="addComponent"  v-b-tooltip.hover title="Add a new simulator to the catalog. Customize it via the 'Simulators' tab.">
      <img src="./../assets/self_service_kiosk.svg" width="25px" /> 
      Add Simulator
      </b-button>

    <!-- <b-button block v-b-modal.modal-clear-resources size="sm" variant="light" style="text-align: left; " >
      <img src="./../assets/automotive.svg" width="25px" class="gray"  /> 
       Clear Simulators
    </b-button> -->

    <!-- <br><br>translators<br> -->
    <b-button block size="sm" variant="light"  style="background-color:#eee; text-align: left;" @click="addTranslator"  v-b-tooltip.hover title="Add a new translator to the catalog. Customize it via the 'translators' tab.">
      <img src="./../assets/collaboration.svg" width="25px" />
     Add Translator
     </b-button>

    <!-- <b-button block v-b-modal.modal-clear-resources size="sm" variant="light"  style="text-align: left; " >
      <span class="img-container">
      <img src="./../assets/workflow.svg" width="25px" class="gray" /> 
      
      </span>
      Clear Translator
    </b-button> -->




<b-modal  id="modal-add-resource"     title="Add resource"     @show="resetResourceModal"      @hidden="resetResourceModal"      @ok="handleResourceOk"
        >
          <form ref="form" @submit.stop.prevent="handleResourceSubmit">
            <p class="my-4" style="padding: 20px; padding-top: 0px">
              <b-row>Resource Path</b-row>
              <b-row
                ><b-form-file
                  v-model="addResourceFile"
                  :state="Boolean(addResourceFile)"
                  placeholder="Choose a file or drop it here..."
                  drop-placeholder="Drop file here..."
                  @input="updateID"
                ></b-form-file>
                <!--div class="mt-3">
                  Selected file:
                  {{ addResourceFilePath ? addResourceFilePath.name : "" }}
                </div-->
              </b-row>
              <br />
              <b-row>
                <label
                  for="inputResourceId"
                  invalid-feedback="ID is required"
                  :state="addResourceFileIDState"
                  >Resource ID:</label
                >
              </b-row>
              <b-row>
                <b-form-input
                  v-model="addResourceFileID"
                  required
                  placeholder="Enter a descriptive ID"
                  id="inputResourceId"
                  ref="inputResourceId"
                  :state="addResourceFileIDState"
                ></b-form-input>
              </b-row>
              <br />
              <b-row>Resource Domain</b-row>
              <b-row>
                <b-form-select
                  v-model="addResourceFileDomain"
                  :options="resourceDomainOptions"
                ></b-form-select>
              </b-row>
              <br />
              <b-row>Resource Layer</b-row>
              <b-row>
                <b-form-select
                  v-model="addResourceFileLayer"
                  :options="resourceLayerOptions"
                ></b-form-select>
              </b-row>
              <br />
              <b-row>Resource Type</b-row>
              <b-row>
                <b-form-select
                  v-model="addResourceFileType"
                  :options="resourceTypeOptions"
                ></b-form-select>
              </b-row>
           </p>
          </form>
        </b-modal>
  </div>
</template>

<style scoped>
.img-container {
  position: relative;
}

.overlay-img {
  position: absolute;
  top: 3px;
  left: -3px;
    filter: grayscale(100%);
}
.gray {
    filter: grayscale(100%);
}
</style>

<script>
import PostService from "./../PostService.js";
export default {
  data() {
    return {
      app: this.$root.$children[0],

      /////////////////
      /// resources
      resourceTypeOptions: [ //todo compute from datamodel
        { value: "RoadMap", text: "RoadMap", selected: true },
        { value: "Traffic", text: "Traffic" },
        { value: "Additional", text: "Additional", disabled: false },
        { value: "Version", text: "Version", disabled: false },
        { value: "Sce", text: "Sce", disabled: false },
        { value: "DomainReference", text: "DomainReference", disabled: false },
      ],
      resourceDomainOptions: [ //todo compute from datamodel
        { value: "Traffic", text: "Traffic", selected: true },
        { value: "Numbers", text: "Numbers" },
        { value: "Communication", text: "Communication" },
      ],
      resourceLayerOptions: [ //todo compute from datamodel
        { value: "Micro", text: "Micro", selected: true },
        { value: "Meso", text: "Meso"},
        { value: "Macro", text: "Macro"},
        { value: "Submicro", text: "Submicro"},
      ],
      addResourceFileID: "",
      addResourceFile: null,
      addResourceFileType:"RoadMap",
      addResourceFileDomain:"",
      addResourceFileLayer:"RoadMap",
      //modal
      addResourceFileIDState: null,



    };
  },
  components: {},
  methods: {

      /***********************
        RESOURCES
      *****************/

     clickUploadResource() {
      document.getElementById("uploadResourceInput").click();
    },
    getResourceList() {
      console.log("fetching resource list from backend");
      PostService.getResourceList().then((result) => {
        console.log("got the following resource list:");
        console.dir(result.data);
        this.app.resourceList = result.data;
      }).catch((error) => {
				console.log('fetching resource list failed');
				console.log(error);
			});
    },
    
    uploadResource(file) {
      PostService.sendResource(file).then((result) => {
        console.dir(result);
        this.getResourceList();
      }).catch((error) => {
				console.log('uploading resource failed');
				console.log(error);
			});
    },

    //for modal
    checkResourceFormValidity() {
      const idValid = this.$refs.inputResourceId.checkValidity();
      this.addResourceFileIDState = idValid;

      return idValid && Boolean(this.addResourceFile);
    },
    resetResourceModal() {
      this.addResourceFileID = "";
      this.addResourceFileDomain = null;
      this.addResourceFileLayer = null;
      this.addResourceFileType = null;
      this.addResourceFile = null;
      this.addResourceFileIDState = false;
    },
    handleResourceOk(bvModalEvt) {
      // Prevent modal from closing
      bvModalEvt.preventDefault();
      // Trigger submit handler
      this.handleResourceSubmit(bvModalEvt);
    },
    handleResourceSubmit(e) {

      // Exit when the form isn't valid
      if (!this.checkResourceFormValidity()) {
        console.log("Form is not valid");
        return;
      }
      console.log("Form is valid");
      console.dir(e);
      // console.log("file:");
      // console.dir(this.addResourceFile);

      // Hide the modal manually
      this.$nextTick(() => {
        this.$bvModal.hide("modal-add-resource");
      });

      let resource = {"ID" : this.addResourceFileID,
      "Domain" : this.addResourceFileDomain,
      "Layer" : this.addResourceFileLayer,
      "Type" : this.addResourceFileType,
      "File" : this.addResourceFile}

      if (resource.Type == 'DomainReference'){
        resource.Layer = ''
      }
      this.uploadResource(resource);
    },


    updateID(p){
      this.addResourceFileID = p.name
    },



    addTranslator(){
      this.app.addTranslator()
      this.app.uploadDatamodel();
    },


    addDomain(){
      this.app.addDomain()
      this.app.uploadDatamodel();
    },

    addComponent(){
      this.app.addComponent()
      this.app.uploadDatamodel();
    },

    addLayer(){
      this.app.addLayer()
      this.app.uploadDatamodel();
    },






  },
  // fetch infos on start
   mounted: function () {
    this.getResourceList();
  },
};
</script>