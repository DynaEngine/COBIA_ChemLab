#include "ChemEngine/COBIA/CapeMaterialStream.h"
#include "ChemEngine/COBIA/CapeMaterialObject.h"
#include "ChemEngine/Flowsheet/Flowsheet.h"
#include <cmath>

namespace ChemEngine {
namespace COBIA {

	CapeMaterialStream::CapeMaterialStream()
	{
		m_type = L"MaterialStream";
		m_ports = {
			{L"Inlet",  PortDirection::Inlet,  PortType::Material},
			{L"Outlet", PortDirection::Outlet, PortType::Material},
		};
	}

	CapeMaterialStream::~CapeMaterialStream()
	{
		if (m_capeMaterialObj)
		{
			::COBIA::CapeOpenObject<CapeMaterialObject>::release(
				static_cast<void*>(m_capeMaterialObj));
			m_capeMaterialObj = nullptr;
		}
	}

	void CapeMaterialStream::createMaterial(CapeStringImpl name)
	{
		m_name = name.c_str();
		m_isCreated = true;
	}

	void CapeMaterialStream::addCompound(const Compound& comp)
	{
		m_compounds.push_back(comp);
	}

	void CapeMaterialStream::addCompounds(const CompoundList& compounds)
	{
		for (const auto& c : compounds)
			m_compounds.push_back(c);
	}

	void CapeMaterialStream::clearCompounds()
	{
		m_compounds.clear();
		m_moleFractions.clear();
	}

	CAPEOPEN_1_2::CapeThermoMaterial CapeMaterialStream::getMaterial() const
	{
		return m_capeMaterial;
	}

	void CapeMaterialStream::setMaterial(CAPEOPEN_1_2::CapeThermoMaterial mat)
	{
		m_capeMaterial = mat;
		m_hasMaterial = true;
	}

	bool CapeMaterialStream::solve()
	{
		auto* inlet = findPort(L"Inlet");
		if (inlet && !inlet->connectedObjectName.empty() && m_flowsheet)
		{
			auto srcObj = m_flowsheet->findObject(inlet->connectedObjectName);
			auto srcStream = dynamic_cast<CapeMaterialStream*>(srcObj.get());
			if (srcStream && srcStream->getStatus() == SimulationObjectStatus::Calculated)
			{
				copyFrom(*srcStream);
			}
		}

		m_status = SimulationObjectStatus::Calculated;
		return true;
	}

	Port* CapeMaterialStream::findPort(const std::wstring& name)
{
	for (auto& p : m_ports)
		if (p.name == name) return &p;
	return nullptr;
}

const Port* CapeMaterialStream::findPort(const std::wstring& name) const
{
	for (const auto& p : m_ports)
		if (p.name == name) return &p;
	return nullptr;
}

std::vector<std::wstring> CapeMaterialStream::getUpstreamObjectNames() const
{
	std::vector<std::wstring> result;
	for (const auto& p : m_ports)
		if (p.direction == PortDirection::Inlet && !p.connectedObjectName.empty())
			result.push_back(p.connectedObjectName);
	return result;
}

std::vector<std::wstring> CapeMaterialStream::getDownstreamObjectNames() const
{
	std::vector<std::wstring> result;
	for (const auto& p : m_ports)
		if (p.direction == PortDirection::Outlet && !p.connectedObjectName.empty())
			result.push_back(p.connectedObjectName);
	return result;
}

	bool CapeMaterialStream::connectPort(const std::wstring& portName,
		const std::wstring& objName, const std::wstring& otherPortName)
	{
		auto* port = findPort(portName);
		if (!port) return false;
		port->connectedObjectName = objName;
		port->connectedPortName = otherPortName;
		return true;
	}

	bool CapeMaterialStream::disconnectPort(const std::wstring& portName)
	{
		auto* port = findPort(portName);
		if (!port) return false;
		port->connectedObjectName.clear();
		port->connectedPortName.clear();
		return true;
	}

	double CapeMaterialStream::getTemperature()
	{
		if (m_capeMaterialObj)
			return m_capeMaterialObj->getCachedOverallProp(L"temperature");
		if (m_hasMaterial)
		{
			double T = 298.15, P = 101325.0;
			CapeArrayRealImpl x;
			m_capeMaterial.GetOverallTPFraction(T, P, x);
			return T;
		}
		return m_standaloneT;
	}

	void CapeMaterialStream::setTemperature(double T)
	{
		m_standaloneT = T;
		if (!m_hasMaterial) return;
		CapeArrayRealImpl vals(1);
		vals[0] = T;
		CapeStringImpl propName(COBIATEXT("temperature"));
		m_capeMaterial.SetOverallProp(propName, (ICapeString*)nullptr, vals);
	}

	double CapeMaterialStream::getPressure()
	{
		if (m_capeMaterialObj)
			return m_capeMaterialObj->getCachedOverallProp(L"pressure");
		if (m_hasMaterial)
		{
			double T = 298.15, P = 101325.0;
			CapeArrayRealImpl x;
			m_capeMaterial.GetOverallTPFraction(T, P, x);
			return P;
		}
		return m_standaloneP;
	}

	void CapeMaterialStream::setPressure(double P)
{
	m_standaloneP = P;
	if (!m_hasMaterial) return;
	CapeArrayRealImpl vals(1);
	vals[0] = P;
	CapeStringImpl propName(COBIATEXT("pressure"));
	m_capeMaterial.SetOverallProp(propName, (ICapeString*)nullptr, vals);
}

double CapeMaterialStream::getEnthalpy(const std::wstring& basis)
{
	return getOverallProp(L"enthalpy", basis);
}

void CapeMaterialStream::setEnthalpy(double H)
{
	if (!m_hasMaterial) return;
	CapeArrayRealImpl vals(1);
	vals[0] = H;
	CapeStringImpl propName(COBIATEXT("enthalpy"));
	CapeStringImpl emptyBasis(COBIATEXT(""));
	m_capeMaterial.SetOverallProp(propName, emptyBasis, vals);
}

void CapeMaterialStream::getOverallFlow(double& molarFlow, double& massFlow)
	{
		molarFlow = 0.0; massFlow = 0.0;
		if (m_capeMaterialObj)
		{
			molarFlow = m_capeMaterialObj->getCachedOverallProp(L"totalflow");
			if (molarFlow == 0.0)
				molarFlow = m_standaloneMolarFlow;
			massFlow = molarFlow * 0.018;
			return;
		}
		if (!m_hasMaterial)
		{
			molarFlow = m_standaloneMolarFlow;
			massFlow = molarFlow * 0.018;
			return;
		}

		CapeStringImpl tfProp(COBIATEXT("totalflow"));
		CapeStringImpl moleBasis(COBIATEXT("mole"));
		CapeStringImpl massBasis(COBIATEXT("mass"));
		CapeArrayRealImpl propVals;

		m_capeMaterial.GetOverallProp(tfProp, moleBasis, propVals);
		molarFlow = propVals.size() > 0 ? (double)propVals[0] : 0.0;

		m_capeMaterial.GetOverallProp(tfProp, massBasis, propVals);
		massFlow = propVals.size() > 0 ? (double)propVals[0] : 0.0;
	}

	void CapeMaterialStream::setOverallFlow(double molarFlow,
		const std::wstring& basis)
	{
		m_standaloneMolarFlow = molarFlow;
		if (!m_hasMaterial) return;
		CapeArrayRealImpl flow(1);
		flow[0] = molarFlow;
		CapeStringImpl propName(COBIATEXT("totalflow"));
		CapeStringImpl basisStr(basis.c_str());
		m_capeMaterial.SetOverallProp(propName, basisStr, flow);
	}

	double CapeMaterialStream::getTotalMolarFlow()
	{
		double mf = 0.0, mf2 = 0.0;
		getOverallFlow(mf, mf2);
		return mf;
	}

	void CapeMaterialStream::setTotalMolarFlow(double f)
	{
		setOverallFlow(f, L"mole");
	}

std::vector<double> CapeMaterialStream::getMoleFractions()
	{
		std::vector<double> result;
		if (m_capeMaterialObj)
		{
			return m_capeMaterialObj->getCachedComposition();
		}
		if (!m_hasMaterial)
		{
			if (!m_moleFractions.empty())
				return m_moleFractions;
			if (!m_compounds.empty())
			{
				result.resize(m_compounds.size(), 1.0 / m_compounds.size());
				return result;
			}
			return result;
		}

		double T, P;
		CapeArrayRealImpl x;
		m_capeMaterial.GetOverallTPFraction(T, P, x);
		for (CapeInteger i = 0; i < x.size(); ++i)
			result.push_back((double)x[i]);
		return result;
	}

	void CapeMaterialStream::setMoleFractions(const std::vector<double>& x)
	{
		m_moleFractions = x;
		if (!m_hasMaterial) return;
		CapeArrayRealImpl vals((CapeInteger)x.size());
		for (size_t i = 0; i < x.size(); ++i)
			vals[(CapeInteger)i] = x[i];
		CapeStringImpl propName(COBIATEXT("fraction"));
		CapeStringImpl basis(COBIATEXT("mole"));
		m_capeMaterial.SetOverallProp(propName, basis, vals);
	}

	void CapeMaterialStream::flashTP()
	{
		m_flashed = true;
		if (m_capeMaterialObj) return;
		if (!m_hasMaterial) return;
		CAPEOPEN_1_2::CapeThermoEquilibriumRoutine equil(m_capeMaterial);

		CapeArrayStringImpl spec1(1), spec2(1);
		spec1[0] = COBIATEXT("temperature");
		spec2[0] = COBIATEXT("pressure");

		CapeStringImpl empty;
		equil.CalcEquilibrium(spec1, spec2, empty);
	}

	void CapeMaterialStream::flashPH()
	{
		if (!m_hasMaterial) return;
		CAPEOPEN_1_2::CapeThermoEquilibriumRoutine equil(m_capeMaterial);

		CapeArrayStringImpl spec1(1), spec2(1);
		spec1[0] = COBIATEXT("pressure");
		spec2[0] = COBIATEXT("enthalpy");

		CapeStringImpl empty;
		equil.CalcEquilibrium(spec1, spec2, empty);
	}

	void CapeMaterialStream::flashPX(double vapFrac)
	{
		if (!m_hasMaterial) return;

		CapeArrayRealImpl fracVal(1);
		fracVal[0] = vapFrac;
		CapeStringImpl vfProp(COBIATEXT("vaporFraction"));
		CapeStringImpl emptyBasis(COBIATEXT(""));
		m_capeMaterial.SetOverallProp(vfProp, emptyBasis, fracVal);

		CAPEOPEN_1_2::CapeThermoEquilibriumRoutine equil(m_capeMaterial);

		CapeArrayStringImpl spec1(1), spec2(1);
		spec1[0] = COBIATEXT("pressure");
		spec2[0] = COBIATEXT("vaporFraction");

		CapeStringImpl empty;
		equil.CalcEquilibrium(spec1, spec2, empty);
	}

	std::vector<double> CapeMaterialStream::getMassFractions()
	{
		std::vector<double> result;
		if (m_capeMaterialObj)
		{
			auto moleFracs = m_capeMaterialObj->getCachedComposition();
			if (moleFracs.empty()) return result;
			result.resize(moleFracs.size(), 0.0);
			double mwSum = 0.0;
			for (size_t i = 0; i < moleFracs.size() && i < m_compounds.size(); ++i)
			{
				double mw = m_compounds[i].getConstantProperties().molecularWeight;
				result[i] = moleFracs[i] * mw;
				mwSum += result[i];
			}
			if (mwSum > 1e-30)
				for (auto& v : result) v /= mwSum;
			return result;
		}
		if (!m_hasMaterial)
		{
			auto moleFracs = getMoleFractions();
			if (moleFracs.empty()) return result;
			result.resize(moleFracs.size(), 0.0);
			double mwSum = 0.0;
			for (size_t i = 0; i < moleFracs.size() && i < m_compounds.size(); ++i)
			{
				double mw = m_compounds[i].getConstantProperties().molecularWeight;
				result[i] = moleFracs[i] * mw;
				mwSum += result[i];
			}
			if (mwSum > 1e-30)
				for (auto& v : result) v /= mwSum;
			return result;
		}
		CapeStringImpl mfProp(COBIATEXT("massfraction"));
		CapeStringImpl emptyBasis(COBIATEXT(""));
		CapeArrayRealImpl w;
		m_capeMaterial.GetOverallProp(mfProp, emptyBasis, w);
		for (CapeInteger i = 0; i < w.size(); ++i)
			result.push_back((double)w[i]);
		return result;
	}

	void CapeMaterialStream::setMassFractions(const std::vector<double>& w)
	{
		if (!m_hasMaterial) return;
		CapeArrayRealImpl newW((CapeInteger)w.size());
		for (size_t i = 0; i < w.size(); ++i)
			newW[(CapeInteger)i] = w[i];
		CapeStringImpl mfProp(COBIATEXT("massfraction"));
		CapeStringImpl emptyBasis(COBIATEXT(""));
		m_capeMaterial.SetOverallProp(mfProp, emptyBasis, newW);
	}

	void CapeMaterialStream::copyFrom(CapeMaterialStream& source)
	{
		if (!m_hasMaterial && source.m_hasMaterial)
		{
			m_capeMaterial = source.m_capeMaterial;
			m_hasMaterial = true;
			return;
		}
		if (m_hasMaterial && source.m_hasMaterial)
		{
			m_capeMaterial.CopyFromMaterial(source.m_capeMaterial);
		}
	}

	size_t CapeMaterialStream::getPhaseCount()
	{
		if (!m_hasMaterial) return 0;
		CapeArrayStringImpl labels;
		CapeArrayEnumerationImpl<CAPEOPEN_1_2::CapePhaseStatus> statuses;
		m_capeMaterial.GetPresentPhases(labels, statuses);
		return (size_t)labels.size();
	}

	std::wstring CapeMaterialStream::getPhaseLabel(size_t i)
	{
		if (!m_hasMaterial) return L"";
		CapeArrayStringImpl labels;
		CapeArrayEnumerationImpl<CAPEOPEN_1_2::CapePhaseStatus> statuses;
		m_capeMaterial.GetPresentPhases(labels, statuses);
		if ((CapeInteger)i < labels.size())
			return (const wchar_t*)(labels[(CapeInteger)i].c_str());
		return L"";
	}

	double CapeMaterialStream::getPhaseFraction(const std::wstring& phaseLabel,
		const std::wstring& basis)
	{
		if (!m_hasMaterial) return 0.0;
		CapeArrayRealImpl frac;
		CapeStringImpl phaseStr(phaseLabel.c_str());
		CapeStringImpl basisStr(basis.c_str());
		CapeStringImpl pfProp(COBIATEXT("phaseFraction"));
		m_capeMaterial.GetSinglePhaseProp(pfProp, phaseStr, basisStr, frac);
		return frac.size() > 0 ? (double)frac[0] : 0.0;
	}

	double CapeMaterialStream::getSinglePhaseProp(const std::wstring& prop,
		const std::wstring& phaseLabel, const std::wstring& basis)
	{
		if (!m_hasMaterial) return 0.0;

		CapeStringImpl propStr(prop.c_str());
		CapeStringImpl phaseStr(phaseLabel.c_str());
		CapeStringImpl basisStr(basis.c_str());

		CapeArrayStringImpl propArr(1);
		propArr[0] = propStr;

		CAPEOPEN_1_2::CapeThermoPropertyRoutine routine(m_capeMaterial);
		routine.CalcSinglePhaseProp(propArr, phaseStr);

		CapeArrayRealImpl vals;
		m_capeMaterial.GetSinglePhaseProp(propStr, phaseStr, basisStr, vals);
		return vals.size() > 0 ? (double)vals[0] : 0.0;
	}

	double CapeMaterialStream::getOverallProp(const std::wstring& prop,
		const std::wstring& basis)
	{
		if (m_capeMaterialObj)
		{
			std::wstring p = prop;
			double val = m_capeMaterialObj->getCachedOverallProp(p);
			if (val != 0.0 || p == L"temperature" || p == L"pressure")
				return val;
			if (m_flashed)
			{
				const auto& phases = m_capeMaterialObj->getPhaseLabels();
				if (!phases.empty())
				{
					val = m_capeMaterialObj->getCachedSinglePhaseProp(p, phases[0]);
					if (val != 0.0)
						return val;
				}
				val = m_capeMaterialObj->getCachedSinglePhaseProp(p, L"Overall");
				if (val != 0.0)
					return val;
			}
		}
		if (m_hasMaterial)
		{
			CapeStringImpl propStr(prop.c_str());
			CapeStringImpl basisStr(basis.c_str());
			CapeArrayRealImpl vals;
			m_capeMaterial.GetOverallProp(propStr, basisStr, vals);
			return vals.size() > 0 ? (double)vals[0] : 0.0;
		}
		return calcStandaloneProperty(prop);
	}

	double CapeMaterialStream::calcStandaloneProperty(const std::wstring& prop) const
	{
		if (m_compounds.empty()) return 0.0;

		static const double R = 8.314462618;
		size_t n = m_compounds.size();

		double MWavg = 0.0;
		for (size_t i = 0; i < n; ++i)
		{
			double xi = (i < m_moleFractions.size()) ? m_moleFractions[i] : (1.0 / n);
			MWavg += xi * m_compounds[i].getConstantProperties().molecularWeight;
		}

		double T = m_standaloneT;
		double P = m_standaloneP;

		if (prop == L"density")
		{
			double TcAvg = 0.0, PcAvg = 0.0, omegaAvg = 0.0;
			for (size_t i = 0; i < n; ++i)
			{
				double xi = (i < m_moleFractions.size()) ? m_moleFractions[i] : (1.0 / n);
				auto& ccp = m_compounds[i].getConstantProperties();
				TcAvg += xi * ccp.criticalTemperature;
				PcAvg += xi * ccp.criticalPressure;
				omegaAvg += xi * ccp.acentricFactor;
			}

			if (T >= TcAvg)
			{
				return P * (MWavg * 1e-3) / (R * T);
			}

			double Zra = 0.29056 - 0.08775 * omegaAvg;
			double tau = 1.0 + std::pow(1.0 - T / TcAvg, 2.0 / 7.0);
			double rho_c = PcAvg * (MWavg * 1e-3) / (R * TcAvg);
			return rho_c / std::pow(Zra, tau);
		}
		else if (prop == L"enthalpy")
		{
			double H = 0.0;
			for (size_t i = 0; i < n; ++i)
			{
				double xi = (i < m_moleFractions.size()) ? m_moleFractions[i] : (1.0 / n);
				H += xi * 4.0 * R * T;
			}
			return H;
		}
		else if (prop == L"entropy")
		{
			static const double Tref = 298.15;
			static const double Pref = 101325.0;
			double S = 0.0;
			for (size_t i = 0; i < n; ++i)
			{
				double xi = (i < m_moleFractions.size()) ? m_moleFractions[i] : (1.0 / n);
				S += xi * 4.0 * R * log(T / Tref);
			}
			S -= R * log(P / Pref);
			return S;
		}
		else if (prop == L"heatOfVaporization")
		{
			double omegaAvg = 0.0;
			double TcProd = 1.0;
			for (size_t i = 0; i < n; ++i)
			{
				double xi = (i < m_moleFractions.size()) ? m_moleFractions[i] : (1.0 / n);
				auto& ccp = m_compounds[i].getConstantProperties();
				omegaAvg += xi * ccp.acentricFactor;
				TcProd *= pow(ccp.criticalTemperature, xi);
			}
			if (TcProd <= 0.0 || T >= TcProd) return 0.0;
			double Tr = T / TcProd;
			return R * TcProd * (7.08 * pow(1.0 - Tr, 0.354) + 10.95 * omegaAvg * pow(1.0 - Tr, 0.456));
		}
		else if (prop == L"surfaceTension")
		{
			double TcAvg = 0.0, PcAvg = 0.0, omegaAvg = 0.0;
			for (size_t i = 0; i < n; ++i)
			{
				double xi = (i < m_moleFractions.size()) ? m_moleFractions[i] : (1.0 / n);
				auto& ccp = m_compounds[i].getConstantProperties();
				TcAvg += xi * ccp.criticalTemperature;
				PcAvg += xi * ccp.criticalPressure;
				omegaAvg += xi * ccp.acentricFactor;
			}
			if (TcAvg <= 0.0 || T >= TcAvg) return 0.0;
			double Tr = T / TcAvg;
			double Pc_bar = PcAvg * 1e-5;
			double Q = 0.1196 * (1.0 + 0.148 * omegaAvg);
			return 1e-3 * Q * pow(TcAvg, 1.0 / 3.0) * pow(Pc_bar, 2.0 / 3.0) * pow(1.0 - Tr, 11.0 / 9.0);
		}
		else if (prop == L"viscosity")
		{
			double TcAvg = 0.0, omegaAvg = 0.0;
			for (size_t i = 0; i < n; ++i)
			{
				double xi = (i < m_moleFractions.size()) ? m_moleFractions[i] : (1.0 / n);
				auto& ccp = m_compounds[i].getConstantProperties();
				TcAvg += xi * ccp.criticalTemperature;
				omegaAvg += xi * ccp.acentricFactor;
			}
			if (TcAvg <= 0.0) return 0.0;
			double Tr = T / TcAvg;
			double Fc = 1.0 - 0.2756 * omegaAvg;
			double Tstar = 1.2593 * Tr;
			double OmegaV = 1.16145 * pow(Tstar, -0.14874) + 0.52487 * exp(-0.77320 * Tstar) + 2.16178 * exp(-2.43787 * Tstar);
			return 40.785 * Fc * sqrt(MWavg * T) / (pow(MWavg / (P * 1e-5 * MWavg * 1e-3 / (R * T)), 2.0 / 3.0) * OmegaV) * 1e-7;
		}

		return 0.0;
	}

	void CapeMaterialStream::setCapeMaterialObject(CapeMaterialObject* obj)
	{
		if (m_capeMaterialObj)
		{
			::COBIA::CapeOpenObject<CapeMaterialObject>::release(
				static_cast<void*>(m_capeMaterialObj));
			m_capeMaterialObj = nullptr;
		}

		if (obj)
		{
			m_capeMaterialObj = new CapeMaterialObject();
			::COBIA::CapeOpenObject<CapeMaterialObject>::addReference(
				static_cast<void*>(m_capeMaterialObj));
			m_capeMaterialObj->Initialize();
			m_capeMaterialObj->copyFrom(*obj);
		}

		m_flashed = false;
	}

}}