/*
 * Warning: This code illustrative only: not well tested, and not meant for production use
 * without regression / tests being applied
 */
// calculate quark propagators using MobiusEOFAFermionD
// check sum_{space}{tr(prop)} value in time to see the boundary condition
#include <Grid/Grid.h>

using namespace std;
using namespace Grid;

template<class Gimpl,class Field> class CovariantLaplacianCshift : public SparseMatrixBase<Field>
{
public:
  INHERIT_GIMPL_TYPES(Gimpl);

  GridBase *grid;
  GaugeField U;

  CovariantLaplacianCshift(GaugeField &_U)    :
    grid(_U.Grid()),
    U(_U) {  };

  virtual GridBase *Grid(void) { return grid; };

  virtual void  M    (const Field &in, Field &out)
  {
    out=Zero();
    for(int mu=0;mu<Nd-1;mu++) {
      GaugeLinkField Umu = PeekIndex<LorentzIndex>(U, mu); // NB: Inefficent
      out = out - Gimpl::CovShiftForward(Umu,mu,in);    
      out = out - Gimpl::CovShiftBackward(Umu,mu,in);    
      out = out + 2.0*in;
    }
  };
  virtual void  Mdag (const Field &in, Field &out) { M(in,out);}; // Laplacian is hermitian
  virtual  void Mdiag    (const Field &in, Field &out)                  {assert(0);}; // Unimplemented need only for multigrid
  virtual  void Mdir     (const Field &in, Field &out,int dir, int disp){assert(0);}; // Unimplemented need only for multigrid
  virtual  void MdirAll  (const Field &in, std::vector<Field> &out)     {assert(0);}; // Unimplemented need only for multigrid
};

void PointSource(Coordinate &coor,LatticePropagator &source)
{
  //  Coordinate coor({0,0,0,0});
  source=Zero();
  SpinColourMatrix kronecker; kronecker=1.0;
  pokeSite(kronecker,source,coor);
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

template<class Action>
void CheckBC(Action &D,LatticePropagator &propagator)
{
  
  GridBase *UGrid = D.GaugeGrid();
  GridBase *FGrid = D.FermionGrid();

  LatticeComplex    VV (UGrid);
  VV = trace(propagator);
  
  std::vector<TComplex> sumVV;
  sliceSum(VV,sumVV,Tdir);
  int Nt{static_cast<int>(sumVV.size())};
  RealD Ctd;
  ComplexD Ct;
  
  std::cout<<GridLogMessage <<"boundary phase"<< D.Params.boundary_phases <<std::endl;
  std::cout<<GridLogMessage <<"t\t C(t)\t\t\t |(C(t)-C(Nt-t)*phase_t)/2| "<< std::endl;
  for(int t=0;t<Nt;t++){
    Ct = TensorRemove(sumVV[t]);
    Ctd= sqrt(norm(0.5*( Ct - TensorRemove(sumVV[(Nt-t)%Nt]*D.Params.boundary_phases[3]))));
    std::cout<<GridLogMessage << t <<"\t "<< Ct <<"\t "<< Ctd <<std::endl;
  }
}


int main (int argc, char ** argv)
{
  const int Ls=8;

  Grid_init(&argc,&argv);
  GridLogLayout();

  // Double precision grids
  GridCartesian         * UGrid   = SpaceTimeGrid::makeFourDimGrid(GridDefaultLatt(), 
								   GridDefaultSimd(Nd,vComplex::Nsimd()),
								   GridDefaultMpi());
  GridRedBlackCartesian * UrbGrid = SpaceTimeGrid::makeFourDimRedBlackGrid(UGrid);
  GridCartesian         * FGrid   = SpaceTimeGrid::makeFiveDimGrid(Ls,UGrid);
  GridRedBlackCartesian * FrbGrid = SpaceTimeGrid::makeFiveDimRedBlackGrid(Ls,UGrid);

  //////////////////////////////////////////////////////////////////////
  // You can manage seeds however you like.
  // Recommend SeedUniqueString.
  //////////////////////////////////////////////////////////////////////
  std::vector<int> seeds4({1,2,3,4}); 
  GridParallelRNG          RNG4(UGrid);  RNG4.SeedFixedIntegers(seeds4);

  LatticeGaugeField Umu(UGrid);
  std::string config;

  std::cout<<GridLogMessage <<"Using cold configuration"<<std::endl;
  SU<Nc>::ColdConfiguration(Umu);
  config="ColdConfig";

  std::cout<<GridLogMessage <<"======================"<<std::endl;
  std::cout<<GridLogMessage <<"MobiusFermion action as Scaled Shamir kernel"<<std::endl;
  std::cout<<GridLogMessage <<"======================"<<std::endl;

  RealD mass = 0.5; // u/d, s, c ??
  RealD M5=1.0;
  RealD b=1.5;// Scale factor b+c=2, b-c=1
  RealD c=0.5;
  Real pv   = 1.0;

  // point source
  LatticePropagator point_source(UGrid);
  Coordinate Origin({0,0,0,0});
  PointSource(Origin,point_source);

  // We test
  // MobiusEOFAFermion(GaugeField& _Umu, GridCartesian& FiveDimGrid, GridRedBlackCartesian& FiveDimRedBlackGrid,
  //                   GridCartesian& FourDimGrid, GridRedBlackCartesian& FourDimRedBlackGrid,
  //                   RealD _mq1, RealD _mq2, RealD _mq3, RealD _shift, int pm,
  //                   RealD _M5, RealD _b, RealD _c, const ImplParams& p=ImplParams());
  {
    std::cout << GridLogMessage << "==============================================" << std::endl;
    std::cout << GridLogMessage << "periodic boundary condition {1,1,1,1}" << std::endl;
    std::vector<Complex> boundary = {1,1,1,1};
    MobiusFermionD::ImplParams Params(boundary);

    MobiusEOFAFermionD Strange_Op_L(Umu, *FGrid, *FrbGrid, *UGrid, *UrbGrid, mass, mass, pv,  0.0, -1, M5, b, c, Params);
    MobiusEOFAFermionD Strange_Op_R(Umu, *FGrid, *FrbGrid, *UGrid, *UrbGrid, pv,   mass, pv, -1.0,  1, M5, b, c, Params);

    LatticePropagator PointProps_L(UGrid);
    LatticePropagator PointProps_R(UGrid);
    Solve(Strange_Op_L, point_source, PointProps_L);
    Solve(Strange_Op_R, point_source, PointProps_R);

    std::cout << GridLogMessage << "----------------------------------------------" << std::endl;
    std::cout << GridLogMessage << "solve done, check C(t)=sum_{space}{tr(prop)}" << std::endl;
    CheckBC(Strange_Op_L,PointProps_L);
    CheckBC(Strange_Op_R,PointProps_R);
  
  }
  {
    std::cout << GridLogMessage << "==============================================" << std::endl;
    std::cout << GridLogMessage << "anti-periodic boundary condition {1,1,1,-1}" << std::endl;
    std::vector<Complex> boundary = {1,1,1,-1};
    MobiusFermionD::ImplParams Params(boundary);

    MobiusEOFAFermionD Strange_Op_L(Umu, *FGrid, *FrbGrid, *UGrid, *UrbGrid, mass, mass, pv,  0.0, -1, M5, b, c, Params);
    MobiusEOFAFermionD Strange_Op_R(Umu, *FGrid, *FrbGrid, *UGrid, *UrbGrid, pv,   mass, pv, -1.0,  1, M5, b, c, Params);

    LatticePropagator PointProps_L(UGrid);
    LatticePropagator PointProps_R(UGrid);
    Solve(Strange_Op_L, point_source, PointProps_L);
    Solve(Strange_Op_R, point_source, PointProps_R);

    std::cout << GridLogMessage << "----------------------------------------------" << std::endl;
    std::cout << GridLogMessage << "solve done, check C(t)=sum_{space}{tr(prop)}" << std::endl;
    CheckBC(Strange_Op_L,PointProps_L);
    CheckBC(Strange_Op_R,PointProps_R);
  

  }
  
  Grid_finalize();
}



