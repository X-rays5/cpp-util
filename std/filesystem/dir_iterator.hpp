#include <iostream>
#include <filesystem>
void ls() {
	std::filesystem::directory_iterator dirIt{ std::filesystem::current_path() };
    for (auto&& dirEntry : dirIt)
    {
        auto path = dirEntry.path();
        if (path.has_filename())
        {
            if (dirEntry.is_regular_file())
                std::cout << path.filename().string() << " ";
            if (dirEntry.is_directory())
                std::cout << "/" << path.filename().string() << " ";
        }
    }
}