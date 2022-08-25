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
<div style="height: 100vh; overflow:auto;" >
          <b-table
            striped
            hover
            bordered
            :items="catalog"
          >   
            <template #head(ID)="ID" sortable>
               {{ ID.label }}
            </template>
            <template #cell(ID)="ID">
                    {{ID.value}}
            </template>



            <template #head(SCE)="SCE">
               {{ SCE.label }}
            </template>
            <template #cell(SCE)="SCE">
               <b-button v-b-toggle="'collapse-catalog-scedef-'+SCE.index" variant="light" size="sm">
                 <img src="./../assets/search.svg" width="25px"   />
                  Show Definition
              </b-button>
               <b-collapse :id="'collapse-catalog-scedef-'+SCE.index" class="mt-2" >
                    <pre>{{SCE.value}} </pre>
               </b-collapse>
            </template>



            <template #head(SummaryTopics)>
                Topics
            </template>
            <template #cell(SummaryTopics)="topics" fixed>
                <b-button v-b-toggle="'collapse-catalog-sce-'+topics.index" variant="light" size="sm">
                    <img src="./../assets/search.svg" width="25px"   />
                  Browse Topics
                </b-button>

                <b-collapse :id="'collapse-catalog-sce-'+topics.index" class="mt-2" >
                  <b-list-group v-for="s in topics.value" v-bind:key="s" >
                      <b-list-group-item  style="padding:2px 10px 2px 10px;" size="sm" target="_blank" :href="'http://127.0.0.1:8086/ui/clusters/0/topics/'+s">{{s}}</b-list-group-item>
                  </b-list-group>
                </b-collapse>
            
            </template>


            <template #head(SummaryResults)>
                Results
            </template>
            <template #cell(SummaryResults)="results" fixed>
                <b-list-group v-for="result in results.value" v-bind:key="result" >
                    <b-list-group-item  style="padding:2px 10px 2px 10px;" size="sm" target="_blank" :href="'file://'+result.value.FileReference">{{result.value.ID}} </b-list-group-item>
                </b-list-group>
            </template>


            </b-table>
</div>
</template>
<script>

export default {
  data() {
    return {
      app: this.$root.$children[0],
    };
  },
  methods: {
  },
  computed: {
      catalog () {
         return this.app.catalog
      } 
    },
  mounted: function () {
  },  
};
</script>