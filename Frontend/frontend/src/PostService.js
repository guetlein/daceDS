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

import Api from './Api.js'

export default {
  sendQuerySce (sceID) {
    return Api().post('querySce',{"sceID" : sceID} )
  },
  sendCtrlSce (sceID, command) {
    let p = {"sceID" : sceID, "command" : command}
    return Api().post('ctrlSce',p )
  },

  sendRunSce (sce) {
    return Api().post('runSce',sce )
  },

  sendResource (resource) {
    var formData = new FormData();


    formData.append("ID", resource.ID);
    formData.append("Domain", resource.Domain);
    formData.append("Layer", resource.Layer);
    formData.append("Type", resource.Type);
    formData.append("Payload", resource.File);


    return Api().post('uploadResource', formData,  {
      headers: {
            "Content-Type": "multipart/form-data",
        }});


  },

  getResourceList () {
    return Api().post('getResourceList')
  },
  getResource (id) {
    return Api().post('getResource',id)
  },
  getDatamodel () {
    return Api().post('getDatamodel')
  },
  setDatamodel (dm) {
    return Api().post('setDatamodel',dm)
  },
  getConnectivityCheck(){
    return Api().post('connectivityCheck');
  },
  getProvision(){
    return Api().post('pollProvisionData');
  },
  getStatus(){
    return Api().post('pollStatusData');
  },
  getSync(){
    return Api().post('pollSyncData');
  }
}

