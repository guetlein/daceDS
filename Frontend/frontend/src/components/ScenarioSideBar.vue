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
    <input type="file" id="sceFileUpload" hidden @change="uploadSce" />
    <b-button
      size="sm"
      @click="chooseSceFile()"
      block variant="light" style="background-color:#eee; text-align: left; "
      left
      ><img src="./../assets/upload.svg" width="25px"/> Upload Scenario</b-button
    >
    <b-button size="sm" block variant="light" style="background-color:#eee; text-align: left; "   @click="exportSceFile()"
      ><img src="./../assets/download.svg" width="25px" />Export Scenario</b-button
    >
    <b-button size="sm" block variant="light" style="background-color:#eee; text-align: left; "  @click="resetSce()"
      ><img src="./../assets/empty_trash.svg" width="25px" /> Reset Scenario</b-button
    >
    <b-button
      size="sm"
      block variant="light" style="background-color:#eee; text-align: left; "
      @click="runSce"
      :disabled=isRunDisabled
      ><img src="./../assets/sports_mode.svg" width="25px" v-if="isCancelDisabled" /> 
      <b-spinner type="border" small v-if="isRunDisabled" style="height:22px;width:22px;" /> Run Scenario</b-button
    >
    
    <b-button size="sm" block variant="light" style="background-color:#eee; text-align: left; "   :disabled=isCancelDisabled
      @click="cancelRun"
      ><img src="./../assets/cancel.svg" width="25px" /> Abort Run</b-button
    >

  </div>
</template>

<script>
export default {
  data() {
    return {
      app: this.$root.$children[0],
    };
  },
  components: {},
  computed: {
    isCancelDisabled(){
      return !this.app.runningSce;
    },
    isRunDisabled(){
      return this.app.runningSce;
    }
  },
  methods: {

    exportSceFile(){
        const data = JSON.stringify(this.app.scee, null, "\t")
        const blob = new Blob([data], {type: 'text/plain'})
        const e = document.createEvent('MouseEvents'),
        a = document.createElement('a');
        let fname = "newScenario"
        if (this.app.scee.scenarioID != undefined && this.app.scee.scenarioID.length>0){
          fname = this.app.scee.scenarioID
        }
        a.download = fname+".sce";
        a.href = window.URL.createObjectURL(blob);
        a.dataset.downloadurl = ['text/json', a.download, a.href].join(':');
        e.initEvent('click', true, false, window, 0, 0, 0, 0, 0, false, false, false, false, 0, null);
        a.dispatchEvent(e);
    },

   resetSce(){
        this.app.resetSCE();
            },

    chooseMapFile() {
      document.getElementById("mapFileUpload").click();
    },
    chooseSceFile() {
      document.getElementById("sceFileUpload").click();
    },


    uploadSce(ev) {
      const file = ev.target.files[0];
      const reader = new FileReader();

      let app = this.app;
      reader.onload = function (event) {
        app.scee =  JSON.parse(event.target.result)
      };

      reader.readAsText(file);
    },
    runSce() { 
      this.app.sceRun()
    },
    cancelRun() { 
     this.app.sceRunCancel()
    },
  },
  mounted: function () {
    console.log();
  },
};
</script>