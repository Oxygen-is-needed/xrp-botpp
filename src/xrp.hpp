#ifndef XRP__HPP
#define XRP__HPP

namespace XRP {

  // Query {{{
  namespace Query {
    bool get_usd(float& r_usd, const int& marketcap) {

      // Query to Orical Xrp Account, for Conversion to USD.
      std::string ret = Network::Query::query("https://xrpl.ws",
          (nlohmann::json){
          {"method", "account_lines"},
          {"params", {
              {
                {"account", "rXUMMaPpZqPutoRszR29jtC8amWq3APkx"}
              }
          }} }.dump());


      if (ret.empty()) {
        Log::print_error(Log::XRP, "Failed to fetch prices");
        return true;
      }

      nlohmann::json json = nlohmann::json::parse(ret);
      if (json.empty()) {
        Log::print_error(Log::XRP, "Problem parsing, or retrieving price data");
        return true;
      }

      std::string hold = "";

      try {
        hold = json["result"]["lines"][0]["limit"];
      } catch (const nlohmann::json::out_of_range& e) {
        Log::print_error(Log::XRP, "Json: key not found ", e.what());
        return true;
      } catch (const nlohmann::json::type_error& e) {
        Log::print_error(Log::XRP, "Json: Type mismatch ", e.what());
        return true;
      } 

      r_usd = std::stof(hold);

      Bot::Publish::data.r_usd  = r_usd;

      Bot::Publish::data.marketcap_xrp  = marketcap;
      Bot::Publish::data.marketcap_usd  = marketcap * r_usd;


      return false;
    } 

    int latest_date = 0;
    Publish::Data_Type test() {
      std::string ret =
          Network::Query::query("https://s1.xrplmeta.org/token/" +
                                Conf::id_code + ":" + Conf::address);
      if (ret.empty()) {
        return Publish::NONE;
      }

      nlohmann::json json = nlohmann::json::parse(ret);
      if (json.empty()) {
        Log::print(Log::XRP, "Problem parsing, or retrieving data");
        return Publish::NONE;
      }

      unsigned int holders=0, line=0;
      int marketcap = 0;
      float r_usd=0, r_coin=0;

      try {
        holders   = json["metrics"]["holders"];
        line      = json["metrics"]["trustlines"];

        std::string m = json["metrics"]["marketcap"];
        marketcap = atof(m.c_str());

        std::string c = json["metrics"]["price"];
        r_coin    = atof(c.c_str());
      } catch (const nlohmann::json::out_of_range& e) {
        Log::print_error(Log::XRP, "Json: key not found ", e.what());
        return Publish::NONE;
      } catch (const nlohmann::json::type_error& e) {
        Log::print_error(Log::XRP, "Json: Type mismatch ", e.what());
        return Publish::NONE;
      } 

      if (holders == Bot::Publish::data.holders) {
        return Publish::NONE;
      }


      (void) get_usd(r_usd, marketcap);


      Bot::Publish::data.holders    = holders;
      Bot::Publish::data.lines      = line;
      Bot::Publish::data.r_coin     = r_coin;

      std::cerr << Bot::Publish::data.holders << std::endl;
      std::cerr << Bot::Publish::data.lines   << std::endl;
      std::cerr << Bot::Publish::data.marketcap_coin   << std::endl;
      std::cerr << Bot::Publish::data.marketcap_xrp   << std::endl;
      std::cerr << Bot::Publish::data.marketcap_usd   << std::endl;
      std::cerr << Bot::Publish::data.r_usd   << std::endl;
      std::cerr << Bot::Publish::data.r_coin  << std::endl;


      return Publish::PURCHASE;
    }
  }
  // }}}

  namespace Update {
    Publish::Data_Type need_update(void) {
      Log::print(Log::XRP, "Checking for update");
      Publish::Data_Type u = Query::test();

      if (u != Publish::NONE) {
        Log::print(Log::XRP, "Update Found");
      }

      return u;
    }
  }

	/**
	 * update() - update publish data if needed.
   *
   * Querys XRP Ledger to see if an update is needed.
   * If an update is needed then updates information, and requests any more
   * information if needed.
   *
	 * return: return if update has occured.
	 */
	bool update(void) {
    // Check update criteria
    Publish::Data_Type u = Update::need_update();


    if (u != Publish::NONE) {
      return true;
    }
		return false;
	}
}

#endif
// vim: tabstop=2 shiftwidth=2 softtabstop=2 expandtab
