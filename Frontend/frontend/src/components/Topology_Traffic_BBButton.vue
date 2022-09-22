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
  <!-- v-for="(val, name) in this.$parent.$parent.testInstanceMap" -->
  <b-row>
    <!-- <span v-for="(name, id) in $instanceMap.entries()" :key="'btn' + name"> -->
    <span v-for="instance in getTrafficBBs" :key="'btn' + instance.instanceID">
                  <!-- ,
                  backgroundColor:getColorOpacity(instance.ID,0.8)}"   > -->
      
       <b-button
            variant="light" squared disabled
            style="margin:0px 4px -1px 4px;padding:2px;  padding-right:20px;  border-style:solid;border-left-width:1px;border-right-width:1px;border-bottom-width:0px;border-top-width:5px"
            v-bind:style="{
                  borderColor:getColor(instance.instanceID)}"
            >
      <b-container  
        
        align-v="end"
        style2="width: 160px; margin:0px 4px 0px 4px;padding:2px; border-radius:4px; background-color:#6c757d;border-top-width:5px;border-left-width:5px;border-right-width:5px" 
        style3="width: 150px; margin:0px 4px 0px 4px;padding:2px; border-radius:4px; border-style:solid;border-left-width:3px;border-right-width:0px;border-top-width:0px;border-bottom-width:3px"     
                v-bind:style="{
                  borderColor:getColor(instance.instanceID)}">

                   <b-row>
                  <b-col cols="auto" style="margin-left:-10px;margin-right:10px;width:60px"><img
                :src="getLogo(instance.type)"
                style="
                  width: 45px;
                  margin: 1px;
                  border-radius: 50%;
                  border-width: 0px;
                  border-style:solid;
                  -webkit-border-radius: 50%;
                  -moz-border-radius: 50%;
                  box-shadow: 0 0 8px rgba(0, 0, 0, 0.4);
                  box-shadow: 0 0 8px var(--sumo-logo-shadow-color);
                  -webkit-box-shadow: 0 0 8px var(--sumo-logo-shadow-color);
                  -moz-box-shadow: 0 0 8px var(--sumo-logo-shadow-color);
                " v-bind:style="{
                  borderColor:getColor(instance.instanceID)}" /></b-col>

                <b-col style="margin:0;">
        <b-row > 
         <span style="font-size:14px;color:black;margin:0;"> 
                {{ instance.instanceID }}
              </span>
        </b-row>

        <b-row align-v="end">
              <b-button style="padding:0px; margin-right:5px; margin-left:5px;" variant="light" @click="setActiveInstance(instance.instanceID)" >
                <img
                 width="20px"
                  src="../assets/baseline_fullscreen_black_24dp.png"
                />
              </b-button>
              <b-button style="padding:0px; margin-right:5px; margin-left:5px;"  variant="light" @click="promoteInstance(instance.instanceID)" >
                               <img
                                width="20px"
                  src="../assets/baseline_west_black_24dp.png"
                />
              </b-button>
              <!-- <b-button style="padding:0px;" variant="light">
                <img
                  width="20px"
                  src="../assets/baseline_settings_black_24dp.png"
                /> 
              </b-button>-->
              <b-button style="padding:0px; margin-right:5px; margin-left:5px;" variant="light" @click="removeInstance(instance.instanceID)">
                <img
                 width="20px"
                  src="../assets/baseline_delete_black_24dp.png"
                />
              </b-button>
            </b-row>
                </b-col>
                   </b-row>
      </b-container>
       </b-button>
    </span>
  </b-row>
</template>

<script>
export default {
  data() {
    return {
      app: this.$root.$children[0],
      // instances: []
    };
  },
  methods: {
    setActiveInstance(instanceID) {
      this.app.activeInstance = instanceID;
    },
    promoteInstance(instanceID) {
      this.app.activeInstance = instanceID;
      this.$root.$emit("promoteInstance");
     },
    removeInstance(instanceID) {
      this.app.removeBB(instanceID);
     },
     
    getColor(id) {
      return this.app.getColor(id);
    },
    getColorOpacity(id, opacity) {
      let color = this.app.getColor(id);
      var _opacity = Math.round(Math.min(Math.max(opacity || 1, 0), 1) * 255);
      return color + _opacity.toString(16).toUpperCase();
    },
    getLogo(type){
      if(type=="SumoWrapper"){
        return "logo-sumo.png";
      } 
      else if(type=="CarlaWrapper"){
        return "logo-carla.png";
      } 
      else if(type=="MATSimWrapper"){
        return "logo-matsim.png";
      } 
      else if(type=="OMNeTWrapper"){
        return "logo-omnet.png";
      } 
      else if(type=="VisumWrapper"){
        return "logo-visum.jpeg";
      } 
      else {
        return "automotive.svg";
      }
    },
  },
  computed: {
    getTrafficBBs() {
      let bbs = []
      for(let bb of this.app.scee.buildingBlocks){
        if(bb.domain == "traffic"){
          bbs.push(bb)
        }
      }
      return bbs
    },
  },
  mounted: function () {},
   watch: {    
  },
};
</script>
