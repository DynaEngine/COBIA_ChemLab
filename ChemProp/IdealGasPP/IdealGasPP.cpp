#include "IdealGasPP.h"
#include <algorithm>
#include <cmath>
#include <cwctype>

namespace IdealGas {

	IdealGasPropertyPackage::IdealGasPropertyPackage()
		: hasMaterial(false)
		, compoundsChecked(false)
		, R(8.314462618)
		, Tref(298.15)
		, Pref(101325.0)
	{
		initCompoundDatabase();
	}

	IdealGasPropertyPackage::~IdealGasPropertyPackage()
	{
	}

	void IdealGasPropertyPackage::initCompoundDatabase()
	{
		CompoundData comp;

		comp.id = L"Nitrogen";
		comp.formula = L"N2";
		comp.name = L"Nitrogen";
		comp.molecularWeight = 28.0134;
		comp.normalBoilingPoint = 77.35;
		comp.criticalTemperature = 126.2;
		comp.criticalPressure = 3390000.0;
		comp.acentricFactor = 0.037;
		comp.cpA = 28.883;
		comp.cpB = -0.00157;
		comp.cpC = 8.081e-6;
		comp.cpD = -8.633e-9;
		comp.cpE = 0.0;
		compoundDB.push_back(comp);

		comp.id = L"Oxygen";
		comp.formula = L"O2";
		comp.name = L"Oxygen";
		comp.molecularWeight = 31.9988;
		comp.normalBoilingPoint = 90.20;
		comp.criticalTemperature = 154.6;
		comp.criticalPressure = 5040000.0;
		comp.acentricFactor = 0.021;
		comp.cpA = 25.477;
		comp.cpB = 1.520e-2;
		comp.cpC = -7.155e-6;
		comp.cpD = 1.312e-9;
		comp.cpE = 0.0;
		compoundDB.push_back(comp);

		comp.id = L"CarbonDioxide";
		comp.formula = L"CO2";
		comp.name = L"Carbon Dioxide";
		comp.molecularWeight = 44.0095;
		comp.normalBoilingPoint = 194.70;
		comp.criticalTemperature = 304.2;
		comp.criticalPressure = 7380000.0;
		comp.acentricFactor = 0.225;
		comp.cpA = 19.797;
		comp.cpB = 7.344e-2;
		comp.cpC = -5.602e-5;
		comp.cpD = 1.715e-8;
		comp.cpE = 0.0;
		compoundDB.push_back(comp);

		comp.id = L"Water";
		comp.formula = L"H2O";
		comp.name = L"Water";
		comp.molecularWeight = 18.01528;
		comp.normalBoilingPoint = 373.15;
		comp.criticalTemperature = 647.3;
		comp.criticalPressure = 22120000.0;
		comp.acentricFactor = 0.344;
		comp.cpA = 32.243;
		comp.cpB = 1.924e-3;
		comp.cpC = 1.055e-5;
		comp.cpD = -3.596e-9;
		comp.cpE = 0.0;
		compoundDB.push_back(comp);

		comp.id = L"Methane";
		comp.formula = L"CH4";
		comp.name = L"Methane";
		comp.molecularWeight = 16.0425;
		comp.normalBoilingPoint = 111.66;
		comp.criticalTemperature = 190.6;
		comp.criticalPressure = 4600000.0;
		comp.acentricFactor = 0.008;
		comp.cpA = 19.887;
		comp.cpB = 5.024e-2;
		comp.cpC = 1.269e-5;
		comp.cpD = -1.101e-8;
		comp.cpE = 0.0;
		compoundDB.push_back(comp);

		comp.id = L"Hydrogen";
		comp.formula = L"H2";
		comp.name = L"Hydrogen";
		comp.molecularWeight = 2.01588;
		comp.normalBoilingPoint = 20.28;
		comp.criticalTemperature = 33.0;
		comp.criticalPressure = 1290000.0;
		comp.acentricFactor = -0.216;
		comp.cpA = 29.113;
		comp.cpB = -1.916e-3;
		comp.cpC = 4.004e-6;
		comp.cpD = -8.704e-10;
		comp.cpE = 0.0;
		compoundDB.push_back(comp);

		comp.id = L"CarbonMonoxide";
		comp.formula = L"CO";
		comp.name = L"Carbon Monoxide";
		comp.molecularWeight = 28.0101;
		comp.normalBoilingPoint = 81.66;
		comp.criticalTemperature = 132.9;
		comp.criticalPressure = 3500000.0;
		comp.acentricFactor = 0.049;
		comp.cpA = 28.156;
		comp.cpB = 1.675e-3;
		comp.cpC = 5.372e-6;
		comp.cpD = -2.222e-9;
		comp.cpE = 0.0;
		compoundDB.push_back(comp);

		comp.id = L"Ethane";
		comp.formula = L"C2H6";
		comp.name = L"Ethane";
		comp.molecularWeight = 30.0690;
		comp.normalBoilingPoint = 184.55;
		comp.criticalTemperature = 305.4;
		comp.criticalPressure = 4880000.0;
		comp.acentricFactor = 0.098;
		comp.cpA = 6.899;
		comp.cpB = 1.723e-1;
		comp.cpC = -6.406e-5;
		comp.cpD = 7.286e-9;
		comp.cpE = 0.0;
		compoundDB.push_back(comp);
	}

	void IdealGasPropertyPackage::getPropertyPackageList(COBIA::CapeArrayString packageNames)
	{
		ICapeArrayString* raw = packageNames;
		ICapeString** data = nullptr;
		raw->vTbl->setsize(raw->me, 1, data);
		data[0]->vTbl->set(data[0]->me, COBIATEXT("IdealGas"), 8);
	}

	COBIA::CapeInterface IdealGasPropertyPackage::GetPropertyPackage(COBIA::CapeString packageName)
	{
		auto name = static_cast<std::wstring>(packageName);
		if (name == COBIATEXT("IdealGas") || name.empty()) {
			IdealGasPropertyPackage* newPP = new IdealGasPropertyPackage();
			return COBIA::CapeInterface(static_cast<COBIA::ICapeInterface*>(newPP));
		}
		throw COBIA::cape_open_error(COBIAERR_NoSuchItem);
	}

	void IdealGasPropertyPackage::SetMaterial(CapeThermoMaterial mat)
	{
		material = mat;
		hasMaterial = true;
		compoundsChecked = false;
	}

	void IdealGasPropertyPackage::UnsetMaterial()
	{
		material = CapeThermoMaterial();
		hasMaterial = false;
		compoundsChecked = false;
	}

	COBIA::CapeInteger IdealGasPropertyPackage::getNumCompounds()
	{
		return static_cast<COBIA::CapeInteger>(compoundDB.size());
	}

	void IdealGasPropertyPackage::GetCompoundList(
		COBIA::CapeArrayString compIds,
		COBIA::CapeArrayString formulae,
		COBIA::CapeArrayString names,
		COBIA::CapeArrayReal boilTemps,
		COBIA::CapeArrayReal molwts,
		COBIA::CapeArrayString casnos)
	{
		CapeSize n = static_cast<CapeSize>(compoundDB.size());

		ICapeArrayString* rawCompIds = compIds;
		ICapeArrayString* rawFormulae = formulae;
		ICapeArrayString* rawNames = names;
		ICapeArrayString* rawCasnos = casnos;

		ICapeString** compData = nullptr;
		ICapeString** formData = nullptr;
		ICapeString** nameData = nullptr;
		ICapeString** casData = nullptr;

		rawCompIds->vTbl->setsize(rawCompIds->me, n, compData);
		rawFormulae->vTbl->setsize(rawFormulae->me, n, formData);
		rawNames->vTbl->setsize(rawNames->me, n, nameData);
		rawCasnos->vTbl->setsize(rawCasnos->me, n, casData);

		boilTemps.resize(n);
		molwts.resize(n);

		for (CapeSize i = 0; i < n; ++i) {
			const CompoundData& comp = compoundDB[i];
			compData[i]->vTbl->set(compData[i]->me, comp.id.c_str(), static_cast<CapeSize>(comp.id.size()));
			formData[i]->vTbl->set(formData[i]->me, comp.formula.c_str(), static_cast<CapeSize>(comp.formula.size()));
			nameData[i]->vTbl->set(nameData[i]->me, comp.name.c_str(), static_cast<CapeSize>(comp.name.size()));
			casData[i]->vTbl->set(casData[i]->me, COBIATEXT(""), 0);
			boilTemps[i] = comp.normalBoilingPoint;
			molwts[i] = comp.molecularWeight;
		}
	}

	void IdealGasPropertyPackage::getConstPropList(COBIA::CapeArrayString props)
	{
		const CapeCharacter* propNames[] = {
			COBIATEXT("molecularWeight"),
			COBIATEXT("criticalTemperature"),
			COBIATEXT("criticalPressure"),
			COBIATEXT("acentricFactor"),
			COBIATEXT("boilingPoint"),
			COBIATEXT("cpA"),
			COBIATEXT("cpB"),
			COBIATEXT("cpC"),
			COBIATEXT("cpD"),
			COBIATEXT("cpE")
		};
		const int n = 10;

		ICapeArrayString* raw = props;
		ICapeString** data = nullptr;
		raw->vTbl->setsize(raw->me, n, data);

		for (int i = 0; i < n; ++i) {
			data[i]->vTbl->set(data[i]->me, propNames[i], static_cast<CapeSize>(wcslen(propNames[i])));
		}
	}

	void IdealGasPropertyPackage::GetCompoundConstant(
		COBIA::CapeArrayString props,
		COBIA::CapeArrayString compIds,
		COBIA::CapeBoolean& containsMissingValues,
		COBIA::CapeArrayValue propVals)
	{
		containsMissingValues = false;

		for (size_t i = 0; i < props.size(); ++i) {
			std::wstring propName = static_cast<std::wstring>(props[i]);
			std::wstring compId = static_cast<std::wstring>(compIds[i]);

			bool found = false;
			for (const auto& comp : compoundDB) {
				if (comp.id == compId || comp.formula == compId) {
					COBIA::CapeValue val = propVals[i];

					if (propName == L"molecularWeight") {
						val.setRealValue(comp.molecularWeight);
					} else if (propName == L"criticalTemperature") {
						val.setRealValue(comp.criticalTemperature);
					} else if (propName == L"criticalPressure") {
						val.setRealValue(comp.criticalPressure);
					} else if (propName == L"acentricFactor") {
						val.setRealValue(comp.acentricFactor);
					} else if (propName == L"boilingPoint") {
						val.setRealValue(comp.normalBoilingPoint);
					} else if (propName == L"cpA") {
						val.setRealValue(comp.cpA);
					} else if (propName == L"cpB") {
						val.setRealValue(comp.cpB);
					} else if (propName == L"cpC") {
						val.setRealValue(comp.cpC);
					} else if (propName == L"cpD") {
						val.setRealValue(comp.cpD);
					} else if (propName == L"cpE") {
						val.setRealValue(comp.cpE);
					} else {
						containsMissingValues = true;
					}
					found = true;
					break;
				}
			}
			if (!found) {
				containsMissingValues = true;
			}
		}
	}

	void IdealGasPropertyPackage::getPDependentPropList(COBIA::CapeArrayString props)
	{
		props.resize(0);
	}

	void IdealGasPropertyPackage::GetPDependentProperty(
		COBIA::CapeArrayString props,
		COBIA::CapeReal pressure,
		COBIA::CapeArrayString compIds,
		COBIA::CapeBoolean& containsMissingValues,
		COBIA::CapeArrayReal propVals)
	{
		containsMissingValues = true;
	}

	void IdealGasPropertyPackage::getTDependentPropList(COBIA::CapeArrayString props)
	{
		const CapeCharacter* propNames[] = {
			COBIATEXT("idealGasEnthalpy"),
			COBIATEXT("idealGasEntropy")
		};
		const int n = 2;

		ICapeArrayString* raw = props;
		ICapeString** data = nullptr;
		raw->vTbl->setsize(raw->me, n, data);

		for (int i = 0; i < n; ++i) {
			data[i]->vTbl->set(data[i]->me, propNames[i], static_cast<CapeSize>(wcslen(propNames[i])));
		}
	}

	void IdealGasPropertyPackage::GetTDependentProperty(
		COBIA::CapeArrayString props,
		COBIA::CapeReal temperature,
		COBIA::CapeArrayString compIds,
		COBIA::CapeBoolean& containsMissingValues,
		COBIA::CapeArrayReal propVals)
	{
		containsMissingValues = false;

		size_t nProps = props.size();
		size_t nComps = compIds.size();

		for (size_t i = 0; i < nComps; ++i) {
			std::wstring propName;
			if (i < nProps) {
				propName = static_cast<std::wstring>(props[i]);
			} else if (nProps > 0) {
				propName = static_cast<std::wstring>(props[0]);
			} else {
				containsMissingValues = true;
				continue;
			}
			std::wstring compId = static_cast<std::wstring>(compIds[i]);

			bool found = false;
			for (const auto& comp : compoundDB) {
				if (comp.id == compId || comp.formula == compId) {
					if (propName == L"idealGasEnthalpy") {
						propVals[i] = calcCompoundEnthalpy(comp, temperature);
					} else if (propName == L"idealGasEntropy") {
						propVals[i] = calcCompoundEntropy(comp, temperature, Pref);
					} else if (propName == L"idealGasCp") {
						propVals[i] = calcCompoundCp(comp, temperature);
					} else {
						containsMissingValues = true;
					}
					found = true;
					break;
				}
			}
			if (!found) {
				containsMissingValues = true;
			}
		}
	}

	COBIA::CapeInteger IdealGasPropertyPackage::getNumPhases()
	{
		return 1;
	}

	void IdealGasPropertyPackage::GetPhaseList(
		COBIA::CapeArrayString phaseLabels,
		COBIA::CapeArrayString stateOfAggregation,
		COBIA::CapeArrayString keyCompoundId)
	{
		ICapeArrayString* rawLabels = phaseLabels;
		ICapeArrayString* rawState = stateOfAggregation;
		ICapeArrayString* rawKey = keyCompoundId;

		ICapeString** labelData = nullptr;
		ICapeString** stateData = nullptr;
		ICapeString** keyData = nullptr;

		rawLabels->vTbl->setsize(rawLabels->me, 1, labelData);
		rawState->vTbl->setsize(rawState->me, 1, stateData);
		rawKey->vTbl->setsize(rawKey->me, 1, keyData);

		labelData[0]->vTbl->set(labelData[0]->me, COBIATEXT("Vapor"), 5);
		stateData[0]->vTbl->set(stateData[0]->me, COBIATEXT("Vapor"), 5);
		keyData[0]->vTbl->set(keyData[0]->me, COBIATEXT("UNDEFINED"), 9);
	}

	void IdealGasPropertyPackage::GetPhaseInfo(
		COBIA::CapeString phaseLabel,
		COBIA::CapeString phaseAttribute,
		COBIA::CapeValue value)
	{
		ICapeValue* rawVal = static_cast<ICapeValue*>(value);
		if (!rawVal) return;
		rawVal->vTbl->clear(rawVal->me);

		std::wstring attr = static_cast<std::wstring>(phaseAttribute);

		const CapeCharacter* data = nullptr;
		CapeSize len = 0;

		for (auto& c : attr) c = towlower(c);

		if (attr == L"stateofaggregation") {
			data = COBIATEXT("Vapor");
			len = 5;
		} else if (attr == L"keycompoundid") {
			data = COBIATEXT("UNDEFINED");
			len = 9;
		} else if (attr == L"excludedcompoundid") {
			data = COBIATEXT("UNDEFINED");
			len = 9;
		} else if (attr == L"densitydescription") {
			data = COBIATEXT("UNDEFINED");
			len = 9;
		} else if (attr == L"typeofsolid") {
			data = COBIATEXT("UNDEFINED");
			len = 9;
		} else if (attr == L"userdescription") {
			data = COBIATEXT("Vapor phase");
			len = 11;
		} else {
			data = COBIATEXT("UNDEFINED");
			len = 9;
		}

		CapeResult res = rawVal->vTbl->setStringValue(rawVal->me, data, len);
		if (res != COBIAERR_NoError) {
			throw COBIA::cape_open_error(res);
		}
	}

	void IdealGasPropertyPackage::CalcAndGetLnPhi(
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
		int n = static_cast<int>(moleFractions.size());

		const CapeInteger CAPE_LOG_FUGACITY_COEFFICIENTS = 0x01;
		const CapeInteger CAPE_T_DERIVATIVE = 0x02;
		const CapeInteger CAPE_P_DERIVATIVE = 0x04;
		const CapeInteger CAPE_MOLE_NUMBERS_DERIVATIVES = 0x08;

		if (fFlags & CAPE_LOG_FUGACITY_COEFFICIENTS) {
			for (int i = 0; i < n; ++i) {
				lnPhi[i] = 0.0;
			}
		}

		if (fFlags & CAPE_T_DERIVATIVE) {
			for (int i = 0; i < n; ++i) {
				lnPhiDT[i] = 0.0;
			}
		}

		if (fFlags & CAPE_P_DERIVATIVE) {
			for (int i = 0; i < n; ++i) {
				lnPhiDP[i] = 0.0;
			}
		}

		if (fFlags & CAPE_MOLE_NUMBERS_DERIVATIVES) {
			for (int i = 0; i < n; ++i) {
				for (int j = 0; j < n; ++j) {
					lnPhiDn[i * n + j] = 0.0;
				}
			}
		}
	}

	void IdealGasPropertyPackage::CalcSinglePhaseProp(
		COBIA::CapeArrayString props,
		COBIA::CapeString phaseLabel)
	{
		if (!hasMaterial) return;
		CheckCompounds();

		ICapeString* phaseLabelPtr = phaseLabel;

		COBIA::CapeStringImpl tempPropStr(COBIATEXT("temperature"));
		COBIA::CapeStringImpl presPropStr(COBIATEXT("pressure"));
		COBIA::CapeStringImpl basisStrM(COBIATEXT("mole"));

		CapeReal T = 0.0, P = 0.0;
		try {
			COBIA::CapeArrayRealImpl t(1);
			material.GetSinglePhaseProp(
				static_cast<ICapeString*>(&tempPropStr),
				phaseLabelPtr,
				static_cast<ICapeString*>(nullptr),
				static_cast<ICapeArrayReal*>(&t));
			T = t[0];
		} catch (...) {
			throw COBIA::cape_open_error(COBIATEXT("IG-SP-T: failed to read T"));
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
			throw COBIA::cape_open_error(COBIATEXT("IG-SP-P: failed to read P"));
		}

		if (!std::isfinite(T) || T <= 0.0)
			throw COBIA::cape_open_error(COBIATEXT("IG-SP-TVAL: invalid T"));
		if (!std::isfinite(P) || P <= 0.0)
			throw COBIA::cape_open_error(COBIATEXT("IG-SP-PVAL: invalid P"));

		{
			COBIA::CapeStringImpl fracStr(COBIATEXT("fraction"));
			COBIA::CapeArrayRealImpl fracVals;
			material.GetSinglePhaseProp(
				static_cast<ICapeString*>(&fracStr),
				phaseLabelPtr,
				static_cast<ICapeString*>(&basisStrM),
				static_cast<ICapeArrayReal*>(&fracVals));
			int nC = static_cast<int>(compoundDB.size());
			if (static_cast<int>(fracVals.size()) != nC)
				throw COBIA::cape_open_error(COBIATEXT("IG-SP-FRAC: fraction count mismatch"));
			double sum = 0.0;
			for (int i = 0; i < nC; ++i) sum += fracVals[i];
			if (std::abs(sum - 1.0) > 1e-3)
				throw COBIA::cape_open_error(COBIATEXT("IG-SP-FRAC: fraction sum not 1"));
		}

		std::vector<double> x(compoundDB.size());
		for (size_t i = 0; i < compoundDB.size(); ++i) {
			x[i] = 1.0 / compoundDB.size();
		}
		{
			COBIA::CapeStringImpl fracStr(COBIATEXT("fraction"));
			COBIA::CapeArrayRealImpl fracVals;
			try {
				material.GetSinglePhaseProp(
					static_cast<ICapeString*>(&fracStr),
					phaseLabelPtr,
					static_cast<ICapeString*>(&basisStrM),
					static_cast<ICapeArrayReal*>(&fracVals));
				for (size_t i = 0; i < fracVals.size() && i < x.size(); ++i)
					x[i] = fracVals[i];
			} catch (...) {}
		}

		double MW = calcAverageMolecularWeight(x);
		double Cp = calcIdealGasCp(T, x);
		double H  = calcIdealGasEnthalpy(T, x);
		double S  = calcIdealGasEntropy(T, P, x);
		double rho = calcDensity(T, P, MW);
		double Cv = Cp - R;

		COBIA::CapeStringImpl propStr;
		COBIA::CapeStringImpl basisStrMol(COBIATEXT("mole"));
		COBIA::CapeArrayRealImpl results(1);

		for (size_t i = 0; i < props.size(); ++i) {
			std::wstring propName = static_cast<std::wstring>(props[i]);

			double value = 0.0;
			COBIA::CapeStringImpl* useBasis = &basisStrMol;

			if (propName == L"enthalpy") {
				value = H;
			} else if (propName == L"entropy") {
				value = S;
			} else if (propName == L"density") {
				value = rho;
			} else if (propName == L"heatCapacityCp") {
				value = Cp;
			} else if (propName == L"heatCapacityCv") {
				value = Cv;
			} else if (propName == L"compressibilityFactor") {
				value = 1.0;
				useBasis = nullptr;
			} else {
				continue;
			}

			propStr = propName;
			results[0] = value;
			material.SetSinglePhaseProp(
				static_cast<ICapeString*>(&propStr),
				phaseLabelPtr,
				static_cast<ICapeString*>(useBasis),
				static_cast<ICapeArrayReal*>(&results));
		}
	}

	void IdealGasPropertyPackage::CalcTwoPhaseProp(
		COBIA::CapeArrayString props,
		COBIA::CapeArrayString phaseLabels)
	{
	}

	COBIA::CapeBoolean IdealGasPropertyPackage::CheckSinglePhasePropSpec(
		COBIA::CapeString property,
		COBIA::CapeString phaseLabel)
	{
		std::wstring prop = static_cast<std::wstring>(property);
		return (prop == L"enthalpy" ||
			prop == L"entropy" ||
			prop == L"density" ||
			prop == L"heatCapacityCp" ||
			prop == L"heatCapacityCv" ||
			prop == L"compressibilityFactor");
	}

	COBIA::CapeBoolean IdealGasPropertyPackage::CheckTwoPhasePropSpec(
		COBIA::CapeString property,
		COBIA::CapeArrayString phaseLabels)
	{
		return false;
	}

	void IdealGasPropertyPackage::getSinglePhasePropList(COBIA::CapeArrayString props)
	{
		const CapeCharacter* propNames[] = {
			COBIATEXT("enthalpy"),
			COBIATEXT("entropy"),
			COBIATEXT("density"),
			COBIATEXT("heatCapacityCp"),
			COBIATEXT("heatCapacityCv"),
			COBIATEXT("compressibilityFactor")
		};
		const int n = 6;

		ICapeArrayString* raw = props;
		ICapeString** data = nullptr;
		raw->vTbl->setsize(raw->me, n, data);

		for (int i = 0; i < n; ++i) {
			data[i]->vTbl->set(data[i]->me, propNames[i], static_cast<CapeSize>(wcslen(propNames[i])));
		}
	}

	void IdealGasPropertyPackage::getTwoPhasePropList(COBIA::CapeArrayString props)
	{
		props.resize(0);
	}

	void IdealGasPropertyPackage::CalcEquilibrium(
		COBIA::CapeArrayString specification1,
		COBIA::CapeArrayString specification2,
		COBIA::CapeString solutionType)
	{
		if (!hasMaterial) {
			throw COBIA::cape_open_error(COBIAERR_NullPointer);
		}
		CheckCompounds();

		CapeInteger numComp = getNumCompounds();

		COBIA::CapeStringImpl fracPropStr(COBIATEXT("fraction"));
		COBIA::CapeStringImpl basisStrM(COBIATEXT("mole"));

		std::wstring spec1 = static_cast<std::wstring>(specification1[0]);
		std::wstring spec2 = static_cast<std::wstring>(specification2[0]);
		for (auto& c : spec1) c = towlower(c);
		for (auto& c : spec2) c = towlower(c);

		bool haveT = false, haveP = false, haveVF = false, haveH = false, haveS = false;
		for (int i = 0; i < 2; ++i) {
			const COBIA::CapeArrayString& specArr = (i == 0) ? specification1 : specification2;
			std::wstring s = (i == 0) ? spec1 : spec2;

			if (s.empty() || specArr.size() < 3) {
				throw COBIA::cape_open_error(COBIATEXT("IG-EQ-EMPTY: empty or short spec"));
			}
			if (specArr.size() >= 4) {
				std::wstring compId = static_cast<std::wstring>(specArr[3]);
				if (!compId.empty())
					throw COBIA::cape_open_error(COBIATEXT("IG-EQ-COMPID: compoundId not empty"));
			}
			std::wstring phase = static_cast<std::wstring>(specArr[2]);
			std::wstring basis = static_cast<std::wstring>(specArr[1]);
			for (auto& c : phase) c = towlower(c);
			for (auto& c : basis) c = towlower(c);

			if (s == L"temperature") {
				if (phase != L"overall" || !basis.empty() || haveT)
					throw COBIA::cape_open_error(COBIATEXT("IG-EQ-T: invalid temperature spec"));
				haveT = true;
			} else if (s == L"pressure") {
				if (phase != L"overall" || !basis.empty() || haveP)
					throw COBIA::cape_open_error(COBIATEXT("IG-EQ-P: invalid pressure spec"));
				haveP = true;
			} else if (s == L"enthalpy") {
				if (phase != L"overall")
					throw COBIA::cape_open_error(COBIATEXT("IG-EQ-H: invalid enthalpy spec"));
				haveH = true;
			} else if (s == L"entropy") {
				if (phase != L"overall")
					throw COBIA::cape_open_error(COBIATEXT("IG-EQ-S: invalid entropy spec"));
				haveS = true;
			} else if (s == L"phasefraction" || s == L"vaporfraction") {
				haveVF = true;
			} else {
				std::wstring msg = L"IG-EQ-UNKNOWN: " + s;
				throw COBIA::cape_open_error(msg.c_str());
			}
		}

		{
			std::wstring solType = static_cast<std::wstring>(solutionType);
			for (auto& c : solType) c = towlower(c);
			if (!solType.empty() && solType != L"unspecified") {
				if (solType == L"normal" && !haveVF)
					throw COBIA::cape_open_error(COBIATEXT("IG-EQ-SOL: Normal requires VF"));
				if (solType != L"normal")
					throw COBIA::cape_open_error(COBIATEXT("IG-EQ-SOL: unsupported solutionType"));
			}
		}

		// 2 read overall T, P, fraction from material
		CapeReal overallT = 0.0, overallP = 0.0;
		COBIA::CapeArrayRealImpl overallFrac(numComp);

		if (haveT && haveP) {
			CapeReal T = 0.0, P = 0.0;
			COBIA::CapeArrayRealImpl X;
			material.GetOverallTPFraction(T, P, X);
			if (!std::isfinite(T) || T <= 0.0)
				throw COBIA::cape_open_error(COBIATEXT("IG-EQ-READ-T: invalid T"));
			if (!std::isfinite(P) || P <= 0.0)
				throw COBIA::cape_open_error(COBIATEXT("IG-EQ-READ-P: invalid P"));
			overallT = T;
			overallP = P;
			for (CapeInteger i = 0; i < numComp && i < static_cast<CapeInteger>(X.size()); ++i)
				overallFrac[i] = X[i];
		} else {
			if (haveT) {
				COBIA::CapeStringImpl tStr(COBIATEXT("temperature"));
				COBIA::CapeArrayRealImpl t(1);
				material.GetOverallProp(
					static_cast<ICapeString*>(&tStr),
					static_cast<ICapeString*>(nullptr),
					static_cast<ICapeArrayReal*>(&t));
				overallT = t[0];
				if (!std::isfinite(overallT) || overallT <= 0.0)
					throw COBIA::cape_open_error(COBIATEXT("IG-EQ-GET-T: invalid T"));
			}
			if (haveP) {
				COBIA::CapeStringImpl pStr(COBIATEXT("pressure"));
				COBIA::CapeArrayRealImpl p(1);
				material.GetOverallProp(
					static_cast<ICapeString*>(&pStr),
					static_cast<ICapeString*>(nullptr),
					static_cast<ICapeArrayReal*>(&p));
				overallP = p[0];
				if (!std::isfinite(overallP) || overallP <= 0.0)
					throw COBIA::cape_open_error(COBIATEXT("IG-EQ-GET-P: invalid P"));
			}
			try {
				material.GetOverallProp(
					static_cast<ICapeString*>(&fracPropStr),
					static_cast<ICapeString*>(&basisStrM),
					static_cast<ICapeArrayReal*>(&overallFrac));
			} catch (...) {
				throw COBIA::cape_open_error(COBIATEXT("IG-EQ-FRAC: failed to read fractions"));
			}
		}
		{
			double sum = 0.0;
			for (CapeInteger i = 0; i < numComp; ++i) sum += overallFrac[i];
			if (std::abs(sum - 1.0) > 1e-3)
				throw COBIA::cape_open_error(COBIATEXT("IG-EQ-FRAC: fraction sum not 1"));
		}

		if (haveH && !haveT && !haveVF) {
			COBIA::CapeStringImpl enthStr(COBIATEXT("enthalpy"));
			COBIA::CapeArrayRealImpl h(1);
			material.GetOverallProp(
				static_cast<ICapeString*>(&enthStr),
				static_cast<ICapeString*>(&basisStrM),
				static_cast<ICapeArrayReal*>(&h));
			double targetH = h[0];
			double Cp = calcIdealGasCp(overallT, overallFrac);
			double Href = calcIdealGasEnthalpy(overallT, overallFrac);
			overallT += (targetH - Href) / Cp;
		}
		if (haveS && !haveT && !haveVF) {
			COBIA::CapeStringImpl entrStr(COBIATEXT("entropy"));
			COBIA::CapeArrayRealImpl s(1);
			material.GetOverallProp(
				static_cast<ICapeString*>(&entrStr),
				static_cast<ICapeString*>(&basisStrM),
				static_cast<ICapeArrayReal*>(&s));
			double targetS = s[0];
			double Cp = calcIdealGasCp(overallT, overallFrac);
			double Sref = calcIdealGasEntropy(overallT, overallP, overallFrac);
			overallT *= std::exp((targetS - Sref) / Cp);
		}
		if (haveH && haveVF) {
			COBIA::CapeStringImpl enthStr(COBIATEXT("enthalpy"));
			COBIA::CapeArrayRealImpl h(1);
			material.GetOverallProp(
				static_cast<ICapeString*>(&enthStr),
				static_cast<ICapeString*>(&basisStrM),
				static_cast<ICapeArrayReal*>(&h));
			double targetH = h[0];
			double Cp = calcIdealGasCp(overallT, overallFrac);
			double Href = calcIdealGasEnthalpy(overallT, overallFrac);
			overallT += (targetH - Href) / Cp;
		}
		if (haveS && haveVF) {
			COBIA::CapeStringImpl entrStr(COBIATEXT("entropy"));
			COBIA::CapeArrayRealImpl s(1);
			material.GetOverallProp(
				static_cast<ICapeString*>(&entrStr),
				static_cast<ICapeString*>(&basisStrM),
				static_cast<ICapeArrayReal*>(&s));
			double targetS = s[0];
			double Cp = calcIdealGasCp(overallT, overallFrac);
			double Sref = calcIdealGasEntropy(overallT, overallP, overallFrac);
			overallT *= std::exp((targetS - Sref) / Cp);
		}

		COBIA::CapeStringImpl pfPropStr(COBIATEXT("phaseFraction"));
		COBIA::CapeArrayStringImpl phaseLabels(1);
		phaseLabels[0] = COBIATEXT("Vapor");
		double resultVapFrac = 1.0;
		if (haveVF) {
			COBIA::CapeArrayRealImpl vf(1);
			material.GetSinglePhaseProp(
				static_cast<ICapeString*>(&pfPropStr),
				static_cast<ICapeString*>(phaseLabels[0]),
				static_cast<ICapeString*>(&basisStrM),
				static_cast<ICapeArrayReal*>(&vf));
			resultVapFrac = std::max(0.0, std::min(1.0, vf[0]));
		}

		// 3 set present phases
		COBIA::CapeArrayEnumerationImpl<CapePhaseStatus> phaseStatus;
		phaseStatus.resize(1);
		phaseStatus[0] = CAPE_ATEQUILIBRIUM;

		material.SetPresentPhases(
				static_cast<ICapeArrayString*>(&phaseLabels),
				static_cast<ICapeArrayEnumeration*>(&phaseStatus));

		// 4 set single-phase properties (Water package pattern)
		material.SetSinglePhaseProp(
			static_cast<ICapeString*>(&fracPropStr),
			static_cast<ICapeString*>(phaseLabels[0]),
			static_cast<ICapeString*>(&basisStrM),
			static_cast<ICapeArrayReal*>(&overallFrac));

		COBIA::CapeArrayRealImpl pfVal(1);
		pfVal[0] = resultVapFrac;
		material.SetSinglePhaseProp(
			static_cast<ICapeString*>(&pfPropStr),
			static_cast<ICapeString*>(phaseLabels[0]),
			static_cast<ICapeString*>(&basisStrM),
			static_cast<ICapeArrayReal*>(&pfVal));

		COBIA::CapeStringImpl tempStr(COBIATEXT("temperature"));
		COBIA::CapeArrayRealImpl tempVal(1);
		tempVal[0] = overallT;
		material.SetSinglePhaseProp(
				static_cast<ICapeString*>(&tempStr),
				static_cast<ICapeString*>(phaseLabels[0]),
				static_cast<ICapeString*>(nullptr),
				static_cast<ICapeArrayReal*>(&tempVal));

		COBIA::CapeStringImpl presStr(COBIATEXT("pressure"));
		COBIA::CapeArrayRealImpl presVal(1);
		presVal[0] = overallP;
		material.SetSinglePhaseProp(
				static_cast<ICapeString*>(&presStr),
				static_cast<ICapeString*>(phaseLabels[0]),
				static_cast<ICapeString*>(nullptr),
				static_cast<ICapeArrayReal*>(&presVal));

		material.SetOverallProp(
				static_cast<ICapeString*>(&tempStr),
				static_cast<ICapeString*>(nullptr),
				static_cast<ICapeArrayReal*>(&tempVal));
		material.SetOverallProp(
				static_cast<ICapeString*>(&presStr),
				static_cast<ICapeString*>(nullptr),
				static_cast<ICapeArrayReal*>(&presVal));
	}

	COBIA::CapeBoolean IdealGasPropertyPackage::CheckEquilibriumSpec(
		COBIA::CapeArrayString specification1,
		COBIA::CapeArrayString specification2,
		COBIA::CapeString solutionType)
	{
		return true;
	}

	void IdealGasPropertyPackage::GetUniversalConstant(
		COBIA::CapeString constantId,
		COBIA::CapeValue constantValue)
	{
		std::wstring id = static_cast<std::wstring>(constantId);
		if (id == L"molarGasConstant" || id == L"gasConstant" || id == L"R") {
			constantValue.setRealValue(R);
		} else if (id == L"boltzmannConstant" || id == L"Boltzmann") {
			constantValue.setRealValue(1.380649e-23);
		} else if (id == L"avogadroConstant" || id == L"Avogadro" || id == L"avogadroNumber") {
			constantValue.setRealValue(6.02214076e23);
		}
	}

	void IdealGasPropertyPackage::getUniversalConstantList(COBIA::CapeArrayString constantIdList)
	{
		const CapeCharacter* constants[] = {
			COBIATEXT("molarGasConstant"),
			COBIATEXT("boltzmannConstant"),
			COBIATEXT("avogadroConstant")
		};
		const int n = 3;

		ICapeArrayString* raw = constantIdList;
		ICapeString** data = nullptr;
		raw->vTbl->setsize(raw->me, n, data);

		for (int i = 0; i < n; ++i) {
			data[i]->vTbl->set(data[i]->me, constants[i], static_cast<CapeSize>(wcslen(constants[i])));
		}
	}

	CapeEditResult IdealGasPropertyPackage::Edit(CapeWindowId)
	{
		throw COBIA::cape_open_error(COBIAERR_NotImplemented);
	}

	void IdealGasPropertyPackage::Initialize()
	{
	}

	void IdealGasPropertyPackage::Terminate()
	{
	}

	CapeCollection<CapeParameter> IdealGasPropertyPackage::getParameters()
	{
		throw COBIA::cape_open_error(COBIAERR_NotImplemented);
	}

	void IdealGasPropertyPackage::putSimulationContext(CapeSimulationContext)
	{
	}

	void IdealGasPropertyPackage::CheckCompounds()
	{
		if (compoundsChecked) return;
		if (!hasMaterial) return;

		int nComp = static_cast<int>(compoundDB.size());
		COBIA::CapeStringImpl fracStr(COBIATEXT("fraction"));
		COBIA::CapeStringImpl basisStrM(COBIATEXT("mole"));
		COBIA::CapeArrayRealImpl fractions;
		try {
			material.GetOverallProp(
				static_cast<ICapeString*>(&fracStr),
				static_cast<ICapeString*>(&basisStrM),
				static_cast<ICapeArrayReal*>(&fractions));
		} catch (...) {
			compoundsChecked = true;
			return;
		}
		if (static_cast<int>(fractions.size()) != nComp)
			throw COBIA::cape_open_error(COBIATEXT("IG-CHK: fraction count mismatch"));
		double sum = 0.0;
		for (int i = 0; i < nComp; ++i) sum += fractions[i];
		if (std::abs(sum - 1.0) > 1e-3)
			throw COBIA::cape_open_error(COBIATEXT("IG-CHK: fraction sum not 1"));
		compoundsChecked = true;
	}

	double IdealGasPropertyPackage::calcCompoundCp(const CompoundData& comp, double T)
	{
		return comp.cpA + comp.cpB * T + comp.cpC * T * T +
			comp.cpD * T * T * T + comp.cpE * T * T * T * T;
	}

	double IdealGasPropertyPackage::calcCompoundEnthalpy(const CompoundData& comp, double T)
	{
		double dT = T - Tref;
		double T2 = T * T - Tref * Tref;
		double T3 = T * T * T - Tref * Tref * Tref;
		double T4 = T * T * T * T - Tref * Tref * Tref * Tref;
		double T5 = T * T * T * T * T - Tref * Tref * Tref * Tref * Tref;

		return comp.cpA * dT +
			comp.cpB * T2 / 2.0 +
			comp.cpC * T3 / 3.0 +
			comp.cpD * T4 / 4.0 +
			comp.cpE * T5 / 5.0;
	}

	double IdealGasPropertyPackage::calcCompoundEntropy(const CompoundData& comp, double T, double P)
	{
		double lnT = std::log(T / Tref);
		double dT = T - Tref;
		double T2 = T * T - Tref * Tref;
		double T3 = T * T * T - Tref * Tref * Tref;
		double T4 = T * T * T * T - Tref * Tref * Tref * Tref;

		return comp.cpA * lnT +
			comp.cpB * dT +
			comp.cpC * T2 / 2.0 +
			comp.cpD * T3 / 3.0 +
			comp.cpE * T4 / 4.0 -
			R * std::log(P / Pref);
	}

	double IdealGasPropertyPackage::calcIdealGasEnthalpy(double T, const std::vector<double>& x)
	{
		double H = 0.0;
		for (size_t i = 0; i < x.size() && i < compoundDB.size(); ++i) {
			H += x[i] * calcCompoundEnthalpy(compoundDB[i], T);
		}
		return H;
	}

	double IdealGasPropertyPackage::calcIdealGasEntropy(double T, double P, const std::vector<double>& x)
	{
		double S = 0.0;
		for (size_t i = 0; i < x.size() && i < compoundDB.size(); ++i) {
			if (x[i] > 1e-30) {
				S += x[i] * (calcCompoundEntropy(compoundDB[i], T, P) - R * std::log(x[i]));
			}
		}
		return S;
	}

	double IdealGasPropertyPackage::calcIdealGasCp(double T, const std::vector<double>& x)
	{
		double Cp = 0.0;
		for (size_t i = 0; i < x.size() && i < compoundDB.size(); ++i) {
			Cp += x[i] * calcCompoundCp(compoundDB[i], T);
		}
		return Cp;
	}

	double IdealGasPropertyPackage::calcAverageMolecularWeight(const std::vector<double>& x)
	{
		double MW = 0.0;
		for (size_t i = 0; i < x.size() && i < compoundDB.size(); ++i) {
			MW += x[i] * compoundDB[i].molecularWeight;
		}
		return MW;
	}

	double IdealGasPropertyPackage::calcDensity(double T, double P, double MW)
	{
		return P / (R * T);
	}

}

COBIA::CapePMCRegistrar dummy_registrar_for_linkage;

#define PMC_REGISTERFORALLUSERS
#define COBIA_PMC_ENTRY_POINTS
#include <COBIA_PMC.h>

namespace IdealGas {
	COBIA_PMC_REGISTER(IdealGasPropertyPackage);
}