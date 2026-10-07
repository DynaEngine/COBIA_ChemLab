#pragma once

#include "ChemEngine/Base/SimulationObject.h"
#include "ChemEngine/Base/CompoundConstantProperties.h"
#include "ChemEngine/Units/UnitOperationBase.h"
#include <COBIA.h>
#include <CapeInterfaces_1_2.h>
#include <string>
#include <vector>
#include <memory>

namespace ChemEngine {
namespace COBIA {

	class CapeMaterialObject;

	using namespace ::COBIA;

	class CapeMaterialStream : public SimulationObject
	{
	public:
		CapeMaterialStream();
		virtual ~CapeMaterialStream();

		const std::vector<Port>& getPorts() const { return m_ports; }
		std::vector<Port>& getPorts() { return m_ports; }
		Port* findPort(const std::wstring& name);
		const Port* findPort(const std::wstring& name) const;
		bool connectPort(const std::wstring& portName, const std::wstring& objName,
			const std::wstring& otherPortName);
		bool disconnectPort(const std::wstring& portName);

		std::vector<std::wstring> getUpstreamObjectNames() const override;
		std::vector<std::wstring> getDownstreamObjectNames() const override;

		void createMaterial(CapeStringImpl name);

		CAPEOPEN_1_2::CapeThermoMaterial getMaterial() const;
		void setMaterial(CAPEOPEN_1_2::CapeThermoMaterial mat);

		void addCompound(const Compound& comp);
		void addCompounds(const CompoundList& compounds);
		void clearCompounds();
		bool hasCompounds() const { return !m_compounds.empty(); }

		bool solve() override;

		bool hasMaterial() const { return m_hasMaterial; }

		double getTemperature();
		void setTemperature(double T);

		double getPressure();
		void setPressure(double P);

		double getEnthalpy(const std::wstring& basis = L"mole");
		void setEnthalpy(double H);

		void getOverallFlow(double& molarFlow, double& massFlow);
		void setOverallFlow(double molarFlow, const std::wstring& basis = L"mole");

		double getTotalMolarFlow();
		void setTotalMolarFlow(double f);

		std::vector<double> getMoleFractions();
		void setMoleFractions(const std::vector<double>& x);

		std::vector<double> getMassFractions();
		void setMassFractions(const std::vector<double>& w);

		void flashTP();
		void flashPH();
		void flashPX(double vapFrac);

		void copyFrom(CapeMaterialStream& source);

		std::wstring getPhaseLabel(size_t i);
		size_t getPhaseCount();

		double getPhaseFraction(const std::wstring& phaseLabel,
			const std::wstring& basis = L"mole");

		double getSinglePhaseProp(const std::wstring& prop,
			const std::wstring& phaseLabel, const std::wstring& basis = L"mole");

		double getOverallProp(const std::wstring& prop,
			const std::wstring& basis = L"mole");

		void setCapeMaterialObject(CapeMaterialObject* obj);
		CapeMaterialObject* getCapeMaterialObject() const { return m_capeMaterialObj; }
		bool hasCapeMaterialObject() const { return m_capeMaterialObj != nullptr; }

	private:
		double calcStandaloneProperty(const std::wstring& prop) const;

		CAPEOPEN_1_2::CapeThermoMaterial m_capeMaterial;
		bool m_hasMaterial = false;
		bool m_isCreated = false;
		bool m_flashed = false;

		double m_standaloneT = 298.15;
		double m_standaloneP = 101325.0;
		double m_standaloneMolarFlow = 0.0;
		CompoundList m_compounds;
		std::vector<double> m_moleFractions;
		CapeMaterialObject* m_capeMaterialObj = nullptr;

		std::vector<Port> m_ports;
	};

	using CapeMaterialStreamPtr = std::shared_ptr<CapeMaterialStream>;

}}