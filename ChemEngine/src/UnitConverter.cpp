#include "ChemEngine/Utils/UnitConverter.h"

namespace ChemEngine {

	double UnitConverter::getSIFactor(const std::wstring& unit, UnitType type)
	{
		if (type == UnitType::Temperature)
		{
			if (unit == L"K")       return 1.0;
			if (unit == L"C")       return 1.0;
			if (unit == L"F")       return 1.0;
			if (unit == L"R")       return 1.8;
		}
		if (type == UnitType::Pressure)
		{
			if (unit == L"Pa")      return 1.0;
			if (unit == L"kPa")     return 1000.0;
			if (unit == L"MPa")     return 1e6;
			if (unit == L"atm")     return 101325.0;
			if (unit == L"bar")     return 1e5;
			if (unit == L"psi")     return 6894.757;
		}
		if (type == UnitType::MolarFlow)
		{
			if (unit == L"mol/s")   return 1.0;
			if (unit == L"kmol/h")  return 1.0 / 3.6;
			if (unit == L"lbmol/h") return 0.125997;
		}
		if (type == UnitType::MassFlow)
		{
			if (unit == L"kg/s")    return 1.0;
			if (unit == L"kg/h")    return 1.0 / 3600.0;
			if (unit == L"lb/h")    return 0.000125997;
		}
		return 1.0;
	}

	double UnitConverter::convertToSI(double value, const std::wstring& fromUnit, UnitType type)
	{
		double factor = getSIFactor(fromUnit, type);
		if (type == UnitType::Temperature)
		{
			if (fromUnit == L"C")   return value + 273.15;
			if (fromUnit == L"F")   return (value + 459.67) / 1.8;
			if (fromUnit == L"R")   return value / 1.8;
		}
		return value * factor;
	}

	double UnitConverter::convertFromSI(double value, const std::wstring& toUnit, UnitType type)
	{
		double factor = getSIFactor(toUnit, type);
		if (type == UnitType::Temperature)
		{
			if (toUnit == L"C")     return value - 273.15;
			if (toUnit == L"F")     return value * 1.8 - 459.67;
			if (toUnit == L"R")     return value * 1.8;
		}
		return value / factor;
	}

	double UnitConverter::convert(double value, const std::wstring& fromUnit,
		const std::wstring& toUnit, UnitType type)
	{
		double si = convertToSI(value, fromUnit, type);
		return convertFromSI(si, toUnit, type);
	}

	std::wstring SystemOfUnits::getTemperatureUnit(int system)
	{
		switch (system)
		{
		case ENG: return L"F";
		case CGS: return L"C";
		default:  return L"K";
		}
	}

	std::wstring SystemOfUnits::getPressureUnit(int system)
	{
		switch (system)
		{
		case ENG: return L"psi";
		case CGS: return L"atm";
		default:  return L"Pa";
		}
	}

	std::wstring SystemOfUnits::getMolarFlowUnit(int system)
	{
		switch (system)
		{
		case ENG: return L"lbmol/h";
		default:  return L"mol/s";
		}
	}

	std::wstring SystemOfUnits::getMassFlowUnit(int system)
	{
		switch (system)
		{
		case ENG: return L"lb/h";
		default:  return L"kg/s";
		}
	}

	std::wstring SystemOfUnits::getEnthalpyUnit(int system)
	{
		switch (system)
		{
		case ENG: return L"BTU/lbmol";
		default:  return L"J/mol";
		}
	}

	std::wstring SystemOfUnits::getEntropyUnit(int system)
	{
		switch (system)
		{
		case ENG: return L"BTU/lbmol·R";
		default:  return L"J/mol·K";
		}
	}

}