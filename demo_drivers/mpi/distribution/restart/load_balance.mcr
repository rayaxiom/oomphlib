#!MC 1120
$!VARSET |lostep|=0
$!VARSET |nstep|=200
$!VARSET |dstep|=1

$!VARSET |n_proc|=2

$!VARSET |postfix|=".plt"
$!VARSET |postfix|=".dat"
$!VARSET |PNG|=0
$!VARSET |dir|="RESLT" # _for_restart"


$!NEWLAYOUT 
$!IF |PNG|==0
     $!EXPORTSETUP EXPORTFORMAT = AVI
     $!EXPORTSETUP IMAGEWIDTH = 806
     $!EXPORTSETUP EXPORTFNAME = 'load_balance.avi'
     $!EXPORTSTART             
       EXPORTREGION = ALLFRAMES 
$!ENDIF             

$!LOOP |nstep|

$!VARSET |step|=(|lostep|+(|loop|-1)*|dstep|)
$!NEWLAYOUT 
$!DRAWGRAPHICS FALSE


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
$!FIELDMAP [1-|NUMZONES|]  EDGELAYER{LINETHICKNESS = 0.4}

$!THREEDAXIS AXISMODE = XYDEPENDENT
$!GLOBALTHREED AXISSCALEFACT{Z = 0.3}
$!FIELDLAYERS SHOWSHADE = YES


$!VARSET |n_proc_minus_one|=(|n_proc|-1)
$!LOOP |n_proc_minus_one|


$!VARSET |first_box|=(|NUMZONES|-1)
$!VARSET |last_box|=|NUMZONES|
$!ACTIVEFIELDMAPS -= [|first_box|-|last_box|]
$!VARSET |first_proc_1|=(|NUMZONES|+1)

$!READDATASET  '"|dir|/soln|step|_on_proc|LOOP|.dat" '
  READDATAOPTION = APPEND
  RESETSTYLE = NO
  INCLUDECUSTOMLABELS = NO
  VARLOADMODE = BYNAME
  ASSIGNSTRANDIDS = YES
  INITIALPLOTTYPE = CARTESIAN3D
  VARNAMELIST = '"V1" "V2" "V3"'

$!VARSET |last_proc_1|=(|NUMZONES|-2)


$!IF |LOOP|==1
  $!FIELDMAP [|first_proc_1|-|last_proc_1|]  EDGELAYER{COLOR = BLUE}  
$!ENDIF
$!IF |LOOP|==2
  $!FIELDMAP [|first_proc_1|-|last_proc_1|]  EDGELAYER{COLOR = YELLOW}  
$!ENDIF
$!IF |LOOP|==3
  $!FIELDMAP [|first_proc_1|-|last_proc_1|]  EDGELAYER{COLOR = BLACK}  
$!ENDIF
$!IF |LOOP|==4
  $!FIELDMAP [|first_proc_1|-|last_proc_1|]  EDGELAYER{COLOR = CYAN}  
$!ENDIF

$!ACTIVEFIELDMAPS += [|first_proc_1|-|last_proc_1|]
$!FIELDMAP [|first_proc_1|-|last_proc_1|]  EDGELAYER{LINETHICKNESS = 0.4}

$!ENDLOOP


$!ATTACHTEXT 
  ANCHORPOS
    {
    X = 60
    Y = 90
    }
  TEXTSHAPE
    {
    FONT = HELV
    HEIGHT = 26
    }
  BOX
    {
    MARGIN = 10
    LINETHICKNESS = 0.4
    }
  SCOPE = GLOBAL
  MACROFUNCTIONCOMMAND = ''
  TEXT = 'Step |step|'



#################################

$!VARSET |first_proc_0|=(|NUMZONES|+1)


$!READDATASET  '"|dir|/exact_soln|step|_on_proc0.dat" '
  READDATAOPTION = APPEND
  RESETSTYLE = NO
  INCLUDECUSTOMLABELS = NO
  VARLOADMODE = BYNAME
  ASSIGNSTRANDIDS = YES
  INITIALPLOTTYPE = CARTESIAN3D
  VARNAMELIST = '"V1" "V2" "V3"'

$!VARSET |last_proc_0|=(|NUMZONES|-2)

$!ACTIVEFIELDMAPS += [|first_proc_0|-|last_proc_0|]
$!FIELDMAP [|first_proc_0|-|last_proc_0|]  EDGELAYER{COLOR = GREEN}
$!FIELDMAP [|first_proc_0|-|last_proc_0|]  EDGELAYER{LINETHICKNESS = 0.1}

$!VARSET |first_box|=(|NUMZONES|-1)
$!VARSET |last_box|=|NUMZONES|
$!ACTIVEFIELDMAPS -= [|first_box|-|last_box|]

$!VARSET |first_proc_1|=(|NUMZONES|+1)


$!LOOP |n_proc_minus_one|

$!READDATASET  '"|dir|/exact_soln|step|_on_proc|LOOP|.dat" '
  READDATAOPTION = APPEND
  RESETSTYLE = NO
  INCLUDECUSTOMLABELS = NO
  VARLOADMODE = BYNAME
  ASSIGNSTRANDIDS = YES
  INITIALPLOTTYPE = CARTESIAN3D
  VARNAMELIST = '"V1" "V2" "V3"'

$!VARSET |last_proc_1|=(|NUMZONES|-2)

$!ACTIVEFIELDMAPS += [|first_proc_1|-|last_proc_1|]
$!FIELDMAP [|first_proc_1|-|last_proc_1|]  EDGELAYER{COLOR = GREEN}
$!FIELDMAP [|first_proc_1|-|last_proc_1|]  EDGELAYER{LINETHICKNESS = 0.1}


$!ENDLOOP

$!FIELDLAYERS USETRANSLUCENCY = YES
$!FIELDMAP [1-|NUMZONES|]  EFFECTS{SURFACETRANSLUCENCY = 80}


##################################################


$!REDRAWALL 
$!THREEDVIEW 
  PSIANGLE = 55.4314
  THETAANGLE = -111.381
  ALPHAANGLE = -1.35572
  VIEWERPOSITION
    {
    X = 7.668456291771297
    Y = 3.39786207341086
    Z = 17.31158296690474
    }
$!VIEW PUSH

$!VIEW FIT


###################################################

$!DRAWGRAPHICS TRUE
$!REDRAWALL


$!IF |PNG|==1
     $!EXPORTSETUP EXPORTFORMAT = PNG
     $!EXPORTSETUP IMAGEWIDTH = 600
     $!EXPORTSETUP EXPORTFNAME = 'load_balance|loop|.png'
     $!EXPORT
       EXPORTREGION = ALLFRAMES
$!ELSE
     $!EXPORTNEXTFRAME
$!ENDIF

$!ENDLOOP

$!IF |PNG|==0
$!EXPORTFINISH
$!ENDIF