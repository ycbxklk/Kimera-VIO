/* ----------------------------------------------------------------------------
 * Copyright 2017, Massachusetts Institute of Technology,
 * Cambridge, MA 02139
 * All Rights Reserved
 * Authors: Luca Carlone, et al. (see THANKS for the full author list)
 * See LICENSE for the license information
 * -------------------------------------------------------------------------- */

/**
 * @file   Visualizer3D-definitions.h
 * @brief  Class definitions for 3D visualizer.
 * @author Antoni Rosinol
 */

#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <string>

#include <opencv2/viz/widgets.hpp>

#include "kimera-vio/utils/ThreadsafeQueue.h"
#include "kimera-vio/common/vio_types.h"
#include "kimera-vio/pipeline/PipelinePayload.h"
#include "kimera-vio/frontend/StereoVisionFrontEnd-definitions.h"
#include "kimera-vio/backend/VioBackEnd-definitions.h"
#include "kimera-vio/mesh/Mesher-definitions.h"
#include "kimera-vio/utils/Macros.h"

namespace VIO {

enum class VisualizerType {
  //! OpenCV 3D viz, uses VTK underneath the hood.
  OpenCV = 0u
};

enum class VisualizationType {
  kMesh2dTo3dSparse = 0,  // same as MESH2DTo3D but filters out triangles
                          // corresponding to non planar obstacles
  kPointcloud = 1,        // visualize 3D VIO points  (no repeated point)
  kNone = 2               // does not visualize map
};

typedef std::unique_ptr<cv::viz::Widget3D> WidgetPtr;
typedef std::unordered_map<std::string, WidgetPtr> WidgetsMap;

struct ImageToDisplay {
  ImageToDisplay() = default;
  ImageToDisplay(const std::string& name, const cv::Mat& image)
      : name_(name), image_(image) {}

  std::string name_;
  cv::Mat image_;
};

struct VisualizerInput : public PipelinePayload {
  KIMERA_POINTER_TYPEDEFS(VisualizerInput);
  KIMERA_DELETE_COPY_CONSTRUCTORS(VisualizerInput);
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
  VisualizerInput(const Timestamp& timestamp,
                  const MesherOutput::Ptr& mesher_output,
                  const BackendOutput::Ptr& backend_output,
                  const FrontendOutput::Ptr& frontend_output)
      : PipelinePayload(timestamp),
        mesher_output_(mesher_output),
        backend_output_(backend_output),
        frontend_output_(frontend_output) {
    CHECK(backend_output);
    CHECK(frontend_output);
    if (mesher_output) CHECK_EQ(timestamp, mesher_output->timestamp_);
    CHECK_EQ(timestamp, frontend_output->timestamp_);
    CHECK_EQ(timestamp, backend_output->timestamp_);
  }
  virtual ~VisualizerInput() = default;

  // Copy the pointers so that we do not need to copy the data.
  const MesherOutput::ConstPtr mesher_output_;
  const BackendOutput::ConstPtr backend_output_;
  const FrontendOutput::ConstPtr frontend_output_;
};

struct VisualizerOutput {
  KIMERA_POINTER_TYPEDEFS(VisualizerOutput);
  KIMERA_DELETE_COPY_CONSTRUCTORS(VisualizerOutput);
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
  VisualizerOutput()
      : visualization_type_(VisualizationType::kNone),
        images_to_display_(),
        widgets_(),
        frustum_pose_(cv::Affine3d::Identity()) {}
  ~VisualizerOutput() = default;

  VisualizationType visualization_type_;
  std::vector<ImageToDisplay> images_to_display_;
  WidgetsMap widgets_;
  cv::Affine3d frustum_pose_;
};
typedef ThreadsafeQueue<VisualizerOutput::UniquePtr> DisplayQueue;

}  // namespace VIO
