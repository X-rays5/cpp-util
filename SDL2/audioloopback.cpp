#include <cstdlib>
#include <exception>
#include <vector>
#include <string>
#include <iostream>
#include <SDL.h>

#define VOLUME_MULTIPLIER 5.f

class AudioLoopBack {
public:
  AudioLoopBack() {
    InitIn();
    InitOut();

    SDL_PauseAudioDevice(audio_out_, false);
    SDL_PauseAudioDevice(audio_in_, false);
  }

  ~AudioLoopBack() {
    SDL_CloseAudioDevice(audio_in_);
    SDL_CloseAudioDevice(audio_out_);
  }

private:
  constexpr static const std::int32_t sampling_rate_ = 48000;
  constexpr static const std::int32_t channels_ = 2;
  constexpr static const std::int32_t frame_size_ = 960;
  constexpr static const std::int32_t samples_ = 8192;
  constexpr static const SDL_AudioFormat format_ = AUDIO_F32SYS;
  SDL_AudioDeviceID audio_in_ = NULL;
  SDL_AudioDeviceID audio_out_ = NULL;

private:
  void InitIn() {
    SDL_AudioSpec wanted_spec;
    SDL_zero(wanted_spec);

    wanted_spec.freq = sampling_rate_;
    wanted_spec.format = format_;
    wanted_spec.channels = channels_;
    wanted_spec.samples = samples_;
    wanted_spec.callback = AudioInCallback;
    wanted_spec.userdata = this;

    audio_in_ = SDL_OpenAudioDevice(nullptr, true, &wanted_spec, &wanted_spec, SDL_AUDIO_ALLOW_ANY_CHANGE);
    if (!audio_in_) {
      throw std::runtime_error(std::string("Failed to open audio in ")+SDL_GetError());
    }
  }

  void InitOut() {
    SDL_AudioSpec wanted_spec;
    SDL_zero(wanted_spec);

    wanted_spec.freq = sampling_rate_;
    wanted_spec.format = format_;
    wanted_spec.channels = channels_;
    wanted_spec.samples = samples_;

    audio_out_ = SDL_OpenAudioDevice(nullptr, false, &wanted_spec, &wanted_spec, false);
    if (!audio_out_) {
      throw std::runtime_error(std::string("Failed to open audio out ")+SDL_GetError());
    }
  }

  static void AudioInCallback(void* data, Uint8* input, int len) {
    auto that = static_cast<AudioLoopBack*>(data);

    Uint8 buffer[AudioLoopBack::samples_];
    auto samples = reinterpret_cast<float*>(&buffer);
    memcpy(buffer, input, len);
    // amplify the input
    for (std::uint32_t i = 0; i < frame_size_ * channels_; i++) {
      samples[i] *= VOLUME_MULTIPLIER;
    }

    SDL_QueueAudio(that->audio_out_, buffer, len);
  }
};

inline void Shutdown() {
  SDL_Quit();
}

int main(int argc, char** argv) {
  atexit(Shutdown);
  at_quick_exit(Shutdown);
  std::set_terminate([]{
    Shutdown();
    std::abort();
  });

  if (SDL_Init(SDL_INIT_AUDIO) != 0) {
    return 1;
  }

  SDL_Log("Using audio driver: %s", SDL_GetCurrentAudioDriver());

  AudioLoopBack audio_loop_back;

  while (true) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT) {
        exit(EXIT_SUCCESS);
      } else if (e.type == SDL_KEYDOWN) {
        if (e.key.keysym.sym == SDLK_ESCAPE) {
          exit(EXIT_SUCCESS);
        }
      }
    }
    SDL_Delay(100); // avoid high cpu usage
  }

  return 0;
}