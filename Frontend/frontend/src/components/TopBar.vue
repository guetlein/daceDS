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
    <b-container fluid>
      <b-row style="padding-top: 10px">
        <b-col cols="2">
          <b-button
            size="sm"
            block
            variant="dark"
            style="text-align: left; height: 72px"
            left
            @click="addInstance"
            ><img src="./../assets/plus.png" />Add Instance</b-button
          >
        </b-col>
        <b-col>
          <!-- -->
          <InstanceButton />
        </b-col>
      </b-row>
    </b-container>
  </div>
</template>

<script>
import InstanceButton from "./InstanceButton.vue";

export default {
  data() {
    return {
    };
  },
  components: {
    InstanceButton,
  },
  methods: {
    addInstance() {
      this.$root.$emit("addInstance");
    },
  },
  mounted: function () {},
};
</script>