#ifndef PUBLISH__HPP
#define PUBLISH__HPP

// New 'BUY' Type:
// ===============
//
// <image>
//
// <emoji new> New Buy of <coin>
//
// <amount in link> xrp | usd | coin
// <buyer link>Buyer
// <Dexscreener url>Chart
// 
// Market Cap: <market cap to cents>
// Holders: <holders>
// Trustlines: <lines>
//

namespace Publish {
  enum Data_Type {
    NONE,     ///< Nothing
    PURCHASE, ///< Data holds information about purchase
  };
  struct Publish_Data {
    enum Data_Type type = NONE; ///< The type of Data Stored.

    float r_usd   = 0;  ///< Ratio of xrp to usd.
    float r_coin  = 0;  ///< Ratio of coin to xrp.

    float p_coin  = 0;  ///< Amount of coin transaction.
    float p_xrp   = 0;  ///< Cost of coin transaction in xrp.
    float p_usd   = 0;  ///< Cost of coin transaction in usd.

    std::string buy_address = ""; ///< The transacting address.
    float buy_coin = 0; ///< Amount of coins the wallet holds.
    float buy_xrp = 0;  ///< Amount of xrp total the wallet holds. 
    float buy_usd = 0;  ///< The Cost of all xrp in usd.

    int marketcap_xrp   = 0;  ///< The coin's market cap in xrp.
    int marketcap_usd   = 0;  ///< The coin's market cap in usd.
    int marketcap_coin  = 0;  ///< The coin's market cap in coin.

    unsigned int holders  = 0;  ///< Amount of users holding the coin.
    unsigned int lines    = 0;  ///< Amount of trustlines the coin has.
  } data;

  std::string method_save_file_base = "";

#include "telegram.hpp"

  struct Methods_Config {
    const std::string name;
    void (*const push)(Publish_Data&);
    void (*const poll)(void);
    bool (*const load)(const std::string);
    void (*const save)(const std::string);
    bool (*const check_key)(const std::string&);
    bool state = false;
  };


  Methods_Config method_config[METHOD_LENGTH] = {
#define X(a,b,c,d,e,f,g,...)  {b,c,d,e,f,g,false},
    METHODS
#undef X
  };



  void push() {
    for (auto a : method_config) {
      if (a.state == false) continue;

      a.push(data);
    }
  }

  void poll(void) {
    for (auto a : method_config) {
      if (a.state == false) continue;

      a.poll();
    }
  }

  void load_data() {
    for (int x=0; x<METHOD_LENGTH; x++) {
      if (method_config[x].state == false)
        continue;

      if (method_config[x].load(Conf::method_save_files[x]) == false) {
        Log::print(Log::SYSTEM, method_config[x].name + " failed to load file");
      }
    }
  }

  void save_data() {
    for (int x=0; x<METHOD_LENGTH; x++) {
      if (method_config[x].state == false)
        continue;

      method_config[x].save(Conf::method_save_files[x]);
    }
  }

}

#endif
// vim: tabstop=2 shiftwidth=2 softtabstop=2 expandtab
