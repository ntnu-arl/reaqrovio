# ReAqROVIO: Refractive Aquatic ROVIO #

This repository contains the implementation of the work [Online refractive camera model calibration in visual-inertial odometry](https://ieeexplore.ieee.org/document/10802302). The method enables Visual-Inertial Odometry (VIO) underwater without the need of camera calibration in the water. The cameras are only calirated in air and the refractive distortion due to water is rectified online by estimating the refractive index in the state of the Iterated Extended Kalman Filter. The work is developed over ROVIO.

Paper:
* [Online refractive camera model calibration in visual-inertial odometry](https://ieeexplore.ieee.org/document/10802302) (IROS 2024)

## Key features:
1)  **Refractive camera model**: to enable online adaptation of the camera model to mitigate the refractive effects of underwater given the calibration in air.
2) **Barometric Depth Update**: Integration of barometric depth update.
3) **Multi-camera**: Extending unified refractive index estimation with odometry from multiple synchronized cameras.

## Multi-Camera Underwater Dataset
ReAqROVIO can be tested on our open-source [Multi Camera Underwater Visual-Inertial Dataset](https://github.com/ntnu-arl/underwater-datasets?tab=readme-ov-file#subset-3-trajectories-with-ground-truth-from-motion-capture). We use the **3rd subset** of the dataset for our experiments in this work.

## [YouTube](https://www.youtube.com/watch?v=i9Cz8xE-0RI&ab_channel=KostasAlexis) Video

![alt text](media/Traj1.gif)

## Installation
### Dependencies
* ros1
    * Tested on ros noetic
* kindr (https://github.com/ethz-asl/kindr)
    * Clone outside rovio ros package, within the same workspace
* lightweight_filtering (clone from [lightweight_filtering](https://github.com/Mohit505Git/lightweight_filtering) or, as submodule, use "git submodule update --init --recursive")
    * Clone inside rovio ros package

### Install without opengl scene (recommended) ###

```
#!build

catkin build rovio --cmake-args -DCMAKE_BUILD_TYPE=Release
```
If the CMakeList is updated, then, clean and rebuild as follows:
```
#! re-build
catkin clean rovio
catkin build rovio --cmake-args -DCMAKE_BUILD_TYPE=Release
```

### Install with opengl scene ###
Additional dependencies: opengl, glut, glew (sudo apt-get install freeglut3-dev, sudo apt-get install libglew-dev)
```
#!command

catkin build rovio --cmake-args -DCMAKE_BUILD_TYPE=Release -DMAKE_SCENE=ON
```

## Usage instructions
### Rosbag download and play

1. Download rosbags for trajectory (eg. Traj 1 from the dataset [link](https://ntnu.app.box.com/s/8tpgvtqlrhol8rts929x9h3rbo57mat6))
2. Move all the bag rosbag files into a directory (eg. ```<path to dataset>/Traj1```)
3. Play the rosbag for either monocular or stereo configuration:
```
# Mono: (i.e setting ROVIO_NCAM to 1 in CMakeList):
 
rosbag play <path to dataset>/Traj1/* --topics /alphasense_driver_ros/cam0 /alphasense_driver_ros/imu --clock

# Stereo: (i.e setting ROVIO_NCAM to 2 in CMakeList):

rosbag play <path to dataset>/Traj1/* --topics /alphasense_driver_ros/cam0 /alphasense_driver_ros/cam1 /alphasense_driver_ros/imu --clock
```

### Setting ROVIO parameters
#### CMakeList
The CMakeList controls the maximum number of features used in the state of ROVIO, the number of cameras used and multi-level patch parameters. 
** Parameters**
```
set(ROVIO_NMAXFEATURE 25 CACHE STRING "Number of features for ROVIO")
set(ROVIO_NCAM 2 CACHE STRING "Number of enabled cameras")
```
#### Launch file
The launch file allows to set the initialization for the refractive index
```
refractive_index = 1.33 (ideal value for fresh water)
```
#### info file 
To enable to estimation of refractive index online, set following parameters as true, if false, the above initialization is used as a constant value.
```
refractiveCalibration true 
useObservabilityCheck true
```
To tune the convergence of the refractive index, following noise parameter can be varied.
```
Prediction.PredictionNoise.ref_0 2.0e-6;
```
### Running Launch file
* Terminal 1
```
roslaunch rovio sim_core.launch
```
* Terminal 2
```
rosbag play <path to dataset>/Traj1/* --topics /alphasense_driver_ros/cam0 /alphasense_driver_ros/cam1 /alphasense_driver_ros/imu --clock
```
* Terminal 3
```
roslaunch rovio rovio_rcm.launch
```
## Potential issues and fixes
* In case ```git submodule update fails``` please clone ```git@github.com:Mohit505Git/lightweight_filtering.git``` in the lightweight_filter directory.
* If any incorrect syntax is present in the .info file, then this leads to formation of a default new info file as .info_new and rovio may fail. Delete the .info_new file and retry with correct syntax.
* Continous synchronization failed error may arise due to mismatch in the topics played from the rosbag file and number or camera set in the CMakeList.

## Further notes ##
* Camera matrix and distortion parameters should be provided by a yaml file or loaded through rosparam
* The cfg/rovio.info provides most parameters for rovio. The camera extrinsics qCM (quaternion from IMU to camera frame, Hamilton-convention) and MrMC (Translation between IMU and Camera expressed in the IMU frame) should also be set there. They are being estimated during runtime so only a rough guess should be sufficient.
* Especially for application with little motion fixing the IMU-camera extrinsics can be beneficial. This can be done by setting the parameter doVECalibration to false. Please be carefull that the overall robustness and accuracy can be very sensitive to bad extrinsic calibrations.

## Papers reference for ROVIO:
* http://dx.doi.org/10.3929/ethz-a-010566547 (IROS 2015)
* http://dx.doi.org/10.1177/0278364917728574 (IJRR 2017)

## Reference
If you use ReAqROVIO in your research, please cite our work!
```
@article{singh2024online,
      title={Online Refractive Camera Model Calibration in Visual Inertial Odometry},
      author={Singh, Mohit and Alexis, Kostas},
      journal={arXiv preprint arXiv:2409.12074},
      year={2024}
    }
```

## Contact
You can contact us for any question or feel free to post an issue on this repository:
* [Mohit Singh](mailto:mohit.singh@ntnu.no)
* [Kostas Alexis](mailto:konstantinos.alexis@ntnu.no)
