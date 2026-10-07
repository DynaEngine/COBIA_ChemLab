#define COBIA_PMC_ENTRY_POINTS
#define PMC_REGISTERFORALLUSERS
#include <COBIA_PMC.h>
#include "MHExch.h"

using namespace COBIA;
using namespace CAPEOPEN_1_2;

// ============================================================================
// MaterialPort
// ============================================================================
const CapeStringImpl MaterialPort::getDescriptionForErrorSource() {
	return portName + COBIATEXT(" port of ") + unitName;
}

MaterialPort::MaterialPort(CapeStringImpl& _un, CapeValidationStatus& _uvs,
	const COBIACHAR* _pn, CapePortDirection _dir, CapeBoolean _pri) :
	unitName(_un), unitValidationStatus(_uvs),
	portName(_pn), direction(_dir), primary(_pri) {}

void MaterialPort::Connect(CapeInterface obj) {
	CapeThermoMaterial mat = obj;
	if (!mat) throw cape_open_error(COBIAERR_NoSuchInterface);
	unitValidationStatus = CAPE_NOT_VALIDATED;
	connectedMaterial = mat;
}

void MaterialPort::Disconnect() {
	unitValidationStatus = CAPE_NOT_VALIDATED;
	connectedMaterial.clear();
}

// ============================================================================
// PortCollection
// ============================================================================
const CapeStringImpl PortCollection::getDescriptionForErrorSource() {
	return COBIATEXT("Ports of ") + unitName;
}

PortCollection::PortCollection(CapeStringImpl& _n) : unitName(_n) {}

CapeUnitPort PortCollection::Item(CapeString name) {
	for (auto& p : items) {
		CapeStringImpl n = getName(static_cast<CapeInterface>(p));
		if (name == ConstCapeString(n.c_str())) return p;
	}
	throw cape_open_error(COBIAERR_NoSuchItem);
}

// ============================================================================
// calcOverallFromPhaseProps
// ============================================================================
CapeReal calcOverallFromPhaseProps(CapeThermoMaterial mat, CapeStringImpl prop) {
	CapeArrayStringImpl props(1); props[0] = prop;
	CapeArrayRealImpl value(1);
	CapeReal overallValue = 0;
	CapeArrayStringImpl phaseLabels;
	CapeArrayEnumerationImpl<CapePhaseStatus> phaseStatus;
	CapeArrayRealImpl phaseFraction;

	mat.GetPresentPhases(phaseLabels, phaseStatus);
	for (size_t i = 0; i < phaseLabels.size(); i++) {
		mat.GetSinglePhaseProp(ConstCapeString(COBIATEXT("phaseFraction")),
			phaseLabels[i], ConstCapeString(COBIATEXT("mole")), phaseFraction);
		CapeThermoPropertyRoutine routine(mat);
		routine.CalcSinglePhaseProp(props, phaseLabels[i]);
		mat.GetSinglePhaseProp(prop, phaseLabels[i],
			ConstCapeString(COBIATEXT("mole")), value);
		overallValue += phaseFraction[0] * value[0];
	}
	return overallValue;
}

// ============================================================================
// MHeatExchangerUnit
// ============================================================================
void MHeatExchangerUnit::Register(CapePMCRegistrar registrar) {
	registrar.putName(MSHEX_UNIT_NAME);
	registrar.putDescription(MSHEX_UNIT_DESC);
	registrar.putCapeVersion(COBIATEXT("1.2"));
	registrar.putComponentVersion(COBIATEXT("1.0.0"));
	registrar.putAbout(COBIATEXT("MultiStream Heat Exchanger using COBIA single-class pattern."));
	registrar.putVendorURL(COBIATEXT("www.polimi.it"));
	registrar.putProgId(COBIATEXT("ChemUnit.MHeatExchangerUnit"));
	registrar.putVersionIndependentProgId(COBIATEXT("ChemUnit.MHeatExchangerUnit"));
	registrar.addCatID(CAPEOPEN::categoryId_UnitOperation);
	registrar.addCatID(CAPEOPEN_1_2::categoryId_Component_1_2);
}

MHeatExchangerUnit::MHeatExchangerUnit() :
	name(MSHEX_UNIT_NAME), description(MSHEX_UNIT_DESC),
	dirty(false), validationStatus(CAPE_NOT_VALIDATED),
	inlets(nPorts), outlets(nPorts),
	sideOptions(3)
{
	sideOptions[0] = COBIATEXT("Ignore");
	sideOptions[1] = COBIATEXT("Hot");
	sideOptions[2] = COBIATEXT("Cold");

	in1side = new SideParameter(name, validationStatus, dirty, COBIATEXT("Inlet 1 Side"), sideOptions);
	in2side = new SideParameter(name, validationStatus, dirty, COBIATEXT("Inlet 2 Side"), sideOptions);
	in3side = new SideParameter(name, validationStatus, dirty, COBIATEXT("Inlet 3 Side"), sideOptions);
	in4side = new SideParameter(name, validationStatus, dirty, COBIATEXT("Inlet 4 Side"), sideOptions);
	in5side = new SideParameter(name, validationStatus, dirty, COBIATEXT("Inlet 5 Side"), sideOptions);

	paramCollection = new ParameterCollection();
	paramCollection->addItem(in1side);
	paramCollection->addItem(in2side);
	paramCollection->addItem(in3side);
	paramCollection->addItem(in4side);
	paramCollection->addItem(in5side);

	portCollection = new PortCollection(name);
	for (CapeInteger i = 0; i < nPorts; i++) {
		CapeStringImpl num = std::to_wstring(i + 1);
		MaterialPortPtr ip = new MaterialPort(name, validationStatus,
			(COBIATEXT("Inlet") + num).c_str(), CAPE_INLET, i < 2);
		MaterialPortPtr op = new MaterialPort(name, validationStatus,
			(COBIATEXT("Outlet") + num).c_str(), CAPE_OUTLET, i < 2);
		inletPorts.push_back(ip);
		outletPorts.push_back(op);
		portCollection->push(ip);
		portCollection->push(op);
	}

	flashCond1.resize(3);
	flashCond1[0] = COBIATEXT("temperature");
	flashCond1[2] = COBIATEXT("overall");
	flashCond2.resize(3);
	flashCond2[0] = COBIATEXT("pressure");
	flashCond2[2] = COBIATEXT("overall");
}

void MHeatExchangerUnit::putComponentName(CapeString n) {
	name = n; dirty = true;
}
void MHeatExchangerUnit::putComponentDescription(CapeString d) {
	description = d; dirty = true;
}

// ============================================================================
// ICapeUnit — validation & calculation
// ============================================================================
CapeBoolean MHeatExchangerUnit::validateMaterialPorts(CapeString message) {
	inlets.clear(); inlets.resize(nPorts);
	outlets.clear(); outlets.resize(nPorts);
	bool hasPrimaryInlet = false;

	CapeArrayStringImpl refCompIDs, compIDs, formulae, names, casNumbers;
	CapeArrayRealImpl boilTemps, molecularWeights;

	for (CapeInteger i = 0; i < nPorts; i++) {
		CapeThermoMaterial mat = inletPorts[i]->getMaterial();
		if (!mat) {
			if (i < 2) {
				message = COBIATEXT("Primary inlet not connected");
				return false;
			}
			if (requiredByMSHEX(message, i)) return false;
			continue;
		}
		inlets[i] = mat;
		if (i < 2) hasPrimaryInlet = true;

		CapeThermoCompounds tc(mat);
		if (refCompIDs.empty()) {
			tc.GetCompoundList(refCompIDs, formulae, names, boilTemps, molecularWeights, casNumbers);
			if (refCompIDs.empty()) { message = COBIATEXT("Zero compounds"); return false; }
		} else {
			tc.GetCompoundList(compIDs, formulae, names, boilTemps, molecularWeights, casNumbers);
			if (compIDs.size() != refCompIDs.size()) {
				message = COBIATEXT("Compound lists inconsistent"); return false;
			}
			for (size_t j = 0; j < compIDs.size(); j++)
				if (compIDs[j] != refCompIDs[j]) { message = COBIATEXT("Compound lists inconsistent"); return false; }
		}
	}
	if (!hasPrimaryInlet) {
		message = COBIATEXT("No primary inlet connected");
		return false;
	}
	for (CapeInteger i = 0; i < nPorts; i++) {
		CapeThermoMaterial mat = outletPorts[i]->getMaterial();
		if (!mat) {
			if (i < 2) continue;
			CapeInterface inObj = inletPorts[i]->getConnectedObject();
			if (inObj) {
				message = COBIATEXT("Port ") + getName(static_cast<CapeInterface>(outletPorts[i])) + COBIATEXT(" is required");
				return false;
			}
			continue;
		}
		outlets[i] = mat;
	}
	return true;
}

CapeBoolean MHeatExchangerUnit::requiredByMSHEX(CapeString message, CapeInteger inletIdx) {
	SideParameter* sp = static_cast<SideParameter*>((ICapeParameter*)paramCollection->Item(inletIdx));

	CapeString sideVal(new CapeStringImpl);
	sp->getValue(sideVal);

	CapeInterface outObj = outletPorts[inletIdx]->getConnectedObject();
	if (outObj || CapeStringImpl(sideVal.c_str()) != sideOptions[0]) {
		message = COBIATEXT("Port ") + getName(static_cast<CapeInterface>(inletPorts[inletIdx])) + COBIATEXT(" is required");
		return true;
	}

	CapeInterface inObj = inletPorts[inletIdx]->getConnectedObject();
	if (inObj) {
		outObj = outletPorts[inletIdx]->getConnectedObject();
		if (!outObj) {
			message = COBIATEXT("Port ") + getName(static_cast<CapeInterface>(outletPorts[inletIdx])) + COBIATEXT(" is required");
			return true;
		}
	}
	return false;
}

CapeBoolean MHeatExchangerUnit::validateParameterSpecifications(CapeString message) {
	for (size_t i = 0; i < (size_t)paramCollection->getCount(); i++) {
		CapeParameter param = paramCollection->Item((CapeInteger)i);
		if (param.getValStatus() != CAPE_VALID) {
			CapeBoolean ok = false;
			if (param.getType() == CAPE_PARAMETER_STRING) {
				SideParameter* sp = static_cast<SideParameter*>((ICapeParameter*)param);
				CapeString val(new CapeStringImpl);
				sp->getValue(val);
				ok = sp->Validate(val, message);
			}
			if (!ok) return false;
		}
	}
	return true;
}

CapeBoolean MHeatExchangerUnit::validateMSHEXSides(CapeString message) {
	CapeString v1(new CapeStringImpl), v2(new CapeStringImpl);
	static_cast<SideParameter*>((ICapeParameter*)in1side)->getValue(v1);
	static_cast<SideParameter*>((ICapeParameter*)in2side)->getValue(v2);
	if (CapeStringImpl(v1.c_str()) == sideOptions[0]) {
		message = COBIATEXT("Primary stream Inlet 1 cannot be ignored");
		return false;
	}
	if (CapeStringImpl(v2.c_str()) == sideOptions[0]) {
		message = COBIATEXT("Primary stream Inlet 2 cannot be ignored");
		return false;
	}
	if (CapeStringImpl(v1.c_str()) == CapeStringImpl(v2.c_str())) {
		message = COBIATEXT("Primary streams cannot be on the same side");
		return false;
	}
	return true;
}

void MHeatExchangerUnit::preparePhaseIDs() {
	productsPhaseIDs.clear();
	productsPhaseStatus.clear();
	for (size_t i = 0; i < (size_t)nPorts; i++) {
		if (!outlets[i]) continue;
		CapeArrayStringImpl phaseIDs, stateOfAggregation, keyCompounds;
		CapeArrayEnumerationImpl<CapePhaseStatus> phaseStatus;
		CapeThermoPhases phases(outlets[i]);
		phases.GetPhaseList(phaseIDs, stateOfAggregation, keyCompounds);
		phaseStatus.resize(phaseIDs.size());
		std::fill(phaseStatus.begin(), phaseStatus.end(), CAPE_UNKNOWNPHASESTATUS);
		productsPhaseIDs.emplace_back(phaseIDs);
		productsPhaseStatus.emplace_back(phaseStatus);
	}
}

void MHeatExchangerUnit::flashOutlet(size_t idx) {
	if (!outlets[idx]) return;
	outlets[idx].CopyFromMaterial(inlets[idx]);
	CapeThermoEquilibriumRoutine eq(outlets[idx]);
	CapeArrayStringImpl spec1;
	spec1.push_back(COBIATEXT("TP"));
	CapeArrayStringImpl spec2;
	CapeStringImpl sol;
	eq.CalcEquilibrium(&spec1, &spec2, &sol);
}

CapeCollection<CapeParameter> MHeatExchangerUnit::getParameters() {
	return paramCollection;
}

CapeBoolean MHeatExchangerUnit::Validate(CapeString message) {
	if (validationStatus == CAPE_VALID) return true;
	if (!validateParameterSpecifications(message)) {
		validationStatus = CAPE_INVALID; return false;
	}
	if (!validateMSHEXSides(message)) {
		validationStatus = CAPE_INVALID; return false;
	}
	if (!validateMaterialPorts(message)) {
		validationStatus = CAPE_INVALID; return false;
	}
	preparePhaseIDs();
	validationStatus = CAPE_VALID;
	return true;
}

void MHeatExchangerUnit::Calculate() {
	if (validationStatus != CAPE_VALID)
		throw cape_open_error(COBIATEXT("Unit not in valid state"));

	size_t j = 0;
	for (size_t i = 0; i < (size_t)nPorts; i++) {
		if (!outlets[i]) continue;
		outlets[i].CopyFromMaterial(inlets[i]);
		outlets[i].SetPresentPhases(productsPhaseIDs[j], productsPhaseStatus[j]);
		CapeThermoEquilibriumRoutine eq(outlets[i]);
		eq.CalcEquilibrium(flashCond1, flashCond2, ConstCapeEmptyString());
		j++;
	}
}

void MHeatExchangerUnit::Terminate() {
	for (auto& p : inletPorts) p->Disconnect();
	for (auto& p : outletPorts) p->Disconnect();
	inlets.clear(); outlets.clear();
}

CapeEditResult MHeatExchangerUnit::Edit(CapeWindowId) {
	throw cape_open_error(COBIAERR_NotImplemented);
}

void MHeatExchangerUnit::Save(CapePersistWriter writer, CapeBoolean clearDirty) {
	writer.Add(ConstCapeString(COBIATEXT("name")), name);
	writer.Add(ConstCapeString(COBIATEXT("description")), description);
	for (size_t i = 0; i < (size_t)paramCollection->getCount(); i++) {
		CapeParameter p = paramCollection->Item((CapeInteger)i);
		CapeStringImpl n = getName(p);
		if (p.getType() == CAPE_PARAMETER_STRING) {
			CapeString val(new CapeStringImpl);
			static_cast<SideParameter*>((ICapeParameter*)p)->getValue(val);
			writer.Add(ConstCapeString(n.c_str()), val);
		}
	}
	if (clearDirty) dirty = false;
}
void MHeatExchangerUnit::Load(CapePersistReader reader) {
	reader.GetString(ConstCapeString(COBIATEXT("name")), name);
	reader.GetString(ConstCapeString(COBIATEXT("description")), description);
	for (size_t i = 0; i < (size_t)paramCollection->getCount(); i++) {
		CapeParameter p = paramCollection->Item((CapeInteger)i);
		CapeStringImpl n = getName(p);
		if (p.getType() == CAPE_PARAMETER_STRING) {
			CapeString val(new CapeStringImpl);
			reader.GetString(ConstCapeString(n.c_str()), val);
			static_cast<SideParameter*>((ICapeParameter*)p)->putValue(val);
		}
	}
}

COBIA_PMC_REGISTER(MHeatExchangerUnit);