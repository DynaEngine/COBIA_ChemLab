#pragma once
#include <COBIA.h>
#include <CapeInterfaces_1_2.h>
#include <CapeInterfaceAdapters_1_2.h>
#include <vector>
#include <string>

using namespace COBIA;
using namespace CAPEOPEN_1_2;

// ============================================================================
// Debug/Release UUID and name
// ============================================================================
#ifdef _DEBUG
  #ifdef _WIN64
    #define MSHEX_UNIT_NAME COBIATEXT("CO MSHEX x64 Debug")
    #define MSHEX_UNIT_UUID 0xaa,0xf0,0x2e,0x89,0x29,0x1c,0x4d,0x7c,0x83,0x6f,0x10,0xec,0x28,0xa7,0x05,0xa9
  #else
    #define MSHEX_UNIT_NAME COBIATEXT("CO MSHEX x86 Debug")
    #define MSHEX_UNIT_UUID 0xaa,0xf0,0x2e,0x89,0x29,0x1c,0x4d,0x7c,0x83,0x6f,0x10,0xec,0x28,0xa7,0x05,0xa8
  #endif
#else
  #ifdef _WIN64
    #define MSHEX_UNIT_NAME COBIATEXT("CO MSHEX x64")
    #define MSHEX_UNIT_UUID 0xaa,0xf0,0x2e,0x89,0x29,0x1c,0x4d,0x7c,0x83,0x6f,0x10,0xec,0x28,0xa7,0x05,0xff
  #else
    #define MSHEX_UNIT_NAME COBIATEXT("CO MSHEX x86")
    #define MSHEX_UNIT_UUID 0xaa,0xf0,0x2e,0x89,0x29,0x1c,0x4d,0x7c,0x83,0x6f,0x10,0xec,0x28,0xa7,0x05,0xaa
  #endif
#endif
#define MSHEX_UNIT_DESC COBIATEXT("MultiStream Heat Exchanger")
constexpr CapeInteger nPorts = 5;

// ============================================================================
// Helper utilities
// ============================================================================
inline CapeStringImpl getName(CapeInterface param) {
	CapeStringImpl n;
	CAPEOPEN_1_2::CapeIdentification id(param);
	id.getComponentName(n);
	return n;
}

CapeReal calcOverallFromPhaseProps(CapeThermoMaterial, CapeStringImpl);

// ============================================================================
// MaterialPort — material stream connection point
// ============================================================================
class MaterialPort :
	public CapeOpenObject<MaterialPort>,
	public CapeIdentificationAdapter<MaterialPort>,
	public CapeUnitPortAdapter<MaterialPort>
{
	CapeStringImpl& unitName;
	CapeValidationStatus& unitValidationStatus;
	CapeStringImpl portName;
	CapePortDirection direction;
	CapeBoolean primary;
	CapeThermoMaterial connectedMaterial;

public:
	const CapeStringImpl getDescriptionForErrorSource();
	MaterialPort(CapeStringImpl& _un, CapeValidationStatus& _uvs,
		const COBIACHAR* _pn, CapePortDirection _dir, CapeBoolean _pri);

	CapeBoolean isPrimary() { return primary; }
	CapeThermoMaterial getMaterial() { return connectedMaterial; }
	void getComponentName(CapeString n) { n = portName; }
	void putComponentName(CapeString) { throw cape_open_error(COBIAERR_Denied); }
	void getComponentDescription(CapeString d) { d = COBIATEXT("Material Port"); }
	void putComponentDescription(CapeString) { throw cape_open_error(COBIAERR_Denied); }
	CapePortType getPortType() { return CAPE_MATERIAL; }
	CapePortDirection getDirection() { return direction; }
	CapeInterface getConnectedObject() { return connectedMaterial; }
	void Connect(CapeInterface obj);
	void Disconnect();
};

using MaterialPortPtr = CapeOpenObjectSmartPointer<MaterialPort>;

// ============================================================================
// PortCollection — minimal inline ICapeCollection for Unit ports
// ============================================================================
class PortCollection :
	public CapeOpenObject<PortCollection>,
	public CapeIdentificationAdapter<PortCollection>,
	public CapeCollectionAdapter<CapeUnitPort, PortCollection>
{
	CapeStringImpl& unitName;
	std::vector<MaterialPortPtr> items;

public:
	const CapeStringImpl getDescriptionForErrorSource();
	PortCollection(CapeStringImpl& _n);
	void push(MaterialPortPtr p) { items.push_back(p); }

	CapeUnitPort Item(CapeInteger i) { return items[i]; }
	CapeUnitPort Item(CapeString name);
	CapeInteger getCount() { return (CapeInteger)items.size(); }

	void getComponentName(CapeString n) { n = COBIATEXT("PortCollection"); }
	void putComponentName(CapeString) { throw cape_open_error(COBIAERR_Denied); }
	void getComponentDescription(CapeString d) { d = COBIATEXT("PortCollection"); }
	void putComponentDescription(CapeString) { throw cape_open_error(COBIAERR_Denied); }
};

using PortCollectionPtr = CapeOpenObjectSmartPointer<PortCollection>;

// ============================================================================
// SideParameter — inline option parameter (Ignore / Hot / Cold)
// ============================================================================
class SideParameter :
	public CapeOpenObject<SideParameter>,
	public CapeIdentificationAdapter<SideParameter>,
	public CapeParameterAdapter<SideParameter>,
	public CapeParameterSpecificationAdapter<SideParameter>,
	public CapeStringParameterAdapter<SideParameter>,
	public CapeStringParameterSpecificationAdapter<SideParameter>
{
	CapeStringImpl& unitName;
	CapeValidationStatus& unitValidationStatus;
	CapeBoolean& dirty;
	CapeStringImpl paramName, value, defaultValue;
	CapeArrayStringImpl& optionNames;
	CapeValidationStatus paramStatus;

public:
	const CapeStringImpl getDescriptionForErrorSource() {
		return COBIATEXT("Parameter \"") + paramName + COBIATEXT("\" of ") + unitName;
	}
	SideParameter(CapeStringImpl& _un, CapeValidationStatus& _uvs, CapeBoolean& _d,
		const COBIACHAR* _pn, CapeArrayStringImpl& _opts) :
		unitName(_un), unitValidationStatus(_uvs), dirty(_d),
		paramName(_pn), optionNames(_opts), paramStatus(CAPE_NOT_VALIDATED) {
		defaultValue = optionNames[0]; value = defaultValue;
	}

	void getComponentName(CapeString n) { n = paramName; }
	void putComponentName(CapeString) { throw cape_open_error(COBIAERR_Denied); }
	void getComponentDescription(CapeString d) { d = COBIATEXT("Side Option"); }
	void putComponentDescription(CapeString) { throw cape_open_error(COBIAERR_Denied); }

	CapeParamType getType() { return CAPE_PARAMETER_STRING; }
	CapeParamMode getMode() { return CAPE_INPUT; }
	CapeValidationStatus getValStatus() { return paramStatus; }
	void Reset() { value = defaultValue; dirty = true; paramStatus = CAPE_NOT_VALIDATED; unitValidationStatus = CAPE_NOT_VALIDATED; }

	void getValue(CapeString v) { v = value; }
	void putValue(CapeString v) { value = v; dirty = true; paramStatus = CAPE_NOT_VALIDATED; unitValidationStatus = CAPE_NOT_VALIDATED; }
	void getDefaultValue(CapeString dv) { dv = defaultValue; }
	void getOptionList(CapeArrayString opts) {
		opts.resize(optionNames.size());
		for (size_t i = 0; i < optionNames.size(); i++) opts[i] = optionNames[i];
	}
	CapeBoolean getRestrictedToList() { return true; }

	CapeBoolean Validate(CapeString v, CapeString message) {
		for (size_t i = 0; i < optionNames.size(); i++)
			if (CapeStringImpl(v.c_str()) == optionNames[i]) { paramStatus = CAPE_VALID; return true; }
		message = paramName + COBIATEXT(" invalid option");
		paramStatus = CAPE_INVALID; return false;
	}
	CapeBoolean Validate(CapeString message) {
		if (getType() != CAPE_PARAMETER_STRING || getMode() != CAPE_INPUT) {
			message = paramName + COBIATEXT(" does not meet specifications");
			paramStatus = CAPE_INVALID; return false;
		}
		return true;
	}
};

using SideParameterPtr = CapeOpenObjectSmartPointer<SideParameter>;

// ============================================================================
// ParameterCollection — minimal inline ICapeCollection for parameters
// ============================================================================
class ParameterCollection :
	public CapeOpenObject<ParameterCollection>,
	public CapeIdentificationAdapter<ParameterCollection>,
	public CapeCollectionAdapter<CapeParameter, ParameterCollection>
{
	std::vector<CapeParameter> items;

public:
	const CapeStringImpl getDescriptionForErrorSource() { return COBIATEXT("ParameterCollection"); }
	void addItem(CapeParameter p) { items.push_back(p); }
	CapeParameter Item(CapeInteger i) { return items[i]; }
	CapeParameter Item(CapeString name) {
		for (auto& it : items) {
			CapeStringImpl n = getName(it);
			if (name == ConstCapeString(n.c_str())) return it;
		}
		throw cape_open_error(COBIAERR_NoSuchItem);
	}
	CapeInteger getCount() { return (CapeInteger)items.size(); }
	void getComponentName(CapeString n) { n = COBIATEXT("ParameterCollection"); }
	void putComponentName(CapeString) { throw cape_open_error(COBIAERR_Denied); }
	void getComponentDescription(CapeString d) { d = COBIATEXT("ParameterCollection"); }
	void putComponentDescription(CapeString) { throw cape_open_error(COBIAERR_Denied); }

	std::vector<CapeParameter>& itemsRef() { return items; }
};

using ParameterCollectionPtr = CapeOpenObjectSmartPointer<ParameterCollection>;

// ============================================================================
// Main class — single-class Unit Operation
// ============================================================================
class MHeatExchangerUnit :
	public CapeOpenObject<MHeatExchangerUnit>,
	public CapeIdentificationAdapter<MHeatExchangerUnit>,
	public CapeUnitAdapter<MHeatExchangerUnit>,
	public CapeUtilitiesAdapter<MHeatExchangerUnit>,
	public CapePersistAdapter<MHeatExchangerUnit>
{
	CapeStringImpl name, description;
	CapeBoolean dirty;
	CapeValidationStatus validationStatus;

	std::vector<MaterialPortPtr> inletPorts;
	std::vector<MaterialPortPtr> outletPorts;
	PortCollectionPtr portCollection;

	CapeArrayStringImpl sideOptions;
	SideParameterPtr in1side, in2side, in3side, in4side, in5side;
	ParameterCollectionPtr paramCollection;

	std::vector<CapeThermoMaterial> inlets, outlets;

	std::vector<CapeArrayStringImpl> productsPhaseIDs;
	std::vector<CapeArrayEnumerationImpl<CapePhaseStatus>> productsPhaseStatus;
	CapeArrayStringImpl flashCond1, flashCond2;

	CapeBoolean validateMaterialPorts(CapeString message);
	CapeBoolean validateMSHEXSides(CapeString message);
	CapeBoolean validateParameterSpecifications(CapeString message);
	CapeBoolean requiredByMSHEX(CapeString message, CapeInteger inletIdx);
	void preparePhaseIDs();
	void flashOutlet(size_t idx);

public:
	const CapeStringImpl getDescriptionForErrorSource() { return name; }
	static const CapeUUID getObjectUUID() { return CapeUUID{{ MSHEX_UNIT_UUID }}; }
	static void Register(CapePMCRegistrar registrar);

	MHeatExchangerUnit();

	void getComponentName(CapeString n) { n = name; }
	void putComponentName(CapeString n);
	void getComponentDescription(CapeString d) { d = description; }
	void putComponentDescription(CapeString d);

	CapeCollection<CapeUnitPort> ports() { return portCollection; }
	CapeCollection<CapeParameter> getParameters();
	CapeValidationStatus getValStatus() { return validationStatus; }
	CapeBoolean Validate(CapeString message);
	void Calculate();

	void Initialize() {}
	void Terminate();
	void putSimulationContext(CapeSimulationContext) { throw cape_open_error(COBIAERR_NotImplemented); }
	CapeEditResult Edit(CapeWindowId);

	void Save(CapePersistWriter, CapeBoolean);
	void Load(CapePersistReader);
	CapeBoolean getIsDirty() { return dirty; }
};