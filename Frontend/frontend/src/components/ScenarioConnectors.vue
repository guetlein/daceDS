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

              
          <b-table
            striped
            hover
            bordered
            :items="translatorsShort"
          >
            <template #cell(name)="name">
              {{name.value}}
                <col style="width:1000px;" />
            </template>

            <template #cell(layer)="layer">
              {{layer.value.a}} &#8646; {{layer.value.b}}
                <col style="width:1000px;" />
            </template>

           <template #cell(inUse)="inUse">
             <b-form-checkbox switch v-model=inUse.value.activated 
                @input="inUseChanged(inUse.value.id, inUse.value.activated)" ></b-form-checkbox>
                <col style="width:100px;" />
           </template>

          </b-table>

   <!-- (Currently only translators are implemented in GUI) -->

  </div>
</template>

<script>
export default {
  data() {
    return {
      app: this.$root.$children[0],
      translatorsShort: [],
    };
  },
  components: {},
  methods: {

    isTranslatorInUse(id){

      for(let translator of this.translatorsInUse){
        console.log("translator")
        console.dir(translator)
        if(translator.ID == id){
          return true;
        }
      }
      return false;
    },
    inUseChanged(id, activated){
      console.log(id, activated)
      if(activated == false){
        this.app.removeTranslatorFromSce(id);
      } else {
        this.app.addTranslatorToSce(id);
        this.app.updateTranslatorResps();
      }

      this.app.refreshNoParticipants();
    }

  },
  computed: {
      translators(){
        return this.app.datamodel.translators
      },
      translatorsInUse(){
        console.dir(this.app.scee)
        return this.app.scee.translators
      },
    },
  watch: {
    translators : function(){
      this.translatorsShort = []
      for(let translator of this.translators){
          var translatorShort = {
            name : translator.name,
            domain : translator.domain,
            layer: { a : translator.layerA, b: translator.layerB},
            inUse : { id : translator.name, activated : this.isTranslatorInUse( translator.name)}
          }
          this.translatorsShort.push(translatorShort)

        }
    }
  },   
  mounted() {
     this.translatorsShort = []
      for(let translator of this.translators){
          var translatorShort = {
            name : translator.name,
            domain : translator.domain,
            layer: { a : translator.upper.layer, b: translator.lower.layer},
            inUse : { id : translator.name, activated : this.isTranslatorInUse( translator.name)}
          }
          this.translatorsShort.push(translatorShort)
        }

      console.log("translatorsInUse")
      console.dir(this.translatorsInUse)
  },
};
</script>