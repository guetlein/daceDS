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
    <b-navbar toggleable="lg"  type="light" variant="light"  z-index="2" style="height:60px">
      <b-navbar-brand href="#">SimService</b-navbar-brand>
      <b-navbar-toggle target="nav-collapse"></b-navbar-toggle>
      <b-collapse id="nav-collapse" is-nav>
        <b-navbar-nav class="ml-auto">
          <span v-if="brokerOnline" style="color:#28a745;padding:9px"> Backend online! </span>
          <span v-else style="color:red;padding:9px">Backend offline!</span>
          <b-nav-item-dropdown text="Preferences" right>
            <b-dropdown-item href="#" v-b-toggle.sidebar-connection>Connection</b-dropdown-item>
            <b-dropdown-item href="#">Guest Mode</b-dropdown-item>
            <b-dropdown-item href="#">Debug Mode</b-dropdown-item>
          </b-nav-item-dropdown>

          <b-nav-item-dropdown right>
            <template #button-content>
              <em>Info</em>
            </template>
            <b-dropdown-item href="#" v-b-toggle.sidebar-info>Info</b-dropdown-item>
            <b-dropdown-item href="#" v-b-toggle.sidebar-help>Help</b-dropdown-item>
          </b-nav-item-dropdown>
        </b-navbar-nav>
      </b-collapse>
    </b-navbar>
  </div>
</template>

<script>

import PostService from "../PostService.js";

export default {
  data() {
    return {
      app: this.$root.$children[0],
      brokerOnline: false,
    };
  },
  methods:{
    checkConnectivity(){
       PostService.getConnectivityCheck().then(() => {
        this.brokerOnline = true;
      }).catch(() => {
        this.brokerOnline = false;
			});
    }
  },
  mounted: function () {
    this.checkConnectivity();
    setInterval(function () {
       this.checkConnectivity();
    }.bind(this), 30000); 
  }
};

</script>