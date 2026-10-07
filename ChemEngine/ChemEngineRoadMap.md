# ChemEngine Road Map

Learn from Docs/dwsim-windows.

## Property Packages

We should create a local property package to store, manage, calculate properties of different materials.

We should can define and use properties as follows:

```c++
Fluid fluid("Water");
double pressure = 0.1; // in MPa
double temperature = 25; // in C
double enthalpy = fluid.h_pt(pressure, temperature); // in kJ/kg
double density = 1.0 / fluid.v_pt(pressure, temperature); // in kg/m³
double specificHeat = fluid.cp_pt(pressure, temperature); // in kJ/kg·K
...
Fluid fluid2("Air");
double enthalpy2 = fluid2.h_pt(pressure, temperature); // in kJ/kg
double density2 = 1.0 / fluid2.v_pt(pressure, temperature); // in kg/m³
double specificHeat2 = fluid2.cp_pt(pressure, temperature); // in kJ/kg·K
...
Fluid fluid3("0.1 Oxygen, 0.9 Air");
double enthalpy3 = fluid3.h_pt(pressure, temperature); // in kJ/kg
double density3 = 1.0 / fluid3.v_pt(pressure, temperature); // in kg/m³
double specificHeat3 = fluid3.cp_pt(pressure, temperature); // in kJ/kg·K
...
```

Fluids are defined in a flowsheet based on the property package. Each flow can have one Fluid.

## UnitOperation

A unit operation is a basic operation in a chemical process, such as a reactor, a separator, a mixer, etc.

Each Unit Operation has a name, a type, it should at least have:

- Input streams
- Output streams
- Parameters

Functions are used to calculate the properties of the unit operation:

- Operation function

Unit operations can be connected in a flowsheet to form a chemical process.

## Link or Connection

A link or connection is used to connect two unit operations in a flowsheet.

Connection is defined as follows:
```c++

Connection connection("Connection1");
connection.connect(#Unit1,"port1", #Unit2,"port2");

// This Unit can find the connection by its port name.
// Connection should make the two port the same. Or user can define other connection method.

```

## Solver

kahn's algorithm is used to solve the flowsheet.