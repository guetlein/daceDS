#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu Sep 30 14:54:08 2021

@author: guetlein
"""

import sys
import weakref
import numpy as np

try:
    sys.path.append('/carla/PythonAPI/carla')
except IndexError:
    pass

import carla
from carla import ColorConverter as cc

class CameraManager(object):
    """ Class for camera management"""

    def __init__(self, parent_actor, pos = "", vid = ""):
        """Constructor method"""
        gamma_correction = 2.2
        self.sensor = None
        self.surface = None
        self._parent = parent_actor
        self.recording = False
        bound_y = 0.5 + self._parent.bounding_box.extent.y
        attachment = carla.AttachmentType
        self._camera_transforms = [
            (carla.Transform(
                carla.Location(x=-5.5, z=2.5), carla.Rotation(pitch=8.0)), attachment.Rigid),
            (carla.Transform(
                carla.Location(x=1.6, z=1.7)), attachment.Rigid),
            (carla.Transform(
                carla.Location(x=5.5, y=1.5, z=1.5)), attachment.Rigid),
            (carla.Transform(
                carla.Location(x=-8.0, z=6.0), carla.Rotation(pitch=6.0)), attachment.Rigid),
            (carla.Transform(
                carla.Location(x=-1, y=-bound_y, z=0.5)), attachment.Rigid),
            (carla.Transform(
                carla.Location(x=0.1, y=-0.2, z=1.3)), attachment.Rigid), #from inside the car,
            (carla.Transform(
                carla.Location(x=-5, y=0, z=2.2), carla.Rotation(pitch=8.0)), attachment.Rigid), #from above/behind the car
            (carla.Transform(
                carla.Location(x=0, y=0, z=2)), attachment.Rigid), #over_hood
            (carla.Transform(
                carla.Location(x=0.0, z=30.0), carla.Rotation(pitch=-91.0)), attachment.Rigid),
            ] 
        if pos == "towards_forward":
            self.transform_index = 2
        elif pos == "forward_view_with_hood":
            self.transform_index = 7
        elif pos == "facing_back":
            self.transform_index = 0
        elif pos == "on_hood_directed_forward":
            self.transform_index = 1
        elif pos == "above_with_half_veh_towards_forward":
            self.transform_index = 3
        elif pos == "next_to_driver_door":
            self.transform_index = 4
        elif pos == "trough_windshield":
            self.transform_index = 5
        elif pos == "facing_back2":
            self.transform_index = 6
        elif pos == "bird":
            self.transform_index = 8
        else:
            self.transform_index = 2
            
        self.posStr = pos
        self.nexttimestamp = ""
        self.vid = vid
        
        self.sensors = [
            ['sensor.camera.rgb', cc.Raw, 'Camera RGB'],
            ['sensor.camera.depth', cc.Raw, 'Camera Depth (Raw)'],
            ['sensor.camera.depth', cc.Depth, 'Camera Depth (Gray Scale)'],
            ['sensor.camera.depth', cc.LogarithmicDepth, 'Camera Depth (Logarithmic Gray Scale)'],
            ['sensor.camera.semantic_segmentation', cc.Raw, 'Camera Semantic Segmentation (Raw)'],
            ['sensor.camera.semantic_segmentation', cc.CityScapesPalette, 'Camera Semantic Segmentation (CityScapes Palette)'],
            ['sensor.lidar.ray_cast', None, 'Lidar (Ray-Cast)']]
        bp_library = self._parent.get_world().get_blueprint_library()
        for item in self.sensors:
            blp = bp_library.find(item[0])
            if item[0].startswith('sensor.camera'):
                blp.set_attribute('image_size_x', '1024')
                blp.set_attribute('image_size_y', '768')
                if blp.has_attribute('gamma'):
                    blp.set_attribute('gamma', str(gamma_correction))
            elif item[0].startswith('sensor.lidar'):
                blp.set_attribute('range', '100')
                blp.set_attribute('points_per_second','600000')
                blp.set_attribute('rotation_frequency','10')
            item.append(blp)
        self.index = 0

    def toggle_camera(self):
        """Activate a camera"""
        # self.transform_index = (self.transform_index + 1) % len(self._camera_transforms)
        # self.transform_index = 6
        self.set_sensor(self.index, force_respawn=True)

    def set_sensor(self, index, force_respawn=False):
        
        print("set_sensor() is called")
        
        """Set a sensor"""
        index = index % len(self.sensors)
        needs_respawn = True if self.index is None else (
            force_respawn or (self.sensors[index][0] != self.sensors[self.index][0]))
        if needs_respawn:
            if self.sensor is not None:
                self.sensor.destroy()
                self.sensor = None
                print("disabling sensor")
            else:
                self.sensor = self._parent.get_world().spawn_actor(
                    self.sensors[index][-1],
                    self._camera_transforms[self.transform_index][0],
                    attach_to=self._parent,
                    attachment_type=self._camera_transforms[self.transform_index][1])
    
                # We need to pass the lambda a weak reference to
                # self to avoid circular reference.
                weak_self = weakref.ref(self)
                self.sensor.listen(lambda image: CameraManager._parse_image(weak_self, image))
                
                print("enabling sensor "+  self.sensors[index][0])
        else:
            print("no respawn needed")

        self.index = index
        
    def destroy(self):
        if self.sensor is not None:
            self.sensor.destroy()
            self.sensor = None
        

    def next_sensor(self):
        """Get the next sensor"""
        self.set_sensor(self.index + 1)

    def toggle_recording(self):
        """Toggle recording on or off"""
        self.recording = not self.recording
        
    def enable_recording(self, timestamp):
        """Toggle recording on or off"""
        self.recording = True
        self.nexttimestamp = str(round(timestamp/1000))
    def disable_recording(self):
        """Toggle recording on or off"""
        self.recording = False


    @staticmethod
    def _parse_image(weak_self, image):
        self = weak_self()
        if not self:
            print("returning cause not self")
            return
        
    
            
            
        #lidar?
        if self.index == 6 and self.recording:
            image.save_to_disk('_out/%ss_%s_%d_%s_%06d.ply' % (self.nexttimestamp,self.vid,self.index,self.posStr,image.frame_number))
            self.recording = False
        #else    
        elif self.recording:
            image.convert(self.sensors[self.index][1])
            array = np.frombuffer(image.raw_data, dtype=np.dtype("uint8"))
            array = np.reshape(array, (image.height, image.width, 4))
            array = array[:, :, :3]
            array = array[:, :, ::-1]
            image.save_to_disk('_out/%ss_%s_%d_%s_%06d.png' % (self.nexttimestamp,self.vid,self.index,self.posStr,image.frame_number))
            self.recording = False
        # else:
        #     print("not saving")