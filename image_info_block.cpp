#include <rx_remote_block.h>

using namespace simex;

class image_info_block : public rx_remote_block {
 public:
  explicit image_info_block(SimStruct* s) : rx_remote_block(s) {
    param_sample_time = register_scalar_parameter<double>("sample_time", false);
    param_num_input = register_scalar_parameter<int>("num_input", false);
    param_num_output = register_scalar_parameter<int>("num_output", false);
  }

 public:
  void on_parameter_updated() override {
    rx_remote_block::on_parameter_updated();
    sample_time.sample_time = param_sample_time->data;
    sample_time.offset_time = 0;

    for (int i = 0; i < param_num_input->data; ++i) {
      auto ptr = register_input_port<double>(fmt::format("input{}", i), {1},
                                             false, false, false);
      set_remote_accessible_port(ptr);
    }

    for (int i = 0; i < param_num_output->data; ++i) {
      auto ptr = register_output_port<double>(fmt::format("output{}", i), {1},
                                              false, false);
      set_remote_accessible_port(ptr);
    }
  }
  void on_start() override {
    rx_remote_block::on_start();
    session_disconnected.get_observable().subscribe(subscriptions, [&](auto next) {
      if (sessions.size() == 0) {
        auto worker = rl->get_scheduler().create_worker(subscriptions);
        auto stopper = rxcpp::schedulers::make_schedulable(worker, [](rxcpp::schedulers::schedulable self){
          throw std::runtime_error("No client is connected. stop.");
        });
        worker.schedule(stopper);
      }
    });

    // wait for connection
    int retries = 10;
    int delay = 1;
    for (int retry = 0; retry < retries; ++retry) {
      if (!sessions.empty()) {
        log("info", "Client connected. Start working.", true);
        return;
      }
      log("info", fmt::format("Waiting for connection... {}", retry), true);
      std::this_thread::sleep_for(std::chrono::seconds(delay));
    }
    // should not come to here
    throw std::runtime_error("No client connected.");
  }

 private:
  std::shared_ptr<rx_scalar_param<double>> param_sample_time;
  std::shared_ptr<rx_scalar_param<int>> param_num_input;
  std::shared_ptr<rx_scalar_param<int>> param_num_output;
};

std::shared_ptr<rx_block> rx_block::create_block(SimStruct* S) {
  return std::make_shared<image_info_block>(S);
}