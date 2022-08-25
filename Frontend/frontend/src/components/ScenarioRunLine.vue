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
<div style="padding-top:20px">
 <line-chart :min="0" :max="20" label="Speed" xtitle="Simulation time" ytitle="Speed in m/s" :xmin="0"
 :data="app.historyMicroSpeed" height="180px" width="99%"></line-chart>
</div>
</template>

<script>
import Vue from 'vue'
import Chartkick from "vue-chartkick";
import Chart from "chart.js";

Vue.use(Chartkick.use(Chart));

export default {
  data() {
    return {
    app: this.$root.$children[0],
    }
  }
}
</script>