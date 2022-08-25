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
<div style="height: 90vh; overflow:auto;" >
    <b-card  ref="sceRunCard" id="sceRunCard"  no-body >

    
    
    <div style="height:150px;  padding:10px; margin:10px; background-color:#fafafa; border:1px solid lightgrey;border-radius:4px;">
      <div style=" float:left;overflow:auto">
        Progress:
      </div>
      <b-form-checkbox switch style="position:absolute; right:22px" @change="toggleGathering($event)" >Gather Data</b-form-checkbox>
      <br>
      <b-progress fluid style="background-color:#fafafa;" striped show-value animated>
        <b-progress-bar :value="currentTime" :max="endTime">
          <span ><strong>{{ currentTime }} / {{ endTime }}</strong></span>
        </b-progress-bar>
      </b-progress>
      Log:<br> 
      <ul style="overflow-y:auto; height:74px; padding-bottom:5px; border-radius:4px; flex-direction: column-reverse;">
        <li v-for="(item,index) in app.statusdata.slice().reverse()" :key="'l'+index" style="list-style-type: none; ">
          <span style="color:grey">{{item.time}}:</span> {{ item.value }}
        </li>
      </ul>
    </div>

      <div style="height:250px; padding:10px; padding-bottom:0px; margin:10px; background-color:#fafafa; border:1px solid lightgrey;border-radius:4px;">
        Traffic Stats:
    <ScenarioRunLine/>
      </div>

    <div ref="sceRunMapDiv" fluid style="padding:10px; margin:10px; border:1px solid lightgrey;border-radius:4px; background-color:#fafafa;"  class="h-100" >

    Traffic Map:
    <v-stage class="h-100" fluid  ref="statsStage"  id="statsStage"  :config="statsStageConfig"  >
      <v-layer ref="layer">
        <v-line
          v-for="(line, index) in getLines"
          :key="'scerunl' + index"
          :config="{
            points: line.points,
            fill: 'rgb(.5,.5,.5,0)',
            stroke: '#ced2d6',
            strokeWidth: 5,
            draggable: false,
          }"
        />
        <v-text
          v-for="(line, index) in getLines" 
          :key="'scerunlinktext' + index"          
          :config="{  
            x: line.labelpoints[0],
            y: line.labelpoints[1],
            text: line.label,        
            fontFamily: 'Calibri',
            fontSize: 12,
            padding: 5,
            textFill: 'white',
            fill: 'gray',
            alpha: 0.75,
            visible: true}" />



        <v-text
          v-for="(circle, index) in getPoints" 
          :key="'scerunnodetext' + index"          
          :config="{  
            x: Math.min(circle.x),
            y: Math.min(circle.y),
            text: circle.label,        
            fontFamily: 'Calibri',
            fontSize: 12,
            padding: 5,
            textFill: 'white',
            fill: 'gray',
            alpha: 0.75,
            visible: true}" />
        <v-circle
          v-for="(circle, index) in getPoints"
          :key="'scerunp'+index"
          :config="{
            x: Math.min(circle.x),
            y: Math.min(circle.y),
            radius: 7,
            fill: '#17a2b8',
            stroke: '#17a2b8',
            strokeWidth: 3,
            draggable: false,
          }"
        />


        <v-circle v-for="(submicro, index) in currentSubmicroTraffic" :key="'scersubmicro'+index"  :config="{  x: submicro.position.x,  y: submicro.position.y,  radius: 10,
            fill: 'green',  stroke: 'green', strokeWidth: 3, draggable: false,}" /> 
        <v-text v-for="(submicro, index) in currentSubmicroTraffic" :key="'scrsubmicrotext' + index" :config="{  x: submicro.position.x, y: submicro.position.y, 
        text: 'veh'+submicro.vehicleID, fontFamily: 'Calibri', fontSize: 18,  padding: 15,  textFill: 'white', fill: 'green',  alpha: 0.75, visible: true}" />


        <v-circle v-for="(micro, index) in currentMicroTraffic" :key="'scermicro'+index"  :config="{  x: micro.position.x,  y: -micro.position.y,  radius: 10,
            fill: 'blue',  stroke: 'blue', strokeWidth: 3, draggable: false,}" /> 
        <v-text v-for="(micro, index) in currentMicroTraffic" :key="'scrmicrotext' + index" :config="{  x: micro.position.x, y: -micro.position.y, 
        text: 'veh'+micro.vehicleID, fontFamily: 'Calibri', fontSize: 18,  padding: 15,  textFill: 'white', fill: 'blue',  alpha: 0.75, visible: true}" />


        <v-circle v-for="(meso, index) in currentMesoTraffic" :key="'scermeso'+index"  :config="{ x: meso.x,  y: meso.y, radius: 10, fill: 'purple', stroke: 'purple',
            strokeWidth: 3, draggable: false, }" /> 
        <v-text v-for="(meso, index) in currentMesoTraffic" :key="'scermesotext' + index" :config="{ x: meso.x, y: meso.y, text: meso.vehicleID, 
        fontFamily: 'Calibri', fontSize: 18, padding: 15, textFill: 'white', fill: 'purple', alpha: 0.75, visible: true}" />

<!-- +macro.flow+'\nspeed:'+macro.speed+'\ndens: '+macro.density -->
        <v-text v-for="(macro, index) in currentMacroTraffic" :key="'scermacrotext' + index"  :config="{  x: macro.x,  y: macro.y, 
          text: macro.linkID+'\nf='+macro.flow,        
            fontFamily: 'Calibri',  fontSize: 18, padding: 15, textFill: 'white', fill: 'red', alpha: 0.75,  visible: true}" />
  
      </v-layer>
      <v-layer> </v-layer>
    </v-stage>
    </div>
  <div style="height:250px; padding:10px; margin:10px; background-color:#fafafa; border:1px solid lightgrey;border-radius:4px;">
    Incoming Data:
    <ul style="overflow-y:auto; height:144px; padding-bottom:5px; border-radius:4px; flex-direction: column-reverse;">
      <!-- <li v-for="(item,index) in localTrafficMap.values()" :key="'t'+index"> -->
      <li v-for="(item,index) in currentMicroTraffic" :key="'t'+index">
        Vehicle {{item.vehicleID}}: {{Number((item.speed).toFixed(2))}} m/s on link={{item.edge}} at pos={{item.position}}
      </li>
    </ul>
    </div>
</b-card>
    </div>
</template>
<script>

import PostService from "../PostService.js";
import ScenarioRunLine from "./ScenarioRunLine.vue";
// import Vue from "vue";
export default {
  data() {
    return {
      app: this.$root.$children[0],
      localTrafficMap: new Map(),
      currentMacroTraffic: [],
      currentMesoTraffic: [],
      currentMicroTraffic: [],
      currentSubmicroTraffic: [],
      localStatus: [],
      currentTime: 0,
      endTime: 100,
      currentSubmicroTime: 0,
      currentMicroTime: 0,
      currentMesoTime: 0,
      currentMacroTime: 0,
      isGathering: false
    };
  },
   components: {
     ScenarioRunLine,
   },
  methods:{



    toggleGathering(gath){
      this.isGathering= gath
      let intervalID = -1;

      if (this.isGathering == true){

        console.log("started polling for sce run updates")

        intervalID = setInterval(function () {

          

          // if(this.isGathering == false){
          //   return;
          // }
            
            this.pollProvision();
            
            this.pollSync();
          
        }.bind(this), 3000); 
      }
      if (this.isGathering == false && intervalID > -1){
        clearInterval(intervalID);
        console.log("stopped polling for sce run updates")
      }
    },


    generateFakeMicro(id){
      let lines = this.getLines
      if (lines == undefined || lines.size == 0){
        let m = {
          "vehicleID" : "veh-1",
          "position" : { "x": 0, "y" : 0 },
          "speed" : 0
        }
        return m
      }

      let randomLine = lines[Math.floor(Math.random()*lines.length)]
      let x = Math.min(randomLine.points[0], randomLine.points[2]) + Math.abs(randomLine.points[0] - randomLine.points[2])  * Math.random()
      let y = Math.min(randomLine.points[1], randomLine.points[3]) + Math.abs(randomLine.points[1] - randomLine.points[3])  * Math.random()
      let m = {
        "vehicleID" : "micro"+String(id),
        "position" : { "x": x, "y" : -y },
        "speed" : Math.floor(Math.random()*15),
        "edge" : "edge"
      }
      return m
    },


    generateFakeSubmicro(id){
      let lines = this.getLines
      if (lines.size == 0){
              let m = {
        "vehicleID" : "veh-1",
        "position" : { "x": 0, "y" : 0 }
      }
        return m
      }

      let randomLine = lines[Math.floor(Math.random()*lines.length)]
      let x = Math.min(randomLine.points[0], randomLine.points[2]) + Math.abs(randomLine.points[0] - randomLine.points[2])  * Math.random()
      let y = Math.min(randomLine.points[1], randomLine.points[3]) + Math.abs(randomLine.points[1] - randomLine.points[3])  * Math.random()
      let m = {
        "vehicleID" : "submicro"+String(id),
        "position" : { "x": x, "y" : y },
        "speed" : 0,
        "road" : "road"
      }
      return m
    },


    generateFakeMeso(id){
      let lines = this.getLines
      if (lines.size == 0){
              let m = {
        "vehicleID" : "veh-1",
        "position" : { "x": 0, "y" : 0 }
      }
        return m
      }

      let randomLine = lines[Math.floor(Math.random()*lines.length)]
      let x = (randomLine.points[0] + randomLine.points[2] ) / 2
      let y = (randomLine.points[1] + randomLine.points[3] ) / 2
      let m = {
        "vehicleID" : "meso"+String(id),
        "x": x, "y" : y 
      }
      return m
    },


    generateFakeMacro(){
      let lines = this.getLines
      if (lines.size == 0){
              let m = {
        "vehicleID" : "veh-1",
        "position" : { "x": 0, "y" : 0 }
      }
        return m
      }

      let randomLine = lines[Math.floor(Math.random()*lines.length)]
      let x = (randomLine.points[0] + randomLine.points[2] ) / 2
      let y = (randomLine.points[1] + randomLine.points[3] ) / 2
      let m = {
        "linkID" : "macro"+randomLine.label,
        "x": x, "y" : y ,
        "flow" : Math.floor(Math.random()*99),
        "speed" : 0,
        "density" : 0
      }
      return m
    },

    rememberStats(){
      let speed = 0;

      if(this.currentMicroTraffic.length>0){
        for(const veh of this.currentMicroTraffic){
          speed += veh.speed

          console.log("adding"+veh.speed)
        }
        speed = speed / (1.0*this.currentMicroTraffic.length)
      }

      this.app.historyMicroSpeed.push([this.currentTime,speed])
      console.log("total:"+speed)
      console.dir(this.currentMicroTraffic)
    },
    pollProvision(){

      if(this.app.dummyRun == true){
        
        this.rememberStats();
        
        //create fake data
        this.currentTime+=1;
        this.currentSubmicroTraffic=[]
        this.currentMicroTraffic=[]
        this.currentMesoTraffic=[]
        this.currentMacroTraffic=[]

        for(let i=0;i<10;i++){
          this.currentMicroTraffic.push(this.generateFakeMicro(i));
          this.currentMacroTraffic.push(this.generateFakeMacro(i));
          this.currentMesoTraffic.push(this.generateFakeMeso(i));
          this.currentSubmicroTraffic.push(this.generateFakeSubmicro(i));
        }

        return;
      }

      console.log('polling for provision ');
       PostService.getProvision().then((result) => {
        console.dir(result);
        var trafficArray = result.data;
        if(trafficArray.length == 0){
          return;
        }

        for(const msg of trafficArray){

          // console.log('got provision message');
          console.dir(msg)

          var topic = msg.topic
          if(topic == undefined){
            continue;
          }

          //todo: get time from headers  
          // console.log("msg.timestamp "+msg.timestamp)
          var timeInSeconds = Number(msg.timestamp / 1000);
          // console.log("timeInSeconds "+timeInSeconds)
          if(timeInSeconds > this.currentTime){
            this.currentTime = timeInSeconds
          }

          if(msg.topic.includes(".micro") ){ //todo: just for demo, replace with better check 
            var micro = msg.value
            if(micro == undefined){
                continue;
            }
            if(timeInSeconds > this.currentMicroTime){

              this.rememberStats();

              console.log("got msg with new timestamp, resetting micro list from "+this.currentMicroTime)
              this.currentMicroTime = timeInSeconds
              
              this.currentMicroTraffic = []
            }
            this.currentMicroTraffic.push(micro)
            // this.localTrafficMap.set(micro.vehicleID, micro)
          } 
          else if(msg.topic.includes("submicro") ){ //todo: just for demo, replace with better check 
            var submicro = msg.value
            if(submicro == undefined){
                continue;
            }
            if(timeInSeconds > this.currentSubmicroTime){
              console.log("got msg with new timestamp, resetting submicro list from "+this.currentSubmicroTime)
              this.currentSubmicroTime = timeInSeconds
              this.currentSubmicroTraffic = []
            }
            this.currentSubmicroTraffic.push(submicro)
          } 
          else if(msg.topic.includes("meso") ){
            var meso = msg.value
            if(meso == undefined){
                continue;
            }
            let link  = this.getlink(meso.link)
            meso.x = link.x +this.currentMesoTraffic.length*5
            meso.y = link.y +this.currentMesoTraffic.length*5

            console.log("got meso")
            console.dir(meso)

            if(timeInSeconds > this.currentMesoTime){
              console.log("got msg with new timestamp, resetting meso list from "+this.currentMesoTime)
              this.currentMesoTime = timeInSeconds
              this.currentMesoTraffic = []

            }
            this.currentMesoTraffic.push(meso)
          }
          else if(msg.topic.includes("macro") ){
            var macro = msg.value

            //todo:
            let fromNode = macro.linkID.split('_')[0];
            console.log("fromNode: "+fromNode)
            macro.x = this.getNode(fromNode).x
            macro.y = this.getNode(fromNode).y

            if(timeInSeconds > this.currentMacroTime){
              this.currentMacroTime = timeInSeconds
              this.currentMacroTraffic = []
            }
            this.currentMacroTraffic.push(macro)
          }
        }
      this.localTrafficMap = new Map(this.localTrafficMap)
      }).catch((e) => {
				console.log('pollProvision failed: '+e);
			});
    },
    pollStatus(){
      // console.log('polling for status ');
       PostService.getStatus().then((result) => {
        // console.dir(result);

        var status = result.data;
        if(status.length == 0){
          return;
        }
        for(const s of status){
          // var ts = s.timestamp;
          // var to = s.topic;

          //todo: get time from headers  
          var ts = new Date(Number(s.timestamp))
          var val = {time : ts.toLocaleString(), value :s.value};
          // var val = {time : s.timestamp, value :s.value};
          this.app.statusdata.push( val);
          this.localStatus.push( val);

          if(s.value.includes("finished")){
            this.app.sceRunFinished();
          }
        }
      }).catch((e) => {
				console.log('pollStatus failed '+e);
			});
    },
  
    pollSync(){
      // console.log('polling for sync ');
       PostService.getSync().then((result) => {
        // console.dir(result);
        for(const msg of result.data){
          var timeInSeconds = Number(msg.value.Time) / 1000;      
          if(timeInSeconds > this.currentTime){
            this.currentTime = timeInSeconds
          }
        }
      }).catch((e) => {
				console.log('pollSync failed '+e);
			});
    },
  
    
  },
  computed: {

      getLines(){

      let traffic = this.app.topologies.traffic
    
      if(traffic == null || traffic == undefined){
        return []
      }
        let nodes = traffic.nodes
        let links = traffic.links
      

      var lines = [];
      for (const link of links.values()) {
        var nodeFrom = nodes.get(link["from"]);
        var nodeTo = nodes.get(link["to"]);

        //case with two links on bidirect road, label overlay
        var yoffset = 0;
        if(nodeFrom["x"]>nodeTo["x"]){
            yoffset = 20;
        }
        var xoffset = 0;
        if(nodeFrom["y"]>nodeTo["y"]){
            xoffset = 20;
            yoffset = 10;
        }
        lines.push({
          points: [
            parseInt(nodeFrom["x"]),
            parseInt(nodeFrom["y"]),
            parseInt(nodeTo["x"]),
            parseInt(nodeTo["y"]),
          ],
          labelpoints : [
            (parseInt(nodeFrom["x"]) + parseInt(nodeTo["x"]) ) /2 - xoffset,
            (parseInt(nodeFrom["y"]) + parseInt(nodeTo["y"]) ) /2 - yoffset,
          ],
          label: link["id"],
        });
      }
      return lines
    },

    getPoints(){

      let traffic = this.app.topologies.traffic
      let nodes = []
      let links = []
      if(traffic != null){
        nodes = traffic.nodes
        links = traffic.links
      }

      var points = [];
      for (const link of links.values()) {
        var nodeFrom = nodes.get(link["from"]);
        points.push(
          {
            x: parseInt(nodeFrom["x"]),
            y: parseInt(nodeFrom["y"]),
            label: nodeFrom["id"],
          },
        );
      }
      return points
    },

    clientHeight (){
      return window.innerHeight
    },

    clientWidth (){
      return window.innerWidth
    },
    
    statsStageConfig (){
      return {
        width: this.clientWidth-450,
        height: this.clientHeight-550,
      }
      },
  },

  
  mounted: function () {
    this.$refs.statsStage.getNode().draggable(true);
    // var micro = {vehicleID : "foo", position : {x : -352, y : -233}}
    // this.localTrafficMap = new Map(this.localTrafficMap.set(micro.vehicleID, micro))
    // alert(this.clientHeight +"_" + this.clientWidth)

    setInterval(function () {
      if(this.app.dummyRun == true){
        this.pollProvision();
      }

      if(this.app.runningSce){

          this.endTime = this.app.scee.simulationEnd
          this.pollStatus(); //always check for status when sce is running
      }
      }.bind(this), 9000); 
  }
};
</script>