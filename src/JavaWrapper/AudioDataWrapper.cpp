#include "JavaWrapper/WrapperMaster.hpp"
#include "Audio/AudioData.hpp"

LAZY_WRAPPER("com.example.portaible.AudioData", AudioData)


// Alternative:
/*

using namespace portaible;
using namespace portaible::JavaWrapper;

namespace portaible
{
    class AudioDataWrapper : public Wrapper<AudioData>
    {
        DECLARE_WRAPPER(AudioData)
    };
}

REGISTER_WRAPPER("com.example.portaible.AudioData", portaible::AudioData, AudioDataWrapper)

*/