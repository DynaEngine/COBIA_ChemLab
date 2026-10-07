#pragma once
#include <COBIA.h>
#include <CapeInterfaces_1_2.h>
#include <CapeInterfaceAdapters_1_2.h>
#include <string>
#include <cmath>
#include <map>

#include "Water.h"

namespace WaterPP {

	using namespace CAPEOPEN_1_2;

	class WaterPPPropertyPackage :
		public COBIA::CapeOpenObject<WaterPPPropertyPackage>,
		public CapeThermoPropertyPackageManagerAdapter<WaterPPPropertyPackage>,
		public CapeThermoMaterialContextAdapter<WaterPPPropertyPackage>,
		public CapeThermoCompoundsAdapter<WaterPPPropertyPackage>,
		public CapeThermoPhasesAdapter<WaterPPPropertyPackage>,
		public CapeThermoPropertyRoutineAdapter<WaterPPPropertyPackage>,
		public CapeThermoEquilibriumRoutineAdapter<WaterPPPropertyPackage>,
		public CapeThermoUniversalConstantAdapter<WaterPPPropertyPackage>,
		public CapeUtilitiesAdapter<WaterPPPropertyPackage> {

	public:
		const COBIA::CapeStringImpl getDescriptionForErrorSource() {
			return COBIATEXT("WaterPPPropertyPackage");
		}

		static const COBIA::CapeUUID getObjectUUID() {
			return COBIA::CapeUUID{{0xd8,0xe7,0xf6,0xa5,0xb4,0xc3,0x42,0x10,0x9f,0x8e,0x7d,0x6c,0x5b,0x4a,0x3f,0x2e}};
		}

		static void Register(COBIA::CapePMCRegistrar registrar) {
			registrar.putName(COBIATEXT("Water IAPWS-97 Property Package"));
			registrar.putDescription(COBIATEXT("IAPWS-97 based Property Package for calculation of water and steam"));
			registrar.putCapeVersion(COBIATEXT("1.2"));
			registrar.putComponentVersion(COBIATEXT("1.0.0.0"));
			registrar.putAbout(COBIATEXT("Water IAPWS-97 Property Package v1.0"));
			registrar.putVendorURL(COBIATEXT("https://github.com/example/waterpp"));
			registrar.putProgId(COBIATEXT("WaterPP.WaterPPPropertyPackage"));
			registrar.putVersionIndependentProgId(COBIATEXT("WaterPP.WaterPPPropertyPackage"));
			registrar.addCatID(CAPEOPEN::categoryId_PropertyPackageManager);
		}

		WaterPPPropertyPackage();
		~WaterPPPropertyPackage();

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

		void setPhaseResults(
			COBIA::CapeArrayStringImpl& phaseLabels,
			COBIA::CapeArrayEnumerationImpl<CapePhaseStatus>& phaseStatus,
			double vapFrac, double T, double P,
			double Hv, double Sv, double Rv,
			double Hl, double Sl, double Rl,
			bool haveVapor = true, bool haveLiquid = true);

		void setFlashPhaseProps(
			Water& w, double T,
			COBIA::CapeArrayStringImpl& phaseLabels,
			COBIA::CapeArrayEnumerationImpl<CapePhaseStatus>& phaseStatus,
			double vapFrac, double resT, double resP,
			bool haveVapor = true, bool haveLiquid = true);

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
		void initWaterConstants();

		enum TDependentProp {
			TDP_SURFACE_TENSION_SAT_LIQUID,
			TDP_THERMAL_CONDUCTIVITY_LIQUID,
			TDP_THERMAL_CONDUCTIVITY_VAPOR,
			TDP_VAPOR_PRESSURE,
			TDP_VOLUME_CHANGE_VAPORIZATION,
			TDP_VOLUME_LIQUID,
			TDP_VISCOSITY_LIQUID,
			TDP_VISCOSITY_VAPOR,
			TDP_IDEAL_GAS_ENTHALPY,
			TDP_IDEAL_GAS_ENTROPY
		};

		enum SinglePhaseProp {
			SPP_DENSITY,
			SPP_ENTHALPY,
			SPP_ENTROPY,
			SPP_GIBBS_ENERGY,
			SPP_HEAT_CAPACITY_CP,
			SPP_HEAT_CAPACITY_CV,
			SPP_INTERNAL_ENERGY,
			SPP_MOLECULAR_WEIGHT,
			SPP_THERMAL_CONDUCTIVITY,
			SPP_VOLUME,
			SPP_VISCOSITY
		};

		enum TwoPhaseProp {
			TPP_SURFACE_TENSION
		};

		bool getTDepPropEnum(const std::wstring& name, TDependentProp& result);
		bool getSinglePhasePropEnum(const std::wstring& name, SinglePhaseProp& result);

		double calcIdealGasEnthalpy(double T);
		double calcIdealGasEntropy(double T);
		void CheckCompounds();
		void CheckEquilibriumSetup();
		void CheckOverallX();

		CapeThermoMaterial material;
		bool hasMaterial;
		bool compoundsChecked;

		std::map<std::wstring, double> constValues;
		double molWt;
		double critT;
		double critP;
	};
}