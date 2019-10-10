#include <rx_remote_block.h>

using namespace simex;

class image_info_block: public rx_remote_block {
 public:
  explicit image_info_block(SimStruct *s) : rx_remote_block(s) {
    param_sample_time = register_scalar_parameter<double>("sample_time", false);
    out_image_median = register_output_port<double>("image_median", {1}, false, false);

    set_remote_accessible_port(out_image_median);
  }

 public:
  void on_parameter_updated() override {
    rx_remote_block::on_parameter_updated();
    sample_time.sample_time = param_sample_time->data;
    sample_time.offset_time = 0;
  }
 private:
  std::shared_ptr<rx_scalar_param<double>> param_sample_time;
  std::shared_ptr<simex::port<double>> out_image_median;
};

std::shared_ptr<rx_block> rx_block::create_block(SimStruct* S) {
  return std::make_shared<image_info_block>(S);
}