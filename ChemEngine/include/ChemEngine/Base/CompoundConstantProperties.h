#pragma once

#include <string>
#include <vector>
#include <map>

namespace ChemEngine {

	struct CompoundConstantProperties
	{
		std::wstring name;
		std::wstring formula;
		std::wstring casNumber;

		double molecularWeight = 0.0;
		double normalBoilingPoint = 0.0;
		double criticalTemperature = 0.0;
		double criticalPressure = 0.0;
		double criticalVolume = 0.0;
		double criticalCompressibility = 0.0;
		double acentricFactor = 0.0;
		double triplePointTemperature = 0.0;
		double triplePointPressure = 0.0;

		bool isEmpty() const { return name.empty(); }
	};

	class Compound
	{
	public:
		Compound() = default;
		Compound(const CompoundConstantProperties& constProps);

		const CompoundConstantProperties& getConstantProperties() const { return m_constProps; }
		void setConstantProperties(const CompoundConstantProperties& props) { m_constProps = props; }

		double getMoleFraction() const { return m_moleFraction; }
		void setMoleFraction(double val) { m_moleFraction = val; }

		double getMassFraction() const { return m_massFraction; }
		void setMassFraction(double val) { m_massFraction = val; }

		double getMolarFlow() const { return m_molarFlow; }
		void setMolarFlow(double val) { m_molarFlow = val; }

		double getMassFlow() const { return m_massFlow; }
		void setMassFlow(double val) { m_massFlow = val; }

	private:
		CompoundConstantProperties m_constProps;
		double m_moleFraction = 0.0;
		double m_massFraction = 0.0;
		double m_molarFlow = 0.0;
		double m_massFlow = 0.0;
	};

	using CompoundList = std::vector<Compound>;

}