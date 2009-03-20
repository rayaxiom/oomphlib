#!MC 1100


#-----------------------------
#Set isolevel
#-----------------------------
$!VARSET |INNER_ISOLEVEL|=41.0
$!VARSET |OUTER_ISOLEVEL|=30.0
$!VARSET |OUTER_ISOLEVEL|=20.0


$!READDATASET  '"potential.dat" '
  READDATAOPTION = NEW
  RESETSTYLE = YES
  INCLUDETEXT = NO
  INCLUDEGEOM = NO
  INCLUDECUSTOMLABELS = NO
  VARLOADMODE = BYNAME
  ASSIGNSTRANDIDS = YES
  INITIALPLOTTYPE = CARTESIAN3D
  VARNAMELIST = '"V1" "V2" "V3" "V4"'
$!FIELDLAYERS SHOWMESH = NO
$!FIELDLAYERS SHOWMESH = YES
$!GLOBALCONTOUR 1  VAR = 4
$!CONTOURLEVELS RESETTONICE
  CONTOURGROUP = 1
  APPROXNUMVALUES = 15
$!ISOSURFACELAYERS SHOW = YES



$!THREEDVIEW 
  PSIANGLE = 73.1735
  THETAANGLE = -179.095
  ALPHAANGLE = -0.65812
  VIEWERPOSITION
    {
    X = 0.24111268283199
    Y = 15.26446545988457
    Z = 5.116892959846271
    }
$!VIEW PUSH


$!ANIMATEISOSURFACES 
  STARTVALUE = |INNER_ISOLEVEL|
  ENDVALUE = |INNER_ISOLEVEL|
  NUMSTEPS = 1
  GROUP = 1
  CREATEMOVIEFILE = NO
  LIMITSCREENSPEED = NO
  MAXSCREENSPEED = 0


$!CREATEISOZONES 
$!WRITEDATASET  "potential_boundary_outer_fluid.dat"
  INCLUDETEXT = NO
  INCLUDEGEOM = NO
  INCLUDECUSTOMLABELS = NO
  ASSOCIATELAYOUTWITHDATAFILE = NO
  ZONELIST =  [2]
  BINARY = NO
  USEPOINTFORMAT = YES
  PRECISION = 9





$!ANIMATEISOSURFACES 
  STARTVALUE = |OUTER_ISOLEVEL|
  ENDVALUE = |OUTER_ISOLEVEL|
  NUMSTEPS = 1
  GROUP = 1
  CREATEMOVIEFILE = NO
  LIMITSCREENSPEED = NO
  MAXSCREENSPEED = 0

$!CREATEISOZONES 
$!WRITEDATASET  "potential_boundary_outer_solid.dat"
  INCLUDETEXT = NO
  INCLUDEGEOM = NO
  INCLUDECUSTOMLABELS = NO
  ASSOCIATELAYOUTWITHDATAFILE = NO
  ZONELIST =  [3]
  BINARY = NO
  USEPOINTFORMAT = YES
  PRECISION = 9



$!FIELDLAYERS SHOWSHADE = YES
$!FIELDMAP [1]  SHADE{SHOW = NO}
$!FIELDMAP [2]  SHADE{COLOR = BLACK}
$!FIELDMAP [2]  EFFECTS{SURFACETRANSLUCENCY = 20}
$!FIELDLAYERS USETRANSLUCENCY = YES
$!FIELDLAYERS SHOWMESH = YES
$!FIELDMAP [1]  MESH{SHOW = NO}
$!FIELDMAP [2]  MESH{LINETHICKNESS = 0.4}
$!FIELDMAP [2]  EDGELAYER{COLOR = RED}
$!FIELDMAP [2]  MESH{COLOR = RED}
$!ISOSURFACEATTRIBUTES 1  CONTOUR{SHOW = NO}
$!ISOSURFACEATTRIBUTES 1  MESH{SHOW = NO}




$!THREEDAXIS XDETAIL{SHOWAXIS = NO}
$!THREEDAXIS YDETAIL{SHOWAXIS = NO}
$!THREEDAXIS ZDETAIL{SHOWAXIS = NO}


$!THREEDVIEW 
  PSIANGLE = 70.7009
  THETAANGLE = 165.514
  ALPHAANGLE = -1.86945
  VIEWERPOSITION
    {
    X = -3.984491941863154
    Y = 15.42196721690497
    Z = 6.077753132343476
    }
$!VIEW PUSH
$!VIEW FIT


$!EXPORTSETUP EXPORTFORMAT = AVI
$!EXPORTSETUP IMAGEWIDTH = 806
$!EXPORTSETUP EXPORTFNAME = 'bifurcation_from_potential.avi'
$!EXPORTSTART 
  EXPORTREGION = CURRENTFRAME
$!ROTATE3DVIEW THETA
  ANGLE = 45
  ROTATEORIGINLOCATION = DEFINEDORIGIN
$!REDRAWALL 
$!EXPORTNEXTFRAME 
$!ROTATE3DVIEW THETA
  ANGLE = 45
  ROTATEORIGINLOCATION = DEFINEDORIGIN
$!REDRAWALL 
$!EXPORTNEXTFRAME 
$!ROTATE3DVIEW THETA
  ANGLE = 45
  ROTATEORIGINLOCATION = DEFINEDORIGIN
$!REDRAWALL 
$!EXPORTNEXTFRAME 
$!ROTATE3DVIEW THETA
  ANGLE = 45
  ROTATEORIGINLOCATION = DEFINEDORIGIN
$!REDRAWALL 
$!EXPORTNEXTFRAME 
$!ROTATE3DVIEW THETA
  ANGLE = 45
  ROTATEORIGINLOCATION = DEFINEDORIGIN
$!REDRAWALL 
$!EXPORTNEXTFRAME 
$!ROTATE3DVIEW THETA
  ANGLE = 45
  ROTATEORIGINLOCATION = DEFINEDORIGIN
$!REDRAWALL 
$!EXPORTNEXTFRAME 
$!ROTATE3DVIEW THETA
  ANGLE = 45
  ROTATEORIGINLOCATION = DEFINEDORIGIN
$!REDRAWALL 
$!EXPORTNEXTFRAME 
$!ROTATE3DVIEW THETA
  ANGLE = 45
  ROTATEORIGINLOCATION = DEFINEDORIGIN
$!REDRAWALL 
$!EXPORTNEXTFRAME 
$!EXPORTFINISH 


