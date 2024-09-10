/*
 * Warning: This code illustrative only: not well tested, and not meant for production use
 * without regression / tests being applied
 */
// 090924 sungwoo
// calculates mesons 2pts for G5,GiG5 and additional PJ5q
// along both time and spatial (x) direction

#include <Grid/Grid.h>

using namespace std;
using namespace Grid;

RealD LLscale =1.0;
RealD LCscale =1.0;

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

void MakePhase(Coordinate mom,LatticeComplex &phase)
{
  GridBase *grid = phase.Grid();
  auto latt_size = grid->GlobalDimensions();
  ComplexD ci(0.0,1.0);
  phase=Zero();

  LatticeComplex coor(phase.Grid());
  for(int mu=0;mu<Nd;mu++){
    RealD TwoPiL =  M_PI * 2.0/ latt_size[mu];
    LatticeCoordinate(coor,mu);
    phase = phase + (TwoPiL * mom[mu]) * coor;
  }
  phase = exp(phase*ci);
}
void PointSource(Coordinate &coor,LatticePropagator &source)
{
  //  Coordinate coor({0,0,0,0});
  source=Zero();
  SpinColourMatrix kronecker; kronecker=1.0;
  pokeSite(kronecker,source,coor);
}

template<class Action>
void Solve(Action &D,LatticePropagator &source,LatticePropagator &propagator,LatticePropagator &prop5)
{
  GridBase *UGrid = D.GaugeGrid();
  GridBase *FGrid = D.FermionGrid();

  LatticeFermion src4  (UGrid); 
  LatticeFermion src5  (FGrid); 
  LatticeFermion result5(FGrid);
  LatticeFermion result4(UGrid);
  // LatticePropagator prop5(FGrid);
  
  ConjugateGradient<LatticeFermion> CG(1.0e-9,100000);
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

      FermToProp<Action>(prop5,result5,s,c);
      FermToProp<Action>(propagator,result4,s,c);
    }
  }
  // LatticePropagator Axial_mu(UGrid); 

  // LatticeComplex    PA (UGrid); 
  // // LatticeComplex    PJ5q(UGrid);

  // std::vector<TComplex> sumPA;
  // // std::vector<TComplex> sumPJ5q;

  // Gamma g5(Gamma::Algebra::Gamma5);
  // D.ContractConservedCurrent(prop5,prop5,Axial_mu,source,Current::Axial,Tdir);
  // PA       = trace(g5*Axial_mu);      // Pseudoscalar-Axial conserved current
  // sliceSum(PA,sumPA,Tdir);

  // int Nt{static_cast<int>(sumPA.size())};

  // for(int t=0;t<Nt;t++) std::cout<<GridLogMessage <<"PAc["<<t<<"] "<<real(TensorRemove(sumPA[t]))*LCscale<<std::endl;
  
  // // D.ContractJ5q(prop5,PJ5q);
  // // sliceSum(PJ5q,sumPJ5q,Tdir);
  // // for(int t=0;t<Nt;t++) std::cout<<GridLogMessage <<"PJ5q["<<t<<"] "<<real(TensorRemove(sumPJ5q[t]))<<std::endl;


}


class MesonFile: Serializable {
public:
  // GRID_SERIALIZABLE_CLASS_MEMBERS(MesonFile, std::vector<std::vector<Complex> >, data);
  GRID_SERIALIZABLE_CLASS_MEMBERS(MesonFile, std::vector<Complex>,  data);
};

void MesonTrace_hdf(Hdf5Writer &WR,LatticePropagator &q1,LatticePropagator &q2)
{
  // const int nchannel=4;
  // Gamma::Algebra Gammas[nchannel][2] = {
  //   {Gamma::Algebra::Gamma5      ,Gamma::Algebra::Gamma5},
  //   {Gamma::Algebra::GammaTGamma5,Gamma::Algebra::GammaTGamma5},
  //   {Gamma::Algebra::GammaTGamma5,Gamma::Algebra::Gamma5},
  //   {Gamma::Algebra::Gamma5      ,Gamma::Algebra::GammaTGamma5}
  // };

  // std::vector<std::string> channel_name({"G5_G5",
  // 					 "GTG5_GTG5",
  // 					 "GTG5_G5",
  // 					 "G5_GTG5"});
  
  const int nchannel=9;
  Gamma::Algebra Gammas[nchannel][2] = {
    {Gamma::Algebra::Gamma5      ,Gamma::Algebra::Gamma5},
    {Gamma::Algebra::GammaTGamma5,Gamma::Algebra::GammaTGamma5},
    {Gamma::Algebra::GammaXGamma5,Gamma::Algebra::GammaXGamma5},
    {Gamma::Algebra::GammaYGamma5,Gamma::Algebra::GammaYGamma5},
    {Gamma::Algebra::GammaZGamma5,Gamma::Algebra::GammaYGamma5},
    {Gamma::Algebra::GammaT      ,Gamma::Algebra::GammaT},
    {Gamma::Algebra::GammaX      ,Gamma::Algebra::GammaX},
    {Gamma::Algebra::GammaY      ,Gamma::Algebra::GammaY},
    {Gamma::Algebra::GammaZ      ,Gamma::Algebra::GammaY},
  };

  std::vector<std::string> channel_name({"G5_G5",
	"GTG5_GTG5",
	"GXG5_GXG5",
	"GYG5_GYG5",
	"GZG5_GZG5",
	"GT_GT",
	"GX_GX",
	"GY_GY",
	"GZ_GZ",
  					 	});
  
  Gamma G5(Gamma::Algebra::Gamma5);

  LatticeComplex meson_CF(q1.Grid());
  // Hdf5Writer WR(file);

  // std::vector<MesonFile> MF;
  // MF.resize(nchannel);
  for(int ch=0;ch<nchannel;ch++){

    MesonFile MF;
    
    Gamma Gsrc(Gammas[ch][0]);
    Gamma Gsnk(Gammas[ch][1]);

    meson_CF = trace(G5*adj(q1)*G5*Gsnk*q2*adj(Gsrc));

    std::vector<TComplex> meson_X;
    sliceSum(meson_CF,meson_X, Xdir);

    int nx=meson_X.size();

    std::vector<Complex> corr_X(nx);
    for(int t=0;t<nx;t++){
      corr_X[t] = TensorRemove(meson_X[t]); // Yes this is ugly, not figured a work around
      std::cout << " channel "<<ch<<" x "<<t<<" " <<corr_X[t]<<std::endl;
    }
    // MF[ch].data.push_back(corr);
    MF.data=corr_X;

    // of << channel_name[ch] << " " << MF << std::endl;
    write(WR,channel_name[ch]+"_x",MF);

    

    std::vector<TComplex> meson_T;
    sliceSum(meson_CF,meson_T, Tdir);

    int nt=meson_T.size();

    std::vector<Complex> corr(nt);
    for(int t=0;t<nt;t++){
      corr[t] = TensorRemove(meson_T[t]); // Yes this is ugly, not figured a work around
      std::cout << " channel "<<ch<<" t "<<t<<" " <<corr[t]<<std::endl;
    }
    // MF[ch].data.push_back(corr);
    MF.data=corr;

    // of << channel_name[ch] << " " << MF << std::endl;
    write(WR,channel_name[ch]+"_t",MF);
    
  }
  // XmlWriter WR(file);
  // write(WR,channel_name[ch],MF);

}

void MesonTrace(std::ofstream &of,LatticePropagator &q1,LatticePropagator &q2)
{
  const int nchannel=4;
  Gamma::Algebra Gammas[nchannel][2] = {
    {Gamma::Algebra::Gamma5      ,Gamma::Algebra::Gamma5},
    {Gamma::Algebra::GammaTGamma5,Gamma::Algebra::GammaTGamma5},
    {Gamma::Algebra::GammaTGamma5,Gamma::Algebra::Gamma5},
    {Gamma::Algebra::Gamma5      ,Gamma::Algebra::GammaTGamma5}
  };

  std::vector<std::string> channel_name({"G5_G5",
					 "GTG5_GTG5",
					 "GTG5_G5",
					 "G5_GTG5"});
  
  Gamma G5(Gamma::Algebra::Gamma5);

  LatticeComplex meson_CF(q1.Grid());

  for(int ch=0;ch<nchannel;ch++){

    MesonFile MF;
    
    Gamma Gsrc(Gammas[ch][0]);
    Gamma Gsnk(Gammas[ch][1]);

    meson_CF = trace(G5*adj(q1)*G5*Gsnk*q2*adj(Gsrc));

    std::vector<TComplex> meson_X;
    sliceSum(meson_CF,meson_X, Xdir);

    int nx=meson_X.size();

    std::vector<Complex> corr_X(nx);
    for(int t=0;t<nx;t++){
      corr_X[t] = TensorRemove(meson_X[t]); // Yes this is ugly, not figured a work around
      std::cout << " channel "<<ch<<" x "<<t<<" " <<corr_X[t]<<std::endl;
    }

    std::vector<TComplex> meson_T;
    sliceSum(meson_CF,meson_T, Tdir);

    int nt=meson_T.size();

    std::vector<Complex> corr(nt);
    for(int t=0;t<nt;t++){
      corr[t] = TensorRemove(meson_T[t]); // Yes this is ugly, not figured a work around
      std::cout << " channel "<<ch<<" t "<<t<<" " <<corr[t]<<std::endl;
    }
    MF.data=corr;
    // MF.data=corr;

    of << channel_name[ch] << " " << MF << std::endl;
    
  }
  // XmlWriter WR(file);
  // write(WR,channel_name[ch],MF);

}

template<class Action>
void Meson5Trace_hdf(Action &D,Hdf5Writer &WR,LatticePropagator &source,LatticePropagator &prop5)
{
  // LatticePropagator Axial_mu(UGrid); 

  GridBase *UGrid = D.GaugeGrid();
  LatticeComplex    PJ5q(UGrid);
  // LatticeComplex    PJ5q(prop5.Grid());

  // std::vector<TComplex> sumPA;
  std::vector<TComplex> sumPJ5q;
  std::vector<TComplex> sumPJ5q_x;

  Gamma g5(Gamma::Algebra::Gamma5);
  // D.ContractConservedCurrent(prop5,prop5,Axial_mu,source,Current::Axial,Tdir);
  // PA       = trace(g5*Axial_mu);      // Pseudoscalar-Axial conserved current
  // sliceSum(PA,sumPA,Tdir);

  // int Nt{static_cast<int>(sumPA.size())};

  // for(int t=0;t<Nt;t++) std::cout<<GridLogMessage <<"PAc["<<t<<"] "<<real(TensorRemove(sumPA[t]))*LCscale<<std::endl;
  
  MesonFile MF;
  D.ContractJ5q(prop5,PJ5q);
  sliceSum(PJ5q,sumPJ5q,Tdir);
  int Nt{static_cast<int>(sumPJ5q.size())};
  std::vector<Complex> corr(Nt);
  // for(int t=0;t<Nt;t++) std::cout<<GridLogMessage <<"PJ5q_t["<<t<<"] "<<real(TensorRemove(sumPJ5q[t]))<<std::endl;
  for(int t=0;t<Nt;t++) {
    corr[t]=TensorRemove(sumPJ5q[t]);
    std::cout<<GridLogMessage <<"PJ5q_t["<<t<<"] "<<corr[t]<<std::endl;
  }
  MF.data=corr;
  write(WR,"PJ5q_t",MF);
  
  MesonFile MF_X;
  sliceSum(PJ5q,sumPJ5q_x,Xdir);
  int Nx{static_cast<int>(sumPJ5q_x.size())};
  std::vector<Complex> corr_X(Nx);
  for(int x=0;x<Nx;x++) {
    corr_X[x]=TensorRemove(sumPJ5q_x[x]);
    std::cout<<GridLogMessage <<"PJ5q_x["<<x<<"] "<< corr_X[x]<<std::endl;
  }
  MF_X.data=corr_X;
  write(WR,"PJ5q_x",MF_X);
  
}

template<class Action>
void Meson5Trace(Action &D,LatticePropagator &source,LatticePropagator &prop5)
{
  GridBase *UGrid = D.GaugeGrid();
  // LatticePropagator Axial_mu(UGrid); 

  // LatticeComplex    PA (UGrid); 
  LatticeComplex    PJ5q(UGrid);

  // std::vector<TComplex> sumPA;
  std::vector<TComplex> sumPJ5q;
  std::vector<TComplex> sumPJ5q_x;

  Gamma g5(Gamma::Algebra::Gamma5);
  // D.ContractConservedCurrent(prop5,prop5,Axial_mu,source,Current::Axial,Tdir);
  // PA       = trace(g5*Axial_mu);      // Pseudoscalar-Axial conserved current
  // sliceSum(PA,sumPA,Tdir);

  // int Nt{static_cast<int>(sumPA.size())};

  // for(int t=0;t<Nt;t++) std::cout<<GridLogMessage <<"PAc["<<t<<"] "<<real(TensorRemove(sumPA[t]))*LCscale<<std::endl;
  
  D.ContractJ5q(prop5,PJ5q);
  sliceSum(PJ5q,sumPJ5q,Tdir);
  int Nt{static_cast<int>(sumPJ5q.size())};
  for(int t=0;t<Nt;t++) std::cout<<GridLogMessage <<"PJ5q_t["<<t<<"] "<<real(TensorRemove(sumPJ5q[t]))<<std::endl;

  sliceSum(PJ5q,sumPJ5q_x,Xdir);
  int Nx{static_cast<int>(sumPJ5q_x.size())};
  for(int t=0;t<Nx;t++) std::cout<<GridLogMessage <<"PJ5q_x["<<t<<"] "<<real(TensorRemove(sumPJ5q_x[t]))<<std::endl;


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

  //////////////////////////////////////////////////////////////////////
  // You can manage seeds however you like.
  // Recommend SeedUniqueString.
  //////////////////////////////////////////////////////////////////////
  std::vector<int> seeds4({1,2,3,4}); 
  GridParallelRNG          RNG4(UGrid);  RNG4.SeedFixedIntegers(seeds4);

  LatticeGaugeField Umu(UGrid);
  std::string config;
  std::string outfile;
  RealD M5, mass;
  if( argc > 1 && argv[1][0] != '-' )
  {
    std::cout<<GridLogMessage <<"Loading configuration from "<<argv[1]<<std::endl;
    FieldMetaData header;
    NerscIO::readConfiguration(Umu, header, argv[1]);
    config=argv[1];
    M5=stod(argv[2]);
    mass=stod(argv[3]);
    outfile=argv[4];
  }
  else
  {
    std::cout<<GridLogMessage <<"Using hot configuration"<<std::endl;
    SU<Nc>::ColdConfiguration(Umu);
    //    SU<Nc>::HotConfiguration(RNG4,Umu);
    config="HotConfig";
    M5=1.5;			// give some default numbers
    mass=0.1;			// give some default numbers
    outfile=config+".h5";
  }

  std::vector<RealD> masses({ mass} ); // u/d, s, c ??
  // put just a single mass from input

  int nmass = masses.size();

  std::vector<MobiusFermionD *> FermActs;
  
  std::cout<<GridLogMessage <<"======================"<<std::endl;
  std::cout<<GridLogMessage <<"MobiusFermion action as Scaled Shamir kernel"<<std::endl;
  std::cout<<GridLogMessage <<"======================"<<std::endl;

  for(auto mass: masses) {

    RealD b=1.5;// Scale factor b+c=2, b-c=1
    RealD c=0.5;

    std::cout << GridLogMessage << "==============================================" << std::endl;
    std::cout << GridLogMessage << "anti-periodic boundary condition {1,1,1,-1}" << std::endl;
    std::vector<Complex> boundary = {1,1,1,-1};
    MobiusFermionD::ImplParams Params(boundary);

    FermActs.push_back(new MobiusFermionD(Umu,*FGrid,*FrbGrid,*UGrid,*UrbGrid,mass,M5,b,c,Params));
   
  }

  LatticePropagator point_source(UGrid);
  // LatticePropagator wall_source(UGrid);
  // LatticePropagator gaussian_source(UGrid);

  Coordinate Origin({0,0,0,0});
  PointSource   (Origin,point_source);
  // WallSource  (0,wall_source);
  // Z2WallSource  (RNG4,0,wall_source);
  // GaussianSource(Origin,Umu,gaussian_source);
  
  std::vector<LatticePropagator> PointProps(nmass,UGrid);
  std::vector<LatticePropagator> PointProps5(nmass,FGrid);
  // std::vector<LatticePropagator> GaussProps(nmass,UGrid);
  // std::vector<LatticePropagator> Z2Props   (nmass,UGrid);
  // std::vector<LatticePropagator> zeromomProps   (nmass,UGrid);


  Hdf5Writer WR(outfile);

  
  for(int m=0;m<nmass;m++) {
    
    Solve(*FermActs[m],point_source   ,PointProps[m], PointProps5[m]);
    Meson5Trace_hdf(*FermActs[m],WR,point_source  ,PointProps5[m]);
    // Meson5Trace(*FermActs[m],point_source  ,PointProps5[m]);
    // Solve(*FermActs[m],gaussian_source,GaussProps[m]);
    // Solve(*FermActs[m],wall_source    ,Z2Props[m]);
    // Solve(*FermActs[m],wall_source    ,zeromomProps[m]);
  
  }

  // LatticeComplex phase(UGrid);
  // Coordinate mom({0,0,0,0});
  // MakePhase(mom,phase);
  

  // std::ofstream of;
  // of.open( outfile, std::ios::out | std::ios::trunc);
  // if(!of) assert(false);
  // of << std::scientific << std::setprecision(15);

  

  for(int m1=0 ;m1<nmass;m1++) {
  for(int m2=m1;m2<nmass;m2++) {
    std::stringstream ssp,ssg,ssz;

    ssp<<config<< "_m" << m1 << "_m"<< m2 << "_point_meson.xml";
    // ssg<<config<< "_m" << m1 << "_m"<< m2 << "_smeared_meson.xml";
    // ssz<<config<< "_m" << m1 << "_m"<< m2 << "_wall_meson.xml";

    // sungwoo: phase not even being used
    MesonTrace_hdf(WR,PointProps[m1],PointProps[m2]);
    // MesonTrace(ssg.str(),GaussProps[m1],GaussProps[m2],phase);
    // MesonTrace(ssz.str(),Z2Props[m1],Z2Props[m2],phase);
    // MesonTrace(ssz.str(),zeromomProps[m1],zeromomProps[m2],phase);
  }}

  Grid_finalize();
}



