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
  <b-container fluid card class="h-100" id="containerbase" >
      <div style="position:absolute; top:100px; right: 20px; margin-top:-30px; text-align:right; z-index:1"> 
        <p>
        <span style="font-size:18px; margin-right:5px; width:250px;" > Domain Reference  </span>
          <b-form-select
            id="select-roadMap"
            size="sm"
             :value="getDomainReferenceID()"
             @change="setDomainReferenceByID($event)"
              style="width:150px"
          >
          <option v-for="ref in getAvailableResourcesByType()" v-bind:value="ref.ID" v-bind:key="ref.ID">
            <!-- {{ ref.ID }}, {{ ref.Path }} -->
            {{ ref.ID }}
          </option>
          </b-form-select>
        </p>
      </div>

  <b-row  fluid class="h-100" > 
   <b-container fluid card id="containerbase" style="height:100%">
      <b-row style="padding-top: 10px;text-align:left z-index: -10000;"  align-v="end" >
        <b-col cols="auto" >
          <b-button
            @click="addInstance"
            variant="light"
            squared
            style="margin:0px 0px -1px 3px;padding:5px;border-top-width:1px;border-left-width:1px;border-bottom-width:0px;border-right-width:1px;border-color:lightgrey"
            >
            <img src="./../assets/plus.png" width="45px"/>
            Add Instance </b-button
          >
        </b-col>
        <b-col>
          <Topology_Traffic_BBButton />
        </b-col>
      </b-row>
      <b-row fluid style="z-index: 10000; height:85%;text-align:left; border:1px solid lightgrey;border-radius:4px; background-color:#fafafa;margin-left:3px" refs="roadMapDrawContainer" class="roadMapDrawContainer" id="roadMapDrawContainer">
        <v-stage
          ref="stage"
          :config="stageConfig"
          @mousemove="handleMouseMove"
          @mouseDown="handleMouseDown"
          @mouseUp="handleMouseUp"
          style=""
        >
         <v-layer ref="layer">
                   <v-text
              ref="foo"
              :config="{
                x: 100,
                y: 100,
                fontSize: 20,
                text: ' ',
                fill: 'black',
                draggable: false,
              }" />

            <v-text
              ref="text"
              :config="{
                x: 10,
                y: 10,
                fontSize: 20,
                text: text,
                fill: 'black',
                draggable: false,
              }"
            />
            <v-rect
              v-for="(rec, index) in instanceRectangleList"
              :key="'r' + index"
              :config="{
                x: rec.x,
                y: rec.y,
                width: rec.width,
                height: rec.height,
                fill: getColor(rec.id),
                stroke: getColor(rec.id),
                opacity: 0.5,
                strokeWidth: 3,
                draggable: false,
              }"
            />

            <v-line
              v-for="(line, index) in getLines"
              :key="'l' + index"
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
              :key="'linktext' + index"          
              :config="{  
                x: line.labelpoints[0],
                y: line.labelpoints[1],
                text: line.label,        
                fontFamily: 'Calibri',
                fontSize: 12,
                padding: 5,
                textFill: 'white',
                fill: 'black',
                alpha: 0.75,
                visible: true}" />



            <v-text
              v-for="(circle, index) in getPoints" 
              :key="'nodetext' + index"          
              :config="{  
                x: Math.min(circle.x),
                y: Math.min(circle.y),
                text: circle.label,        
                fontFamily: 'Calibri',
                fontSize: 12,
                padding: 5,
                textFill: 'white',
                fill: 'black',
                alpha: 0.75,
                visible: true}" />
            <v-circle
              v-for="(circle, index) in getPoints"
              :key="'pc'+index"
              :config="{
                x: Math.min(circle.x),
                y: Math.min(circle.y),
                radius: 7,
                fill: '#007bff',// '#17a2b8',
                stroke: '#007bff',
                strokeWidth: 3,
                draggable: false,
              }"
            />
      
          </v-layer>
        </v-stage>
      </b-row>
    </b-container>
    </b-row>
  </b-container>
</template>
<script>
// import Vue from "vue";
import Topology_Traffic_BBButton from "./Topology_Traffic_BBButton.vue";
import PostService from "../PostService.js";

export default {
  data() {
    return {
      app: this.$root.$children[0],
      stageConfig: {
        width: 500,
        height: 500,
        roadMapDrawContainer: "roadMapDrawContainer",
      },
      text: "",
      // borderLinks:[],
      isDrawing: false,
      instanceRectangleList: [],
      links: [
        { fromx: 10, fromy: 0, tox: 123, toy: 423 },
        { fromx: 30, fromy: 220, tox: 323, toy: 1423 },
        { fromx: 50, fromy: 20, tox: 13, toy: 23 },
      ],
      textContent: "",
      instanceRectangleMap: new Map(),
      currentScale: 1,
      responsibilities: new Map(),
      // borders: [],
      drawDebugOutput: true,
    };
  },
  components: {
    Topology_Traffic_BBButton,
  },
  methods: {


    getDomainReferenceID(){
      let r =  this.app.getDomainReferenceID("traffic");
      console.log("domainreference is "+r )
      return r
    },
    setDomainReferenceByID(ref){
      //write ref in sce
      this.app.setDomainReferenceByID("traffic", ref);

      //fetch map and draw it
      console.log("fetching map: '"+ref+"'")
      PostService.getResource({id : ref}).then((result) => {
          console.log("returned new road map ")
          console.dir(result)
          this.app.parseMap(result.data.str)
          console.log("drawing that map ")
          this.drawMap(); 
        });
    },
    getAvailableResourcesByType(){
      let l = this.app.getAvailableResourcesByType("traffic", "", "DomainReference")
      // console.log("getAvailableResourcesByType traffic:")
      // console.dir(l)
      return l
    },

    addInstance() {
      this.$root.$emit("addInstance");
    },








  /* GUI*/
    handleMouseDown() {
      if (this.app.activeInstance == undefined) {
        return;
      }

      this.$refs.stage.getNode().draggable(false);
      this.isDrawing = true;

      console.log(
        "started drawing rect for instance : " + this.app.activeInstance
      );
      const pos = this.getPointer();
      this.instanceRectangleMap.set(this.app.activeInstance, {
        id: this.app.activeInstance,
        x: pos.x,
        y: pos.y,
        width: 0,
        height: 0,
      });
    },
    handleMouseUp() {
      if (this.app.activeInstance == undefined || !this.isDrawing) {
        return;
      }
      //set orientation
      let curRec = this.instanceRectangleMap.get(this.app.activeInstance);
      console.log("curRec:");
      console.log(curRec);
      curRec.x = Math.min(curRec.x, curRec.x + curRec.width);
      curRec.y = Math.min(curRec.y, curRec.y + curRec.height);
      curRec.width = Math.abs(curRec.width);
      curRec.height = Math.abs(curRec.height);

      this.isDrawing = false;

      this.$refs.stage.getNode().draggable(true);
      this.app.activeInstance = undefined;

      this.refreshResponsibilities();
    },
    getColor(id) {
      return this.app.getColor(id);
    },
    nodeInRect(node,rect){          
      let originalX = rect.x;
      let originalY = rect.y;
      return (
      node["x"] > originalX &&
      node["x"] < originalX + rect.width &&
      node["y"] > originalY &&
      node["y"] < originalY + rect.height)
    },
    refreshResponsibilities() {
      // let nodes = Vue.prototype.$roadMap.nodes;
      let nodes = this.app.topologies.traffic.nodes;

      console.log("nodes:")
      console.dir(nodes)
      console.log("instances:")
      console.dir(this.app.scee.buildingBlocks)
      var resps = new Map()
      for (const inst of this.app.scee.buildingBlocks) {
        resps.set(inst.ID, [])
      }

      //each node is only assigned to at max one rect
      for (const node of nodes.values()) {
        console.log("checking if "+node["id"]+" belongs to some rectangle");
        let assignedTo = null;

        
       
        //iterate over all instances (for correct order)
        for (const inst of this.app.scee.buildingBlocks) {
          //check if inst has a rect
          console.dir(inst)
          let instanceID = inst.ID
          let rect = this.instanceRectangleMap.get(instanceID);
          if( rect == null) {
            console.log("no rect for "+instanceID);
            continue;
          }
          //this implies that instances at the back of the list have a higher prio
          if ( this.nodeInRect(node, rect)){
            assignedTo = instanceID;
          }
        }
        if (assignedTo != null){
          console.log("         --> assigned to "+assignedTo);
          resps.get(assignedTo).push(node["id"]);
        } else{
          console.log("         --> not assigned");
        }
      }

      //now update sce with changes
      for (const inst of this.app.scee.buildingBlocks) {
        let instanceID = inst.ID;
        this.app.setResp(instanceID, resps.get(instanceID));
      }


      this.app.updateTranslatorResps()
    },


    handleMouseMove() {
      //console.log(this.getPointer().x);
      // no drawing - skipping
      if (!this.isDrawing) {
        return;
      }
      const point = this.getPointer();
      console.log("pointer position:");
      console.dir(point);
      // handle  rectangle part
      let curRec = this.instanceRectangleMap.get(this.app.activeInstance);
      curRec.width = point.x - curRec.x;
      curRec.height = point.y - curRec.y;

      this.instanceRectangleList = Array.from(
        this.instanceRectangleMap.values()
      );
    },

    getPointer() {
      let stage = this.$refs.stage.getNode();
      var pointer = stage.getPointerPosition();
      var scale = stage.scaleX();
      var mousePointTo = {
        x: (pointer.x - stage.x()) / scale,
        y: (pointer.y - stage.y()) / scale,
      };

      return mousePointTo;
    },

    setZoom(newScale) {
      let stage = this.$refs.stage.getNode();
      var oldScale = stage.scaleX();
      var center = {
        x: stage.width() / 2,
        y: stage.height() / 2,
      };

      var relatedTo = {
        x: (center.x - stage.x()) / oldScale,
        y: (center.y - stage.y()) / oldScale,
      };

      stage.scale({
        x: newScale,
        y: newScale,
      });

      var newPos = {
        x: center.x - relatedTo.x * newScale,
        y: center.y - relatedTo.y * newScale,
      };
      stage.position(newPos);
    },
    moveCanvas(p) {
      //console.log("shifting canvas", p.x, p.y);
      this.$refs.stage.getNode().position(p);
    },

    drawMap() {
    //only if we have a map
      if (this.app.topologies.traffic == null){
        console.log("canceling draw map request since we got no resource")
        return
      }

      let links = this.app.topologies.traffic.links
      let nodes = this.app.topologies.traffic.nodes;
      let x1 = this.app.topologies.traffic.x_min;
      let x2 = this.app.topologies.traffic.x_max;
      let y1 = this.app.topologies.traffic.y_min;
      let y2 = this.app.topologies.traffic.y_max;


      this.$refs.stage.getNode().draggable(true);
      this.app.activeInstance = undefined;

      if(x2 == x1){
        x2 += 2
      }
      if(y2 == y1){
        y2 += 2
      }

      var mapWidth = x2 - x1;
      var mapHeight = y2 - y1;

      var stageWidth = this.stageConfig.width
      var stageHeight = this.stageConfig.height


      //fit in window
      var mult = 0.7 * Math.min(stageWidth / mapWidth, stageHeight / mapHeight);

      //center map
      //var offX = -x1 - mapWidth / 2;
      var mapXCenter = (x1 + x2) / 2;
      var mapYCenter = (y1 + y2) / 2;
      var canvasXCenter = stageWidth / 2;
      var canvasYCenter = stageHeight / 2;
      var offX = canvasXCenter - mult*mapXCenter;
      var offY = canvasYCenter - mult*mapYCenter;
      if(this.drawDebugOutput==true){
        console.dir("stageWidth=" + stageWidth + ", mapWidth=" +mapWidth+"; stageHeight=" + stageHeight + ", mapHeight=" +mapHeight)
        console.dir("scaling with factor=" +mult) 
        console.log("map boundaries", x1, x2, y1, y2);
        console.log("mapWidth is " + mapWidth);
        console.log("mapXCenter is " + mapXCenter);
        console.log("canvasXCenter is " + canvasXCenter);
        console.log("mapHeight is " + mapHeight);
        console.log("mapYCenter is " + mapYCenter);
        console.log("canvasYCenter is " + canvasYCenter);
        console.log("canvasWidth is " + stageWidth);
        console.log("offX is " + offX);
        console.log("offY is " + offY);
      }
      var p = { x: offX, y: offY };
      
      this.moveCanvas(p);
      this.setZoom(mult);


      //links and nodes
      var lines = [];
      var circles = [];
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
        circles.push(
          {
            x: parseInt(nodeFrom["x"]),
            y: parseInt(nodeFrom["y"]),
            label: nodeFrom["id"],
          },
        );
      }
    },
  },


  computed: {
    instances() {
      return this.app.scee.TrafficSimulators;
    },
    getLines(){

      let traffic = this.app.topologies.traffic
      let nodes = []
      let links = []
      if(traffic != null){
        nodes = traffic.nodes
        links = traffic.links
      }

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
    }
  },

  watch: {
    $roadMap: {
      handler() {
        console.log("roadMap changed, redrawing");
        this.drawMap();
      },
      deep: true,
    },
    instances : function (){

      console.log("instances changed, refreshing resps");
      //this.app.activeInstance = undefined;
      this.refreshResponsibilities();
      this.refreshBorderLinks();
    }
  },

  mounted: function () {

    const roadMapDrawContainer = document.querySelector(".roadMapDrawContainer"); //todo: does not work
    const observer = new ResizeObserver(() => {
    if(roadMapDrawContainer.offsetWidth < 2){ //not visible
      console.log("drawContainer is not visible")
      return
    }
    this.stageConfig.width = roadMapDrawContainer.offsetWidth -30;
    this.stageConfig.height = roadMapDrawContainer.offsetHeight -30;
    //console.log("resize windows, dims now: " + this.stageConfig.width +"|" +this.stageConfig.height)

    this.drawMap();
    });
    observer.observe(roadMapDrawContainer);

    //const container = document.querySelector(".container");
    //console.dir(container)
    // this.stageConfig.width = window.innerWidth-800;
    // this.stageConfig.height = window.innerHeight-300;
    var scaleBy = 1.1;
    let stage = this.$refs.stage.getNode();


    //console.dir("stage width is " +  this.stageConfig.width)

    stage.on("wheel", (e) => {
      e.evt.preventDefault();
      var oldScale = stage.scaleX();

      var pointer = stage.getPointerPosition();

      var mousePointTo = {
        x: (pointer.x - stage.x()) / oldScale,
        y: (pointer.y - stage.y()) / oldScale,
      };

      var newScale = e.evt.deltaY > 0 ? oldScale * scaleBy : oldScale / scaleBy;

      stage.scale({ x: newScale, y: newScale });

      var newPos = {
        x: pointer.x - mousePointTo.x * newScale,
        y: pointer.y - mousePointTo.y * newScale,
      };
      this.moveCanvas(newPos);
      stage.batchDraw();
    });

    // this.drawMap();

  },
};
</script>