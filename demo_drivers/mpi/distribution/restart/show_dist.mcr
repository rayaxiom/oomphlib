#!MC 1120



$!LOOP 3

$!VarSet |first_step| = 5
$!VarSet |dead_step| = (|first_step|+1)

$!IF |LOOP|==1
$!VarSet |dir| = "RESLT"
$!VarSet |step| = |first_step| 
$!ENDIF


$!IF |LOOP|==2
$!VarSet |dir| = "RESLT_after_load_balance"
$!VarSet |step| = |dead_step|
$!ENDIF

$!IF |LOOP|==3
$!VarSet |dir| = "RESLT"
$!VarSet |step| = |dead_step| 
$!ENDIF



$!READDATASET  '"|dir|/soln|step|_on_proc0.dat" '
  READDATAOPTION = NEW
  RESETSTYLE = YES
  INCLUDECUSTOMLABELS = NO
  VARLOADMODE = BYNAME
  ASSIGNSTRANDIDS = YES
  INITIALPLOTTYPE = CARTESIAN3D
  VARNAMELIST = '"V1" "V2" "V3"'
$!FIELDLAYERS SHOWMESH = NO

$!FIELDMAP [1-|NUMZONES|]  EDGELAYER{COLOR = RED}
$!FIELDMAP [1-|NUMZONES|]  EDGELAYER{LINETHICKNESS = 0.0200000000000000004}
$!REDRAWALL 

$!VarSet |FIRST_1| = (|NUMZONES|+1) 



$!READDATASET  '"|dir|/soln|step|_on_proc1.dat" '
  READDATAOPTION = APPEND
  RESETSTYLE = NO
  INCLUDECUSTOMLABELS = NO
  VARLOADMODE = BYNAME
  ASSIGNSTRANDIDS = YES
  INITIALPLOTTYPE = CARTESIAN3D
  VARNAMELIST = '"V1" "V2" "V3"'


$!ACTIVEFIELDMAPS += [|FIRST_1|-|NUMZONES|]
$!FIELDMAP [|FIRST_1|-|NUMZONES|]  EDGELAYER{COLOR = GREEN}
$!FIELDMAP [|FIRST_1|-|NUMZONES|]  EDGELAYER{LINETHICKNESS = 0.100000000000000006}
$!THREEDAXIS AXISMODE = XYDEPENDENT
$!GLOBALTHREED AXISSCALEFACT{Z = 0.1}
$!VIEW FIT
$!THREEDVIEW 
  PSIANGLE = 52.6371
  THETAANGLE = 246.866
  ALPHAANGLE = 0
  VIEWERPOSITION
    {
    X = 9.73526089941098
    Y = 4.531637265417084
    Z = 75.43495950669568
    }
$!VIEW PUSH


$!REDRAWALL 

$!PAUSE "bla"
$!ENDLOOP 
