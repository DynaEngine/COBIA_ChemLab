#include "WaterPP.h"
#include "Water.h"
#include <algorithm>
#include <cwctype>

namespace WaterPP {

	WaterPPPropertyPackage::WaterPPPropertyPackage()
		: hasMaterial(false)
		, compoundsChecked(false)
	{
		initWaterConstants();
	}

	WaterPPPropertyPackage::~WaterPPPropertyPackage()
	{
	}

	void WaterPPPropertyPackage::initWaterConstants()
	{
		molWt = Water::MOLWT;
		critT = Water::TCRIT;
		critP = Water::PCRIT;

		constValues[L"criticalTemperature"] = Water::TCRIT;
		constValues[L"criticalPressure"] = Water::PCRIT * 1e6;
		constValues[L"criticalDensity"] = Water::RHOCRIT / (Water::MOLWT * 1e-3);
		constValues[L"criticalVolume"] = Water::MOLWT * 1e-3 / Water::RHOCRIT;
		constValues[L"molecularWeight"] = Water::MOLWT;
		constValues[L"triplePointTemperature"] = Water::TTRIP;
		constValues[L"triplePointPressure"] = Water::PTRIP * 1e6;
		constValues[L"normalBoilingPoint"] = Water::Tsat(101325.0e-6);
		constValues[L"acentricFactor"] = 0.344;
	}

	void WaterPPPropertyPackage::getPropertyPackageList(COBIA::CapeArrayString packageNames)
	{
		ICapeArrayString* raw = packageNames;
		ICapeString** data = nullptr;
		raw->vTbl->setsize(raw->me, 1, data);
		data[0]->vTbl->set(data[0]->me, COBIATEXT("WaterPP"), 7);
	}

	COBIA::CapeInterface WaterPPPropertyPackage::GetPropertyPackage(COBIA::CapeString packageName)
	{
		auto name = static_cast<std::wstring>(packageName);
		if (name == COBIATEXT("WaterPP") || name.empty()) {
			WaterPPPropertyPackage* newPP = new WaterPPPropertyPackage();
			return COBIA::CapeInterface(static_cast<COBIA::ICapeInterface*>(newPP));
		}
		throw COBIA::cape_open_error(COBIAERR_NoSuchItem);
	}

	void WaterPPPropertyPackage::SetMaterial(CapeThermoMaterial mat)
	{
		material = mat;
		hasMaterial = true;
		compoundsChecked = false;
	}

	void WaterPPPropertyPackage::UnsetMaterial()
	{
		material = CapeThermoMaterial();
		hasMaterial = false;
		compoundsChecked = false;
	}

	COBIA::CapeInteger WaterPPPropertyPackage::getNumCompounds()
	{
		return 1;
	}

	void WaterPPPropertyPackage::GetCompoundList(
		COBIA::CapeArrayString compIds,
		COBIA::CapeArrayString formulae,
		COBIA::CapeArrayString names,
		COBIA::CapeArrayReal boilTemps,
		COBIA::CapeArrayReal molwts,
		COBIA::CapeArrayString casnos)
	{
		ICapeArrayString* rawCompIds = compIds;
		ICapeArrayString* rawFormulae = formulae;
		ICapeArrayString* rawNames = names;
		ICapeArrayString* rawCasnos = casnos;

		ICapeString** compData = nullptr;
		ICapeString** formData = nullptr;
		ICapeString** nameData = nullptr;
		ICapeString** casData = nullptr;

		rawCompIds->vTbl->setsize(rawCompIds->me, 1, compData);
		rawFormulae->vTbl->setsize(rawFormulae->me, 1, formData);
		rawNames->vTbl->setsize(rawNames->me, 1, nameData);
		rawCasnos->vTbl->setsize(rawCasnos->me, 1, casData);

		boilTemps.resize(1);
		molwts.resize(1);

		compData[0]->vTbl->set(compData[0]->me, COBIATEXT("Water"), 5);
		formData[0]->vTbl->set(formData[0]->me, COBIATEXT("H2O"), 3);
		nameData[0]->vTbl->set(nameData[0]->me, COBIATEXT("Water"), 5);
		casData[0]->vTbl->set(casData[0]->me, COBIATEXT("7732-18-5"), 9);
		boilTemps[0] = Water::Tsat(101325.0e-6);
		molwts[0] = Water::MOLWT;
	}

	void WaterPPPropertyPackage::getConstPropList(COBIA::CapeArrayString props)
	{
		const CapeCharacter* propNames[] = {
			COBIATEXT("molecularWeight"),
			COBIATEXT("criticalTemperature"),
			COBIATEXT("criticalPressure"),
			COBIATEXT("criticalDensity"),
			COBIATEXT("criticalVolume"),
			COBIATEXT("triplePointTemperature"),
			COBIATEXT("triplePointPressure"),
			COBIATEXT("normalBoilingPoint"),
			COBIATEXT("acentricFactor"),
			COBIATEXT("casRegistryNumber"),
			COBIATEXT("chemicalFormula"),
			COBIATEXT("iupacName")
		};
		const int n = 12;

		ICapeArrayString* raw = props;
		ICapeString** data = nullptr;
		raw->vTbl->setsize(raw->me, n, data);

		for (int i = 0; i < n; ++i) {
			data[i]->vTbl->set(data[i]->me, propNames[i], static_cast<CapeSize>(wcslen(propNames[i])));
		}
	}

	void WaterPPPropertyPackage::GetCompoundConstant(
		COBIA::CapeArrayString props,
		COBIA::CapeArrayString compIds,
		COBIA::CapeBoolean& containsMissingValues,
		COBIA::CapeArrayValue propVals)
	{
		containsMissingValues = false;

		for (size_t i = 0; i < props.size(); ++i) {
			std::wstring propName = static_cast<std::wstring>(props[i]);
			std::wstring compId = static_cast<std::wstring>(compIds[i]);

			if (compId != L"Water" && !compId.empty()) {
				containsMissingValues = true;
				continue;
			}

			COBIA::CapeValue val = propVals[i];

			if (propName == L"molecularWeight") {
				val.setRealValue(Water::MOLWT);
			} else if (propName == L"criticalTemperature") {
				val.setRealValue(Water::TCRIT);
			} else if (propName == L"criticalPressure") {
				val.setRealValue(Water::PCRIT * 1e6);
			} else if (propName == L"criticalDensity") {
				val.setRealValue(Water::RHOCRIT / (Water::MOLWT * 1e-3));
			} else if (propName == L"criticalVolume") {
				val.setRealValue(Water::MOLWT * 1e-3 / Water::RHOCRIT);
			} else if (propName == L"triplePointTemperature") {
				val.setRealValue(Water::TTRIP);
			} else if (propName == L"triplePointPressure") {
				val.setRealValue(Water::PTRIP * 1e6);
			} else if (propName == L"normalBoilingPoint") {
				val.setRealValue(Water::Tsat(101325.0e-6));
			} else if (propName == L"acentricFactor") {
				val.setRealValue(0.344);
			} else if (propName == L"casRegistryNumber") {
				val.setStringValue(COBIATEXT("7732-18-5"), 9);
			} else if (propName == L"chemicalFormula") {
				val.setStringValue(COBIATEXT("H2O"), 3);
			} else if (propName == L"iupacName") {
				val.setStringValue(COBIATEXT("oxidane"), 7);
			} else {
				containsMissingValues = true;
			}
		}
	}

	void WaterPPPropertyPackage::getPDependentPropList(COBIA::CapeArrayString props)
	{
		ICapeArrayString* raw = props;
		ICapeString** data = nullptr;
		raw->vTbl->setsize(raw->me, 0, data);
	}

	void WaterPPPropertyPackage::GetPDependentProperty(
		COBIA::CapeArrayString props,
		COBIA::CapeReal pressure,
		COBIA::CapeArrayString compIds,
		COBIA::CapeBoolean& containsMissingValues,
		COBIA::CapeArrayReal propVals)
	{
		containsMissingValues = true;
	}

	bool WaterPPPropertyPackage::getTDepPropEnum(const std::wstring& name, TDependentProp& result)
	{
		if (name == L"surfaceTension") {
			result = TDP_SURFACE_TENSION_SAT_LIQUID; return true;
		} else if (name == L"thermalConductivityOfLiquid") {
			result = TDP_THERMAL_CONDUCTIVITY_LIQUID; return true;
		} else if (name == L"thermalConductivityOfVapor") {
			result = TDP_THERMAL_CONDUCTIVITY_VAPOR; return true;
		} else if (name == L"vaporPressure") {
			result = TDP_VAPOR_PRESSURE; return true;
		} else if (name == L"volumeChangeUponVaporization") {
			result = TDP_VOLUME_CHANGE_VAPORIZATION; return true;
		} else if (name == L"volumeOfLiquid") {
			result = TDP_VOLUME_LIQUID; return true;
		} else if (name == L"viscosityOfLiquid") {
			result = TDP_VISCOSITY_LIQUID; return true;
		} else if (name == L"viscosityOfVapor") {
			result = TDP_VISCOSITY_VAPOR; return true;
		} else if (name == L"idealGasEnthalpy") {
			result = TDP_IDEAL_GAS_ENTHALPY; return true;
		} else if (name == L"idealGasEntropy") {
			result = TDP_IDEAL_GAS_ENTROPY; return true;
		}
		return false;
	}

	bool WaterPPPropertyPackage::getSinglePhasePropEnum(const std::wstring& name, SinglePhaseProp& result)
	{
		if (name == L"density") {
			result = SPP_DENSITY; return true;
		} else if (name == L"enthalpy") {
			result = SPP_ENTHALPY; return true;
		} else if (name == L"entropy") {
			result = SPP_ENTROPY; return true;
		} else if (name == L"gibbsEnergy") {
			result = SPP_GIBBS_ENERGY; return true;
		} else if (name == L"heatCapacityCp") {
			result = SPP_HEAT_CAPACITY_CP; return true;
		} else if (name == L"heatCapacityCv") {
			result = SPP_HEAT_CAPACITY_CV; return true;
		} else if (name == L"internalEnergy") {
			result = SPP_INTERNAL_ENERGY; return true;
		} else if (name == L"molecularWeight") {
			result = SPP_MOLECULAR_WEIGHT; return true;
		} else if (name == L"thermalConductivity") {
			result = SPP_THERMAL_CONDUCTIVITY; return true;
		} else if (name == L"volume") {
			result = SPP_VOLUME; return true;
		} else if (name == L"viscosity") {
			result = SPP_VISCOSITY; return true;
		}
		return false;
	}

	double WaterPPPropertyPackage::calcIdealGasEnthalpy(double T)
	{
		const double Pref = 101325.0e-6;
		const double Tref = 373.15;
		Water w;
		w.SetStateTPX(T, 0.0, Water::TPVAPOR);
		return w.enthalpy() * Water::MOLWT;
	}

	double WaterPPPropertyPackage::calcIdealGasEntropy(double T)
	{
		const double A = 36542.0;
		const double B = -34.8040008545;
		const double C = 0.116820000112;
		const double D = -0.000130040003569;
		const double E = 5.25470014168e-008;
		const double Pref = 101325e-6;
		const double Tref = 373.15;
		double value = T * (B + T * (0.5 * C + T * (D / 3.0 + T * 0.25 * E)))
			+ A * log(T)
			- (Tref * (B + Tref * (0.5 * C + Tref * (D / 3.0 + Tref * 0.25 * E))) + A * log(Tref));
		value *= 1e-3;
		Water w;
		w.SetStateTPX(Tref, Pref, Water::TPVAPOR);
		value += w.entropy() * Water::MOLWT;
		return value;
	}

	void WaterPPPropertyPackage::getTDependentPropList(COBIA::CapeArrayString props)
	{
		const CapeCharacter* propNames[] = {
			COBIATEXT("surfaceTension"),
			COBIATEXT("thermalConductivityOfLiquid"),
			COBIATEXT("thermalConductivityOfVapor"),
			COBIATEXT("vaporPressure"),
			COBIATEXT("volumeChangeUponVaporization"),
			COBIATEXT("volumeOfLiquid"),
			COBIATEXT("viscosityOfLiquid"),
			COBIATEXT("viscosityOfVapor"),
			COBIATEXT("idealGasEnthalpy"),
			COBIATEXT("idealGasEntropy")
		};
		const int n = 10;

		ICapeArrayString* raw = props;
		ICapeString** data = nullptr;
		raw->vTbl->setsize(raw->me, n, data);

		for (int i = 0; i < n; ++i) {
			data[i]->vTbl->set(data[i]->me, propNames[i], static_cast<CapeSize>(wcslen(propNames[i])));
		}
	}

	void WaterPPPropertyPackage::GetTDependentProperty(
		COBIA::CapeArrayString props,
		COBIA::CapeReal temperature,
		COBIA::CapeArrayString compIds,
		COBIA::CapeBoolean& containsMissingValues,
		COBIA::CapeArrayReal propVals)
	{
		containsMissingValues = false;

		if (std::isnan(temperature) || std::isinf(temperature)) {
			containsMissingValues = true;
			return;
		}
		if (temperature <= 0.0) {
			containsMissingValues = true;
			return;
		}

		double Psat = Water::Psat(temperature);

		size_t nComps = compIds.size();

		for (size_t i = 0; i < nComps; ++i) {
			std::wstring compId = static_cast<std::wstring>(compIds[i]);
			if (compId != L"Water" && !compId.empty()) {
				containsMissingValues = true;
				propVals[i] = 0.0;
				continue;
			}

			std::wstring propName = static_cast<std::wstring>(props[std::min(i, props.size() - 1)]);
			TDependentProp propEnum;
			if (!getTDepPropEnum(propName, propEnum)) {
				containsMissingValues = true;
				propVals[i] = 0.0;
				continue;
			}

			double value = 0.0;
			Water w;

			switch (propEnum) {
			case TDP_SURFACE_TENSION_SAT_LIQUID:
				value = Water::surfaceTension(temperature);
				break;
			case TDP_THERMAL_CONDUCTIVITY_LIQUID:
				w.SetStateTPX(temperature, Psat, Water::TPLIQUID);
				value = w.thermalConductivity();
				break;
			case TDP_THERMAL_CONDUCTIVITY_VAPOR:
				w.SetStateTPX(temperature, Psat, Water::TPVAPOR);
				value = w.thermalConductivity();
				break;
			case TDP_VAPOR_PRESSURE:
				value = Psat * 1e6;
				break;
			case TDP_VOLUME_CHANGE_VAPORIZATION: {
				w.SetStateTPX(temperature, Psat, Water::TPLIQUID);
				double vLiq = w.volume();
				w.SetStateTPX(temperature, Psat, Water::TPVAPOR);
				double vVap = w.volume();
				value = (vVap - vLiq) * (Water::MOLWT * 1e-3);
				break;
			}
			case TDP_VOLUME_LIQUID:
				w.SetStateTPX(temperature, Psat, Water::TPLIQUID);
				value = w.volume() * (Water::MOLWT * 1e-3);
				break;
			case TDP_VISCOSITY_LIQUID:
				w.SetStateTPX(temperature, Psat, Water::TPLIQUID);
				value = w.viscosity();
				break;
			case TDP_VISCOSITY_VAPOR:
				w.SetStateTPX(temperature, Psat, Water::TPVAPOR);
				value = w.viscosity();
				break;
			case TDP_IDEAL_GAS_ENTHALPY:
				value = calcIdealGasEnthalpy(temperature);
				break;
			case TDP_IDEAL_GAS_ENTROPY:
				value = calcIdealGasEntropy(temperature);
				break;
			}
			propVals[i] = value;
		}
	}

	COBIA::CapeInteger WaterPPPropertyPackage::getNumPhases()
	{
		return 2;
	}

	void WaterPPPropertyPackage::GetPhaseList(
		COBIA::CapeArrayString phaseLabels,
		COBIA::CapeArrayString stateOfAggregation,
		COBIA::CapeArrayString keyCompoundId)
	{
		ICapeArrayString* rawPhaseLabels = phaseLabels;
		ICapeArrayString* rawStates = stateOfAggregation;
		ICapeArrayString* rawKeyIds = keyCompoundId;

		ICapeString** labelData = nullptr;
		ICapeString** stateData = nullptr;
		ICapeString** keyData = nullptr;

		rawPhaseLabels->vTbl->setsize(rawPhaseLabels->me, 2, labelData);
		rawStates->vTbl->setsize(rawStates->me, 2, stateData);
		rawKeyIds->vTbl->setsize(rawKeyIds->me, 2, keyData);

		labelData[0]->vTbl->set(labelData[0]->me, COBIATEXT("Vapor"), 5);
		stateData[0]->vTbl->set(stateData[0]->me, COBIATEXT("Vapor"), 5);
		keyData[0]->vTbl->set(keyData[0]->me, COBIATEXT("Water"), 5);

		labelData[1]->vTbl->set(labelData[1]->me, COBIATEXT("Liquid"), 6);
		stateData[1]->vTbl->set(stateData[1]->me, COBIATEXT("Liquid"), 6);
		keyData[1]->vTbl->set(keyData[1]->me, COBIATEXT("Water"), 5);
	}

	void WaterPPPropertyPackage::GetPhaseInfo(
		COBIA::CapeString phaseLabel,
		COBIA::CapeString phaseAttribute,
		COBIA::CapeValue value)
	{
		std::wstring attr = static_cast<std::wstring>(phaseAttribute);
		std::wstring phase = static_cast<std::wstring>(phaseLabel);

		if (attr == L"StateOfAggregation") {
			if (phase == L"Vapor") {
				value.setStringValue(COBIATEXT("Vapor"), 5);
			} else if (phase == L"Liquid") {
				value.setStringValue(COBIATEXT("Liquid"), 6);
			}
		} else if (attr == L"KeyCompoundId") {
			value.setStringValue(COBIATEXT("Water"), 5);
		} else if (attr == L"ExcludedCompoundId" ||
		           attr == L"DensityDescription" ||
		           attr == L"UserDescription" ||
		           attr == L"TypeOfSolid") {
			value.setStringValue(COBIATEXT(""), 0);
		} else {
			throw COBIA::cape_open_error(COBIAERR_InvalidArgument);
		}
	}

	void WaterPPPropertyPackage::CalcAndGetLnPhi(
		COBIA::CapeString phaseLabel,
		COBIA::CapeReal temperature,
		COBIA::CapeReal pressure,
		COBIA::CapeArrayReal moleFractions,
		COBIA::CapeInteger fFlags,
		COBIA::CapeArrayReal lnPhi,
		COBIA::CapeArrayReal lnPhiDT,
		COBIA::CapeArrayReal lnPhiDP,
		COBIA::CapeArrayReal lnPhiDn)
	{
		throw COBIA::cape_open_error(COBIAERR_NotImplemented);
	}

	void WaterPPPropertyPackage::getSinglePhasePropList(COBIA::CapeArrayString props)
	{
		const CapeCharacter* propNames[] = {
			COBIATEXT("density"),
			COBIATEXT("enthalpy"),
			COBIATEXT("entropy"),
			COBIATEXT("gibbsEnergy"),
			COBIATEXT("heatCapacityCp"),
			COBIATEXT("heatCapacityCv"),
			COBIATEXT("internalEnergy"),
			COBIATEXT("molecularWeight"),
			COBIATEXT("thermalConductivity"),
			COBIATEXT("volume"),
			COBIATEXT("viscosity")
		};
		const int n = 11;

		ICapeArrayString* raw = props;
		ICapeString** data = nullptr;
		raw->vTbl->setsize(raw->me, n, data);

		for (int i = 0; i < n; ++i) {
			data[i]->vTbl->set(data[i]->me, propNames[i], static_cast<CapeSize>(wcslen(propNames[i])));
		}
	}

	COBIA::CapeBoolean WaterPPPropertyPackage::CheckSinglePhasePropSpec(
		COBIA::CapeString property,
		COBIA::CapeString phaseLabel)
	{
		if (static_cast<std::wstring>(phaseLabel).empty()) return false;
		std::wstring phase = static_cast<std::wstring>(phaseLabel);
		if (phase != L"Vapor" && phase != L"Liquid") return false;

		SinglePhaseProp dummy;
		return getSinglePhasePropEnum(static_cast<std::wstring>(property), dummy);
	}

	void WaterPPPropertyPackage::CheckCompounds()
	{
		if (compoundsChecked) return;

		CapeThermoCompounds comps(material);

		COBIA::CapeArrayStringImpl compIds;
		COBIA::CapeArrayStringImpl formulae;
		COBIA::CapeArrayStringImpl names;
		COBIA::CapeArrayRealImpl boilTemps;
		COBIA::CapeArrayRealImpl molwts;
		COBIA::CapeArrayStringImpl casnos;

		comps.GetCompoundList(
			static_cast<ICapeArrayString*>(&compIds),
			static_cast<ICapeArrayString*>(&formulae),
			static_cast<ICapeArrayString*>(&names),
			static_cast<ICapeArrayReal*>(&boilTemps),
			static_cast<ICapeArrayReal*>(&molwts),
			static_cast<ICapeArrayString*>(&casnos));

		if (compIds.size() == 0) {
			compoundsChecked = true;
			return;
		}
		if (compIds.size() != 1) {
			throw COBIA::cape_open_error(COBIAERR_InvalidOperation);
		}
		if (static_cast<std::wstring>(compIds[0]) != L"Water") {
			throw COBIA::cape_open_error(COBIAERR_InvalidOperation);
		}

		compoundsChecked = true;
	}

	void WaterPPPropertyPackage::CheckOverallX()
	{
		COBIA::CapeStringImpl fracStr(COBIATEXT("fraction"));
		COBIA::CapeStringImpl basisM(COBIATEXT("mole"));
		COBIA::CapeArrayRealImpl comp;
		material.GetOverallProp(
			static_cast<ICapeString*>(&fracStr),
			static_cast<ICapeString*>(&basisM),
			static_cast<ICapeArrayReal*>(&comp));

		if (comp.size() != 1 || std::abs(comp[0] - 1.0) > 1e-3) {
			throw COBIA::cape_open_error(COBIAERR_InvalidOperation);
		}
	}

	void WaterPPPropertyPackage::CheckEquilibriumSetup()
	{
		CheckCompounds();
	}

	void WaterPPPropertyPackage::CalcSinglePhaseProp(
		COBIA::CapeArrayString props,
		COBIA::CapeString phaseLabel)
	{
		if (!hasMaterial) return;
		CheckCompounds();

		std::wstring phase = static_cast<std::wstring>(phaseLabel);
		Water::STATETP state;
		if (phase == L"Vapor") {
			state = Water::TPVAPOR;
		} else if (phase == L"Liquid") {
			state = Water::TPLIQUID;
		} else {
			return;
		}

		ICapeString* phaseLabelPtr = phaseLabel;

		CapeReal T = 0.0, P = 0.0;
		COBIA::CapeStringImpl tempPropStr(COBIATEXT("temperature"));
		COBIA::CapeStringImpl presPropStr(COBIATEXT("pressure"));
		COBIA::CapeStringImpl emptyBasis;

		try {
			COBIA::CapeArrayRealImpl t(1);
			material.GetSinglePhaseProp(
				static_cast<ICapeString*>(&tempPropStr),
				phaseLabelPtr,
				static_cast<ICapeString*>(nullptr),
				static_cast<ICapeArrayReal*>(&t));
			T = t[0];
		} catch (...) {
			throw COBIA::cape_open_error(COBIAERR_InvalidOperation);
		}
		try {
			COBIA::CapeArrayRealImpl p(1);
			material.GetSinglePhaseProp(
				static_cast<ICapeString*>(&presPropStr),
				phaseLabelPtr,
				static_cast<ICapeString*>(nullptr),
				static_cast<ICapeArrayReal*>(&p));
			P = p[0];
		} catch (...) {
			throw COBIA::cape_open_error(COBIAERR_InvalidOperation);
		}

		if (!std::isfinite(T) || T <= 0.0) {
			throw COBIA::cape_open_error(COBIAERR_InvalidOperation);
		}
		if (!std::isfinite(P) || P <= 0.0) {
			throw COBIA::cape_open_error(COBIAERR_InvalidOperation);
		}

		{
			COBIA::CapeStringImpl fracStr(COBIATEXT("fraction"));
			COBIA::CapeStringImpl fracBasis(COBIATEXT("mole"));
			COBIA::CapeArrayRealImpl fracVals;
			material.GetSinglePhaseProp(
				static_cast<ICapeString*>(&fracStr),
				phaseLabelPtr,
				static_cast<ICapeString*>(&fracBasis),
				static_cast<ICapeArrayReal*>(&fracVals));
			if (fracVals.size() != 1 || std::abs(fracVals[0] - 1.0) > 1e-3)
				throw COBIA::cape_open_error(COBIAERR_InvalidOperation);
		}

		double P_MPa = P * 1e-6;

		Water w;
		w.SetStateTPX(T, P_MPa, state);

		COBIA::CapeStringImpl propStr;
		COBIA::CapeStringImpl basisStrM(COBIATEXT("mole"));
		COBIA::CapeArrayRealImpl results(1);

		for (size_t i = 0; i < props.size(); ++i) {
			std::wstring propName = static_cast<std::wstring>(props[i]);
			SinglePhaseProp propEnum;
			if (!getSinglePhasePropEnum(propName, propEnum)) continue;

			double value = 0.0;
			switch (propEnum) {
			case SPP_DENSITY:
				value = 1.0 / (w.volume() * (Water::MOLWT * 1e-3)); break;
			case SPP_ENTHALPY:
				value = w.enthalpy() * Water::MOLWT; break;
			case SPP_ENTROPY:
				value = w.entropy() * Water::MOLWT; break;
			case SPP_GIBBS_ENERGY:
				value = w.gibbs() * Water::MOLWT; break;
			case SPP_HEAT_CAPACITY_CP:
				value = w.cP() * Water::MOLWT; break;
			case SPP_HEAT_CAPACITY_CV:
				value = w.cV() * Water::MOLWT; break;
			case SPP_INTERNAL_ENERGY:
				value = w.energy() * Water::MOLWT; break;
			case SPP_MOLECULAR_WEIGHT:
				value = Water::MOLWT; break;
			case SPP_THERMAL_CONDUCTIVITY:
				value = w.thermalConductivity(); break;
			case SPP_VOLUME:
				value = w.volume() * (Water::MOLWT * 1e-3); break;
			case SPP_VISCOSITY:
				value = w.viscosity(); break;
			}

			propStr = propName;
			results[0] = value;
			COBIA::CapeStringImpl* useBasis = &basisStrM;
			if (propEnum == SPP_THERMAL_CONDUCTIVITY ||
			    propEnum == SPP_VISCOSITY ||
			    propEnum == SPP_MOLECULAR_WEIGHT) {
				useBasis = &emptyBasis;
			}
			material.SetSinglePhaseProp(
				static_cast<ICapeString*>(&propStr),
				phaseLabelPtr,
				static_cast<ICapeString*>(useBasis),
				static_cast<ICapeArrayReal*>(&results));
		}
	}

	void WaterPPPropertyPackage::getTwoPhasePropList(COBIA::CapeArrayString props)
	{
		const CapeCharacter* propNames[] = {
			COBIATEXT("surfaceTension")
		};
		const int n = 1;

		ICapeArrayString* raw = props;
		ICapeString** data = nullptr;
		raw->vTbl->setsize(raw->me, n, data);

		for (int i = 0; i < n; ++i) {
			data[i]->vTbl->set(data[i]->me, propNames[i], static_cast<CapeSize>(wcslen(propNames[i])));
		}
	}

	COBIA::CapeBoolean WaterPPPropertyPackage::CheckTwoPhasePropSpec(
		COBIA::CapeString property,
		COBIA::CapeArrayString phaseLabels)
	{
		if (phaseLabels.size() != 2) return false;
		std::wstring p0 = static_cast<std::wstring>(phaseLabels[0]);
		std::wstring p1 = static_cast<std::wstring>(phaseLabels[1]);
		if (p0 == p1) return false;
		if ((p0 != L"Vapor" && p0 != L"Liquid") ||
		    (p1 != L"Vapor" && p1 != L"Liquid")) return false;

		std::wstring prop = static_cast<std::wstring>(property);
		return (prop == L"surfaceTension");
	}

	void WaterPPPropertyPackage::CalcTwoPhaseProp(
		COBIA::CapeArrayString props,
		COBIA::CapeArrayString phaseLabels)
	{
		if (!hasMaterial) return;
		if (props.size() == 0) return;
		CheckCompounds();

		CapeReal T = 0.0;
		COBIA::CapeStringImpl tempPropStr(COBIATEXT("temperature"));
		COBIA::CapeStringImpl emptyBasis;
		COBIA::CapeStringImpl phaseLiq(COBIATEXT("Liquid"));

		COBIA::CapeArrayRealImpl t(1);
		material.GetSinglePhaseProp(
			static_cast<ICapeString*>(&tempPropStr),
			static_cast<ICapeString*>(&phaseLiq),
			static_cast<ICapeString*>(nullptr),
			static_cast<ICapeArrayReal*>(&t));
		T = t[0];

		COBIA::CapeStringImpl propStr;
		COBIA::CapeArrayRealImpl results(1);

		for (size_t i = 0; i < props.size(); ++i) {
			std::wstring propName = static_cast<std::wstring>(props[i]);

			if (propName == L"surfaceTension") {
				results[0] = Water::surfaceTension(T);
				material.SetTwoPhaseProp(
					static_cast<ICapeString*>(props[i]),
					static_cast<ICapeArrayString*>(phaseLabels),
					static_cast<ICapeString*>(nullptr),
					static_cast<ICapeArrayReal*>(&results));
			}
		}
	}

	COBIA::CapeBoolean WaterPPPropertyPackage::CheckEquilibriumSpec(
		COBIA::CapeArrayString specification1,
		COBIA::CapeArrayString specification2,
		COBIA::CapeString solutionType)
	{
		return true;
	}

	void WaterPPPropertyPackage::CalcEquilibrium(
		COBIA::CapeArrayString specification1,
		COBIA::CapeArrayString specification2,
		COBIA::CapeString solutionType)
	{
		if (!hasMaterial) return;
		CheckEquilibriumSetup();

		std::wstring spec1 = static_cast<std::wstring>(specification1[0]);
		std::wstring spec2 = static_cast<std::wstring>(specification2[0]);
		for (auto& c : spec1) c = towlower(c);
		for (auto& c : spec2) c = towlower(c);

		bool haveT = false, haveP = false, haveVF = false, haveH = false, haveS = false;
		bool vfIsLiquid = false;
		std::wstring vfBasis = L"mole";
		for (int i = 0; i < 2; ++i) {
			const COBIA::CapeArrayString& specArr = (i == 0) ? specification1 : specification2;
			std::wstring s = (i == 0) ? spec1 : spec2;

			if (s.empty() || specArr.size() < 3) {
				throw COBIA::cape_open_error(COBIATEXT("EQ-EMPTY: empty or short spec"));
			}
			if (specArr.size() >= 4) {
				std::wstring compId = static_cast<std::wstring>(specArr[3]);
				if (!compId.empty())
					throw COBIA::cape_open_error(COBIATEXT("EQ-COMPID: compoundId not empty"));
			}
			std::wstring phase = static_cast<std::wstring>(specArr[2]);
			std::wstring basis = static_cast<std::wstring>(specArr[1]);
			for (auto& c : phase) c = towlower(c);
			for (auto& c : basis) c = towlower(c);

			if (s == L"temperature") {
				if (phase != L"overall" || !basis.empty() || haveT)
					throw COBIA::cape_open_error(COBIATEXT("EQ-T: invalid temperature spec"));
				haveT = true;
			}
			else if (s == L"pressure") {
				if (phase != L"overall" || !basis.empty() || haveP)
					throw COBIA::cape_open_error(COBIATEXT("EQ-P: invalid pressure spec"));
				haveP = true;
			}
			else if (s == L"phasefraction" || s == L"vaporfraction") {
				if (phase != L"vapor" && phase != L"liquid")
					throw COBIA::cape_open_error(COBIATEXT("EQ-VF-PHASE: invalid phaseFraction phase"));
				if (basis != L"mole" && basis != L"mass")
					throw COBIA::cape_open_error(COBIATEXT("EQ-VF-BASIS: invalid phaseFraction basis"));
				haveVF = true;
				vfIsLiquid = (phase == L"liquid");
				vfBasis = basis;
			}
			else if (s == L"enthalpy") {
				if (phase != L"overall")
					throw COBIA::cape_open_error(COBIATEXT("EQ-H: invalid enthalpy spec"));
				haveH = true;
			}
			else if (s == L"entropy") {
				if (phase != L"overall")
					throw COBIA::cape_open_error(COBIATEXT("EQ-S: invalid entropy spec"));
				haveS = true;
			}
			else {
				std::wstring msg = L"EQ-UNKNOWN: ";
				msg += s;
				throw COBIA::cape_open_error(msg.c_str());
			}
		}

		{
			std::wstring solType = static_cast<std::wstring>(solutionType);
			for (auto& c : solType) c = towlower(c);
			if (!solType.empty() && solType != L"unspecified") {
				if (solType == L"normal" && !haveVF)
					throw COBIA::cape_open_error(COBIATEXT("EQ-SOL: Normal requires VF"));
				if (solType != L"normal")
					throw COBIA::cape_open_error(COBIATEXT("EQ-SOL: unsupported solutionType"));
			}
		}

		COBIA::CapeStringImpl tempPropStr(COBIATEXT("temperature"));
		COBIA::CapeStringImpl presPropStr(COBIATEXT("pressure"));
		COBIA::CapeStringImpl fracPropStr(COBIATEXT("fraction"));
		COBIA::CapeStringImpl pfPropStr(COBIATEXT("phaseFraction"));
		COBIA::CapeStringImpl enthPropStr(COBIATEXT("enthalpy"));
		COBIA::CapeStringImpl entrPropStr(COBIATEXT("entropy"));
		COBIA::CapeStringImpl vfBasisStr(vfBasis.c_str());
		COBIA::CapeStringImpl basisStrM(COBIATEXT("mole"));
		COBIA::CapeStringImpl basisStrMass(COBIATEXT("mass"));
		COBIA::CapeStringImpl emptyBasis;
		COBIA::CapeStringImpl phaseVap(COBIATEXT("Vapor"));
		COBIA::CapeStringImpl phaseLiq(COBIATEXT("Liquid"));

		CapeReal overallT = 0.0, overallP = 0.0;

		if (haveT && haveP) {
			CapeReal T = 0.0, P = 0.0;
			COBIA::CapeArrayRealImpl X;
			material.GetOverallTPFraction(T, P, X);

			if (!std::isfinite(T) || T <= 0.0) {
				throw COBIA::cape_open_error(COBIATEXT("EQ-READ-T: invalid T from OverallTPFraction"));
			}
			if (!std::isfinite(P) || P <= 0.0) {
				throw COBIA::cape_open_error(COBIATEXT("EQ-READ-P: invalid P from OverallTPFraction"));
			}
			overallT = T;
			overallP = P;
		} else {
			CheckOverallX();
			if (haveT) {
				COBIA::CapeArrayRealImpl t(1);
				material.GetOverallProp(
					static_cast<ICapeString*>(&tempPropStr),
					static_cast<ICapeString*>(&emptyBasis),
					static_cast<ICapeArrayReal*>(&t));
				overallT = t[0];
				if (!std::isfinite(overallT) || overallT <= 0.0) {
					throw COBIA::cape_open_error(COBIATEXT("EQ-GET-T: invalid T from GetOverallProp"));
				}
			}
			if (haveP) {
				COBIA::CapeArrayRealImpl p(1);
				material.GetOverallProp(
					static_cast<ICapeString*>(&presPropStr),
					static_cast<ICapeString*>(&emptyBasis),
					static_cast<ICapeArrayReal*>(&p));
				overallP = p[0];
				if (!std::isfinite(overallP) || overallP <= 0.0) {
					throw COBIA::cape_open_error(COBIATEXT("EQ-GET-P: invalid P from GetOverallProp"));
				}
			}
		}

		Water w;

		double resultT = overallT, resultP = overallP, resultVapFrac = 0.0;
		double resultHv = 0.0, resultSv = 0.0, resultRv = 0.0;
		double resultHl = 0.0, resultSl = 0.0, resultRl = 0.0;
		bool resultHaveVapor = true, resultHaveLiquid = true;

		if (haveT && haveP) {
			CapeReal T = 0.0, P = 0.0;
			COBIA::CapeArrayRealImpl X;
			material.GetOverallTPFraction(T, P, X);

			if (!std::isfinite(T) || T <= 0.0) {
				throw COBIA::cape_open_error(COBIATEXT("EQ-FLASH-T: invalid T for TP flash"));
			}
			if (!std::isfinite(P) || P <= 0.0) {
				throw COBIA::cape_open_error(COBIATEXT("EQ-FLASH-P: invalid P for TP flash"));
			}
			double P_MPa = P * 1e-6;

			w.SetStateTPX(T, P_MPa, Water::TPAUTO);
			resultVapFrac = w.GetQuality();
			resultVapFrac = std::max(0.0, std::min(1.0, resultVapFrac));

			resultHaveVapor = (resultVapFrac > 0.0);
			resultHaveLiquid = (resultVapFrac < 1.0);
			resultT = T;
			resultP = P;

			if (resultHaveVapor) {
				w.SetStateTPX(T, P_MPa, Water::TPVAPOR);
				resultHv = w.enthalpy() * Water::MOLWT;
				resultSv = w.entropy() * Water::MOLWT;
				resultRv = 1.0 / (w.volume() * (Water::MOLWT * 1e-3));
			}
			if (resultHaveLiquid) {
				w.SetStateTPX(T, P_MPa, Water::TPLIQUID);
				resultHl = w.enthalpy() * Water::MOLWT;
				resultSl = w.entropy() * Water::MOLWT;
				resultRl = 1.0 / (w.volume() * (Water::MOLWT * 1e-3));
			}
		}
		else if (haveT && haveVF) {
			double T = overallT;
			double vapFrac;
			COBIA::CapeArrayRealImpl vf(1);
			if (vfIsLiquid) {
				material.GetSinglePhaseProp(
					static_cast<ICapeString*>(&pfPropStr),
					static_cast<ICapeString*>(&phaseLiq),
					static_cast<ICapeString*>(&vfBasisStr),
					static_cast<ICapeArrayReal*>(&vf));
				vapFrac = 1.0 - vf[0];
			} else {
				material.GetSinglePhaseProp(
					static_cast<ICapeString*>(&pfPropStr),
					static_cast<ICapeString*>(&phaseVap),
					static_cast<ICapeString*>(&vfBasisStr),
					static_cast<ICapeArrayReal*>(&vf));
				vapFrac = vf[0];
			}
			vapFrac = std::max(0.0, std::min(1.0, vapFrac));

			double Psat_MPa = Water::Psat(T);
			double P = Psat_MPa * 1e6;
			resultT = T;
			resultP = P;
			resultVapFrac = vapFrac;

			w.SetStateTPX(T, Psat_MPa, Water::TPVAPOR);
			resultHv = w.enthalpy() * Water::MOLWT;
			resultSv = w.entropy() * Water::MOLWT;
			resultRv = 1.0 / (w.volume() * (Water::MOLWT * 1e-3));

			w.SetStateTPX(T, Psat_MPa, Water::TPLIQUID);
			resultHl = w.enthalpy() * Water::MOLWT;
			resultSl = w.entropy() * Water::MOLWT;
			resultRl = 1.0 / (w.volume() * (Water::MOLWT * 1e-3));
		}
		else if (haveP && haveVF) {
			double P = overallP;
			double P_MPa = P * 1e-6;
			double vapFrac;
			COBIA::CapeArrayRealImpl vf(1);
			if (vfIsLiquid) {
				material.GetSinglePhaseProp(
					static_cast<ICapeString*>(&pfPropStr),
					static_cast<ICapeString*>(&phaseLiq),
					static_cast<ICapeString*>(&vfBasisStr),
					static_cast<ICapeArrayReal*>(&vf));
				vapFrac = 1.0 - vf[0];
			} else {
				material.GetSinglePhaseProp(
					static_cast<ICapeString*>(&pfPropStr),
					static_cast<ICapeString*>(&phaseVap),
					static_cast<ICapeString*>(&vfBasisStr),
					static_cast<ICapeArrayReal*>(&vf));
				vapFrac = vf[0];
			}
			vapFrac = std::max(0.0, std::min(1.0, vapFrac));

			double T = Water::Tsat(P_MPa);
			resultT = T;
			resultP = P;
			resultVapFrac = vapFrac;

			w.SetStateTPX(T, P_MPa, Water::TPVAPOR);
			resultHv = w.enthalpy() * Water::MOLWT;
			resultSv = w.entropy() * Water::MOLWT;
			resultRv = 1.0 / (w.volume() * (Water::MOLWT * 1e-3));

			w.SetStateTPX(T, P_MPa, Water::TPLIQUID);
			resultHl = w.enthalpy() * Water::MOLWT;
			resultSl = w.entropy() * Water::MOLWT;
			resultRl = 1.0 / (w.volume() * (Water::MOLWT * 1e-3));
		}
		else if (haveP && haveH) {
			double P = overallP;
			double H = 0.0;
			COBIA::CapeArrayRealImpl h(1);
			material.GetOverallProp(
				static_cast<ICapeString*>(&enthPropStr),
				static_cast<ICapeString*>(&basisStrMass),
				static_cast<ICapeArrayReal*>(&h));
			H = h[0];
			if (!std::isfinite(H)) {
				throw COBIA::cape_open_error(COBIAERR_InvalidOperation);
			}
			double P_MPa = P * 1e-6;
			double Hmass = H;
			w.SetStatePH(P_MPa, Hmass * 1e-3);
			resultT = w.GetTemperature();
			resultP = P;
			resultVapFrac = w.GetQuality();
			resultVapFrac = std::max(0.0, std::min(1.0, resultVapFrac));
			resultHaveVapor = (resultVapFrac > 0.0);
			resultHaveLiquid = (resultVapFrac < 1.0);
			if (resultHaveVapor) {
				w.SetStateTPX(resultT, P_MPa, Water::TPVAPOR);
				resultHv = w.enthalpy() * Water::MOLWT;
				resultSv = w.entropy() * Water::MOLWT;
				resultRv = 1.0 / (w.volume() * (Water::MOLWT * 1e-3));
			}
			if (resultHaveLiquid) {
				w.SetStateTPX(resultT, P_MPa, Water::TPLIQUID);
				resultHl = w.enthalpy() * Water::MOLWT;
				resultSl = w.entropy() * Water::MOLWT;
				resultRl = 1.0 / (w.volume() * (Water::MOLWT * 1e-3));
			}
		}
		else if (haveT && haveH) {
			double T = overallT;
			double H = 0.0;
			COBIA::CapeArrayRealImpl h(1);
			material.GetOverallProp(
				static_cast<ICapeString*>(&enthPropStr),
				static_cast<ICapeString*>(&basisStrMass),
				static_cast<ICapeArrayReal*>(&h));
			H = h[0];
			if (!std::isfinite(H)) {
				throw COBIA::cape_open_error(COBIAERR_InvalidOperation);
			}
			double Hmass = H;
			w.SetStateTH(T, Hmass * 1e-3);
			double P_MPa = w.GetPressure();
			resultT = T;
			resultP = P_MPa * 1e6;
			resultVapFrac = w.GetQuality();
			resultVapFrac = std::max(0.0, std::min(1.0, resultVapFrac));
			resultHaveVapor = (resultVapFrac > 0.0);
			resultHaveLiquid = (resultVapFrac < 1.0);
			if (resultHaveVapor) {
				w.SetStateTPX(resultT, P_MPa, Water::TPVAPOR);
				resultHv = w.enthalpy() * Water::MOLWT;
				resultSv = w.entropy() * Water::MOLWT;
				resultRv = 1.0 / (w.volume() * (Water::MOLWT * 1e-3));
			}
			if (resultHaveLiquid) {
				w.SetStateTPX(resultT, P_MPa, Water::TPLIQUID);
				resultHl = w.enthalpy() * Water::MOLWT;
				resultSl = w.entropy() * Water::MOLWT;
				resultRl = 1.0 / (w.volume() * (Water::MOLWT * 1e-3));
			}
		}
		else if (haveP && haveS) {
			double P = overallP;
			double S = 0.0;
			COBIA::CapeArrayRealImpl s(1);
			material.GetOverallProp(
				static_cast<ICapeString*>(&entrPropStr),
				static_cast<ICapeString*>(&basisStrMass),
				static_cast<ICapeArrayReal*>(&s));
			S = s[0];
			if (!std::isfinite(S)) {
				throw COBIA::cape_open_error(COBIAERR_InvalidOperation);
			}
			double P_MPa = P * 1e-6;
			double Smass = S;
			w.SetStatePS(P_MPa, Smass * 1e-3);
			resultT = w.GetTemperature();
			resultP = P;
			resultVapFrac = w.GetQuality();
			resultVapFrac = std::max(0.0, std::min(1.0, resultVapFrac));
			resultHaveVapor = (resultVapFrac > 0.0);
			resultHaveLiquid = (resultVapFrac < 1.0);
			if (resultHaveVapor) {
				w.SetStateTPX(resultT, P_MPa, Water::TPVAPOR);
				resultHv = w.enthalpy() * Water::MOLWT;
				resultSv = w.entropy() * Water::MOLWT;
				resultRv = 1.0 / (w.volume() * (Water::MOLWT * 1e-3));
			}
			if (resultHaveLiquid) {
				w.SetStateTPX(resultT, P_MPa, Water::TPLIQUID);
				resultHl = w.enthalpy() * Water::MOLWT;
				resultSl = w.entropy() * Water::MOLWT;
				resultRl = 1.0 / (w.volume() * (Water::MOLWT * 1e-3));
			}
		}
		else if (haveT && haveS) {
			double T = overallT;
			double S = 0.0;
			COBIA::CapeArrayRealImpl s(1);
			material.GetOverallProp(
				static_cast<ICapeString*>(&entrPropStr),
				static_cast<ICapeString*>(&basisStrMass),
				static_cast<ICapeArrayReal*>(&s));
			S = s[0];
			if (!std::isfinite(S)) {
				throw COBIA::cape_open_error(COBIAERR_InvalidOperation);
			}
			double Smass = S;
			w.SetStateTS(T, Smass * 1e-3);
			double P_MPa = w.GetPressure();
			resultT = T;
			resultP = P_MPa * 1e6;
			resultVapFrac = w.GetQuality();
			resultVapFrac = std::max(0.0, std::min(1.0, resultVapFrac));
			resultHaveVapor = (resultVapFrac > 0.0);
			resultHaveLiquid = (resultVapFrac < 1.0);
			if (resultHaveVapor) {
				w.SetStateTPX(resultT, P_MPa, Water::TPVAPOR);
				resultHv = w.enthalpy() * Water::MOLWT;
				resultSv = w.entropy() * Water::MOLWT;
				resultRv = 1.0 / (w.volume() * (Water::MOLWT * 1e-3));
			}
			if (resultHaveLiquid) {
				w.SetStateTPX(resultT, P_MPa, Water::TPLIQUID);
				resultHl = w.enthalpy() * Water::MOLWT;
				resultSl = w.entropy() * Water::MOLWT;
				resultRl = 1.0 / (w.volume() * (Water::MOLWT * 1e-3));
			}
		}
		else if (haveH && haveVF) {
			double H = 0.0;
			double vapFrac;
			COBIA::CapeArrayRealImpl h(1);
			material.GetOverallProp(
				static_cast<ICapeString*>(&enthPropStr),
				static_cast<ICapeString*>(&basisStrMass),
				static_cast<ICapeArrayReal*>(&h));
			H = h[0];
			if (!std::isfinite(H)) {
				throw COBIA::cape_open_error(COBIAERR_InvalidOperation);
			}
			COBIA::CapeArrayRealImpl vf(1);
			if (vfIsLiquid) {
				material.GetSinglePhaseProp(
					static_cast<ICapeString*>(&pfPropStr),
					static_cast<ICapeString*>(&phaseLiq),
					static_cast<ICapeString*>(&vfBasisStr),
					static_cast<ICapeArrayReal*>(&vf));
				vapFrac = 1.0 - vf[0];
			} else {
				material.GetSinglePhaseProp(
					static_cast<ICapeString*>(&pfPropStr),
					static_cast<ICapeString*>(&phaseVap),
					static_cast<ICapeString*>(&vfBasisStr),
					static_cast<ICapeArrayReal*>(&vf));
				vapFrac = vf[0];
			}
			vapFrac = std::max(0.0, std::min(1.0, vapFrac));
			double Hmass = H;
			w.SetStateHVF(Hmass * 1e-3, vapFrac);
			double P_MPa = w.GetPressure();
			resultT = w.GetTemperature();
			resultP = P_MPa * 1e6;
			resultVapFrac = vapFrac;
			w.SetStateTPX(resultT, P_MPa, Water::TPVAPOR);
			resultHv = w.enthalpy() * Water::MOLWT;
			resultSv = w.entropy() * Water::MOLWT;
			resultRv = 1.0 / (w.volume() * (Water::MOLWT * 1e-3));
			w.SetStateTPX(resultT, P_MPa, Water::TPLIQUID);
			resultHl = w.enthalpy() * Water::MOLWT;
			resultSl = w.entropy() * Water::MOLWT;
			resultRl = 1.0 / (w.volume() * (Water::MOLWT * 1e-3));
		}
		else if (haveS && haveVF) {
			double S = 0.0;
			double vapFrac;
			COBIA::CapeArrayRealImpl s(1);
			material.GetOverallProp(
				static_cast<ICapeString*>(&entrPropStr),
				static_cast<ICapeString*>(&basisStrMass),
				static_cast<ICapeArrayReal*>(&s));
			S = s[0];
			if (!std::isfinite(S)) {
				throw COBIA::cape_open_error(COBIAERR_InvalidOperation);
			}
			COBIA::CapeArrayRealImpl vf(1);
			if (vfIsLiquid) {
				material.GetSinglePhaseProp(
					static_cast<ICapeString*>(&pfPropStr),
					static_cast<ICapeString*>(&phaseLiq),
					static_cast<ICapeString*>(&vfBasisStr),
					static_cast<ICapeArrayReal*>(&vf));
				vapFrac = 1.0 - vf[0];
			} else {
				material.GetSinglePhaseProp(
					static_cast<ICapeString*>(&pfPropStr),
					static_cast<ICapeString*>(&phaseVap),
					static_cast<ICapeString*>(&vfBasisStr),
					static_cast<ICapeArrayReal*>(&vf));
				vapFrac = vf[0];
			}
			vapFrac = std::max(0.0, std::min(1.0, vapFrac));
			double Smass = S;
			w.SetStateSVF(Smass * 1e-3, vapFrac);
			double P_MPa = w.GetPressure();
			resultT = w.GetTemperature();
			resultP = P_MPa * 1e6;
			resultVapFrac = vapFrac;
			w.SetStateTPX(resultT, P_MPa, Water::TPVAPOR);
			resultHv = w.enthalpy() * Water::MOLWT;
			resultSv = w.entropy() * Water::MOLWT;
			resultRv = 1.0 / (w.volume() * (Water::MOLWT * 1e-3));
			w.SetStateTPX(resultT, P_MPa, Water::TPLIQUID);
			resultHl = w.enthalpy() * Water::MOLWT;
			resultSl = w.entropy() * Water::MOLWT;
			resultRl = 1.0 / (w.volume() * (Water::MOLWT * 1e-3));
		}
		else {
			throw COBIA::cape_open_error(COBIAERR_InvalidOperation);
		}

		if (!std::isfinite(resultT) || resultT <= 0.0) {
			throw COBIA::cape_open_error(COBIAERR_InvalidOperation);
		}
		if (!std::isfinite(resultP) || resultP <= 0.0) {
			throw COBIA::cape_open_error(COBIAERR_InvalidOperation);
		}

		int nPhases = 0;
		if (resultHaveVapor) ++nPhases;
		if (resultHaveLiquid) ++nPhases;
		COBIA::CapeArrayStringImpl phaseLabels(nPhases);
		COBIA::CapeArrayEnumerationImpl<CapePhaseStatus> phaseStatus;
		phaseStatus.resize(nPhases);
		int pi = 0;
		if (resultHaveVapor) {
			phaseLabels[pi] = COBIATEXT("Vapor");
			phaseStatus[pi] = CAPE_ATEQUILIBRIUM;
			++pi;
		}
		if (resultHaveLiquid) {
			phaseLabels[pi] = COBIATEXT("Liquid");
			phaseStatus[pi] = CAPE_ATEQUILIBRIUM;
			++pi;
		}
		material.SetPresentPhases(
			static_cast<ICapeArrayString*>(&phaseLabels),
			static_cast<ICapeArrayEnumeration*>(&phaseStatus));

		COBIA::CapeStringImpl fracStr(COBIATEXT("fraction"));
		COBIA::CapeStringImpl pfStr(COBIATEXT("phaseFraction"));
		COBIA::CapeStringImpl densStr(COBIATEXT("density"));
		COBIA::CapeStringImpl enthStr(COBIATEXT("enthalpy"));
		COBIA::CapeStringImpl entrStr(COBIATEXT("entropy"));

		if (resultHaveVapor) {
			{
				COBIA::CapeArrayRealImpl v(1); v[0] = 1.0;
				material.SetSinglePhaseProp(
					static_cast<ICapeString*>(&fracStr), static_cast<ICapeString*>(&phaseVap),
					static_cast<ICapeString*>(&basisStrM), static_cast<ICapeArrayReal*>(&v));
			}
			{
				COBIA::CapeArrayRealImpl v(1); v[0] = resultVapFrac;
				material.SetSinglePhaseProp(
					static_cast<ICapeString*>(&pfStr), static_cast<ICapeString*>(&phaseVap),
					static_cast<ICapeString*>(&basisStrM), static_cast<ICapeArrayReal*>(&v));
			}
			{
				COBIA::CapeArrayRealImpl v(1); v[0] = resultT;
				material.SetSinglePhaseProp(
					static_cast<ICapeString*>(&tempPropStr), static_cast<ICapeString*>(&phaseVap),
					static_cast<ICapeString*>(nullptr), static_cast<ICapeArrayReal*>(&v));
			}
			{
				COBIA::CapeArrayRealImpl v(1); v[0] = resultP;
				material.SetSinglePhaseProp(
					static_cast<ICapeString*>(&presPropStr), static_cast<ICapeString*>(&phaseVap),
					static_cast<ICapeString*>(nullptr), static_cast<ICapeArrayReal*>(&v));
			}
			{
				COBIA::CapeArrayRealImpl v(1); v[0] = resultHv;
				material.SetSinglePhaseProp(
					static_cast<ICapeString*>(&enthStr), static_cast<ICapeString*>(&phaseVap),
					static_cast<ICapeString*>(&basisStrM), static_cast<ICapeArrayReal*>(&v));
			}
			{
				COBIA::CapeArrayRealImpl v(1); v[0] = resultSv;
				material.SetSinglePhaseProp(
					static_cast<ICapeString*>(&entrStr), static_cast<ICapeString*>(&phaseVap),
					static_cast<ICapeString*>(&basisStrM), static_cast<ICapeArrayReal*>(&v));
			}
			{
				COBIA::CapeArrayRealImpl v(1); v[0] = resultRv;
				material.SetSinglePhaseProp(
					static_cast<ICapeString*>(&densStr), static_cast<ICapeString*>(&phaseVap),
					static_cast<ICapeString*>(&basisStrM), static_cast<ICapeArrayReal*>(&v));
			}
		}
		if (resultHaveLiquid) {
			{
				COBIA::CapeArrayRealImpl v(1); v[0] = 1.0;
				material.SetSinglePhaseProp(
					static_cast<ICapeString*>(&fracStr), static_cast<ICapeString*>(&phaseLiq),
					static_cast<ICapeString*>(&basisStrM), static_cast<ICapeArrayReal*>(&v));
			}
			{
				COBIA::CapeArrayRealImpl v(1); v[0] = 1.0 - resultVapFrac;
				material.SetSinglePhaseProp(
					static_cast<ICapeString*>(&pfStr), static_cast<ICapeString*>(&phaseLiq),
					static_cast<ICapeString*>(&basisStrM), static_cast<ICapeArrayReal*>(&v));
			}
			{
				COBIA::CapeArrayRealImpl v(1); v[0] = resultT;
				material.SetSinglePhaseProp(
					static_cast<ICapeString*>(&tempPropStr), static_cast<ICapeString*>(&phaseLiq),
					static_cast<ICapeString*>(nullptr), static_cast<ICapeArrayReal*>(&v));
			}
			{
				COBIA::CapeArrayRealImpl v(1); v[0] = resultP;
				material.SetSinglePhaseProp(
					static_cast<ICapeString*>(&presPropStr), static_cast<ICapeString*>(&phaseLiq),
					static_cast<ICapeString*>(nullptr), static_cast<ICapeArrayReal*>(&v));
			}
			{
				COBIA::CapeArrayRealImpl v(1); v[0] = resultHl;
				material.SetSinglePhaseProp(
					static_cast<ICapeString*>(&enthStr), static_cast<ICapeString*>(&phaseLiq),
					static_cast<ICapeString*>(&basisStrM), static_cast<ICapeArrayReal*>(&v));
			}
			{
				COBIA::CapeArrayRealImpl v(1); v[0] = resultSl;
				material.SetSinglePhaseProp(
					static_cast<ICapeString*>(&entrStr), static_cast<ICapeString*>(&phaseLiq),
					static_cast<ICapeString*>(&basisStrM), static_cast<ICapeArrayReal*>(&v));
			}
			{
				COBIA::CapeArrayRealImpl v(1); v[0] = resultRl;
				material.SetSinglePhaseProp(
					static_cast<ICapeString*>(&densStr), static_cast<ICapeString*>(&phaseLiq),
					static_cast<ICapeString*>(&basisStrM), static_cast<ICapeArrayReal*>(&v));
			}
		}

		if (!haveT) {
			COBIA::CapeArrayRealImpl overallVals(1);
			overallVals[0] = resultT;
			material.SetOverallProp(
				static_cast<ICapeString*>(&tempPropStr),
				static_cast<ICapeString*>(nullptr),
				static_cast<ICapeArrayReal*>(&overallVals));
		}
		if (!haveP) {
			COBIA::CapeArrayRealImpl overallVals(1);
			overallVals[0] = resultP;
			material.SetOverallProp(
				static_cast<ICapeString*>(&presPropStr),
				static_cast<ICapeString*>(nullptr),
				static_cast<ICapeArrayReal*>(&overallVals));
		}

		{
			COBIA::CapeArrayRealImpl overallVals(1);
			overallVals[0] = 1.0;
			material.SetOverallProp(
				static_cast<ICapeString*>(&fracStr),
				static_cast<ICapeString*>(&basisStrM),
				static_cast<ICapeArrayReal*>(&overallVals));
		}

	}

	void WaterPPPropertyPackage::setPhaseResults(
		COBIA::CapeArrayStringImpl& phaseLabels,
		COBIA::CapeArrayEnumerationImpl<CapePhaseStatus>& phaseStatus,
		double vapFrac, double T, double P,
		double Hv, double Sv, double Rv,
		double Hl, double Sl, double Rl,
		bool haveVapor, bool haveLiquid)
	{
		int phaseCount = 0;
		if (haveVapor) ++phaseCount;
		if (haveLiquid) ++phaseCount;

		COBIA::CapeArrayStringImpl presentPhases(phaseCount);
		COBIA::CapeArrayEnumerationImpl<CapePhaseStatus> presentStatus;
		presentStatus.resize(phaseCount);

		int idx = 0;
		if (haveVapor) {
			presentPhases[idx] = phaseLabels[0];
			presentStatus[idx] = CAPE_ATEQUILIBRIUM;
			++idx;
		}
		if (haveLiquid) {
			presentPhases[idx] = phaseLabels[1];
			presentStatus[idx] = CAPE_ATEQUILIBRIUM;
			++idx;
		}

		material.SetPresentPhases(
			static_cast<ICapeArrayString*>(&presentPhases),
			static_cast<ICapeArrayEnumeration*>(&presentStatus));

		COBIA::CapeStringImpl tempStr(COBIATEXT("temperature"));
		COBIA::CapeStringImpl presStr(COBIATEXT("pressure"));
		COBIA::CapeStringImpl fracStr(COBIATEXT("fraction"));
		COBIA::CapeStringImpl pfStr(COBIATEXT("phaseFraction"));
		COBIA::CapeStringImpl enthStr(COBIATEXT("enthalpy"));
		COBIA::CapeStringImpl entrStr(COBIATEXT("entropy"));
		COBIA::CapeStringImpl densStr(COBIATEXT("density"));
		COBIA::CapeStringImpl basisM(COBIATEXT("mole"));
		COBIA::CapeStringImpl emptyB;

		if (haveVapor) {
			COBIA::CapeArrayRealImpl comp(1); comp[0] = 1.0;
			material.SetSinglePhaseProp(
				static_cast<ICapeString*>(&fracStr),
				static_cast<ICapeString*>(phaseLabels[0]),
				static_cast<ICapeString*>(&basisM),
				static_cast<ICapeArrayReal*>(&comp));

			COBIA::CapeArrayRealImpl pfV(1); pfV[0] = vapFrac;
			material.SetSinglePhaseProp(
				static_cast<ICapeString*>(&pfStr),
				static_cast<ICapeString*>(phaseLabels[0]),
				static_cast<ICapeString*>(&basisM),
				static_cast<ICapeArrayReal*>(&pfV));

			COBIA::CapeArrayRealImpl t(1); t[0] = T;
			COBIA::CapeArrayRealImpl p(1); p[0] = P;
			material.SetSinglePhaseProp(
				static_cast<ICapeString*>(&tempStr),
				static_cast<ICapeString*>(phaseLabels[0]),
				static_cast<ICapeString*>(nullptr),
				static_cast<ICapeArrayReal*>(&t));
			material.SetSinglePhaseProp(
				static_cast<ICapeString*>(&presStr),
				static_cast<ICapeString*>(phaseLabels[0]),
				static_cast<ICapeString*>(nullptr),
				static_cast<ICapeArrayReal*>(&p));

			COBIA::CapeArrayRealImpl h(1); h[0] = Hv;
			COBIA::CapeArrayRealImpl s(1); s[0] = Sv;
			COBIA::CapeArrayRealImpl d(1); d[0] = Rv;
			material.SetSinglePhaseProp(
				static_cast<ICapeString*>(&enthStr),
				static_cast<ICapeString*>(phaseLabels[0]),
				static_cast<ICapeString*>(&basisM),
				static_cast<ICapeArrayReal*>(&h));
			material.SetSinglePhaseProp(
				static_cast<ICapeString*>(&entrStr),
				static_cast<ICapeString*>(phaseLabels[0]),
				static_cast<ICapeString*>(&basisM),
				static_cast<ICapeArrayReal*>(&s));
			material.SetSinglePhaseProp(
				static_cast<ICapeString*>(&densStr),
				static_cast<ICapeString*>(phaseLabels[0]),
				static_cast<ICapeString*>(&basisM),
				static_cast<ICapeArrayReal*>(&d));
		}

		if (haveLiquid) {
			COBIA::CapeArrayRealImpl comp(1); comp[0] = 1.0;
			material.SetSinglePhaseProp(
				static_cast<ICapeString*>(&fracStr),
				static_cast<ICapeString*>(phaseLabels[1]),
				static_cast<ICapeString*>(&basisM),
				static_cast<ICapeArrayReal*>(&comp));

			COBIA::CapeArrayRealImpl pfL(1); pfL[0] = 1.0 - vapFrac;
			material.SetSinglePhaseProp(
				static_cast<ICapeString*>(&pfStr),
				static_cast<ICapeString*>(phaseLabels[1]),
				static_cast<ICapeString*>(&basisM),
				static_cast<ICapeArrayReal*>(&pfL));

			COBIA::CapeArrayRealImpl t(1); t[0] = T;
			COBIA::CapeArrayRealImpl p(1); p[0] = P;
			material.SetSinglePhaseProp(
				static_cast<ICapeString*>(&tempStr),
				static_cast<ICapeString*>(phaseLabels[1]),
				static_cast<ICapeString*>(nullptr),
				static_cast<ICapeArrayReal*>(&t));
			material.SetSinglePhaseProp(
				static_cast<ICapeString*>(&presStr),
				static_cast<ICapeString*>(phaseLabels[1]),
				static_cast<ICapeString*>(nullptr),
				static_cast<ICapeArrayReal*>(&p));

			COBIA::CapeArrayRealImpl h(1); h[0] = Hl;
			COBIA::CapeArrayRealImpl s(1); s[0] = Sl;
			COBIA::CapeArrayRealImpl d(1); d[0] = Rl;
			material.SetSinglePhaseProp(
				static_cast<ICapeString*>(&enthStr),
				static_cast<ICapeString*>(phaseLabels[1]),
				static_cast<ICapeString*>(&basisM),
				static_cast<ICapeArrayReal*>(&h));
			material.SetSinglePhaseProp(
				static_cast<ICapeString*>(&entrStr),
				static_cast<ICapeString*>(phaseLabels[1]),
				static_cast<ICapeString*>(&basisM),
				static_cast<ICapeArrayReal*>(&s));
			material.SetSinglePhaseProp(
				static_cast<ICapeString*>(&densStr),
				static_cast<ICapeString*>(phaseLabels[1]),
				static_cast<ICapeString*>(&basisM),
				static_cast<ICapeArrayReal*>(&d));
		}

		COBIA::CapeArrayRealImpl t(1); t[0] = T;
		COBIA::CapeArrayRealImpl p(1); p[0] = P;
		material.SetOverallProp(
			static_cast<ICapeString*>(&tempStr),
			static_cast<ICapeString*>(nullptr),
			static_cast<ICapeArrayReal*>(&t));
		material.SetOverallProp(
			static_cast<ICapeString*>(&presStr),
			static_cast<ICapeString*>(nullptr),
			static_cast<ICapeArrayReal*>(&p));
	}

	void WaterPPPropertyPackage::setFlashPhaseProps(
		Water& w, double T,
		COBIA::CapeArrayStringImpl& phaseLabels,
		COBIA::CapeArrayEnumerationImpl<CapePhaseStatus>& phaseStatus,
		double vapFrac, double resT, double resP,
		bool haveVapor, bool haveLiquid)
	{
		double Psat_MPa = Water::Psat(T);

		w.SetStateTPX(T, Psat_MPa, Water::TPVAPOR);
		double Hv = w.enthalpy() * Water::MOLWT;
		double Sv = w.entropy() * Water::MOLWT;
		double Rv = 1.0 / (w.volume() * (Water::MOLWT * 1e-3));

		w.SetStateTPX(T, Psat_MPa, Water::TPLIQUID);
		double Hl = w.enthalpy() * Water::MOLWT;
		double Sl = w.entropy() * Water::MOLWT;
		double Rl = 1.0 / (w.volume() * (Water::MOLWT * 1e-3));

		setPhaseResults(phaseLabels, phaseStatus, vapFrac, resT, resP,
			Hv, Sv, Rv, Hl, Sl, Rl, haveVapor, haveLiquid);
	}

	void WaterPPPropertyPackage::getUniversalConstantList(COBIA::CapeArrayString constantIdList)
	{
		const CapeCharacter* constNames[] = {
			COBIATEXT("universalGasConstant"),
			COBIATEXT("molarGasConstant"),
			COBIATEXT("avogadroConstant"),
			COBIATEXT("boltzmannConstant"),
			COBIATEXT("speedOfLightInVacuum"),
			COBIATEXT("standardAccelerationOfGravity")
		};
		const int n = 6;

		ICapeArrayString* raw = constantIdList;
		ICapeString** data = nullptr;
		raw->vTbl->setsize(raw->me, n, data);

		for (int i = 0; i < n; ++i) {
			data[i]->vTbl->set(data[i]->me, constNames[i], static_cast<CapeSize>(wcslen(constNames[i])));
		}
	}

	void WaterPPPropertyPackage::GetUniversalConstant(
		COBIA::CapeString constantId,
		COBIA::CapeValue constantValue)
	{
		std::wstring name = static_cast<std::wstring>(constantId);
		if (name == L"universalGasConstant") {
			constantValue.setRealValue(8.314);
		} else if (name == L"molarGasConstant") {
			constantValue.setRealValue(8.31447215);
		} else if (name == L"avogadroConstant") {
			constantValue.setRealValue(6.0221419947e23);
		} else if (name == L"boltzmannConstant") {
			constantValue.setRealValue(1.380650324e-23);
		} else if (name == L"speedOfLightInVacuum") {
			constantValue.setRealValue(299792458.1);
		} else if (name == L"standardAccelerationOfGravity") {
			constantValue.setRealValue(9.80665);
		}
	}

	void WaterPPPropertyPackage::Initialize()
	{
	}

	void WaterPPPropertyPackage::Terminate()
	{
		material = CapeThermoMaterial();
		hasMaterial = false;
	}

	CapeEditResult WaterPPPropertyPackage::Edit(CapeWindowId)
	{
		throw COBIA::cape_open_error(COBIAERR_NotImplemented);
	}

	CapeCollection<CapeParameter> WaterPPPropertyPackage::getParameters()
	{
		throw COBIA::cape_open_error(COBIAERR_NotImplemented);
	}

	void WaterPPPropertyPackage::putSimulationContext(CapeSimulationContext)
	{
	}

}

COBIA::CapePMCRegistrar dummy_registrar_for_linkage;

#define PMC_REGISTERFORALLUSERS
#define COBIA_PMC_ENTRY_POINTS
#include <COBIA_PMC.h>

namespace WaterPP {
	COBIA_PMC_REGISTER(WaterPPPropertyPackage);
}