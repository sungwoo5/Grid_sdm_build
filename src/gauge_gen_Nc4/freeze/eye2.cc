#include <Grid/Grid.h>

#include <fstream>

using namespace std;
using namespace Grid;

typedef WilsonFermionD FermionOp;
typedef typename WilsonFermionD::FermionField FermionField;

int main(int argc, char** argv) {
  Grid_init(&argc, &argv);

  GridCartesian* UGrid = SpaceTimeGrid::makeFourDimGrid(
      GridDefaultLatt(), GridDefaultSimd(Nd, vComplex::Nsimd()),
      GridDefaultMpi());
  GridRedBlackCartesian* UrbGrid =
    SpaceTimeGrid::makeFourDimRedBlackGrid(UGrid);
  GridCartesian* FGrid = UGrid;
  GridRedBlackCartesian* FrbGrid = UrbGrid;

  std::vector<int> seeds4({1, 2, 3, 4});
  std::vector<int> seeds5({5, 6, 7, 8});
  GridParallelRNG RNG5(FGrid);
  RNG5.SeedFixedIntegers(seeds5);
  GridParallelRNG RNG4(UGrid);
  RNG4.SeedFixedIntegers(seeds4);

  LatticeGaugeField Umu(UGrid);
  if( argc > 1 && argv[1][0] != '-' )
    {
      std::cout << GridLogMessage << "Loading configuration from " << argv[1] << std::endl;
      FieldMetaData header;
      NerscIO::readConfiguration(Umu, header, argv[1]);
    }
  else
    {
      std::cout<<GridLogMessage <<"Using cold configuration"<<std::endl;
      SU<Nc>::ColdConfiguration(Umu);
      // SU<Nc>::HotConfiguration(RNG4,Umu);
    }


  std::ofstream of;
  of.open( "evals.dat", std::ios::out | std::ios::trunc);
  if(!of) assert(false);
  of << std::scientific << std::setprecision(15);

  for(RealD M5=-0.2; M5<2.5; M5+=0.05){
    RealD mass = -M5;
    FermionOp WilsonOperator(Umu,*FGrid,*FrbGrid,mass);
    // Gamma G5(Gamma::Algebra::Gamma5);
    MdagMLinearOperator<FermionOp,LatticeFermion> HermOp(WilsonOperator); /// <-----
    // SchurDiagMooeeOperator<FermionOp,LatticeFermion> HermOp(WilsonOperator);
    // HermitianLinearOperator<FermionOp, LatticeFermion> HermOp(WilsonOperator); /// <-----
    // Gamma5HermitianLinearOperator<FermionOp, LatticeFermion> HermOp(WilsonOperator);
    // Gamma5R5HermitianLinearOperator<FermionOp, LatticeFermion> HermOp(WilsonOperator);
    // HermitianOperator<WilsonFermion,LatticeFermion> HermOp(Dw);
    // SchurDiagTwoOperator<FermionOp,LatticeFermion> HermOp(WilsonOperator);
    // SchurDiagOneOperator<FermionOp,LatticeFermion> HermOp(WilsonOperator);

    const int Nstop = 10;
    const int Nk = 12;
    const int Np = 8;
    const int Nm = Nk + Np;
    const int MaxIt = 10000;
    RealD resid = 1.0e-6;

    std::vector<double> Coeffs{0, 1.};
    Polynomial<FermionField> PolyX(Coeffs);
    Chebyshev<FermionField> Cheby(0.0, 10., 12);

    FunctionHermOp<FermionField> OpCheby(Cheby,HermOp);
    PlainHermOp<FermionField> Op     (HermOp);

    ImplicitlyRestartedLanczos<FermionField> IRL(OpCheby, Op, Nstop, Nk, Nm, resid, MaxIt);

    std::vector<RealD> eval(Nm);

    FermionField src(FGrid);
    // FermionField src(FrbGrid);

    gaussian(RNG5, src);
    std::vector<FermionField> evec(Nm, FGrid);
    for (int i = 0; i < 1; i++) {
      std::cout << i << " / " << Nm << " grid pointer " << evec[i].Grid()
                << std::endl;
    };

    int Nconv;
    IRL.calc(eval, evec, src, Nconv);

    of << M5 << " " << eval << std::endl;
  }

  Grid_finalize();
}
