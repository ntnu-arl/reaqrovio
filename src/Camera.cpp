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

#include "rovio/Camera.hpp"
#include "yaml-cpp/yaml.h"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d.hpp>

namespace rovio{

  Camera::Camera(){
    k1_ = 0.0; k2_ = 0.0; k3_ = 0.0; k4_ = 0.0; k5_ = 0.0; k6_ = 0.0;
    p1_ = 0.0; p2_ = 0.0; s1_ = 0.0; s2_ = 0.0; s3_ = 0.0; s4_ = 0.0;
    refrac_ind_ = 1.0;
    K_.setIdentity();
    type_ = RADTAN;
    valid_radius_ = std::numeric_limits<double>::max();
  };

  Camera::~Camera(){};

  void Camera::loadCameraMatrix(const std::string& filename){
    YAML::Node config = YAML::LoadFile(filename);
    K_(0,0) = config["camera_matrix"]["data"][0].as<double>();
    K_(0,1) = config["camera_matrix"]["data"][1].as<double>();
    K_(0,2) = config["camera_matrix"]["data"][2].as<double>();
    K_(1,0) = config["camera_matrix"]["data"][3].as<double>();
    K_(1,1) = config["camera_matrix"]["data"][4].as<double>();
    K_(1,2) = config["camera_matrix"]["data"][5].as<double>();
    K_(2,0) = config["camera_matrix"]["data"][6].as<double>();
    K_(2,1) = config["camera_matrix"]["data"][7].as<double>();
    K_(2,2) = config["camera_matrix"]["data"][8].as<double>();
    std::cout << "Set Camera Matrix to:\n" << K_ << std::endl;
  }

  void Camera::loadRadtan(const std::string& filename){
    loadCameraMatrix(filename);
    YAML::Node config = YAML::LoadFile(filename);
    k1_ = config["distortion_coefficients"]["data"][0].as<double>();
    k2_ = config["distortion_coefficients"]["data"][1].as<double>();
    p1_ = config["distortion_coefficients"]["data"][2].as<double>();
    p2_ = config["distortion_coefficients"]["data"][3].as<double>();
    k3_ = config["distortion_coefficients"]["data"][4].as<double>();
    std::cout << "Set distortion parameters (Radtan) to: k1(" << k1_ << "), k2(" << k2_ << "), k3(" << k3_ << "), p1(" << p1_ << "), p2(" << p2_ << ")" << std::endl;
  }

  void Camera::loadRefractive(const std::string& filename){
    loadCameraMatrix(filename);
    YAML::Node config = YAML::LoadFile(filename);
    // refrac_ind_ = config["refractive_index"]["data"][0].as<double>();

    std::cout << "Set distortion parameters (Refractive) to: refrac_ind(" << refrac_ind_ << ")" << std::endl;
  }

  void Camera::loadEquidist(const std::string& filename){
    loadCameraMatrix(filename);
    YAML::Node config = YAML::LoadFile(filename);
    k1_ = config["distortion_coefficients"]["data"][0].as<double>();
    k2_ = config["distortion_coefficients"]["data"][1].as<double>();
    k3_ = config["distortion_coefficients"]["data"][2].as<double>();
    k4_ = config["distortion_coefficients"]["data"][3].as<double>();
    std::cout << "Set distortion parameters (Equidist) to: k1(" << k1_ << "), k2(" << k2_ << "), k3(" << k3_ << "), k4(" << k4_ << ")" << std::endl;
  }

  void Camera::loadDoubleSphere(const std::string& filename){
    loadCameraMatrix(filename);
    YAML::Node config = YAML::LoadFile(filename);
    k1_ = config["distortion_coefficients"]["data"][0].as<double>();
    k2_ = config["distortion_coefficients"]["data"][1].as<double>();

    if(config["valid_radius"]){
      valid_radius_ = config["valid_radius"].as<double>();
    }

    std::cout << "Set distortion parameters (Double Sphere) to: k1(" << k1_ << "), k2(" << k2_ << "), valid_radius(" << valid_radius_ << ")" << std::endl;
  }

  void Camera::load(const std::string& filename){
    YAML::Node config = YAML::LoadFile(filename);
    std::string distortionModel;
    distortionModel = config["distortion_model"].as<std::string>();
    if(distortionModel == "plumb_bob"){
      type_ = RADTAN;
      loadRadtan(filename);
    } else if(distortionModel == "refractive"){
      type_ = REFRAC;
      loadRefractive(filename);
    } else if(distortionModel == "equidistant"){
      type_ = EQUIDIST;
      loadEquidist(filename);
    } else if(distortionModel == "equirefractive"){
      std::cout << "Loading equirefractive model" << std::endl;
      type_ = EQUIREFRAC;
      loadEquidist(filename);
      loadRefractive(filename);
    }
    else if(distortionModel == "ds"){
      type_ = DS;
      loadDoubleSphere(filename);
    } else {
      std::cout << "ERROR: no camera Model detected!";
    }
  }

  void Camera::distortRadtan(const Eigen::Vector2d& in, Eigen::Vector2d& out) const{
    const double x2 = in(0) * in(0);
    const double y2 = in(1) * in(1);
    const double xy = in(0) * in(1);
    const double r2 = x2 + y2;
    const double kr = (1 + ((k3_ * r2 + k2_) * r2 + k1_) * r2);
    out(0) = in(0) * kr + p1_ * 2 * xy + p2_ * (r2 + 2 * x2);
    out(1) = in(1) * kr + p1_ * (r2 + 2 * y2) + p2_ * 2 * xy;
  }

  void Camera::distortRadtan(const Eigen::Vector2d& in, Eigen::Vector2d& out, Eigen::Matrix2d& J) const{
    const double x2 = in(0) * in(0);
    const double y2 = in(1) * in(1);
    const double xy = in(0) * in(1);
    const double r2 = x2 + y2;
    const double kr = (1 + ((k3_ * r2 + k2_) * r2 + k1_) * r2);
    out(0) = in(0) * kr + p1_ * 2 * xy + p2_ * (r2 + 2 * x2);
    out(1) = in(1) * kr + p1_ * (r2 + 2 * y2) + p2_ * 2 * xy;
    J(0,0) = kr + 2.0 * k1_ * x2 + 4.0 * k2_ * x2 * r2 + 6.0 * k3_ * x2 * r2 * r2 + 2.0 * p1_ * in(1) + 6.0 * p2_ * in(0);
    J(0,1) = 2.0 * k1_ * xy + 4.0 * k2_ * xy * r2 + 6.0 * k3_ * xy * r2 * r2 + 2 * p1_ * in(0) + 2 * p2_ * in(1);
    J(1,0) = J(0,1);
    J(1,1) = kr + 2.0 * k1_ * y2 + 4.0 * k2_ * y2 * r2 + 6.0 * k3_ * y2 * r2 * r2 + 6.0 * p1_ * in(1) + 2.0 * p2_ * in(0);
  }

  void Camera::distortRefractive(const Eigen::Vector2d& in, Eigen::Vector2d& out) const{
    const double x2 = in(0) * in(0);
    const double y2 = in(1) * in(1);
    const double x_y = in(0) * in(1);
    const double r2 = x2 + y2;
    const double n = refrac_ind_;
    const double n2 = n * n;

    const double m_distort = n/sqrt(1 + r2 - (n2*r2));
    out(0) = in(0) * m_distort;
    out(1) = in(1) * m_distort;


  }
  void Camera::distortEquiRefractive(const Eigen::Vector2d& in, Eigen::Vector2d& out) const{
    Eigen::Vector2d temp;
    distortRefractive(in, temp);
    distortEquidist(temp, out);

  }

  void Camera::distortRefractive(const Eigen::Vector2d& in, Eigen::Vector2d& out, const double& refrac_index) const{
    const double x2 = in(0) * in(0);
    const double y2 = in(1) * in(1);
    const double x_y = in(0) * in(1);
    const double r2 = x2 + y2;
    const double n = refrac_index;    // refractive index from state
    const double n2 = n * n;

    const double m_distort = n/sqrt(1 + r2 - (n2*r2));
    out(0) = in(0) * m_distort;
    out(1) = in(1) * m_distort;

  }

  void Camera::distortRefractive(const Eigen::Vector2d& in, Eigen::Vector2d& out, Eigen::Matrix2d& J) const{
    const double x2 = in(0) * in(0);
    const double y2 = in(1) * in(1);
    const double x_y = in(0) * in(1);
    const double r2 = x2 + y2;
    const double n = refrac_ind_;
    const double n2 = n * n;

    const double m_distort = n/sqrt(1 + r2 - (n2*r2));
    out(0) = in(0) * m_distort;
    out(1) = in(1) * m_distort;

    const double g = 1  + r2 - (n2*r2);

    J(0,0) = n*pow(g, -2.0)*(sqrt(g)*x2*(n2 - 1) + pow(g, 1.5));
    J(0,1) = n*pow(g, -1.5)*x_y*(n2 - 1);
    J(1,0) = n*pow(g, -1.5)*x_y*(n2 - 1);
    J(1,1) = n*pow(g, -2.0)*(sqrt(g)*y2*(n2 - 1) + pow(g, 1.5));
  }

  void Camera::distortEquiRefractive(const Eigen::Vector2d& in, Eigen::Vector2d& out, Eigen::Matrix2d& J) const{
    Eigen::Vector2d temp;
    Eigen::Matrix2d J_temp_refrac;
    Eigen::Matrix2d J_temp_equi;

    distortRefractive(in, temp, J_temp_refrac);
    distortEquidist(temp, out, J_temp_equi);
    J = J_temp_equi * J_temp_refrac;
  }

  void Camera::distortEquiRefractive(const Eigen::Vector2d& in, Eigen::Vector2d& out, Eigen::Matrix2d& J_equi, Eigen::Matrix2d& J_refrac) const{
    Eigen::Vector2d temp;

    distortRefractive(in, temp, J_refrac);
    distortEquidist(temp, out, J_equi);
  }

  void Camera::distortRefractive(const Eigen::Vector2d& in, Eigen::Vector2d& out, const double& refrac_index ,Eigen::Matrix2d& J) const{
    const double x2 = in(0) * in(0);
    const double y2 = in(1) * in(1);
    const double x_y = in(0) * in(1);
    const double r2 = x2 + y2;
    const double n = refrac_index; // refractive index from state
    const double n2 = n * n;

    const double m_distort = n/sqrt(1 + r2 - (n2*r2));
    out(0) = in(0) * m_distort;
    out(1) = in(1) * m_distort;

    const double g = 1  + r2 - (n2*r2);

    J(0,0) = n*pow(g, -2.0)*(sqrt(g)*x2*(n2 - 1) + pow(g, 1.5));
    J(0,1) = n*pow(g, -1.5)*x_y*(n2 - 1);
    J(1,0) = n*pow(g, -1.5)*x_y*(n2 - 1);
    J(1,1) = n*pow(g, -2.0)*(sqrt(g)*y2*(n2 - 1) + pow(g, 1.5));
  }

  void Camera::distortEquiRefractive(const Eigen::Vector2d& in, Eigen::Vector2d& out, const double& refrac_index, Eigen::Matrix2d& J) const{
    Eigen::Vector2d temp;
    Eigen::Matrix2d J_temp_refrac;
    Eigen::Matrix2d J_temp_equi;

    distortRefractive(in, temp, refrac_index, J_temp_refrac);
    distortEquidist(temp, out, J_temp_equi);
    J = J_temp_equi * J_temp_refrac;
  }

  void Camera::distortEquidist(const Eigen::Vector2d& in, Eigen::Vector2d& out) const{
    const double x2 = in(0) * in(0);
    const double y2 = in(1) * in(1);
    const double r = std::sqrt(x2 + y2); // 1/r*x

    if(r < 1e-8){
      out(0) = in(0);
      out(1) = in(1);
      return;
    }

    const double th = atan(r); // 1/(r^2 + 1)
    const double th2 = th*th;
    const double th4 = th2*th2;
    const double th6 = th2*th4;
    const double th8 = th2*th6;
    const double thd = th * (1.0 + k1_ * th2 + k2_ * th4 + k3_ * th6 + k4_ * th8);
    const double s = thd/r;

    out(0) = in(0) * s;
    out(1) = in(1) * s;
  }

  void Camera::distortEquidist(const Eigen::Vector2d& in, Eigen::Vector2d& out, Eigen::Matrix2d& J) const{
    const double x2 = in(0) * in(0);
    const double y2 = in(1) * in(1);
    const double r = std::sqrt(x2 + y2);

    if(r < 1e-8){
      out(0) = in(0);
      out(1) = in(1);
      J.setIdentity();
      return;
    }

    const double r_x = 1/r*in(0);
    const double r_y = 1/r*in(1);

    const double th = atan(r); // 1/(r^2 + 1)
    const double th_r = 1/(r*r+1);
    const double th2 = th*th;
    const double th4 = th2*th2;
    const double th6 = th2*th4;
    const double th8 = th2*th6;
    const double thd = th * (1.0 + k1_ * th2 + k2_ * th4 + k3_ * th6 + k4_ * th8);
    const double thd_th = 1.0 + 3 * k1_ * th2 + 5* k2_ * th4 + 7 * k3_ * th6 + 9 * k4_ * th8;
    const double s = thd/r;
    const double s_r = thd_th*th_r/r - thd/(r*r);

    out(0) = in(0) * s;
    out(1) = in(1) * s;

    J(0,0) = s + in(0)*s_r*r_x;
    J(0,1) = in(0)*s_r*r_y;
    J(1,0) = in(1)*s_r*r_x;
    J(1,1) = s + in(1)*s_r*r_y;
  }

  void Camera::distortDoubleSphere(const Eigen::Vector2d& in, Eigen::Vector2d& out) const{

      const double x2 = in(0) * in(0);
      const double y2 = in(1) * in(1);

      if((x2 + y2) < 1e-16){
        out(0) = in(0);
        out(1) = in(1);
        return;
      }

      const double d1 = std::sqrt(x2 + y2 + 1.0);
      const double d2 = std::sqrt(x2 + y2 + (k1_*d1 + 1.0)*(k1_*d1 + 1.0));
      const double scaling = 1.0f/(k2_*d2 + (1-k2_)*(k1_*d1+1.0));

      out(0) = in(0) * scaling;
      out(1) = in(1) * scaling;
  }

  void Camera::distortDoubleSphere(const Eigen::Vector2d& in, Eigen::Vector2d& out, Eigen::Matrix2d& J) const{
    const double x2 = in(0) * in(0);
    const double y2 = in(1) * in(1);

    if((x2 + y2) < 1e-16){
      out(0) = in(0);
      out(1) = in(1);
      J.setIdentity();
      return;
    }

    const double d1 = std::sqrt(x2 + y2 + 1.0);
    const double d2 = std::sqrt(x2 + y2 + (k1_*d1 + 1.0)*(k1_*d1 + 1.0));
    const double s = 1.0f/(k2_*d2 + (1-k2_)*(k1_*d1+1.0));

    out(0) = in(0) * s;
    out(1) = in(1) * s;

    const double d1dx = in(0)/d1;
    const double d1dy = in(1)/d1;
    const double d2dx = (in(0) + d1dx*k1_*(d1*k1_ + 1.0))/(d2);
    const double d2dy = (in(1) + d1dy*k1_*(d1*k1_ + 1.0))/(d2);

    J(0,0) = -in(0)*(d2dx*k2_ - d1dx*k1_*(k2_ - 1.0))*s*s + s;
    J(0,1) = -s*s*in(0)*(d2dy*k2_ - d1dy*k1_*(k2_ - 1.0));
    J(1,0) = -s*s*in(1)*(d2dx*k2_ - d1dx*k1_*(k2_ - 1.0));
    J(1,1) = -in(1)*(d2dy*k2_ - d1dy*k1_*(k2_ - 1.0))*s*s + s;
  }

  void Camera::distort(const Eigen::Vector2d& in, Eigen::Vector2d& out) const{
    switch(type_){
      case RADTAN:
        distortRadtan(in,out);
        break;
      case REFRAC:
        distortRefractive(in,out);
        break;
      case EQUIDIST:
        distortEquidist(in,out);
        break;
      case EQUIREFRAC:
        distortEquiRefractive(in,out);
        break;
      case DS:
        distortDoubleSphere(in,out);
        break;
      default:
        distortRadtan(in,out);
        break;
    }
  }

  void Camera::distort(const Eigen::Vector2d& in, Eigen::Vector2d& out, const double& refrac_index) const{
    std:: cout << "in distort with refrac_index" << std::endl;
    switch(type_){
      case RADTAN:
        distortRadtan(in,out);
        break;
      case REFRAC:
        distortRefractive(in,out,refrac_index);
        break;
      case EQUIDIST:
        distortEquidist(in,out);
        break;
      case EQUIREFRAC:
        distortEquiRefractive(in,out);
        break;
      case DS:
        distortDoubleSphere(in,out);
        break;
      default:
        distortRadtan(in,out);
        break;
    }
  }

  void Camera::distort(const Eigen::Vector2d& in, Eigen::Vector2d& out, Eigen::Matrix2d& J) const{
    switch(type_){
      case RADTAN:
        distortRadtan(in,out,J);
        break;
      case REFRAC:
        distortRefractive(in,out,J);
        break;
      case EQUIDIST:
        distortEquidist(in,out,J);
        break;
      case EQUIREFRAC:
        distortEquiRefractive(in,out,J);
        break;
      case DS:
        distortDoubleSphere(in,out,J);
        break;
      default:
        distortRadtan(in,out,J);
        break;
    }
  }

void Camera::distort(const Eigen::Vector2d& in, Eigen::Vector2d& out, const double& refrac_index, Eigen::Matrix2d& J) const{

    switch(type_){
      case RADTAN:
        distortRadtan(in,out,J);
        break;
      case REFRAC:
        distortRefractive(in,out, refrac_index,J);
        break;
      case EQUIDIST:
        distortEquidist(in,out,J);
        break;
      case EQUIREFRAC:
        distortEquiRefractive(in,out, refrac_index,J);
        break;
      case DS:
        distortDoubleSphere(in,out,J);
        break;
      default:
        distortRadtan(in,out,J);
        break;
    }
  }
  

  bool Camera::bearingToPixel(const Eigen::Vector3d& vec, cv::Point2f& c) const{
    // Project
    if(vec(2)<=0) return false;
    const Eigen::Vector2d undistorted = Eigen::Vector2d(vec(0)/vec(2),vec(1)/vec(2));

    // Distort
    Eigen::Vector2d distorted;
    distort(undistorted,distorted);

    // Shift origin and scale
    c.x = static_cast<float>(K_(0, 0)*distorted(0) + K_(0, 2));
    c.y = static_cast<float>(K_(1, 1)*distorted(1) + K_(1, 2));
    return true;
  }

  bool Camera::bearingToPixel(const Eigen::Vector3d& vec, cv::Point2f& c, const double& refrac_index) const{
    // Project
    if(vec(2)<=0) return false;
    const Eigen::Vector2d undistorted = Eigen::Vector2d(vec(0)/vec(2),vec(1)/vec(2));

    // Distort
    Eigen::Vector2d distorted;
    // std::cout << "in bearingToPixel with refrac_index" << std::endl;
    distort(undistorted,distorted, refrac_index);

    // Shift origin and scale
    c.x = static_cast<float>(K_(0, 0)*distorted(0) + K_(0, 2));
    c.y = static_cast<float>(K_(1, 1)*distorted(1) + K_(1, 2));
    return true;
  }

  bool Camera::bearingToPixel(const Eigen::Vector3d& vec, cv::Point2f& c, Eigen::Matrix<double,2,3>& J, Eigen::Matrix<double,2,1>& Jdpdn, const double& refrac_index) const{
    // Project
    if(vec(2)<=0) return false;
    const Eigen::Vector2d undistorted = Eigen::Vector2d(vec(0)/vec(2),vec(1)/vec(2));
    Eigen::Matrix<double,2,3> J1; J1.setZero();
    J1(0,0) = 1.0/vec(2);
    J1(0,2) = -vec(0)/pow(vec(2),2);
    J1(1,1) = 1.0/vec(2);
    J1(1,2) = -vec(1)/pow(vec(2),2);

    // Distort
    Eigen::Vector2d distorted;
    Eigen::Matrix2d J2;
    Eigen::Matrix2d J_equi;
    Eigen::Matrix2d J_refrac;
    // std::cout << "in bearingToPixel with distort(undistorted,distorted, refrac_index, J2)" << std::endl;
    
    // temporary fix for equirefractive
    if (type_ == EQUIREFRAC){
      distortEquiRefractive( undistorted,  distorted, J_equi, J_refrac);
      J2 = J_equi * J_refrac;
    }
    else{
      distort(undistorted,distorted, refrac_index, J2);
    }

    // Shift origin and scale
    c.x = static_cast<float>(K_(0, 0)*distorted(0) + K_(0, 2));
    c.y = static_cast<float>(K_(1, 1)*distorted(1) + K_(1, 2));
    Eigen::Matrix2d J3; J3.setZero();
    J3(0,0) = K_(0, 0);
    J3(1,1) = K_(1, 1);

    J = J3*J2*J1;
    
    // Jacobian of distorted point w.r.t. refractive index
    double n = refrac_index;
    double r2 = undistorted(0)*undistorted(0) + undistorted(1)*undistorted(1);
    double g = 1 + r2 - (n*n*r2);
    // clip g to avoid division by zero
    // common_term = \frac{\g^{1/2}*n^2*r^2 + g^{3/2}}{g^2}
    double common_term = (sqrt(g)*n*n*r2 + pow(g, 1.5))/(g*g);

    // Jdpdn(0) = (K_(0, 0)*undistorted(0)*common_term);
    // Jdpdn(1) = (K_(1, 1)*undistorted(1)*common_term);

    Jdpdn(0) = undistorted(0)*common_term;
    Jdpdn(1) = undistorted(1)*common_term;

    Jdpdn = K_.block<2,2>(0,0)*J_equi*Jdpdn;

    return true;
  }

  bool Camera::bearingToPixel(const Eigen::Vector3d& vec, cv::Point2f& c, Eigen::Matrix<double,2,3>& J) const{
    // Project
    if(vec(2)<=0) return false;
    const Eigen::Vector2d undistorted = Eigen::Vector2d(vec(0)/vec(2),vec(1)/vec(2));
    Eigen::Matrix<double,2,3> J1; J1.setZero();
    J1(0,0) = 1.0/vec(2);
    J1(0,2) = -vec(0)/pow(vec(2),2);
    J1(1,1) = 1.0/vec(2);
    J1(1,2) = -vec(1)/pow(vec(2),2);

    // Distort
    Eigen::Vector2d distorted;
    Eigen::Matrix2d J2;
    // std::cout << "in bearingToPixel with distort(undistorted,distorted,J2)" << std::endl;

    distort(undistorted,distorted,J2);

    // Shift origin and scale
    c.x = static_cast<float>(K_(0, 0)*distorted(0) + K_(0, 2));
    c.y = static_cast<float>(K_(1, 1)*distorted(1) + K_(1, 2));
    Eigen::Matrix2d J3; J3.setZero();
    J3(0,0) = K_(0, 0);
    J3(1,1) = K_(1, 1);

    J = J3*J2*J1;

    return true;
  }

  bool Camera::bearingToPixel(const Eigen::Vector3d& vec, cv::Point2f& c, Eigen::Matrix<double,2,3>& J, const double& refrac_index) const{
    // Project
    if(vec(2)<=0) return false;
    const Eigen::Vector2d undistorted = Eigen::Vector2d(vec(0)/vec(2),vec(1)/vec(2));
    Eigen::Matrix<double,2,3> J1; J1.setZero();
    J1(0,0) = 1.0/vec(2);
    J1(0,2) = -vec(0)/pow(vec(2),2);
    J1(1,1) = 1.0/vec(2);
    J1(1,2) = -vec(1)/pow(vec(2),2);

    // Distort
    Eigen::Vector2d distorted;
    Eigen::Matrix2d J2;
    // std::cout << "in bearingToPixel with distort(undistorted,distorted,refrac_index,J2)" << std::endl;

    distort(undistorted,distorted,refrac_index,J2);

    // Shift origin and scale
    c.x = static_cast<float>(K_(0, 0)*distorted(0) + K_(0, 2));
    c.y = static_cast<float>(K_(1, 1)*distorted(1) + K_(1, 2));
    Eigen::Matrix2d J3; J3.setZero();
    J3(0,0) = K_(0, 0);
    J3(1,1) = K_(1, 1);

    J = J3*J2*J1;

    return true;
  }

  bool Camera::bearingToPixel(const LWF::NormalVectorElement& n, cv::Point2f& c) const{
    return bearingToPixel(n.getVec(),c);
  }
  
  bool Camera::bearingToPixel(const LWF::NormalVectorElement& n, cv::Point2f& c, const double& refrac_index) const{
    return bearingToPixel(n.getVec(),c, refrac_index);
  }

  bool Camera::bearingToPixel(const LWF::NormalVectorElement& n, cv::Point2f& c, Eigen::Matrix<double,2,2>& J) const{
    Eigen::Matrix<double,3,2> J1;
    J1 = n.getM();
    Eigen::Matrix<double,2,3> J2;
    const bool success = bearingToPixel(n.getVec(),c,J2);
    J = J2*J1;
    return success;
  }

  bool Camera::bearingToPixel(const LWF::NormalVectorElement& n, cv::Point2f& c, Eigen::Matrix<double,2,2>& J, const double& refrac_index) const{
    Eigen::Matrix<double,3,2> J1;
    J1 = n.getM();
    Eigen::Matrix<double,2,3> J2;
    const bool success = bearingToPixel(n.getVec(),c,J2, refrac_index);
    J = J2*J1;
    return success;
  }

  bool Camera::bearingToPixel(const LWF::NormalVectorElement& n, cv::Point2f& c, Eigen::Matrix<double,2,2>& J, Eigen::Matrix<double,2,1>& Jdpdn, const double& refrac_index) const{
    Eigen::Matrix<double,3,2> J1;
    J1 = n.getM();
    Eigen::Matrix<double,2,3> J2;
    const bool success = bearingToPixel(n.getVec(),c,J2,Jdpdn, refrac_index);
    J = J2*J1;
    return success;
  }

  bool Camera::pixelToBearing(const cv::Point2f& c,Eigen::Vector3d& vec) const{
    // Shift origin and scale
    Eigen::Vector2d y;
    y(0) = (static_cast<double>(c.x) - K_(0, 2)) / K_(0, 0);
    y(1) = (static_cast<double>(c.y) - K_(1, 2)) / K_(1, 1);

    // Undistort by optimizing
    const int max_iter = 100;
    const double tolerance = 1e-10;
    Eigen::Vector2d ybar = y; // current guess (undistorted)
    Eigen::Matrix2d J;
    Eigen::Vector2d y_tmp; // current guess (distorted)
    Eigen::Vector2d e;
    Eigen::Vector2d du;
    bool success = false;
    for (int i = 0; i < max_iter; i++) {
      distort(ybar,y_tmp,J);
      e = y - y_tmp;
      du = (J.transpose() * J).inverse() * J.transpose() * e;
      ybar += du;
      if (e.dot(e) <= tolerance){
        success = true;
        break;
      }
      // else{
      //   std::cout << "Failed"<< std::endl;
      //   std::cout << "e.dot(e) = " << e.dot(e) << std::endl;
      //   std::cout << "ybar = " << ybar.transpose() << std::endl;
        
      // }
    }
    if(success){
      y = ybar;
      vec = Eigen::Vector3d(y(0),y(1),1.0).normalized();
    }
    return success;
  }

  bool Camera::pixelToBearingAnalytical(const cv::Point2f& c,Eigen::Vector3d& vec) const{
    // Shift origin and scale
    Eigen::Vector2d y;
    // y(0) = (static_cast<double>(c.x) - K_(0, 2)) / K_(0, 0);
    // y(1) = (static_cast<double>(c.y) - K_(1, 2)) / K_(1, 1);
    y(0) = static_cast<double>(c.x);
    y(1) = static_cast<double>(c.y);

    
    // Convert point to opencv format
    cv::Mat y_mat(1, 2, CV_32F);
    y_mat.at<float>(0, 0) = y(0);
    y_mat.at<float>(0, 1) = y(1);
    y_mat = y_mat.reshape(2); // Nx1, 2-channel


    cv::Matx33d K(K_(0,0), 0, K_(0,2), 0, K_(1,1), K_(1,2), 0, 0, 1);
    cv::Vec4d D(k1_, k2_, k3_, k4_);
    // cv::undistortPoints(y_mat, undistorted, K, D);
    cv::fisheye::undistortPoints(y_mat, y_mat, K, D);

    y(0) = y_mat.at<float>(0, 0);
    y(1) = y_mat.at<float>(0, 1);

    // Undistort by analytical solution
    const double x2 = y(0) * y(0);
    const double y2 = y(1) * y(1);
    const double r2 = x2 + y2;
    const double n = refrac_ind_;
    const double n2 = n * n;
    const double m_undistort = sqrt((n2*r2) + n2 - r2);
    y = y / m_undistort;

    vec = Eigen::Vector3d(y(0),y(1),1.0).normalized();
    // std::cout << "###### pixelToBearingAnalytical using refrac_ind_ = " << refrac_ind_ << "\n";
    return true;
  }


  bool Camera::pixelToBearing(const cv::Point2f& c,LWF::NormalVectorElement& n) const{
    Eigen::Vector3d vec;
    bool success;
    if (type_==REFRAC || type_==EQUIREFRAC){
      success = pixelToBearingAnalytical(c,vec);
    }
    else{
      success = pixelToBearing(c,vec);
    }

    n.setFromVector(vec);
    return success;
  }



  void Camera::testCameraModel(){
    double d = 1e-4;
    LWF::NormalVectorElement b_s;
    LWF::NormalVectorElement b_s1;
    LWF::NormalVectorElement b_s2;
    Eigen::Vector3d v_s;
    Eigen::Vector3d v_s1;
    Eigen::Vector3d v_s2;
    LWF::NormalVectorElement b_e;
    Eigen::Matrix2d J1;
    Eigen::Matrix2d J1_FD;
    Eigen::Matrix<double,2,3> J2;
    Eigen::Matrix<double,2,3> J2_FD;
    cv::Point2f p_s;
    cv::Point2f p_s1;
    cv::Point2f p_s2;
    cv::Point2f p_s3;
    Eigen::Vector2d diff;
    for(unsigned int s = 1; s<10;){
      b_s.setRandom(s);
      if(b_s.getVec()(2)<0) b_s = b_s.inverted();
      bearingToPixel(b_s,p_s,J1);
      pixelToBearing(p_s,b_e);
      b_s.boxMinus(b_e,diff);
      std::cout << b_s.getVec().transpose() << std::endl;
      std::cout << "Error after back and forward mapping: " << diff.norm() << std::endl;
      diff = Eigen::Vector2d(d,0);
      b_s.boxPlus(diff,b_s1);
      bearingToPixel(b_s1,p_s1);
      J1_FD(0,0) = static_cast<double>((p_s1-p_s).x)/d;
      J1_FD(1,0) = static_cast<double>((p_s1-p_s).y)/d;
      diff = Eigen::Vector2d(0,d);
      b_s.boxPlus(diff,b_s2);
      bearingToPixel(b_s2,p_s2);
      J1_FD(0,1) = static_cast<double>((p_s2-p_s).x)/d;
      J1_FD(1,1) = static_cast<double>((p_s2-p_s).y)/d;
      std::cout << J1 << std::endl;
      std::cout << J1_FD << std::endl;

      v_s = b_s.getVec();
      bearingToPixel(v_s,p_s,J2);
      bearingToPixel(v_s + Eigen::Vector3d(d,0,0),p_s1);
      bearingToPixel(v_s + Eigen::Vector3d(0,d,0),p_s2);
      bearingToPixel(v_s + Eigen::Vector3d(0,0,d),p_s3);
      J2_FD(0,0) = static_cast<double>((p_s1-p_s).x)/d;
      J2_FD(1,0) = static_cast<double>((p_s1-p_s).y)/d;
      J2_FD(0,1) = static_cast<double>((p_s2-p_s).x)/d;
      J2_FD(1,1) = static_cast<double>((p_s2-p_s).y)/d;
      J2_FD(0,2) = static_cast<double>((p_s3-p_s).x)/d;
      J2_FD(1,2) = static_cast<double>((p_s3-p_s).y)/d;
      std::cout << J2 << std::endl;
      std::cout << J2_FD << std::endl;
    }
  }
}
