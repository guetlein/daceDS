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
<!-- " -->
  <b-container fluid style="height:calc(100% - 60px);" >
    <b-card fluid no-body class="h-100">
      <b-tabs no-body pills card vertical class="h-100" >

        <!-- GENERAL SECTION  -->
        <b-tab no-body title="General"  class="h-100" >
                    <template #title>
                      <div id="mtb-general"> General </div>
             <b-tooltip target="mtb-general" placement="right">
This section allows managing resources, datamodels, and components.
            </b-tooltip>
          </template>

          <b-row  class="h-100" >
            <b-col cols="1.4" style="border-right:1px solid #ccc;border-left:1px solid #ccc;">
              <GeneralSideBar />
            </b-col>
            <b-col >
              <b-card no-body fluid class="h-100"  style="margin-left:-16px;" >
                <b-tabs no-body fluid card class="h-100 d-flex flex-column" content-class="flex-grow-1">
                  <b-tab no-body fluid title="Resources" class="h-100" >

                    <GeneralResourceManager />
                  </b-tab>
                  <b-tab no-body title="Domains"  class="h-100">
                     <GeneralDomains />
                  </b-tab>
                  <b-tab no-body title="Layers"  class="h-100">
                     <GeneralLayers />
                  </b-tab>
                  <b-tab no-body title="Simulators"  class="h-100">
                     <GeneralSimulators />
                  </b-tab>
                  <b-tab no-body title="Translators"  class="h-100">
                     <GeneralTranslators />
                  </b-tab>
                  <b-tab no-body title="Projectors"  class="h-100">
                    
                  </b-tab>
                </b-tabs>
              </b-card>
            </b-col>
          </b-row>
        </b-tab>

                 
        <!-- SCENARIO SECTION  -->
        <b-tab no-body title="Scenario"  class="h-100" >
          <template #title>
                      <div id="mtb-scenario"> Scenario </div>
             <b-tooltip target="mtb-scenario" placement="right">
Here, you can design, run, and observe custom scenarios.
            </b-tooltip>
          </template>

          <b-row  class="h-100" >
            <b-col cols="1.4" style="border-right:1px solid #ccc;border-left:1px solid #ccc;">
              <ScenarioSideBar />
            </b-col>
            <b-col >
              <b-card no-body fluid class="h-100" style="margin-left:-16px;" >
                <b-tabs card class="h-100 d-flex flex-column"  content-class="flex-grow-1" >
                  <b-tab no-body fluid title="Setup" class="h-100" >
                    <ScenarioSetup />
                  </b-tab>   
                  <b-tab no-body fluid title="SCE Viewer" >
                    <ScenarioDefinitionViewer></ScenarioDefinitionViewer>
                  </b-tab>
                  <b-tab no-body fluid title="Topology" class="h-100"  >
                    <ScenarioTopology></ScenarioTopology>
                  </b-tab>

                  <b-tab no-body fluid title="Connectors" class="h-100">
                    <ScenarioConnectors />
                  </b-tab>
                  <b-tab no-body title="Scenario Run">
                    <ScenarioRun />
                  </b-tab>
                </b-tabs>
              </b-card>
            </b-col>
          </b-row>
        </b-tab>



        <!-- Catalog  -->
        <b-tab no-body fluid  class="h-100" title="Catalog"  >

        <template #title>
            <div id="mtb-catalog"> Catalog </div>
             <b-tooltip target="mtb-catalog" placement="right">
              Browse finished scenario runs.
            </b-tooltip>
          </template>

            <b-row  class="h-100" >
               <b-col >
              <b-card no-body fluid class="h-100" style="margin-left:-16px;" >
                <b-tabs card class="h-100 d-flex flex-column"  content-class="flex-grow-1" >
                  <b-tab no-body fluid title="Recent" class="h-100" >
                    <Catalog />
                  </b-tab>   
                  <b-tab no-body fluid title="Archive" disabled >
                  </b-tab>
                </b-tabs>
              </b-card>
            </b-col>
            </b-row>

        </b-tab>


      </b-tabs>
    </b-card>
    
     <b-sidebar id="sidebar-connection" title="Connection Settings" right shadow>
      <div class="px-3 py-2">
        Backend:
        <b-input placeholder="127.0.0.1:8087"></b-input>
        <br>
        Sce Run Pull Intervall (sec):
        <b-input type="number" placeholder="9"></b-input>
      </div>
    </b-sidebar>
    
     <b-sidebar id="sidebar-info" title="Info" right shadow>
      <div class="px-3 py-2">
        <p>
          This is a demo frontend for my PhD project "Data-Centric Distributed Simulation in the Traffic Domain".
        </p>
      </div>
    </b-sidebar>
    
     <b-sidebar id="sidebar-help" title="Help" right shadow>
      <div class="px-3 py-2">
        <p>
         Maybe later.
        </p>
      </div>
    </b-sidebar>

  </b-container>
</template>


<script>
import ScenarioTopology from "./ScenarioTopology.vue";
import ScenarioDefinitionViewer from "./ScenarioDefinitionViewer.vue";
//import GeneralSideBar from "./GeneralSideBar.vue";
import ScenarioSideBar from "./ScenarioSideBar.vue";
import ScenarioSetup from "./ScenarioSetup.vue";
import ScenarioRun from "./ScenarioRun.vue";
import ScenarioConnectors from "./ScenarioConnectors.vue";
import GeneralResourceManager from "./GeneralResourceManager.vue";
import GeneralDomains from "./GeneralDomains.vue";
import GeneralLayers from "./GeneralLayers.vue";
import GeneralSimulators from "./GeneralSimulators.vue";
import GeneralTranslators from "./GeneralTranslators.vue";
import GeneralSideBar from "./GeneralSideBar.vue";
import Catalog from "./Catalog.vue";
export default {
  data() {
    return {
      meals: [],
    };
  },
  components: {
    ScenarioTopology,
    ScenarioDefinitionViewer,
    ScenarioSideBar,
    ScenarioConnectors,
    ScenarioRun,
    ScenarioSetup,
    GeneralResourceManager,
    GeneralDomains,
    GeneralLayers,
    GeneralSimulators,
    GeneralTranslators,
    GeneralSideBar,
    Catalog,
  },

  mounted() {
  },
};
</script>