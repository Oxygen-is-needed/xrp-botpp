#ifndef XRP__HPP
#define XRP__HPP

namespace XRP {

  // Query {{{
  namespace Query {
    bool parse_json(nlohmann::json &json, const std::string &parse,
        const std::string &name) {
      try {
        json = nlohmann::json::parse(parse);
      } catch (const nlohmann::json::parse_error &e) {
        Log::print_error(Log::XRP, "`", name, "`:Json: parse error", e.what());
        return false;
      } catch (...) {
        Log::print_error(Log::XRP, "`", name, "`:Json: unknown parse error");
        return false;
      }

      if (json.empty()) {
        Log::print_error(Log::XRP, "`", name,
            "`:Problem parsing, or retrieving data");
        return false;
      }

      return true;
    }

    bool set_usd() {
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
        return false;
      }

      nlohmann::json json;
      if (parse_json(json, ret, __FUNCTION__) == false) {
        return false;
      }

      float r_usd=0;

      try {
        r_usd = std::stof(
            std::string(json["result"]["lines"][0]["limit"]).c_str()
            );
      } catch (const nlohmann::json::out_of_range& e) {
        Log::print_error(Log::XRP, __LINE__, ": Json: key not found ", e.what());
        return false;
      } catch (const nlohmann::json::type_error& e) {
        Log::print_error(Log::XRP, __LINE__, ": Json: Type mismatch ", e.what());
        return false;
      } catch (...) {
        Log::print_error(Log::XRP, __LINE__, ": Json: Unknown Error");
        return false;
      }

      Bot::Publish::data.r_usd  = r_usd;
      Bot::Publish::data.p_usd  = Bot::Publish::data.p_xrp * r_usd;
      Bot::Publish::data.marketcap_usd  = Bot::Publish::data.marketcap_xrp * r_usd;

      return true;
    } 

    bool set_info() {
      std::string ret =
          Network::Query::query("https://s1.xrplmeta.org/token/" +
                                Conf::id_code + ":" + Conf::address);

      if (ret.empty()) {
        return false;
      }

      nlohmann::json json = nlohmann::json::parse(ret);
      if (json.empty()) {
        Log::print(Log::XRP, "Problem parsing, or retrieving data");
        return false;
      }

      unsigned int holders=0, line=0;
      int marketcap = 0;
      float r_coin=0;

      try {
        holders   = json["metrics"]["holders"];
        line      = json["metrics"]["trustlines"];

        std::string m = json["metrics"]["marketcap"];
        marketcap = std::stof(m.c_str());

        r_coin = std::stof(std::string(json["metrics"]["price"]).c_str());
      } catch (const nlohmann::json::out_of_range& e) {
        Log::print_error(Log::XRP, "Json: key not found ", e.what());
        return false;
      } catch (const nlohmann::json::type_error& e) {
        Log::print_error(Log::XRP, "Json: Type mismatch ", e.what());
        return false;
      }

      Bot::Publish::data.holders = holders;
      Bot::Publish::data.lines = line;
      Bot::Publish::data.r_coin = r_coin;
      Bot::Publish::data.marketcap_xrp = marketcap;
      //Bot::Publish::data.marketcap_coin = 0;
      Bot::Publish::data.p_xrp = Bot::Publish::data.p_coin * r_coin;

      return true;
    }

    // TODO: move, and generalize.
    std::tm string2tm(const std::string& s) {
      std::tm time = {};
      std::istringstream ss(s);
      ss.imbue(std::locale("en_US.utf-8"));
      ss >> std::get_time(&time, "%Y-%m-%dT%H:%M:%S");
      return time;
    }

    bool tmNewer(const std::tm& tm1, const std::tm& tm2) {
      // Convert std::tm to time_t
      time_t time1 = mktime(const_cast<std::tm*>(&tm1));
      time_t time2 = mktime(const_cast<std::tm*>(&tm2));

      // Compare the time_t values
      return difftime(time1, time2) > 0; // Returns true if tm1 is newer than tm2
    }
    //bool tmNewer(std::tm n, std::tm o) {
    //  if (n.tm_year < o.tm_year)
    //    return false;
    //  if (n.tm_yday < o.tm_yday)
    //    return false;
    //  if (n.tm_hour < o.tm_hour)
    //    return false;
    //  if (n.tm_min < o.tm_min)
    //    return false;
    //  if (n.tm_sec <= o.tm_sec)
    //    return false;
    //  return false;
    //}

    std::tm last_time;
    Publish::Data_Type get_update() {
      std::string ret = Network::Query::query(
          "https://api.xrpscan.com/api/v1/account/" + Conf::address +
          "/transactions?origin=xrp-transaction-tracker");

      if (ret.empty()) {
        Log::print_error(Log::XRP, "Failed to get data");
        return Publish::NONE;
      }

      nlohmann::json json;
      if (parse_json(json, ret, __FUNCTION__) == false) {
        return Publish::NONE;
      }


      Publish::Data_Type u = Publish::NONE;
      std::string address = "";
      float value = 0;
      std::string tx_hash = "";
      try {

        int tlen = json["transactions"].size();;
        for (int x=0; x<tlen-1; x++) {
          if (json["transactions"][x]["TransactionType"] != "Payment")
            continue;

          if (json["transactions"][x]["meta"]["delivered_amount"]["currency"] !=
              Conf::id_code)
            continue;


          std::string t = json["transactions"][x]["date"];
          std::tm time = string2tm(t);
          if (tmNewer(time, last_time) == false) {
            continue;
          }


          address = json["transactions"][x]["Account"];

          value = std::stof(
              std::string(
                  json["transactions"][x]["meta"]["delivered_amount"]["value"])
                  .c_str());
          tx_hash = json["transactions"][x]["hash"];


          last_time = time;
          u = Publish::PURCHASE;
          break;
        }
      } catch (const nlohmann::json::out_of_range& e) {
        Log::print_error(Log::XRP, __LINE__, ": Json: key not found ", e.what());
        return Publish::NONE;
      } catch (const nlohmann::json::type_error& e) {
        Log::print_error(Log::XRP, __LINE__, ": Json: Type mismatch ", e.what());
        return Publish::NONE;
      } catch (...) {
        Log::print_error(Log::XRP, __LINE__, ": Json: Unknown Error");
        return Publish::NONE;
      }

      if (u == Publish::NONE) {
        return u;
      }
      Bot::Publish::data = {};

      Bot::Publish::data.tx_hash = tx_hash;
      Bot::Publish::data.time = last_time;
      Bot::Publish::data.buy_address = address;
      Bot::Publish::data.p_coin = value;

      (void) set_info();
      (void) set_usd();

      return u;
    }
  }
  // }}}

  namespace Update {
    Publish::Data_Type need_update() {
      Log::print(Log::XRP, "Checking for update");
      //Publish::Data_Type u = Query::test();
      Publish::Data_Type u = Query::get_update();

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
