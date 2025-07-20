#ifndef TELEGRAM__HPP
#define TELEGRAM__HPP

namespace Telegram {
  // Settings {{{
  namespace Settings {
#define BOT_COMMANDS  \
    X("start",  Poll::startMsg, "start sending info")  \
    X("stop",   Poll::stopMsg,  "stop sending all info")  \
    X("end",    Poll::endMsg,   "remove all info about chat")  \
    X("leave",  Poll::leaveMsg, "remove all info and leave the group")  \
    X("help",   Poll::helpMsg,  "print this help text")

    const std::string bot_commands_help = {
      "BOT commands:\n"
#define X(a,b,c)  "/" a "\t- " c "\n"
      BOT_COMMANDS
#undef X
    };

    const std::string welcome_msg = "Welcome to BOT\nYou are now added to the list";

    const std::string start_msg         = "Starting Bot";
    const std::string start_msg_already = "Bot has already been started";

    bool have_stop_msg = true;
    const std::string stop_msg  = "Ended";
    const std::string end_msg   = "Ending Chat";
    const std::string leave_msg = "Leaving Chat";

    bool disable_msg_notifications = true;
  }
  // }}}

	struct Message {
    bool msg    = false;  ///< Allowed to Message
		long int id = 0;      ///< Message ID
	};
	std::vector<Message> msgs;

  std::unique_ptr<TgBot::Bot> bot;
  std::unique_ptr<TgBot::TgLongPoll> long_poll = nullptr;
  bool started = false;
  bool loaded = false;

  // Save/Load {{{
  bool load(const std::string file_name) {
    loaded = true;

    std::ifstream file(file_name);
    if (!file) {
      return false;
    }

    std::string line;
    while(std::getline(file, line)) {
      if (line.empty()) continue;

      struct Message buf;

      size_t len = line.size()-1;
      if (line[len-1] != '\t') {
        Log::print(Log::TELEGRAM, "log file failed parsing syntax checks.");
        file.close();
        return false;
      }
      char c = line[len];
      c -= '0';
      if (c == true) buf.msg = true;
      else if (c == false) buf.msg = false;
      line[len] = 0;

      buf.id = std::stoul(line);
      msgs.push_back(buf);
    }
    file.close();

    return true;
  }
  
  void save(const std::string file_name) {
    if (loaded == false) {
      Log::print(Log::TELEGRAM, "failed to save, never loaded from file.");
      return;
    }

    std::ofstream file(file_name);
    if (!file) {
      Log::print(Log::TELEGRAM, "failed to open file '", file_name, "' to save data.");
      file.close();
      exit(1);
    }

    for (auto a : msgs) {
      file << a.id << '\t' << a.msg << '\n';
    }

    file.close();
  }
  // }}}

  // Utils {{{
  void sendMessage(std::int64_t id, const std::string text) {
    // NOTE: this returns a pointer to the message sent.
    bot->getApi().sendMessage(
        id, text,
        nullptr, nullptr, nullptr, "",
        Settings::disable_msg_notifications);
  }
  // }}}
  
  // Polling {{{
  namespace {
    ssize_t findMsg(long int id) {
      for (unsigned int x = 0; x < (unsigned int)msgs.size(); x++) {
        if (id == msgs[x].id) {
          return x; 
        }
      }
      return -1;
    }

    void send2MI(ssize_t mi, const std::string text) {
      if (mi == -1) return; // NOTE: by default do not message
      if (msgs[mi].msg == false)
        return;

      sendMessage(msgs[mi].id, text);
    }
  }

  namespace Poll {
    namespace {
      void cleanChat(ssize_t mi) {
        if (mi == -1) return;
        msgs.erase(msgs.begin() + mi);
      }

      // NOTE: the leaveChat() function returns a potential fail state which
      // this function does not pay atention to.
      void leaveChat(long int id) {
        enum TgBot::Chat::Type ct = bot->getApi().getChat(id)->type;
        if (ct == TgBot::Chat::Type::Private) {
          return;
        }
        bool ret = bot->getApi().leaveChat(id);
        if (ret == false) {
          Log::print(Log::TELEGRAM, "failed to leave channel");
        }
      }
    }

    void startMsg(TgBot::Message::Ptr msg) {
      long int id = msg->chat->id;
      ssize_t mi = findMsg(id);

      if (mi == -1) {
        msgs.push_back({true, id});
        save(method_save_file_base);
        sendMessage(id, Settings::welcome_msg);
      } else {
        if (msgs[mi].msg == true) {
          sendMessage(id, Settings::start_msg_already);
          return;
        }
        msgs[mi].msg = true;
      }
      
      sendMessage(id, Settings::start_msg);
    }

    void stopMsg(TgBot::Message::Ptr msg) {
      ssize_t mi = findMsg(msg->chat->id);

      if (mi == -1) {
        return;
      }

      if (msgs[mi].msg == false)
        return;

      if (Settings::have_stop_msg == true) {
        sendMessage(msg->chat->id, Settings::stop_msg);
      }
      msgs[mi].msg = false;
    }

    void endMsg(TgBot::Message::Ptr msg) {
      ssize_t mi = findMsg(msg->chat->id);

      send2MI(mi, Settings::end_msg);
      cleanChat(mi);
    }

    void leaveMsg(TgBot::Message::Ptr msg) {
      long int id = msg->chat->id;
      ssize_t mi = findMsg(id);
      cleanChat(mi);
      send2MI(mi, Settings::leave_msg);
      leaveChat(id);
    }

    void helpMsg(TgBot::Message::Ptr msg) {
      long int id = msg->chat->id;
      ssize_t mi = findMsg(id);

      if (mi == -1) {
        Log::print(Log::TELEGRAM, "an unregistered local id tried to print help.");
        return;
      }
      if (msgs[mi].msg == false)
        return;

      sendMessage(id, Settings::bot_commands_help);
    }

  }
  // }}}


  void start(void) {
    Log::print(Log::TELEGRAM, "Starting Bot");
#define X(a,b,c)  bot->getEvents().onCommand(a, b);
    BOT_COMMANDS
#undef X
    started = true;
  }


  void poll(void) {
    if (started == false) start();

    if (long_poll == nullptr) {
      Log::print(Log::TELEGRAM, "Was unable to initalize long_poll");
      exit(1);
    }
    long_poll->start();

    return;
  }

  // Push {{{
  namespace Push {
    void message(Message& m, const std::string& msg) {
      const std::string photo = "img/header.jpg";
      const std::string mime = "image/jpeg";

      if (Utils::file_exists(photo.c_str()) == false) {
        Log::print_error(Log::TELEGRAM, "photo file does not exist");
        exit(1);
      }

      bot->getApi().sendPhoto(m.id,
          TgBot::InputFile::fromFile(photo, mime),
          msg,
          nullptr, nullptr, "Markdown",
          Settings::disable_msg_notifications);
    }

    std::string format(Publish_Data& pd) {
      std::string buyer = fmt::format("https://xrpscan.com/account/{}",
          pd.buy_address); 
      std::string chart = "https://dexscreener.com/xrpl/FML.rw4tietmzbPG2G66UudSGaQ5uYztNow3gQ_xrp";

      std::string out = fmt::format(
          "💵 New Buy of FML\n"
          "\n"
          "USD ${2:.2f} | XRP {1:.2f} | FML {0:.2f}\n"
          "[Buyer]({3}): ${8}\n"
          "[Chart]({4})\n"
          "\n"
          "Market Cap: ${5:} | XRP {9}\n"
          "Hold: {6} | Trust: {7}\n",

          pd.p_coin, pd.p_xrp, pd.p_usd,
          buyer,
          chart,
          Utils::formatWithCommas(pd.marketcap_usd),
          pd.holders,
          pd.lines,
          pd.buy_usd,
          Utils::formatWithCommas(pd.marketcap_xrp)
        );
      return out;
    }
  }
  // }}}

	void push(Publish_Data& pd) {
    if (started == false)
      start();

		if (msgs.size() <= 0) {
			return;
		}

    std::string msg = Push::format(pd);

		for (auto m : msgs) {
      if (m.msg == true)
        Push::message(m, msg);
		}
	}

  std::string bot_username = "";

  bool check_key(const std::string& key) {
    std::string ret = Network::Query::query("https://api.telegram.org/bot" + key + "/getMe");

    if (ret.empty()) {
      Log::print_error(Log::TELEGRAM, "Failed to fetch telegram api");
      return false;
    }

    nlohmann::json json = nlohmann::json::parse(ret);
    if (json.empty()) {
      Log::print_error(Log::TELEGRAM, "Problem parsing, or retrieving, from telegram api");
      return false;
    }

    try {
      if (json["ok"] != true) {
        return false;
      }

      if (json["result"]["is_bot"] != true) {
        Log::print(Log::TELEGRAM, "Provided API key is not for a bot");
        return false;
      }
      bot_username = json["result"]["username"];
    } catch (const nlohmann::json::out_of_range& e) {
      Log::print_error(Log::TELEGRAM, "Json: key not found ", e.what());
      return false;
    } catch (const nlohmann::json::type_error& e) {
      Log::print_error(Log::TELEGRAM, "Json: Type mismatch ", e.what());
      return false;
    } 

    bot = std::make_unique<TgBot::Bot>(key);
    long_poll = std::make_unique<TgBot::TgLongPoll>(*bot);
    return true;
  }

#undef BOT_COMMANDS
}

#endif
// vim: tabstop=2 shiftwidth=2 softtabstop=2 expandtab
