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
  <div style="height: 80vh; overflow:auto">
          <b-table     
            striped
            hover
            bordered
            :items="resourceList"
            :fields="fields"
          >      
            <template #cell(ID)="ID">
              {{ID.value}}
                <col style="width:2000px;" />
            </template>   
            <template #cell(Actions)="Actions">
              {{Actions.value}}
                <b-button variant="light-outline"  style="padding:0px;margin-bottom:0px" size="sm" @click="dlete(Actions)" ><img src="./../assets/empty_trash.svg" width="25px"   /></b-button>
            </template>
        </b-table>
  </div>
</template>
<script>

export default {
  data() {
    return {
      app: this.$root.$children[0],
      resource: "",
      fields: [
        {
          key: "ID",
          sortable: true,
        },
        {
          key: "Domain",
          sortable: true,
        },
        {
          key: "Layer",
          sortable: true,
        },
        {
          key: "Type",
          sortable: true,
        },
        {
          key: "Path",
          label: "Path",
          sortable: true,
        },
        {
          key: "Actions",
          label: "Actions",
          sortable: false,
        },
      ],
      
    };
  },
  computed: {
      resourceList (){
        return this.app.resourceList
      } 
  },
  methods: {
    dlete(e){
      this.app.removeResourceByID(e.item.ID);
    }
  },

};
</script>