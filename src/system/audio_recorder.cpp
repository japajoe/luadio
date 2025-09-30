#include "audio_recorder.hpp"
#include <chrono>
#include <filesystem>

namespace luadio
{
	audio_recorder::audio_recorder()
	{
		outputBuffer.resize(4096);
		bytesWritten = 0;
		state = recorder_state_idle;
	}

	audio_recorder::~audio_recorder()
	{
		close_file();
	}

	bool audio_recorder::is_recording() const
	{
		return get_state() != recorder_state_idle;
	}

	void audio_recorder::start()
	{
		if(get_state() != recorder_state_idle) 
			return;

		set_state(recorder_state_write_header);
	}

	void audio_recorder::stop()
	{
		close_file();
	}

	void audio_recorder::on_process(const float* pFrames, uint32_t frameCount, uint32_t channels)
	{
		write_header(channels);
		write_data(pFrames, frameCount, channels);
	}

	audio_recorder::recorder_state audio_recorder::get_state() const
	{
		std::lock_guard<std::mutex> lock(mutex);
		return state;
	}

	void audio_recorder::set_state(audio_recorder::recorder_state newState)
	{
		std::lock_guard<std::mutex> lock(mutex);
		state = newState;
	}

	void audio_recorder::write_header(uint32_t channels)
	{
		if(get_state() != recorder_state_write_header)
			return;

		if(bytesWritten > 0)
			return;

		if(stream.is_open())
			return;

		auto now = std::chrono::high_resolution_clock::now();
		auto ticks = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();

		std::filesystem::path dirPath = "recordings";

		currentFileName = std::to_string(ticks) + ".wav";
		
		if(std::filesystem::exists(dirPath))
		{
			currentFileName = "recordings/" + std::to_string(ticks) + ".wav";
		}

		uint8_t header[44];

		const int32_t bitDepth = 16;

		int32_t chunkId = 1179011410;           //"RIFF
		int32_t chunkSize = 0;
		int32_t format = 1163280727;            //"WAVE"
		int32_t subChunk1Id = 544501094;        //"fmt "
		int32_t subChunk1Size = 16;
		int16_t audioFormat = 1;
		int16_t numChannels = static_cast<int16_t>(channels);
		int32_t sampleRate = 44100;
		int32_t byteRate = sampleRate * numChannels * bitDepth / 8;
		int16_t blockAlign = static_cast<int16_t>(numChannels * bitDepth / 8);
		int16_t bitsPerSample = bitDepth;
		int32_t subChunk2Id = 1635017060;       //"data"
		int32_t subChunk2Size = 0;

		write_int32(chunkId, header, 0);
		write_int32(chunkSize, header, 4);
		write_int32(format, header, 8);
		write_int32(subChunk1Id, header, 12);
		write_int32(subChunk1Size, header, 16);
		write_int16(audioFormat, header, 20);
		write_int16(numChannels, header, 22);
		write_int32(sampleRate, header, 24);
		write_int32(byteRate, header, 28);
		write_int16(blockAlign, header, 32);
		write_int16(bitsPerSample, header, 34);
		write_int32(subChunk2Id, header, 36);
		write_int32(subChunk2Size, header, 40);

		stream = std::ofstream(currentFileName, std::ios::out | std::ios::trunc);

		if(!stream.is_open())
			return;

		stream.write(reinterpret_cast<const char*>(header), 44);

		bytesWritten = 0;

		set_state(recorder_state_write_data);
	}

	void audio_recorder::write_data(const float* pFrames, uint32_t frameCount, uint32_t channels)
	{
		if(get_state() != recorder_state_write_data)
			return;		

		if(!stream.is_open())
			return;


		const uint32_t byteSize = static_cast<uint32_t>(frameCount * channels * sizeof(short));
		const uint32_t numSamples = frameCount * channels;

		if(byteSize == 0)
			return;

		if(outputBuffer.size() < byteSize)
			outputBuffer.resize(byteSize);

		int32_t index = 0;

		int16_t *pBuffer = reinterpret_cast<int16_t*>(outputBuffer.data());

		for(int32_t i = 0; i < numSamples; i++)
		{
			pBuffer[index] = static_cast<int16_t>(pFrames[i] * 32767);
			index++;
		}

		stream.write(reinterpret_cast<const char*>(outputBuffer.data()), byteSize);

		bytesWritten += byteSize;
	}

	void audio_recorder::close_file()
	{
		if(stream.is_open())
		{
			if(bytesWritten > 0)
			{
				const int32_t headerSize = 44;
				int32_t chunkSize = headerSize + static_cast<int32_t>(bytesWritten - 8);//file size - 8;
				int32_t subChunk2Size = static_cast<int32_t>(bytesWritten);

				uint8_t buffer[8];

				write_int32(chunkSize, buffer, 0);
				stream.seekp(4, std::ios::beg);
				stream.write(reinterpret_cast<const char*>(buffer), sizeof(int32_t));
				
				write_int32(subChunk2Size, buffer, 0);
				stream.seekp(40, std::ios::beg);
				stream.write(reinterpret_cast<const char*>(buffer), sizeof(int32_t));

				bytesWritten = 0;
			}

			stream.close();
		}

		set_state(recorder_state_idle);
	}

	void audio_recorder::write_int16(int16_t value, uint8_t *buffer, int32_t offset)
	{
		int16_t *pBuffer = reinterpret_cast<int16_t*>(&buffer[offset]);
		*pBuffer = value;
	}

	void audio_recorder::write_int32(int32_t value, uint8_t *buffer, int32_t offset)
	{
		int32_t *pBuffer = reinterpret_cast<int32_t*>(&buffer[offset]);
		*pBuffer = value;
	}

	void audio_recorder::write_float(float value, uint8_t *buffer, int32_t offset)
	{
		float *pBuffer = reinterpret_cast<float*>(&buffer[offset]);
		*pBuffer = value;
	}
}