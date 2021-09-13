#include <functional>
#include <unordered_map>
#include <iostream>

namespace event {
    class handler {
    public:
        void HandleEvent(std::string event, std::string raw) {
            auto result = events_.equal_range(event);

            unsigned int handlerC = 0;
            for (auto it = result.first; it != result.second; it++) {
                handlerC++;

                std::cout << "Handling event: " << event << "\n";
                std::invoke(it->second, event, raw);
            }

            if (handlerC < 1) {
                std::cout << "Unkown event occured: " << event << "\n";
            }
        }

        using event_handler_t = std::function<void(std::string, std::string)>;
        void AddEventHandler(std::string eventname, event_handler_t handler) {
            events_.insert(std::pair<std::string, event_handler_t>(std::move(eventname), std::move(handler)));
        }
    private:
        std::unordered_multimap<std::string, event_handler_t> events_;
    };
}
