#include <string>
#include <algorithm>
#include <map>
class argshandler {
public:
    argshandler(std::string argprefix, bool handlehelp) {
        ArgPrefix_ = argprefix;
        HandleHelp_ = handlehelp;
        if (HandleHelp_)
            AddKnownArg("h", "Shows this help");
    }

    // this is for auto help handling
    void AddKnownArg(std::string name, std::string description) {
        KnownArgs_[name] = description;
    }

    void HandleArgs(int argc, char* argv[]) {
        std::string lastarg;
        for (int i = 0; i < argc; i++) {
            std::string arg(argv[i]);
            if (arg.substr(0, ArgPrefix_.size()) == ArgPrefix_) {
                lastarg = arg;
                ErasePrefix(arg);
                if (HandleHelp_ && arg == "h") {
                    DoHelp();
                } else {
                    ReceivedArgs_.insert(std::pair<std::string, std::string>(arg, ""));
                }
            } else {
                if (lastarg.find(ArgPrefix_) != std::string::npos) {
                    ErasePrefix(lastarg);
                    ReceivedArgs_[lastarg] = arg;
                }
                lastarg = arg;
            }
        }
    }

    std::string GetArg(std::string key) {
        auto entry = ReceivedArgs_.find(key);

        if (entry != ReceivedArgs_.end()) {
            return entry->second;
        } else {
            return "";
        }
    }
private:
    std::string ArgPrefix_;
    bool HandleHelp_;

    std::map<std::string, std::string> KnownArgs_;
    std::map<std::string, std::string> ReceivedArgs_;

    void DoHelp() {
        for (auto&& entry : KnownArgs_) {
            std::cout << ArgPrefix_ << entry.first << ": " << entry.second << "\n";
        }
    }

    void ErasePrefix(std::string& EraseFrom) {
        EraseFrom.erase(0, ArgPrefix_.size());
    }
};

