/* ----------------------------------------------------------------------------
 * Copyright 2017, Massachusetts Institute of Technology,
 * Cambridge, MA 02139
 * All Rights Reserved
 * Authors: Luca Carlone, et al. (see THANKS for the full author list)
 * See LICENSE for the license information
 * -------------------------------------------------------------------------- */

/**
 * @file   DisplayModule.cpp
 * @brief  Pipeline module to render/display 2D/3D visualization.
 * @author Antoni Rosinol
 */

#pragma once

#include <opencv2/viz.hpp>

#include "kimera-vio/utils/ThreadsafeQueue.h"
#include "kimera-vio/visualizer/Visualizer3D-definitions.h"

namespace VIO {

/**
 * @brief The DisplayType enum: enumerates the types of supported renderers.
 */
enum class DisplayType { kOpenCV = 0 };

/**
 * @brief The WindowData struct: Contains internal data for Visualizer3D window.
 */
struct WindowData {
 public:
  WindowData();
  ~WindowData();

 public:
  //! 3D visualization
  cv::viz::Viz3d window_;

  //! Colors
  cv::viz::Color background_color_;

  //! Stores the user set mesh representation.
  //! These objects are further modified by callbacks in the display module
  //! And are as well read by the visualizer module.
  int mesh_representation_;
  int mesh_shading_;
  bool mesh_ambient_;
  bool mesh_lighting_;
};

/**
 * @brief displayImage helper function to push images to a given display queue
 * @param title Title of the image that will be displayed
 * @param img Actual image to be displayed
 * @param display_queue Threadsafe queue were images will be pushed and that
 * needs to be consumed by a display thread.
 */
inline void displayImage(const std::string& title,
                         const cv::Mat& img,
                         DisplayQueue* display_queue) {
  VisualizerOutput::UniquePtr visualizer_output =
      VIO::make_unique<VisualizerOutput>();
  ImageToDisplay img_to_display(title, img);
  visualizer_output->images_to_display_.push_back(img_to_display);
  visualizer_output->visualization_type_ = VisualizationType::kNone;
  if (display_queue) {
    display_queue->push(std::move(visualizer_output));
  } else {
    LOG(ERROR) << "Requested display image, but display_queue is "
                  "not valid.";
  }
}

}  // namespace VIO
