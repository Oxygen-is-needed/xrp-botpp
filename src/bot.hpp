#ifndef BOT__HPP
#define BOT__HPP

namespace Bot {
#include "publish.hpp"
#include "xrp.hpp"

  void set() {}
  void set(bool state[METHOD_LENGTH]) {
    for (int x=0; x<METHOD_LENGTH; x++) {
      if (state[x] == true)
        Publish::method_config[x].state = true;
    }
  }

  void load() {
    // NOTE: Publish uses this as the first part of the file name.
    Publish::load_data();
  }

  void save() {
    // NOTE: Publish uses this as the first part of the file name.
    Publish::save_data();
  }

	void run() {
    do {
      bool u = false;
      try {
        u = XRP::update();
      } catch (std::exception& e) {
        Log::print_error(Log::XRP, "Retreiving update failed: ", e.what());
      } catch (...) {
        Log::print_error(Log::XRP, "Retreiving update failed");
      }

      Publish::poll();

      sleep(Conf::request_wait_length);
      if (u == false)
        continue;

      Publish::push();

    } while(true);
	}
}

#endif
// vim: tabstop=2 shiftwidth=2 softtabstop=2 expandtab
