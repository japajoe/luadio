#ifndef LUADIO_AUDIO_RECORDER_HPP
#define LUADIO_AUDIO_RECORDER_HPP

#include <vector>
#include <string>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <mutex>

namespace luadio
{
	class audio_recorder
	{
	public:
		audio_recorder();
		~audio_recorder();
		void start();
		void stop();
		void on_process(const float* pFrames, uint32_t frameCount, uint32_t channels);
		bool is_recording() const;
	private:
        enum recorder_state
        {
            recorder_state_idle,
            recorder_state_write_header,
            recorder_state_write_data
        };

        std::string currentFileName;
        std::vector<uint8_t> outputBuffer;
        uint64_t bytesWritten;
        std::ofstream stream;
        recorder_state state;
        mutable std::mutex mutex;

		recorder_state get_state() const;
		void set_state(recorder_state newState);
		void write_header(uint32_t channels);
		void write_data(const float* pFrames, uint32_t frameCount, uint32_t channels);
		void close_file();
		void write_int16(int16_t value, uint8_t *buffer, int32_t offset);
		void write_int32(int32_t value, uint8_t *buffer, int32_t offset);
		void write_float(float value, uint8_t *buffer, int32_t offset);
	};
}

#endif