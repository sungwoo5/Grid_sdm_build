#include <Grid/Grid.h>

#include <fstream>

using namespace std;
using namespace Grid;

typedef WilsonFermionD FermionOp;
typedef typename WilsonFermionD::FermionField FermionField;

int main(int argc, char** argv) {
  Grid_init(&argc, &argv);
  GridLogLayout();

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
  // if( argc > 1 && argv[1][0] != '-' )
  //   {
  //     std::cout << GridLogMessage << "Loading configuration from " << argv[1] << std::endl;
  //     FieldMetaData header;
  //     NerscIO::readConfiguration(Umu, header, argv[1]);
  //   }
  // else
  //   {
  //     std::cout<<GridLogMessage <<"Using cold configuration"<<std::endl;
  //     SU<Nc>::ColdConfiguration(Umu);
  //     // SU<Nc>::HotConfiguration(RNG4,Umu);
  //   }
  cout << "argc = " << argc << endl;
  // input -------------------------------
  if( argc < 3 )
    {
      cout << "Error!" << endl;
      Grid_finalize();
      return 1;
    }
  // string confPath = argv[1];
  // std::cout << GridLogMessage << "Loading configuration from " << argv[1] << std::endl;
  // if( argv[1] != 'cold' )
  //   {
  //     std::cout << GridLogMessage << "Loading configuration from " << argv[1] << std::endl;
  //     FieldMetaData header;
  //     NerscIO::readConfiguration(Umu, header, argv[1]);
  //   }
  // else
  //   {
  //     std::cout<<GridLogMessage <<"Using cold configuration"<<std::endl;
  //     SU<Nc>::ColdConfiguration(Umu);
  //     // SU<Nc>::HotConfiguration(RNG4,Umu);
  //   }

  FieldMetaData header;
  NerscIO::readConfiguration(Umu, header, argv[1]);
  RealD alpha = stod(argv[2]);
  RealD beta = stod(argv[3]);
  RealD M5 = stod(argv[4]);
  // ------------------------- input

  std::cout << GridLogMessage << "==============================================" << std::endl;
  std::cout << GridLogMessage << "anti-periodic boundary condition {1,1,1,-1}" << std::endl;
  std::vector<Complex> boundary = {1,1,1,-1};
  FermionOp::ImplParams Params(boundary);

  std::ofstream of;
  of.open( "evals.dat", std::ios::out | std::ios::trunc);
  if(!of) assert(false);
  of << std::scientific << std::setprecision(15);

  // for(RealD M5=0.05; M5<2.5; M5+=0.05){
    RealD mass = -M5;
    // FermionOp WilsonOperator(Umu,*FGrid,*FrbGrid,mass);
    FermionOp WilsonOperator(Umu,*FGrid,*FrbGrid,mass,Params);
    // Gamma G5(Gamma::Algebra::Gamma5);
    MdagMLinearOperator<FermionOp,LatticeFermion> HermOp(WilsonOperator); /// <-----
    // SchurDiagMooeeOperator<FermionOp,LatticeFermion> HermOp(WilsonOperator);
    // HermitianLinearOperator<FermionOp, LatticeFermion> HermOp(WilsonOperator); /// <-----
    Gamma5HermitianLinearOperator<FermionOp, LatticeFermion> g5Dwilson(WilsonOperator);
    // Gamma5R5HermitianLinearOperator<FermionOp, LatticeFermion> HermOp(WilsonOperator);
    // HermitianOperator<WilsonFermion,LatticeFermion> HermOp(Dw);
    // SchurDiagTwoOperator<FermionOp,LatticeFermion> HermOp(WilsonOperator);
    // SchurDiagOneOperator<FermionOp,LatticeFermion> HermOp(WilsonOperator);


    // const int Nstop = 10;
    // const int Nk = 12;
    // const int Np = 8;
    // const int Nm = Nk + Np;
    // const int MaxIt = 10000;
    // RealD resid = 1.0e-6;

    // std::vector<double> Coeffs{0, 1.};
    // Polynomial<FermionField> PolyX(Coeffs);
    // Chebyshev<FermionField> Cheby(0.0, 10., 12);
    
    ChebyParams chebyParams;
    chebyParams.alpha = alpha;
    chebyParams.beta = beta;
    chebyParams.Npoly = 101;

    cout << "Chebyshev polynomial parameters: " << chebyParams.alpha << ", " << chebyParams.beta << ", " << chebyParams.Npoly << endl;
    Chebyshev< FermionField > chebyshev( chebyParams );

    FunctionHermOp<FermionField> OpCheby(chebyshev,HermOp);
    PlainHermOp<FermionField> Op     (HermOp);

    // ImplicitlyRestartedLanczos<FermionField> IRL(OpCheby, Op, Nstop, Nk, Nm, resid, MaxIt);

    // https://github.com/paboyle/Grid/blob/d299c86633e877847510947a00a031e42e80d431/Grid/algorithms/iterative/ImplicitlyRestartedLanczos.h#L97
    int Nstop = 10;	    // Number of evecs checked for convergence
    int Nk = 15;	    // Number of converged sought --> Nstop + 10 or large enough
    // int Np = 8;	            // Np -- Number of spare vecs in krylov space //  == Nm - Nk
    int Nm = 50;            // total number of vectors --> 100

    RealD tol = 1e-12;
    int maxIter = 1000;
    RealD betastp = 0;  // not used
    int restartMin = 0;
    int reorth_period = 1;

    ImplicitlyRestartedLanczos< FermionField > IRL( OpCheby, Op, Nstop, Nk, Nm, tol, maxIter, betastp, restartMin, reorth_period );


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
  // }

    for( int i=0; i< Nstop; i++){
      RealD norm;
      ComplexD l;
      FermionField Mevec(FGrid);
      norm=norm2(evec[i]);
      // WilsonOperator.M(evec[i],Mevec);
      g5Dwilson.Op(evec[i],Mevec);
      l = innerProduct(evec[i],Mevec);
      l = l/norm;
      std::cout << i 
		<< " sqrt(lambda)= " << std::sqrt(eval[i])
		<< " ,l= " << l
		<< std::endl;
      
    }

  Grid_finalize();
}
