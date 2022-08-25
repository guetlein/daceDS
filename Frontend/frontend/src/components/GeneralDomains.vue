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
          id="modal-raw-edit-domain"
          title="Edit Domain Raw"
          size="lg"
          @ok="handleRawEditOk"> 
          <form ref="form" @submit.stop.prevent="handleRawEditSubmit">
            <p class="my-4" style="padding: 20px; padding-top: 0px">
                <b-row style="height:3em"> <b-col><b>Raw Edit Mode</b><hr> </b-col>          </b-row>
                <b-row>
              <b-form-textarea
      id="textarea"
      v-model="currentDomain"
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
            :items="domainsShort"
          >   

            <template #cell(name)="name" >
              {{name.value}}
                <col style="width:1000px;" />
            </template>
            <template #cell(version)="version">
              {{version.value}}
            </template>
            <template #cell(actions)="domain" style="width:100px;" >
                <div style="width:100px;" />
            <!-- <b-button
                  style="width: 36px; background-color:#ddd; margin-left:3px"
              size="sm"
              @click="rawEditDomain(domain.value.name, sim.value.domain, sim.value.version)"
            >               <img
                  src="../assets/baseline_translate_black_18dp.png"
                /></b-button>  
            <b-button
              size="sm"
                  style="width: 36px; background-color:#ddd; margin-left:3px"
              @click="delDomainSim(domain.value.name, sim.value.domain, sim.value.version)"
            >                <img
                  src="../assets/baseline_delete_forever_black_18dp.png"
                /></b-button>  -->


                <img src="./../assets/edit_image.svg" width="25px" @click="rawEditDomain(domain.value.domain)"  />
                <img src="./../assets/empty_trash.svg" width="25px" @click="delDomain(domain.value.domain)"  />
            </template>
            </b-table>
    <div style="text-align:center">
      <!-- <b-button size="sm" variant="primary" style="margin-right:5px">
        <img src="./../assets/plus.png" width="25px" /> 
        Add Domain
      </b-button>

      <b-button v-b-modal.modal-clear-resources size="sm" variant="primary"> 
        <img src="./../assets/filing_cabinet.png"  width="25px" /> 
        Clear Domains 
      </b-button> -->
    </div>
  </div>
</template>
<script>

export default {
  data() {
    return {
      app: this.$root.$children[0],
      domainsShort : [],
      currentDomain : 1,
      dummyDomain: {
        name: "",
        domain: "",
        version: -1
      },
    };
  },
  methods: {
    getDomain(name, domain, version){
      for(let d of this.domains){
        if (d.domain == domain && d.name == name && d.version == version){
          return d
        }
      }
    console.log(domain+"."+name+"."+version+" was not found")
    return null
    },    
    
    setDomain(refname, refdomain, refversion, newContent){
      for(let domainID in this.domains){
        let domain = this.domains[domainID]
        if (domain.domain == refdomain && domain.name == refname && domain.version == refversion){
          this.domains[domainID] = newContent

          console.log("set contents of "+refdomain+"."+refname+"."+refversion+" to ")
          console.dir(newContent)
          return
        }
      }
      console.log(refdomain+"."+refname+"."+refversion+" was not found")
    },
    rawEditDomain(domainObject){
      this.currentDomain = JSON.parse(JSON.stringify(domainObject))
      this.referenceDomain = JSON.parse(JSON.stringify(domainObject))
      console.log("editing following domain")
      console.dir(this.currentDomain)
      this.$bvModal.show('modal-raw-edit-domain')
    },

    delDomain(domainObject){
      this.app.removeDomain(domainObject);   
         this.app.uploadDatamodel();
    },
    resetModal() {
      this.currentDomain = JSON.parse(JSON.stringify(this.dummyDomain));
      this.referenceDomain = JSON.parse(JSON.stringify(this.dummyDomain));
    },
      handleRawEditOk(bvModalEvt) {
      bvModalEvt.preventDefault();
      this.handleRawEditSubmit(bvModalEvt);
    },
    handleRawEditSubmit() {
      // Don't validate anything.....
      // Hide the modal manually
      this.$nextTick(() => {
        this.$bvModal.hide('modal-raw-edit-domain');
      });

      this.currentDomain = JSON.parse(this.currentDomain)
      
      //save layer
      this.setDomain(this.referenceDomain.name, this.referenceDomain.domain, this.referenceDomain.version, this.currentDomain)
      this.app.uploadDatamodel();
      this.resetModal();
  },
  },
  computed: {
      domains () {
         return this.app.datamodel.domains
      } 
    },
  mounted: function () {
  },  
  watch: {
    domains : function(){
      this.domainsShort = []
      for(let domain of this.domains){
          var domainShort = {
            name : domain.name,
            domain : domain.domain,
            description : domain.description,
            version : domain.version,
            reference : domain.reference,
            referenceDescription : domain.referenceDescription,
            actions : {domain : domain}
          }
          this.domainsShort.push(domainShort)
        }
    }
  },  
};
</script>