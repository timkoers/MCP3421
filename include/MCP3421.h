#ifndef __MCP3421_H__
#define __MCP3421_H__

#define DEFAULT_ADDRESS 0x68 // == 1101 000 (last three bits are 0 by default)

#include <Arduino.h>
#include <Wire.h>

namespace MCP3421_Constants{
	
	typedef enum ConversionMode{
		OneShot = 0,
		Continuous = 1
	} ConversionMode_t;
	
	typedef enum SampleRate{
		SPS_240 = 0, 	// 12 bits resolution
		SPS_60 = 0x1,	// 14 bits resolution
		SPS_15 = 0x2,	// 16 bits resolution
		SPS_3_75 = 0x3	// 18 bits resolution
	}SampleRate_t;
	
	typedef enum Gain{
		x1 = 0,
		x2 = 0x1,
		x4 = 0x2,
		x8 = 0x3
	} Gain_t;
}

class MCP3421{	
	public:
	
		/**
		 @brief Constructor
		 @param [in] inAddress the device number (Ax) that you ordered.
		*/
		MCP3421(uint8_t inAddress = DEFAULT_ADDRESS);
		
		/**
		 @brief Sets the conversion mode
		 @param [in] inMode the conversion mode that should be set
		*/
		void setConversionMode(MCP3421_Constants::ConversionMode_t inMode);
		
		/**
		 @brief Sets the sample rate
		 @param [in] inSampleRate the sample rate that should be set
		*/
		void setSampleRate(MCP3421_Constants::SampleRate_t inSampleRate);
		
		/**
		 @brief Sets the gain
		 @param [in] inGain the gain that should be used
		*/
		void setGain(MCP3421_Constants::Gain_t inGain);
		
		/**
		 @brief Processes the incomming data and requests for more
		 @param [out] value true if a new adc value has been received
		*/
		bool isConversionComplete(); // Call this periodically
		
		/**
		 @brief Returns the last value
		 @param [out] value the last adc value read
		*/
		uint32_t getValue(); // Returns the ADC value

		/**
		 @brief Returns the last voltage value
		 @param [out] value the last voltage value read
		*/
		float getVoltage(); // Returns the ADC voltage
		
	private:
		void writeConfig();		
		void writeData(uint8_t inData);
		bool readData(uint8_t* outData, size_t length);
		
	private:
		uint8_t mAddress;
	
		MCP3421_Constants::ConversionMode_t mConversionMode = MCP3421_Constants::ConversionMode::Continuous;
		MCP3421_Constants::SampleRate_t mSampleRate = MCP3421_Constants::SampleRate::SPS_240;
		MCP3421_Constants::Gain_t mGain = MCP3421_Constants::Gain::x1;
	
		bool configChangePending = false;
		
		uint32_t lastValue = 0;
};

#endif // __MCP3421_H__