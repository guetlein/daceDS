/*
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
*/

import Vue from 'vue'
import App from './App.vue'
import BootstrapVue from 'bootstrap-vue'
import 'bootstrap/dist/css/bootstrap.css'
import 'bootstrap-vue/dist/bootstrap-vue.css'
import './main.css'
import VueKonva from 'vue-konva'


Vue.use(VueKonva)
Vue.use(BootstrapVue)
Vue.config.productionTip = false


const instanceMap = Vue.observable({ instanceMap: new Map() })
Object.defineProperty(Vue.prototype, '$instanceMap', {
  get () {
    return instanceMap.instanceMap
  },
  
  set (value) {
    instanceMap.instanceMap = value
  }
});

const roadMap = Vue.observable({ roadMap: { edges: [], nodes: [], x_min:-1,x_max:-1,y_min:-1,y_max:-1 } });
Object.defineProperty(Vue.prototype, 
  '$roadMap', {
  get() {
    return roadMap.roadMap;
  },

  set(value) {
    roadMap.roadMap = value;
  },
});
const eventHub = new Vue() 

Vue.mixin({
    data: function () {
        return {
            eventHub: eventHub
        }
    }
})
new Vue({
  render: h => h(App),
}).$mount('#app')

