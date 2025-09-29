#include "oscillator_module.hpp"

namespace luadio
{
	static std::string gSource = R"(-- oscillator.lua
local oscillator = {}
oscillator.__index = oscillator

-- wavetype Enum
oscillator.wavetype = {}
oscillator.wavetype.sine = 1
oscillator.wavetype.square = 2
oscillator.wavetype.triangle = 3
oscillator.wavetype.saw = 4

local TAU = 2 * math.pi

-- Constructor
function oscillator.new(type, frequency, amplitude, sampleRate)
    local self = setmetatable({}, oscillator)
    self.type = type
    self.frequency = frequency
    self.amplitude = amplitude
    self.phase = 0.0
    self.sampleRate = sampleRate
    self:set_phase_increment()
    self:set_wave_function()
    return self
end

-- Set wave function based on type
function oscillator:set_wave_function()
    if self.type == oscillator.wavetype.saw then
        self.waveFunc = self.get_saw_sample
    elseif self.type == oscillator.wavetype.sine then
        self.waveFunc = self.get_sine_sample
    elseif self.type == oscillator.wavetype.square then
        self.waveFunc = self.get_square_sample
    elseif self.type == oscillator.wavetype.triangle then
        self.waveFunc = self.get_triangle_sample
    end
end

-- Set phase increment based on frequency
function oscillator:set_phase_increment()
    self.phaseIncrement = TAU * self.frequency / self.sampleRate
end

function oscillator:set_type(type)
    self.type = type
    self:set_wave_function()
end

function oscillator:set_frequency(frequency)
    self.frequency = frequency
    self:set_phase_increment()
end

function oscillator:set_phase(phase)
    self.phase = phase
end

function oscillator:set_amplitude(amplitude)
    self.amplitude = amplitude
end

-- Reset phase
function oscillator:reset()
    self.phase = 0
end

-- Get current value
function oscillator:get_value()
    local result = self.waveFunc(self.phase)
    self.phase = (self.phase + self.phaseIncrement) % TAU
    return result * self.amplitude
end

-- Get value at a specific phase
function oscillator:get_value_at_phase(phase)
    return self.waveFunc(phase) * self.amplitude
end

-- Get modulated value
function oscillator:get_modulated_value(phase)
    local result = self.waveFunc(self.phase + phase)
    self.phase = (self.phase + self.phaseIncrement) % TAU
    return result * self.amplitude
end

-- Math sign function
function math.sign(x)
    if x > 0 then
        return 1
    elseif x < 0 then
        return -1
    else
        return 0
    end
end

-- Wave functions
function oscillator.get_saw_sample(phase)
    phase = phase / TAU
    return 2.0 * phase - 1.0
end

function oscillator.get_sine_sample(phase)
    return math.sin(phase)
end

function oscillator.get_square_sample(phase)
    return math.sign(math.sin(phase))
end

function oscillator.get_triangle_sample(phase)
    phase = phase / TAU
    return 2 * math.abs(2 * (phase - 0.5)) - 1
end

-- Metatable to prevent overwriting
local mt = {
    __newindex = function(table, key, value)
        error('Attempt to modify read-only method: ' .. key)
    end,
}

setmetatable(oscillator, mt)

return oscillator)";

	void oscillator_module::load(lua_State *L)
	{
		register_source(L, gSource, "oscillator");
	}
}