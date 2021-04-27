#include <functional>
#include <map>
#include <iostream>

namespace event {
    class handler {
    public:
        void HandleEvent(std::string event, std::string raw) {
			auto handler = events_.find(event);
			
			if (handler != events_.end()) {
				std::invoke(handler->second, event, raw);
			} else {
				std::cout << "Unkown event occured: " << event << "\n";
			}				
        }

        using event_handler_t = std::function<void(std::string, std::string)>;
        void AddEventHandler(std::string eventname, event_handler_t handler) {
			events_[eventname] = handler;
        }
    private:
        std::map<std::string, event_handler_t> events_;
    };
}
