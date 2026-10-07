#pragma once
#include <COBIA.h>
#include <CapeInterfaces_1_2.h>
#include <CapeInterfaceAdapters_1_2.h>
#include <vector>
#include <string>
#include <cmath>
#include <map>

namespace IdealGas {

	using namespace CAPEOPEN_1_2;

	struct CompoundData {
		std::wstring id;
		std::wstring formula;
		std::wstring name;
		double molecularWeight;
		double normalBoilingPoint;
		double criticalTemperature;
		double criticalPressure;
		double acentricFactor;
		double cpA, cpB, cpC, cpD, cpE;
	};

	class IdealGasPropertyPackage :
		public COBIA::CapeOpenObject<IdealGasPropertyPackage>,
		public CapeThermoPropertyPackageManagerAdapter<IdealGasPropertyPackage>,
		public CapeThermoMaterialContextAdapter<IdealGasPropertyPackage>,
		public CapeThermoCompoundsAdapter<IdealGasPropertyPackage>,
		public CapeThermoPhasesAdapter<IdealGasPropertyPackage>,
		public CapeThermoPropertyRoutineAdapter<IdealGasPropertyPackage>,
		public CapeThermoEquilibriumRoutineAdapter<IdealGasPropertyPackage>,
		public CapeThermoUniversalConstantAdapter<IdealGasPropertyPackage>,
		public CapeUtilitiesAdapter<IdealGasPropertyPackage> {

	public:
		const COBIA::CapeStringImpl getDescriptionForErrorSource() {
			return COBIATEXT("IdealGasPropertyPackage");
		}

		static const COBIA::CapeUUID getObjectUUID() {
			return COBIA::CapeUUID{{0xb5,0x01,0xe3,0x0b,0x5f,0x42,0x4d,0x57,0xbb,0xe0,0x0d,0x9a,0xfe,0xf9,0x55,0x5b}};
		}

		static void Register(COBIA::CapePMCRegistrar registrar) {
			registrar.putName(COBIATEXT("Ideal Gas Property Package"));
			registrar.putDescription(COBIATEXT("An ideal gas property package for educational purposes"));
			registrar.putCapeVersion(COBIATEXT("1.2"));
			registrar.putComponentVersion(COBIATEXT("1.0.0.0"));
			registrar.putAbout(COBIATEXT("Ideal Gas Property Package v1.0"));
			registrar.putVendorURL(COBIATEXT("https://github.com/example/idealgaspp"));
			registrar.putProgId(COBIATEXT("IdealGas.IdealGasPropertyPackage"));
			registrar.putVersionIndependentProgId(COBIATEXT("IdealGas.IdealGasPropertyPackage"));
			registrar.addCatID(CAPEOPEN::categoryId_PropertyPackageManager);
		}

		IdealGasPropertyPackage();
		~IdealGasPropertyPackage();

		void getPropertyPackageList(COBIA::CapeArrayString packageNames);
		COBIA::CapeInterface GetPropertyPackage(COBIA::CapeString packageName);

		void SetMaterial(CapeThermoMaterial material);
	void UnsetMaterial();

		COBIA::CapeInteger getNumCompounds();
		void GetCompoundList(COBIA::CapeArrayString compIds,
			COBIA::CapeArrayString formulae,
			COBIA::CapeArrayString names,
			COBIA::CapeArrayReal boilTemps,
			COBIA::CapeArrayReal molwts,
			COBIA::CapeArrayString casnos);

		void getConstPropList(COBIA::CapeArrayString props);
		void GetCompoundConstant(COBIA::CapeArrayString props,
			COBIA::CapeArrayString compIds,
			COBIA::CapeBoolean& containsMissingValues,
			COBIA::CapeArrayValue propVals);

		void getPDependentPropList(COBIA::CapeArrayString props);
		void GetPDependentProperty(COBIA::CapeArrayString props,
			COBIA::CapeReal pressure,
			COBIA::CapeArrayString compIds,
			COBIA::CapeBoolean& containsMissingValues,
			COBIA::CapeArrayReal propVals);

		void getTDependentPropList(COBIA::CapeArrayString props);
		void GetTDependentProperty(COBIA::CapeArrayString props,
			COBIA::CapeReal temperature,
			COBIA::CapeArrayString compIds,
			COBIA::CapeBoolean& containsMissingValues,
			COBIA::CapeArrayReal propVals);

		COBIA::CapeInteger getNumPhases();
		void GetPhaseList(COBIA::CapeArrayString phaseLabels,
			COBIA::CapeArrayString stateOfAggregation,
			COBIA::CapeArrayString keyCompoundId);
		void GetPhaseInfo(COBIA::CapeString phaseLabel,
			COBIA::CapeString phaseAttribute,
			COBIA::CapeValue value);

		void CalcAndGetLnPhi(COBIA::CapeString phaseLabel,
			COBIA::CapeReal temperature,
			COBIA::CapeReal pressure,
			COBIA::CapeArrayReal moleFractions,
			COBIA::CapeInteger fFlags,
			COBIA::CapeArrayReal lnPhi,
			COBIA::CapeArrayReal lnPhiDT,
			COBIA::CapeArrayReal lnPhiDP,
			COBIA::CapeArrayReal lnPhiDn);

		void CalcSinglePhaseProp(COBIA::CapeArrayString props,
			COBIA::CapeString phaseLabel);

		void CalcTwoPhaseProp(COBIA::CapeArrayString props,
			COBIA::CapeArrayString phaseLabels);

		COBIA::CapeBoolean CheckSinglePhasePropSpec(COBIA::CapeString property,
			COBIA::CapeString phaseLabel);

		COBIA::CapeBoolean CheckTwoPhasePropSpec(COBIA::CapeString property,
			COBIA::CapeArrayString phaseLabels);

		void getSinglePhasePropList(COBIA::CapeArrayString props);
		void getTwoPhasePropList(COBIA::CapeArrayString props);

		void CalcEquilibrium(COBIA::CapeArrayString specification1,
			COBIA::CapeArrayString specification2,
			COBIA::CapeString solutionType);

		COBIA::CapeBoolean CheckEquilibriumSpec(COBIA::CapeArrayString specification1,
			COBIA::CapeArrayString specification2,
			COBIA::CapeString solutionType);

		void GetUniversalConstant(COBIA::CapeString constantId,
			COBIA::CapeValue constantValue);
		void getUniversalConstantList(COBIA::CapeArrayString constantIdList);

		CapeEditResult Edit(CapeWindowId);
		void Initialize();
		void Terminate();
		CapeCollection<CapeParameter> getParameters();
		void putSimulationContext(CapeSimulationContext);

	private:
		void CheckCompounds();
		void initCompoundDatabase();

		double calcCompoundCp(const CompoundData& comp, double T);
		double calcCompoundEnthalpy(const CompoundData& comp, double T);
		double calcCompoundEntropy(const CompoundData& comp, double T, double P);
		double calcIdealGasEnthalpy(double T, const std::vector<double>& x);
		double calcIdealGasEntropy(double T, double P, const std::vector<double>& x);
		double calcIdealGasCp(double T, const std::vector<double>& x);
		double calcAverageMolecularWeight(const std::vector<double>& x);
		double calcDensity(double T, double P, double MW);

		std::vector<CompoundData> compoundDB;
	CapeThermoMaterial material;
		bool hasMaterial;
		bool compoundsChecked;

		double R;
		double Tref;
		double Pref;
	};

}