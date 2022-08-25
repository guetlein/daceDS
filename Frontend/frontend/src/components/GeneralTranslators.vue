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
          id="modal-raw-edit-trans"
          title="Edit Translator Raw"
          size="lg"
          @ok="handleRawEditOk"> 
          <form ref="form" @submit.stop.prevent="handleRawEditSubmit">
            <p class="my-4" style="padding: 20px; padding-top: 0px">
                <b-row style="height:3em"> <b-col><b>Raw Edit Mode</b><hr> </b-col>          </b-row>
                <b-row>
              <b-form-textarea
      id="textarea"
      v-model="currentTranslator"
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
            :items="translatorsShort"
          >
           <template #cell(ID)="ID">
              {{ID.value}}
                <!-- <col style="width:1000px;" /> -->
            </template>
           <template #cell(layers)="translator">
              {{translator.value.upper}} &#8646; {{translator.value.lower}}
           </template>

           <template #head(resources)>
              Suitable Resources
            </template>
           <template #cell(resources)="resources">
             <!-- <ul>
              <li v-for="res in resources.value"  v-bind:value="res" v-bind:key="res"> res</li>
             </ul> -->
              <span v-for="res in resources.value"  v-bind:value="res" v-bind:key="res">{{res}} <br/> </span>
             
           </template>

            <template #cell(actions)="translator">

                <!-- <div style="width:100px;" /> -->
            <!-- <b-button
                  style="width: 36px; background-color:#ddd; margin-left:3px"
              size="sm"
              @click="rawEditTranslator(translator.value.ID, translator.value.domain)"
            >               <img
                  src="../assets/baseline_translate_black_18dp.png"
                /> </b-button> 
            <b-button
              size="sm"
                  style="width: 36px; background-color:#ddd; margin-left:3px"
              @click="delTranslator(translator.value.ID, translator.value.domain)"
            >                <img
                  src="../assets/baseline_delete_forever_black_18dp.png"
                /> </b-button>  -->


              <b-button variant="light-outline"  style="padding:0px;margin-bottom:0px" size="sm" @click="rawEditTranslator(translator.value.translator)" ><img src="./../assets/edit_image.svg" width="25px"   /></b-button>
            
              <b-button variant="light-outline"  style="padding:0px;margin-bottom:0px" size="sm" @click="delTranslator(translator.value.translator)" ><img src="./../assets/empty_trash.svg" width="25px"   /></b-button>
            
            </template>
          </b-table
        >

    <!-- <div style="text-align:center">
                        <b-button
      size="sm"
      variant="Secondary"
      ><img src="./../assets/plus.png" width="25px"  /> Add Translator</b-button>

<b-button v-b-modal.modal-clear-resources  size="sm" variant="Secondary"> <img src="./../assets/filing_cabinet.png"  width="25px" /> Clear translators </b-button>
    </div> -->
  </div>
</template>

<script>
export default {
  data() {
    return {
      app: this.$root.$children[0],
      
      translatorsShort: [],
      dummyTranslator: {
        ID: "",
        domain: "",
        version: -1
      },
      referenceTranslator: null,
      currentTranslator: null,
    };
  },
  components: {},
  methods: {

    getTranslator(name, domain){
      for(let translator of this.translators){
        if (translator.domain == domain && translator.ID == name){
          return translator
        }
      }
    console.log(domain+"."+name+" was not found")
    return null
    },    
    
    setTranslator(refname, refdomain, newContent){
      for(let translatorID in this.translators){
        let translator = this.translators[translatorID]
        if (translator.domain == refdomain && translator.ID == refname){
          this.translators[translatorID] = newContent
          console.log("set contents of "+refdomain+"."+refname+" to ")
          console.dir(newContent)
          return
        }
      }
      console.log(refdomain+"."+refname+" was not found")
    },

    delTranslator(translator){
      this.app.removeTranslator(translator)
      this.app.uploadDatamodel();
    },

    rawEditTranslator(translator){
      this.currentTranslator = JSON.parse(JSON.stringify(translator))
      this.referenceTranslator = JSON.parse(JSON.stringify(translator))
      console.log("editing following translator")
      console.dir(this.currentTranslator)
      this.$bvModal.show('modal-raw-edit-trans')
    },
    resetModal() {
      this.currentTranslator = JSON.parse(JSON.stringify(this.dummyTranslator));
      this.referenceTranslator = JSON.parse(JSON.stringify(this.dummyTranslator));
    },
      handleRawEditOk(bvModalEvt) {
      bvModalEvt.preventDefault();
      this.handleRawEditSubmit(bvModalEvt);
    },
    handleRawEditSubmit() {
      // Don't validate anything.....
      // Hide the modal manually
      this.$nextTick(() => {
        this.$bvModal.hide('modal-raw-edit-trans');
      });

      this.currentTranslator = JSON.parse(this.currentTranslator)
      
      //save layer
      this.setTranslator(this.referenceTranslator.ID, this.referenceTranslator.domain, this.currentTranslator)
      this.app.uploadDatamodel();
      this.resetModal();
  },
  },
  computed: {
      translators(){
        return this.app.datamodel.translators
      },
    },
  watch: {
    translators : function(){
      this.translatorsShort = []
      for(let translator of this.translators){

          var translatorShort = {
            ID : translator.name,
            domain : translator.domain,
            layers : {upper: translator.layerA, lower : translator.layerB},
            version : translator.version,
            resources : translator.resources,
            description : translator.description,
            actions : {translator : translator}
          }
          this.translatorsShort.push(translatorShort)
        }
    }
  },   
  beforeMount: function(){
    this.currentTranslator = JSON.parse(JSON.stringify(this.dummyTranslator))
   },
  mounted() {
  },
};
</script>