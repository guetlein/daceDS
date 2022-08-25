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
    <b-card-text bg-variant="dark" text-variant="white" style="height: 80vh; overflow:auto; padding:20px;">
      <pre>
        <span
        v-for="(line,lineNumber) of sce" 
        v-bind:key="lineNumber" >{{line}}
      </span>
        </pre>
    </b-card-text>
</template>

<script>
export default {
  data() {
    return {
      app: this.$root.$children[0],
    };
  },
  computed: {
    sce(){
      let s = JSON.stringify(this.app.scee, null, '\t');
      let ls = []
      ls = s.split('\n');
      // s = s.replace(/(?:\r\n|\r|\n)/g, '<br />')
      console.dir(ls)
      return ls;
    }
  },
  mounted: function () {},
};
</script>

<style>

pre {
    background: #f8f8f8;
    color: #f1f1f1;
    padding: 10px 16px;
    border-radius: 2px;
    -moz-box-shadow: inset 0 0 10px rgb(209, 209, 209);
    box-shadow: inset 0 0 10px rgb(209, 209, 209);
}

pre span {
    display: block;
    line-height: 1em;
    counter-increment: line;
}
pre span:before {
      content: counter(line);
      display: inline-block; 
      border-right: 1px solid #ddd;
      padding: 0 .5em;
      margin-right: .5em; 
      color: rgb(0, 0, 0);

}
</style>