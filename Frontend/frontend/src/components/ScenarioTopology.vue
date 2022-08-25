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
              <b-card no-body fluid class="h-100" style="margin-top:0px; border-top:0px;background-color:#fff;">
                 <b-tabs class="h-100 d-flex flex-column" content-class="flex-grow-1" style="margin-left:2px;margin-top:-1px" >
                 <b-tab  fluid class="h-100" v-for="d in app.involvedDomains" v-bind:title="d" v-bind:value="d" v-bind:key="d"   >
                   <Topology_Traffic fluid  class="h-100"  v-if="d == 'traffic'"  />
                   <Topology_Communication fluid  class="h-100"  v-if="d == 'communication'"  />
                 </b-tab>
                  <p v-if="app.involvedDomains.length==0" style="margin:100px;">
                    Currently, no domains are activated. <br> Go to setup tab and activate at least one domain.
                  </p>
                  <b-tab title="(More domains can be added via Setup tab)" disabled><p>Add more in Setup</p></b-tab>
                 </b-tabs> 
              </b-card>
</template>
<script>
import Topology_Traffic from "./Topology_Traffic.vue";
import Topology_Communication from "./Topology_Communication.vue";

export default {
  data() {
    return {
      app: this.$root.$children[0],
      currentTab: 0
    };
  },
  components: {
    Topology_Traffic,
    Topology_Communication,
  },
};
</script>