#include <Grid/Grid.h>

using namespace std;
using namespace Grid;

void PointSource(Coordinate &coor,LatticePropagator &source)
{
  source=Zero();
  SpinColourMatrix kronecker; kronecker=1.0;
  pokeSite(kronecker, source, coor);
}

void StochasticSource(GridParallelRNG &RNG, LatticePropagator &source)
{
  GridBase *grid = source.Grid();
  LatticeComplex noise(grid);
  bernoulli(RNG, noise); // 0,1 50:50 in cplx

  // auto latt_size = grid->GlobalDimensions();
  // int x=0; 
  // int y=0;
  // // for(int x=0; x<latt_size[0]; x++)
  // // for(int y=0; y<latt_size[1]; y++)
  // for(int z=0; z<latt_size[2]; z++)
  // for(int t=0; t<latt_size[3]; t++){
    
  //   Coordinate coor({x,y,z,t});
  //   ComplexD c;
  //   peekLocalSite(c,noise,coor);
  //   std::cout<<GridLogMessage
  //   	     <<"noise["<<x<<y<<z<<t<<"]="<<c<<std::endl;
  // }

  RealD nrm = 1.0/sqrt(2.0);
  noise = ( 2.0*noise - Complex(1.0,1.0) )*nrm;

  // std::cout<<GridLogMessage
  // 	   <<"( 2.0*noise - Complex(1.0,1.0) )*nrm "<<std::endl;
  // // for(int x=0; x<latt_size[0]; x++)
  // // for(int y=0; y<latt_size[1]; y++)
  // for(int z=0; z<latt_size[2]; z++)
  // for(int t=0; t<latt_size[3]; t++){
    
  //   Coordinate coor({x,y,z,t});
  //   ComplexD c;
  //   peekLocalSite(c,noise,coor);
  //   std::cout<<GridLogMessage
  //   	     <<"noise["<<x<<y<<z<<t<<"]="<<c<<std::endl;
  // }


  source = 1.0;
  source = source*noise;
}

template<class Action>
void Solve(Action &D,LatticePropagator &source,LatticePropagator &propagator)
{
  GridBase *UGrid = D.GaugeGrid();
  GridBase *FGrid = D.FermionGrid();

  LatticeFermion src4  (UGrid);
  LatticeFermion src5  (FGrid);
  LatticeFermion result5(FGrid);
  LatticeFermion result4(UGrid);

  ConjugateGradient<LatticeFermion> CG(1.0e-8,100000);
  SchurRedBlackDiagMooeeSolve<LatticeFermion> schur(CG);
  ZeroGuesser<LatticeFermion> ZG; // Could be a DeflatedGuesser if have eigenvectors
  for(int s=0;s<Nd;s++){
    for(int c=0;c<Nc;c++){
      PropToFerm<Action>(src4,source,s,c);

      D.ImportPhysicalFermionSource(src4,src5);

      result5=Zero();
      schur(D,src5,result5,ZG);
      std::cout<<GridLogMessage
               <<"spin "<<s<<" color "<<c
               <<" norm2(src5d) "   <<norm2(src5)
               <<" norm2(result5d) "<<norm2(result5)<<std::endl;

      D.ExportPhysicalFermionSolution(result5,result4);

      FermToProp<Action>(propagator,result4,s,c);
    }
  }
}

// void ChCondPtSrc(std::string file, LatticePropagator &q, Coordinate& coord)
// {
//   LatticeComplex meson_CF( q.Grid() );

//   // Gamma G5(Gamma::Algebra::Gamma5);
//   meson_CF = trace(q);
//   TComplex ChCond;
//   peekSite(ChCond, meson_CF, coord);

//   Complex res = TensorRemove(ChCond); // Yes this is ugly, not figured a work around
//   std::cout << res << std::endl;

//   std::cout << "chcond, " << file << ", " << ChCond << std::endl;
//   {
//     XmlWriter WR(file);
//     write(WR, "MesonFile", res);
//   }
// }

void ChCondStochSrc(std::string file, LatticePropagator &psi, LatticePropagator &eta)
{
  LatticeComplex meson_CF( eta.Grid() );
  meson_CF = trace(psi*adj(eta));
  auto ChCond = sum( meson_CF );

  LatticeComplex identity_CF( eta.Grid() );
  identity_CF = trace(adj(eta)*eta);
  auto norm = sum( identity_CF );

  auto res = ChCond()()/norm()();


  std::ofstream of;
  of.open( file, std::ios::out | std::ios::trunc);
  if(!of) assert(false);
  of << std::scientific << std::setprecision(15);

  std::cout << "chcond, " << file << ", " << res << std::endl; 
  {
    // XmlWriter WR(file);
    // write(WR, "MesonFile", res);
    of << res << std::endl;
  }
}


int main (int argc, char ** argv)
{
  const int Ls=16;
  Grid_init(&argc,&argv);

  // Double precision grids
  GridCartesian         * UGrid   = SpaceTimeGrid::makeFourDimGrid(GridDefaultLatt(),
                                                                   GridDefaultSimd(Nd,vComplex::Nsimd()),
                                                                   GridDefaultMpi());
  GridRedBlackCartesian * UrbGrid = SpaceTimeGrid::makeFourDimRedBlackGrid(UGrid);
  GridCartesian         * FGrid   = SpaceTimeGrid::makeFiveDimGrid(Ls,UGrid);
  GridRedBlackCartesian * FrbGrid = SpaceTimeGrid::makeFiveDimRedBlackGrid(Ls,UGrid);

  std::vector<int> seeds4({1,2,3,4});
  GridParallelRNG  RNG4(UGrid);  RNG4.SeedFixedIntegers(seeds4);

  LatticeGaugeField Umu(UGrid);
  std::string config;
  std::string config_path;
  std::string outfile;
  RealD mass;
  if( argc > 4 && argv[1][0] != '-' )
  {
    config_path=argv[1];    
    config=argv[2];
    mass = stod(argv[3]);
    std::cout << GridLogMessage << "Loading configuration from " << config_path+config << std::endl;
    std::cout << GridLogMessage << "mass=" << mass << std::endl;
    FieldMetaData header;
    NerscIO::readConfiguration(Umu, header, config_path+config);
  
    outfile="./analysis/"+config+"_chcond.dat";
  }
  else
  {
    // std::cout<<GridLogMessage <<"Using cold configuration"<<std::endl;
    // SU<Nc>::ColdConfiguration(Umu);
    // //    SU<Nc>::HotConfiguration(RNG4,Umu);
    // config="ColdConfig";
    std::cout << GridLogMessage << "./pbp <cfgpath> <cfgfilename> <mass> --grid xx.xx.xx.x " << config_path+config << std::endl;
    exit(1);
  }

  RealD M5=1.8;
  RealD b=1.5;// Scale factor b+c=2, b-c=1
  RealD c=0.5;
  MobiusFermionD::ImplParams bdy( std::vector<Complex>({1,1,1,-1}) );
  MobiusFermionD FermAct(Umu, *FGrid, *FrbGrid, *UGrid, *UrbGrid, mass, M5, b, c, bdy);

  std::cout<<GridLogMessage <<"======================"<<std::endl;
  std::cout<<GridLogMessage <<"MobiusFermion action as Scaled Shamir kernel"<<std::endl;
  std::cout<<GridLogMessage <<"======================"<<std::endl;

  // std::stringstream ss;
  // ss << "./analysis/" << config << "_chcond.xml";
  // std::cout << ss.str() << std::endl;

  // point source
  // LatticePropagator point_source(UGrid);
  // Coordinate Origin({0,0,0,0});
  // PointSource   (Origin, point_source);
  // LatticePropagator PointProp(UGrid);
  // Solve(FermAct, point_source, PointProp);
  // ChCondPtSrc(ss.str(), PointProp, Origin);

  // noisy estimator with Z4
  LatticePropagator stochastic_source(UGrid);
  StochasticSource( RNG4, stochastic_source );

  LatticePropagator StochProp(UGrid);
  Solve(FermAct, stochastic_source, StochProp);

  // ChCondStochSrc( ss.str(), StochProp, stochastic_source );
  ChCondStochSrc( outfile, StochProp, stochastic_source );

  Grid_finalize();
}




// {
//   SpinColourMatrix tmp;
//   peekSite(tmp, point_source, Origin);
//   std::cout << tmp << std::endl;
// }

// TComplex tmp;
// Coordinate Origin({0,0,0,0});
// peekSite(tmp, check_CF, Origin);
// std::cout << tmp << std::endl;

