#ifndef __MCP3421_H__
#define __MCP3421_H__

#define DEFAULT_ADDRESS 0x68 // == 1101 000 (last three bits are 0 by default)

#include <Arduino.h>
#include <Wire.h>

namespace MCP3421{
	
	typedef enum ConversionMode{
		OneShot = 0,
		Continuous = 1
	} ConversionMode_t;
	
	typedef enum SampleRate{
		240_SPS = 0x0, 	// 12 bits resolution
		60_SPS = 0x1,	// 14 bits resolution
		15_SPS = 0x2,	// 16 bits resolution
		3_75_SPS = 0x3	// 18 bits resolution
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
		void setConversionMode(ConversionMode_t inMode);
		
		/**
		 @brief Sets the sample rate
		 @param [in] inSampleRate the sample rate that should be set
		*/
		void setSampleRate(SampleRate_t inSampleRate);
		
		/**
		 @brief Sets the gain
		 @param [in] inGain the gain that should be used
		*/
		void setGain(Gain_t inGain);
		
		/**
		 @brief Processes the incomming data and requests for more
		 @param [out] value true if a new adc value has been received
		*/
		bool isConversionComplete(); // Call this periodically
		
		/**
		 @brief Returns the last value
		 @param [out] value the last adc value read
		*/
		double getValue(); // Returns the ADC value
		
	private:
		void writeConfig();		
		void writeData(uint8_t inData);
		bool readData(uint8_t* outData, size_t length);
		
	private:
		uint8_t mAddress;
	
		ConversionMode_t mConversionMode = ConversionMode::Continuous;
		SampleRate_t mSampleRate = SampleRate::240_SPS;
		Gain_t mGain = Gain::x1;
	
		bool configChangePending = false;
		
		double lastValue = 0.00;		
};

#endif // __MCP3421_H__