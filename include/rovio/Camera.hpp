/*
* For parts of code or files added at the Autonomous Robots Lab, Norwegian University of Science and Technology, the following license is applicable:
*
* Copyright (c) 2024, Autonomous Robots Lab, Norwegian University of Science and Technology
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice, this
*    list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright notice,
*    this list of conditions and the following disclaimer in the documentation
*    and/or other materials provided with the distribution.
*
* 3. Neither the name of the copyright holder nor the names of its
*    contributors may be used to endorse or promote products derived from
*    this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
* For parts of code or files directly adopted from the Autonomous Systems Lab, the following license is applicable (as from original repository for ROVIO: https://github.com/ethz-asl/rovio):
*
* Copyright (c) 2014, Autonomous Systems Lab
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
* * Redistributions of source code must retain the above copyright
* notice, this list of conditions and the following disclaimer.
* * Redistributions in binary form must reproduce the above copyright
* notice, this list of conditions and the following disclaimer in the
* documentation and/or other materials provided with the distribution.
* * Neither the name of the Autonomous Systems Lab, ETH Zurich nor the
* names of its contributors may be used to endorse or promote products
* derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef ROVIO_CAMERA_HPP_
#define ROVIO_CAMERA_HPP_

#include "lightweight_filtering/common.hpp"
#include <opencv2/features2d/features2d.hpp>
#include "lightweight_filtering/State.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d.hpp>

namespace rovio{

class Camera{
 public:
  /** \brief Distortion model of the camera.
   * */
  enum ModelType{
    REFRAC, //!< Refractive distortion model.
    RADTAN,    //!< Radial tangential distortion model.
    EQUIDIST,  //!< Equidistant distortion model.
    EQUIREFRAC, //!< Equidistant(Refractive) distortion model.
    DS         //!< Double sphere distortion model.
  } type_;

  Eigen::Matrix3d K_; //!< Intrinsic parameter matrix.

  //@{
  /** \brief Distortion Parameter. */
  double k1_,k2_,k3_,k4_,k5_,k6_;
  double p1_,p2_,s1_,s2_,s3_,s4_;
  double refrac_ind_;
  //@}

  //@{
  /** \brief Radius (as ratio of images shortest side) within which features can be initalized. */
  double valid_radius_;
  //@}

  /** \brief Constructor.
   *
   *  Initializes the camera object as pinhole camera, i.e. all distortion coefficients are set to zero.
   *  The intrinsic parameter matrix is set equal to the identity matrix.
   * */
  Camera();

  /** \brief Destructor.
   * */
  virtual ~Camera();

  /** \brief Loads and sets the intrinsic parameter matrix K_ from yaml-file.
   *
   *   @param filename - Path to the yaml-file, containing the intrinsic parameter matrix coefficients.
   */
  void loadCameraMatrix(const std::string& filename);

  /** \brief Loads and sets the distortion parameters {k1_, k2_, k3_, p1_, p2_} for the Radtan distortion model from
   *         yaml-file.
   *   @param filename - Path to the yaml-file, containing the distortion coefficients.
   */
  void loadRadtan(const std::string& filename);

  /**
   * @brief Load and set refractive distortion parameters {n} from yaml file
   * 
   * @param filename - Path to the yaml-file, containing the refractive index.
   */
  void loadRefractive(const std::string& filename);
  
  /** \brief Loads and sets the distortion parameters {k1_, k2_, k3_, k4_} for the Equidistant distortion model from
   *         yaml-file.
   *   @param filename - Path to the yaml-file, containing the distortion coefficients.
   */
  void loadEquidist(const std::string& filename);

  /** \brief Loads and sets the distortion parameters {k1_, k2_} for the Double Sphere distortion model from
   *         yaml-file.
   *   @param filename - Path to the yaml-file, containing the distortion coefficients.
   */
  void loadDoubleSphere(const std::string& filename);

  /** \brief Loads and sets the distortion model and the corresponding distortion coefficients from yaml-file.
   *
   *   @param filename - Path to the yaml-file, containing the distortion model and distortion coefficient data.
   */
  void load(const std::string& filename);

  /** \brief Distorts a point on the unit plane (in camera coordinates) according to the Radtan distortion model.
   *
   *   @param in  - Undistorted point coordinates on the unit plane (in camera coordinates).
   *   @param out - Distorted point coordinates on the unit plane (in camera coordinates).
   */
  void distortRadtan(const Eigen::Vector2d& in, Eigen::Vector2d& out) const;

  /** \brief Distorts a point on the unit plane (in camera coordinates) according to the Radtan distortion model and
   *         outputs additionally the corresponding jacobian matrix (input to output).
   *
   *   @param in  - Undistorted point coordinates on the unit plane (in camera coordinates).
   *   @param out - Distorted point coordinates on the unit plane (in camera coordinates).
   *   @param J   - Jacobian matrix of the distortion process (input to output).
   */
  void distortRadtan(const Eigen::Vector2d& in, Eigen::Vector2d& out, Eigen::Matrix2d& J) const;

  /** \brief Distorts a point on the unit plane (in camera coordinates) according to the Refractive distortion model.
   *
   *   @param in  - Undistorted point coordinates on the unit plane (in camera coordinates).
   *   @param out - Distorted point coordinates on the unit plane (in camera coordinates).
   */
  void distortRefractive(const Eigen::Vector2d& in, Eigen::Vector2d& out) const;
  void distortEquiRefractive(const Eigen::Vector2d& in, Eigen::Vector2d& out) const;

  /**
   * @brief Distorts a point on the unit plane (in camera coordinates) according to the Refractive distortion model
   * 
   * @param in  - Undistorted point coordinates on the unit plane (in camera coordinates).
   * @param out - Distorted point coordinates on the unit plane (in camera coordinates).
   * @param refrac_index - Refractive index of the medium
   */
  void distortRefractive(const Eigen::Vector2d& in, Eigen::Vector2d& out, const double& refrac_index) const;
  
  /** \brief Distorts a point on the unit plane (in camera coordinates) according to the Refractive distortion model
   *         and outputs additionally the corresponding jacobian matrix (input to output).
   *
   *   @param in  - Undistorted point coordinates on the unit plane (in camera coordinates).
   *   @param out - Distorted point coordinates on the unit plane (in camera coordinates).
   *   @param J   - Jacobian matrix of the distortion process (input to output).
   */
  void distortRefractive(const Eigen::Vector2d& in, Eigen::Vector2d& out, Eigen::Matrix2d& J) const;
  void distortEquiRefractive(const Eigen::Vector2d& in, Eigen::Vector2d& out, Eigen::Matrix2d& J) const;

  /**
   * @brief Distorts a point on the unit plane (in camera coordinates) according to the Refractive distortion model
   * 
   * @param in  - Undistorted point coordinates on the unit plane (in camera coordinates).
   * @param out - Distorted point coordinates on the unit plane (in camera coordinates).
   * @param refrac_index - Refractive index of the medium
   * @param J   - Jacobian matrix of the distortion process (input to output).
   */
  void distortRefractive(const Eigen::Vector2d& in, Eigen::Vector2d& out, const double& refrac_index, Eigen::Matrix2d& J) const;
  void distortEquiRefractive(const Eigen::Vector2d& in, Eigen::Vector2d& out, const double& refrac_index, Eigen::Matrix2d& J) const;
  void distortEquiRefractive(const Eigen::Vector2d& in, Eigen::Vector2d& out, Eigen::Matrix2d& J_equi, Eigen::Matrix2d& J_refrac) const;

  /** \brief Distorts a point on the unit plane (in camera coordinates) according to the Equidistant distortion model.
   *
   *   @param in  - Undistorted point coordinates on the unit plane (in camera coordinates).
   *   @param out - Distorted point coordinates on the unit plane (in camera coordinates).
   */
  void distortEquidist(const Eigen::Vector2d& in, Eigen::Vector2d& out) const;

  /** \brief Distorts a point on the unit plane (in camera coordinates) according to the Equidistant distortion model
   *         and outputs additionally the corresponding jacobian matrix (input to output).
   *
   *   @param in  - Undistorted point coordinates on the unit plane (in camera coordinates).
   *   @param out - Distorted point coordinates on the unit plane (in camera coordinates).
   *   @param J   - Jacobian matrix of the distortion process (input to output).
   */
  void distortEquidist(const Eigen::Vector2d& in, Eigen::Vector2d& out, Eigen::Matrix2d& J) const;

  /** \brief Distorts a point on the unit plane (in camera coordinates) according to the Double Sphere distortion model.
   *
   *   @param in  - Undistorted point coordinates on the unit plane (in camera coordinates).
   *   @param out - Distorted point coordinates on the unit plane (in camera coordinates).
   */
  void distortDoubleSphere(const Eigen::Vector2d& in, Eigen::Vector2d& out) const;

  /** \brief Distorts a point on the unit plane (in camera coordinates) according to the Double Sphere distortion model
   *         and outputs additionally the corresponding jacobian matrix (input to output).
   *
   *   @param in  - Undistorted point coordinates on the unit plane (in camera coordinates).
   *   @param out - Distorted point coordinates on the unit plane (in camera coordinates).
   *   @param J   - Jacobian matrix of the distortion process (input to output).
   */
  void distortDoubleSphere(const Eigen::Vector2d& in, Eigen::Vector2d& out, Eigen::Matrix2d& J) const;

  /** \brief Distorts a point on the unit plane, according to the set distortion model (#ModelType) and to the set
   *         distortion coefficients.
   *
   *   @param in  - Undistorted point coordinates on the unit plane (in camera coordinates).
   *   @param out - Distorted point coordinates on the unit plane (in camera coordinates).
   */
  void distort(const Eigen::Vector2d& in, Eigen::Vector2d& out) const;

  /** \brief Distorts a point on the unit plane, according to the set distortion model (#ModelType) and to the set
   *         distortion coefficients.
   *
   *   @param in  - Undistorted point coordinates on the unit plane (in camera coordinates).
   *   @param out - Distorted point coordinates on the unit plane (in camera coordinates).
   *   @param refrac_index - Refractive index of the medium
   */
  void distort(const Eigen::Vector2d& in, Eigen::Vector2d& out, const double& refrac_index) const;

  /** \brief Distorts a point on the unit plane, according to the set distortion model (#ModelType) and to the set
   *         distortion coefficients. Outputs additionally the corresponding jacobian matrix (input to output).
   *
   *   @param in  - Undistorted point coordinates on the unit plane (in camera coordinates).
   *   @param out - Distorted point coordinates on the unit plane (in camera coordinates).
   *   @param J   - Jacobian matrix of the distortion process (input to output).
   */
  void distort(const Eigen::Vector2d& in, Eigen::Vector2d& out, Eigen::Matrix2d& J) const;

  /** \brief Distorts a point on the unit plane, according to the set distortion model (#ModelType) and to the set
   *         distortion coefficients. Outputs additionally the corresponding jacobian matrix (input to output).
   *
   *   @param in  - Undistorted point coordinates on the unit plane (in camera coordinates).
   *   @param out - Distorted point coordinates on the unit plane (in camera coordinates).
   *   @param refrac_index - Refractive index of the medium
   *   @param J   - Jacobian matrix of the distortion process (input to output).
   */
  void distort(const Eigen::Vector2d& in, Eigen::Vector2d& out , const double& refrac_index , Eigen::Matrix2d& J) const;

  /** \brief Outputs the (distorted) pixel coordinates corresponding to a given bearing vector,
   *         using the set distortion model.
   *
   *   @param vec - Bearing vector (in camera coordinates | unit length not necessary).
   *   @param c   - (Distorted) pixel coordinates.
   *   @return True, if process successful.
   */
  bool bearingToPixel(const Eigen::Vector3d& vec, cv::Point2f& c) const;
  bool bearingToPixel(const Eigen::Vector3d& vec, cv::Point2f& c, const double& refrac_index) const;

  /** \brief Outputs the (distorted) pixel coordinates corresponding to a given bearing vector,
   *         using the set distortion model. Outputs additionally the corresponding jacobian matrix (input to output).
   *
   *   @param vec - Bearing vector (in camera coordinates | unit length not necessary).
   *   @param c   - (Distorted) pixel coordinates.
   *   @param J   - Jacobian matrix of the distortion process (input to output).
   *   @return True, if process successful.
   */
  bool bearingToPixel(const Eigen::Vector3d& vec, cv::Point2f& c, Eigen::Matrix<double,2,3>& J) const;
  bool bearingToPixel(const Eigen::Vector3d& vec, cv::Point2f& c, Eigen::Matrix<double,2,3>& J, const double& refrac_index) const;
  bool bearingToPixel(const Eigen::Vector3d& vec, cv::Point2f& c, Eigen::Matrix<double,2,3>& J, Eigen::Matrix<double,2,1>& Jdpdn, const double& refrac_index) const;

  /** \brief Outputs the (distorted) pixel coordinates corresponding to a given NormalVectorElement-Object
   *         (bearing vector), using the set distortion model.
   *
   *   @param n   - NormalVectorElement-Object.
   *   @param c   - (Distorted pixel) coordinates.
   *   @return True, if process successful.
   */
  bool bearingToPixel(const LWF::NormalVectorElement& n, cv::Point2f& c) const;
  bool bearingToPixel(const LWF::NormalVectorElement& n, cv::Point2f& c, const double& refrac_index) const;

  /** \brief Outputs the (distorted) pixel coordinates corresponding to a given NormalVectorElement-Object
   *         (bearing vector), using the set distortion model.
   *
   *   @param n   - NormalVectorElement-Object.
   *   @param c   - (Distorted) pixel coordinates.
   *   @param J   - Jacobian matrix of the distortion process (input to output).
   *   @return True, if process successful.
   */
  bool bearingToPixel(const LWF::NormalVectorElement& n, cv::Point2f& c, Eigen::Matrix<double,2,2>& J) const;
  bool bearingToPixel(const LWF::NormalVectorElement& n, cv::Point2f& c, Eigen::Matrix<double,2,2>& J, const double& refrac_index) const;

  bool bearingToPixel(const LWF::NormalVectorElement& n, cv::Point2f& c, Eigen::Matrix<double,2,2>& J, Eigen::Matrix<double,2,1>& Jdpdn, const double& refrac_index) const;

  /** \brief Get the bearing vector, corresponding to a specific (distorted) pixel.
   *
   *   @param c   - (Distorted) pixel.
   *   @param vec - Bearing vector (unit length).
   *   @return True, if process successful.
   */
  bool pixelToBearing(const cv::Point2f& c,Eigen::Vector3d& vec) const;
  
  /** \brief Get the bearing vector, corresponding to a specific (distorted) pixel.
   *
   *   @param c   - (Distorted) pixel.
   *   @param vec - Bearing vector (unit length).
   *   @return True, if process successful.
   */
  bool pixelToBearingAnalytical(const cv::Point2f& c,Eigen::Vector3d& vec) const;

  /** \brief Get the bearing vector, corresponding to a specific (distorted) pixel.
   *
   *   @param c   - (Distorted) pixel.
   *   @param vec - Bearing vector (unit length).
   *   @param refrac_index - Refractive index of the medium
   *   @return True, if process successful.
   */
  bool pixelToBearingAnalytical(const cv::Point2f& c,Eigen::Vector3d& vec, const double& refrac_index) const;

  /** \brief Get the NormalVectorElement-Object (bearing vector) corresponding to a specific (distorted) pixel.
   *
   *   @param c   - (Distorted) pixel.
   *   @param vec - NormalVectorElement-Object.
   *   @return True, if process successful.
   */
  bool pixelToBearing(const cv::Point2f& c,LWF::NormalVectorElement& n) const;

  /** \brief Get the NormalVectorElement-Object (bearing vector) corresponding to a specific (distorted) pixel.
   *
   *   @param c   - (Distorted) pixel.
   *   @param vec - NormalVectorElement-Object.
   *   @param refrac_index - Refractive index of the medium
   *   @return True, if process successful.
   */
  bool pixelToBearing(const cv::Point2f& c,LWF::NormalVectorElement& n, const double& refrac_index) const;


  /** \brief Function testing the camera model by randomly mapping bearing vectors to pixel coordinates and vice versa.
   */
  void testCameraModel();
};

}


#endif /* ROVIO_CAMERA_HPP_ */
