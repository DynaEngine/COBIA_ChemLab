#pragma once

#include <string>
#include <map>

namespace ChemEngine {

	enum class UnitType
	{
		Temperature,
		Pressure,
		MolarFlow,
		MassFlow,
		VolumetricFlow,
		MolarEnthalpy,
		MassEnthalpy,
		MolarEntropy,
		MassEntropy,
		Density,
		MolarDensity,
		MolarVolume,
		MolecularWeight,
		HeatFlow,
		Length,
		Area,
		Volume,
		Time,
		Dimensionless
	};

	class UnitConverter
	{
	public:
		static double convert(double value, const std::wstring& fromUnit,
			const std::wstring& toUnit, UnitType type);

		static double convertToSI(double value, const std::wstring& fromUnit, UnitType type);
		static double convertFromSI(double value, const std::wstring& toUnit, UnitType type);

	private:
		static double getSIFactor(const std::wstring& unit, UnitType type);
	};

	class SystemOfUnits
	{
	public:
		static const int SI = 0;
		static const int ENG = 1;
		static const int CGS = 2;

		static std::wstring getTemperatureUnit(int system);
		static std::wstring getPressureUnit(int system);
		static std::wstring getMolarFlowUnit(int system);
		static std::wstring getMassFlowUnit(int system);
		static std::wstring getEnthalpyUnit(int system);
		static std::wstring getEntropyUnit(int system);
	};

}