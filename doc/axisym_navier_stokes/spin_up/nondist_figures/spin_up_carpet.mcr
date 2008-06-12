#!MC 1000

$!VARSET |PNG|=0


#$!GETUSERINPUT |lostep| INSTRUCTIONS = "Loop. First Step??"
$!VARSET  |lostep|=0
#$!GETUSERINPUT |dlstep| INSTRUCTIONS = "Loop. Step Increment?"
$!VARSET  |dlstep|=1
$!GETUSERINPUT |nstep| INSTRUCTIONS = "Loop. Number of Steps??"
#$!VARSET |nstep| = 101

$!LOOP |nstep|
$!VarSet |nnstep| = |LOOP|
$!VarSet |nnstep| -= 1
$!VarSet |iistep| = |dlstep|
$!VarSet |iistep| *= |nnstep|
$!VarSet |iistep| += |lostep|
$!NEWLAYOUT
$!DRAWGRAPHICS FALSE
#    $!IF |iistep| < 10 
#      $!VARSET |istep|='00|iistep|'
#    $!ENDIF
#    $!IF |iistep| > 9 
#      $!VARSET |istep|='0|iistep|'
#    $!ENDIF
#    $!IF |iistep| > 99 
#      $!VARSET |istep|=|iistep|
#    $!ENDIF
$!VARSET |istep|=|iistep|
#$!VARSET |istep|+=1
#$!VARSET |istep|*=10

$!DRAWGRAPHICS FALSE


$!VarSet |LFDSFN1| = '"soln|istep|.dat"'
$!VarSet |LFDSVL1| = '"V1" "V2" "V3" "V4" "V5" "V6"'
$!SETSTYLEBASE FACTORY
$!PAPER 
  BACKGROUNDCOLOR = WHITE
  ISTRANSPARENT = YES
  ORIENTPORTRAIT = NO
  SHOWGRID = YES
  SHOWRULER = YES
  SHOWPAPER = YES
  PAPERSIZE = A4
  PAPERSIZEINFO
    {
    A3
      {
      WIDTH = 11.693
      HEIGHT = 16.535
      }
    A4
      {
      WIDTH = 8.2677
      HEIGHT = 11.693
      LEFTHARDCLIPOFFSET = 0.125
      RIGHTHARDCLIPOFFSET = 0.125
      TOPHARDCLIPOFFSET = 0.125
      BOTTOMHARDCLIPOFFSET = 0.125
      }
    }
  RULERSPACING = ONECENTIMETER
  PAPERGRIDSPACING = ONETENTHCENTIMETER
  REGIONINWORKAREA
    {
    X1 = -0.05
    Y1 = -0.05
    X2 = 11.74
    Y2 = 8.318
    }
$!COLORMAP 
  CONTOURCOLORMAP = SMRAINBOW
$!COLORMAPCONTROL RESETTOFACTORY
### Frame Number 1 ###
$!READDATASET  '|LFDSFN1|' 
  INITIALPLOTTYPE = CARTESIAN3D
  INCLUDETEXT = NO
  INCLUDEGEOM = NO
  VARLOADMODE = BYNAME
  VARNAMELIST = '|LFDSVL1|' 

$!VARSET |ZONES| = |NUMZONES|
$!VARSET |ZONES| -= 2
$!VARSET |EXTRA| = |ZONES|
$!VARSET |EXTRA| += 1

$!RENAMEDATASETVAR 
  VAR = 1
  NAME = 'r' 
$!RENAMEDATASETVAR 
  VAR = 2
  NAME = 'z' 
$!RENAMEDATASETVAR 
  VAR = 3
  NAME = 'u(r,z,t)' 
$!RENAMEDATASETVAR 
  VAR = 4
  NAME = 'w(r,z,t)' 
$!RENAMEDATASETVAR 
  VAR = 5
  NAME = 'v(r,z,t)' 
$!RENAMEDATASETVAR 
  VAR = 6
  NAME = 'p(r,z,t)' 
$!FRAMELAYOUT 
  SHOWBORDER = NO
  SHOWHEADER = NO
  HEADERCOLOR = RED
  XYPOS
    {
    X = 0.13919
    Y = 0.21438
    }
  WIDTH = 5.7112
  HEIGHT = 3.9132
$!PLOTTYPE  = CARTESIAN3D
$!FRAMENAME  = 'Frame 001' 
$!ACTIVEFIELDZONES  =  [1-|NUMZONES|]
$!GLOBALRGB 
  RANGEMIN = 0
  RANGEMAX = 1
$!GLOBALCONTOUR  1
  LEGEND
    {
    XYPOS
      {
      X = 95
      }
    }
  COLORMAPFILTER
    {
    CONTINUOUSCOLOR
      {
      CMIN = 0
      CMAX = 1
      }
    }
$!GLOBALCONTOUR  2
  LEGEND
    {
    XYPOS
      {
      X = 95
      }
    }
  COLORMAPFILTER
    {
    CONTINUOUSCOLOR
      {
      CMIN = 0
      CMAX = 1
      }
    }
$!GLOBALCONTOUR  3
  LEGEND
    {
    XYPOS
      {
      X = 95
      }
    }
  COLORMAPFILTER
    {
    CONTINUOUSCOLOR
      {
      CMIN = 0
      CMAX = 1
      }
    }
$!GLOBALCONTOUR  4
  LEGEND
    {
    XYPOS
      {
      X = 95
      }
    }
  COLORMAPFILTER
    {
    CONTINUOUSCOLOR
      {
      CMIN = 0
      CMAX = 1
      }
    }
$!GLOBALSCATTER 
  LEGEND
    {
    XYPOS
      {
      X = 95
      }
    }
  REFSCATSYMBOL
    {
    COLOR = RED
    FILLCOLOR = RED
    }
$!FIELD  [1-|ZONES|]
  MESH
    {
    COLOR = RED
    }
  CONTOUR
    {
    CONTOURTYPE = FLOOD
    COLOR = RED
    USELIGHTINGEFFECT = YES
    }
  VECTOR
    {
    COLOR = RED
    }
  SCATTER
    {
    COLOR = RED
    }
  SHADE
    {
    COLOR = WHITE
    }
  BOUNDARY
    {
    SHOW = YES
    COLOR = BLACK
    LINETHICKNESS = 0.02
    }
  POINTS
    {
    POINTSTOPLOT = SURFACENODES
    }
  SURFACES
    {
    SURFACESTOPLOT = KPLANES
    }
  VOLUMEMODE
    {
    VOLUMEOBJECTSTOPLOT
      {
      SHOWISOSURFACES = NO
      SHOWSLICES = NO
      SHOWSTREAMTRACES = NO
      }
    }




$!FIELD  [|EXTRA|-|NUMZONES|]
  MESH
    {
    COLOR = RED
    }
  CONTOUR
    {
    CONTOURTYPE = FLOOD
    COLOR = RED
    USELIGHTINGEFFECT = YES
    }
  VECTOR
    {
    COLOR = RED
    }
  SCATTER
    {
    COLOR = RED
    }
  SHADE
    {
    SHOW = NO
    COLOR = WHITE
    }
  BOUNDARY
    {
    SHOW = NO
    COLOR = BLACK
    LINETHICKNESS = 0.02
    }
  POINTS
    {
    POINTSTOPLOT = SURFACENODES
    }
  SURFACES
    {
    SURFACESTOPLOT = KPLANES
    }
  VOLUMEMODE
    {
    VOLUMEOBJECTSTOPLOT
      {
      SHOWISOSURFACES = NO
      SHOWSLICES = NO
      SHOWSTREAMTRACES = NO
      }
    }
  GROUP = 2

$!THREEDAXIS 
  XDETAIL
    {
    VARNUM = 1
    }
  YDETAIL
    {
    VARNUM = 2
    }
  ZDETAIL
    {
    VARNUM = 3
    }
$!VIEW FIT
$!THREEDAXIS 
  AXISMODE = XYDEPENDENT
  XYDEPXTOYRATIO = 1
  DEPXTOYRATIO = 1
  DEPXTOZRATIO = 0.0207407421021
$!THREEDAXIS 
  XDETAIL
    {
    RANGEMIN = -0.10499999839812516889
    RANGEMAX = 1.1049999499693512472
    GRSPACING = 0.2
    AXISLINE
      {
      EDGE = 2
      }
    }
$!THREEDAXIS 
  YDETAIL
    {
    RANGEMIN = -0.11749999839812517999
    RANGEMAX = 1.3674999499693512028
    GRSPACING = 0.2
    AXISLINE
      {
      EDGE = 3
      }
    }
$!THREEDAXIS 
  ZDETAIL
    {
    RANGEMIN = -0.0077000002376735208684
    RANGEMAX = 0.0077000002376735208684
    GRSPACING = 0.005
    AXISLINE
      {
      EDGE = 2
      }
    }
$!GLOBALISOSURFACE 
  MARCHINGCUBEALGORITHM = CLASSICPLUS
$!GLOBALSLICE 
  BOUNDARY
    {
    SHOW = NO
    }
$!GLOBALTHREED 
  AXISSCALEFACT
    {
    X = 1
    Y = 1
    Z = 48.2142825497
    }
  ROTATEORIGIN
    {
    X = 0.499999975786
    Y = 0.624999975786
    Z = 0
    }
  LIGHTSOURCE
    {
    INTENSITY = 75
    BACKGROUNDLIGHT = 30
    }
  LINELIFTFRACTION = 0.2
  SYMBOLLIFTFRACTION = 0.6
  VECTORLIFTFRACTION = 0.7
$!THREEDVIEW 
  PSIANGLE = 82.8049
  THETAANGLE = 239.756
  VIEWERPOSITION
    {
    X = 11.0903179555
    Y = 6.79957775824
    Z = 0.0320982278157
    }
  VIEWWIDTH = 2.08156
$!FIELDLAYERS 
  SHOWMESH = NO
  SHOWSHADE = YES
  SHOWBOUNDARY = YES
### Frame Number 2 ###
$!CREATENEWFRAME 
$!FRAMELAYOUT 
  SHOWBORDER = NO
  SHOWHEADER = NO
  HEADERCOLOR = RED
  XYPOS
    {
    X = 5.8568
    Y = 0.2284
    }
  WIDTH = 5.7112
  HEIGHT = 3.9132
$!PLOTTYPE  = CARTESIAN3D
$!FRAMENAME  = 'Frame 001' 
$!ACTIVEFIELDZONES  =  [1-|NUMZONES|]
$!GLOBALRGB 
  RANGEMIN = 0
  RANGEMAX = 1
$!GLOBALCONTOUR  1
  LEGEND
    {
    XYPOS
      {
      X = 95
      }
    }
  COLORMAPFILTER
    {
    CONTINUOUSCOLOR
      {
      CMIN = 0
      CMAX = 1
      }
    }
$!GLOBALCONTOUR  2
  LEGEND
    {
    XYPOS
      {
      X = 95
      }
    }
  COLORMAPFILTER
    {
    CONTINUOUSCOLOR
      {
      CMIN = 0
      CMAX = 1
      }
    }
$!GLOBALCONTOUR  3
  LEGEND
    {
    XYPOS
      {
      X = 95
      }
    }
  COLORMAPFILTER
    {
    CONTINUOUSCOLOR
      {
      CMIN = 0
      CMAX = 1
      }
    }
$!GLOBALCONTOUR  4
  LEGEND
    {
    XYPOS
      {
      X = 95
      }
    }
  COLORMAPFILTER
    {
    CONTINUOUSCOLOR
      {
      CMIN = 0
      CMAX = 1
      }
    }
$!GLOBALSCATTER 
  LEGEND
    {
    XYPOS
      {
      X = 95
      }
    }
  REFSCATSYMBOL
    {
    COLOR = RED
    FILLCOLOR = RED
    }
$!FIELD  [1-|ZONES|]
  MESH
    {
    COLOR = RED
    }
  CONTOUR
    {
    CONTOURTYPE = FLOOD
    COLOR = RED
    USELIGHTINGEFFECT = YES
    }
  VECTOR
    {
    COLOR = RED
    }
  SCATTER
    {
    COLOR = RED
    }
  SHADE
    {
    COLOR = WHITE
    }
  BOUNDARY
    {
    SHOW = YES
    COLOR = BLACK
    LINETHICKNESS = 0.02
    }
  POINTS
    {
    POINTSTOPLOT = SURFACENODES
    }
  SURFACES
    {
    SURFACESTOPLOT = KPLANES
    }
  VOLUMEMODE
    {
    VOLUMEOBJECTSTOPLOT
      {
      SHOWISOSURFACES = NO
      SHOWSLICES = NO
      SHOWSTREAMTRACES = NO
      }
    }




$!FIELD  [|EXTRA|-|NUMZONES|]
  MESH
    {
    COLOR = RED
    }
  CONTOUR
    {
    CONTOURTYPE = FLOOD
    COLOR = RED
    USELIGHTINGEFFECT = YES
    }
  VECTOR
    {
    COLOR = RED
    }
  SCATTER
    {
    COLOR = RED
    }
  SHADE
    {
    SHOW = NO
    COLOR = WHITE
    }
  BOUNDARY
    {
    SHOW = NO
    COLOR = BLACK
    LINETHICKNESS = 0.02
    }
  POINTS
    {
    POINTSTOPLOT = SURFACENODES
    }
  SURFACES
    {
    SURFACESTOPLOT = KPLANES
    }
  VOLUMEMODE
    {
    VOLUMEOBJECTSTOPLOT
      {
      SHOWISOSURFACES = NO
      SHOWSLICES = NO
      SHOWSTREAMTRACES = NO
      }
    }
  GROUP = 2





$!THREEDAXIS 
  XDETAIL
    {
    VARNUM = 1
    }
  YDETAIL
    {
    VARNUM = 2
    }
  ZDETAIL
    {
    VARNUM = 4
    }
$!VIEW FIT
$!THREEDAXIS 
  AXISMODE = XYDEPENDENT
  XYDEPXTOYRATIO = 1
  DEPXTOYRATIO = 1
  DEPXTOZRATIO = 0.0207407421021
$!THREEDAXIS 
  XDETAIL
    {
    RANGEMIN = -0.10499999839812516889
    RANGEMAX = 1.1049999499693512472
    GRSPACING = 0.2
    AXISLINE
      {
      EDGE = 2
      }
    }
$!THREEDAXIS 
  YDETAIL
    {
    RANGEMIN = -0.11749999839812517999
    RANGEMAX = 1.3674999499693512028
    GRSPACING = 0.2
    AXISLINE
      {
      EDGE = 3
      }
    }
$!THREEDAXIS 
  ZDETAIL
    {
    RANGEMIN = -0.0077000002376735208684
    RANGEMAX = 0.0077000002376735208684
    GRSPACING = 0.005
    AXISLINE
      {
      EDGE = 2
      }
    }
$!GLOBALISOSURFACE 
  MARCHINGCUBEALGORITHM = CLASSICPLUS
$!GLOBALSLICE 
  BOUNDARY
    {
    SHOW = NO
    }
$!GLOBALTHREED 
  AXISSCALEFACT
    {
    X = 1
    Y = 1
    Z = 48.2142825497
    }
  ROTATEORIGIN
    {
    X = 0.499999975786
    Y = 0.624999975786
    Z = 0
    }
  LIGHTSOURCE
    {
    INTENSITY = 75
    BACKGROUNDLIGHT = 30
    }
  LINELIFTFRACTION = 0.2
  SYMBOLLIFTFRACTION = 0.6
  VECTORLIFTFRACTION = 0.7
$!THREEDVIEW 
  PSIANGLE = 82.8049
  THETAANGLE = 239.756
  VIEWERPOSITION
    {
    X = 11.0903179555
    Y = 6.79957775824
    Z = 0.0320982278157
    }
  VIEWWIDTH = 2.08156
$!FIELDLAYERS 
  SHOWMESH = NO
  SHOWSHADE = YES
  SHOWBOUNDARY = YES
### Frame Number 3 ###
$!CREATENEWFRAME 
$!FRAMELAYOUT 
  SHOWBORDER = NO
  SHOWHEADER = NO
  HEADERCOLOR = RED
  XYPOS
    {
    X = 0.125
    Y = 4.1261
    }
  WIDTH = 5.7112
  HEIGHT = 3.9132
$!PLOTTYPE  = CARTESIAN3D
$!FRAMENAME  = 'Frame 001' 
$!ACTIVEFIELDZONES  =  [1-|NUMZONES|]
$!GLOBALRGB 
  RANGEMIN = 0
  RANGEMAX = 1
$!GLOBALCONTOUR  1
  LEGEND
    {
    XYPOS
      {
      X = 95
      }
    }
  COLORMAPFILTER
    {
    CONTINUOUSCOLOR
      {
      CMIN = 0
      CMAX = 1
      }
    }
$!GLOBALCONTOUR  2
  LEGEND
    {
    XYPOS
      {
      X = 95
      }
    }
  COLORMAPFILTER
    {
    CONTINUOUSCOLOR
      {
      CMIN = 0
      CMAX = 1
      }
    }
$!GLOBALCONTOUR  3
  LEGEND
    {
    XYPOS
      {
      X = 95
      }
    }
  COLORMAPFILTER
    {
    CONTINUOUSCOLOR
      {
      CMIN = 0
      CMAX = 1
      }
    }
$!GLOBALCONTOUR  4
  LEGEND
    {
    XYPOS
      {
      X = 95
      }
    }
  COLORMAPFILTER
    {
    CONTINUOUSCOLOR
      {
      CMIN = 0
      CMAX = 1
      }
    }
$!GLOBALSCATTER 
  LEGEND
    {
    XYPOS
      {
      X = 95
      }
    }
  REFSCATSYMBOL
    {
    COLOR = RED
    FILLCOLOR = RED
    }
$!FIELD  [1-|ZONES|]
  MESH
    {
    COLOR = RED
    }
  CONTOUR
    {
    CONTOURTYPE = FLOOD
    COLOR = RED
    USELIGHTINGEFFECT = YES
    }
  VECTOR
    {
    COLOR = RED
    }
  SCATTER
    {
    COLOR = RED
    }
  SHADE
    {
    COLOR = WHITE
    }
  BOUNDARY
    {
    SHOW = YES
    COLOR = BLACK
    LINETHICKNESS = 0.02
    }
  POINTS
    {
    POINTSTOPLOT = SURFACENODES
    }
  SURFACES
    {
    SURFACESTOPLOT = KPLANES
    }
  VOLUMEMODE
    {
    VOLUMEOBJECTSTOPLOT
      {
      SHOWISOSURFACES = NO
      SHOWSLICES = NO
      SHOWSTREAMTRACES = NO
      }
    }




$!FIELD  [|EXTRA|-|NUMZONES|]
  MESH
    {
    COLOR = RED
    }
  CONTOUR
    {
    CONTOURTYPE = FLOOD
    COLOR = RED
    USELIGHTINGEFFECT = YES
    }
  VECTOR
    {
    COLOR = RED
    }
  SCATTER
    {
    COLOR = RED
    }
  SHADE
    {
    SHOW = NO
    COLOR = WHITE
    }
  BOUNDARY
    {
    SHOW = NO
    COLOR = BLACK
    LINETHICKNESS = 0.02
    }
  POINTS
    {
    POINTSTOPLOT = SURFACENODES
    }
  SURFACES
    {
    SURFACESTOPLOT = KPLANES
    }
  VOLUMEMODE
    {
    VOLUMEOBJECTSTOPLOT
      {
      SHOWISOSURFACES = NO
      SHOWSLICES = NO
      SHOWSTREAMTRACES = NO
      }
    }
  GROUP = 2


$!THREEDAXIS 
  XDETAIL
    {
    VARNUM = 1
    }
  YDETAIL
    {
    VARNUM = 2
    }
  ZDETAIL
    {
    VARNUM = 5
    }
$!VIEW FIT
$!THREEDAXIS 
  AXISMODE = XYZDEPENDENT
  XYDEPXTOYRATIO = 1
  DEPXTOYRATIO = 1
  DEPXTOZRATIO = 1
$!THREEDAXIS 
  XDETAIL
    {
    RANGEMIN = -0.10499999839812516889
    RANGEMAX = 1.1049999499693512472
    GRSPACING = 0.5
    AXISLINE
      {
      EDGE = 2
      }
    }
$!THREEDAXIS 
  YDETAIL
    {
    RANGEMIN = -0.11749999839812517999
    RANGEMAX = 1.3674999499693512028
    GRSPACING = 0.2
    AXISLINE
      {
      EDGE = 1
      }
    }
$!THREEDAXIS 
  ZDETAIL
    {
    RANGEMIN = -0.10499999839812518276
    RANGEMAX = 1.1049999499693512472
    GRSPACING = 0.2
    AXISLINE
      {
      EDGE = 1
      }
    }
$!GLOBALISOSURFACE 
  MARCHINGCUBEALGORITHM = CLASSICPLUS
$!GLOBALSLICE 
  BOUNDARY
    {
    SHOW = NO
    }
$!GLOBALTHREED 
  AXISSCALEFACT
    {
    X = 1
    Y = 1
    Z = 1
    }
  ROTATEORIGIN
    {
    X = 0.499999975786
    Y = 0.624999975786
    Z = 0.499999975786
    }
  LIGHTSOURCE
    {
    INTENSITY = 75
    BACKGROUNDLIGHT = 30
    }
  LINELIFTFRACTION = 0.2
  SYMBOLLIFTFRACTION = 0.6
  VECTORLIFTFRACTION = 0.7
$!THREEDVIEW 
  PSIANGLE = 75.2033
  THETAANGLE = 126.138
  VIEWERPOSITION
    {
    X = -10.3247779537
    Y = 8.52968632613
    Z = 4.04060042355
    }
  VIEWWIDTH = 2.63786
$!FIELDLAYERS 
  SHOWMESH = NO
  SHOWSHADE = YES
  SHOWBOUNDARY = YES
### Frame Number 4 ###
$!CREATENEWFRAME 
$!FRAMELAYOUT 
  SHOWBORDER = NO
  SHOWHEADER = NO
  HEADERCOLOR = RED
  XYPOS
    {
    X = 5.8426
    Y = 4.1401
    }
  WIDTH = 5.7112
  HEIGHT = 3.9132
$!PLOTTYPE  = CARTESIAN3D
$!FRAMENAME  = 'Frame 001' 
$!ACTIVEFIELDZONES  =  [1-|NUMZONES|]
$!GLOBALRGB 
  RANGEMIN = 0
  RANGEMAX = 1
$!GLOBALCONTOUR  1
  LEGEND
    {
    XYPOS
      {
      X = 95
      }
    }
  COLORMAPFILTER
    {
    CONTINUOUSCOLOR
      {
      CMIN = 0
      CMAX = 1
      }
    }
$!GLOBALCONTOUR  2
  LEGEND
    {
    XYPOS
      {
      X = 95
      }
    }
  COLORMAPFILTER
    {
    CONTINUOUSCOLOR
      {
      CMIN = 0
      CMAX = 1
      }
    }
$!GLOBALCONTOUR  3
  LEGEND
    {
    XYPOS
      {
      X = 95
      }
    }
  COLORMAPFILTER
    {
    CONTINUOUSCOLOR
      {
      CMIN = 0
      CMAX = 1
      }
    }
$!GLOBALCONTOUR  4
  LEGEND
    {
    XYPOS
      {
      X = 95
      }
    }
  COLORMAPFILTER
    {
    CONTINUOUSCOLOR
      {
      CMIN = 0
      CMAX = 1
      }
    }
$!GLOBALSCATTER 
  LEGEND
    {
    XYPOS
      {
      X = 95
      }
    }
  REFSCATSYMBOL
    {
    COLOR = RED
    FILLCOLOR = RED
    }
$!FIELD  [1-|ZONES|]
  MESH
    {
    COLOR = RED
    }
  CONTOUR
    {
    CONTOURTYPE = FLOOD
    COLOR = RED
    USELIGHTINGEFFECT = YES
    }
  VECTOR
    {
    COLOR = RED
    }
  SCATTER
    {
    COLOR = RED
    }
  SHADE
    {
    COLOR = WHITE
    }
  BOUNDARY
    {
    SHOW = YES
    COLOR = BLACK
    LINETHICKNESS = 0.02
    }
  POINTS
    {
    POINTSTOPLOT = SURFACENODES
    }
  SURFACES
    {
    SURFACESTOPLOT = KPLANES
    }
  VOLUMEMODE
    {
    VOLUMEOBJECTSTOPLOT
      {
      SHOWISOSURFACES = NO
      SHOWSLICES = NO
      SHOWSTREAMTRACES = NO
      }
    }




$!FIELD  [|EXTRA|-|NUMZONES|]
  MESH
    {
    COLOR = RED
    }
  CONTOUR
    {
    CONTOURTYPE = FLOOD
    COLOR = RED
    USELIGHTINGEFFECT = YES
    }
  VECTOR
    {
    COLOR = RED
    }
  SCATTER
    {
    COLOR = RED
    }
  SHADE
    {
    SHOW = NO
    COLOR = WHITE
    }
  BOUNDARY
    {
    SHOW = NO
    COLOR = BLACK
    LINETHICKNESS = 0.02
    }
  POINTS
    {
    POINTSTOPLOT = SURFACENODES
    }
  SURFACES
    {
    SURFACESTOPLOT = KPLANES
    }
  VOLUMEMODE
    {
    VOLUMEOBJECTSTOPLOT
      {
      SHOWISOSURFACES = NO
      SHOWSLICES = NO
      SHOWSTREAMTRACES = NO
      }
    }
  GROUP = 2




$!THREEDAXIS 
  XDETAIL
    {
    VARNUM = 1
    }
  YDETAIL
    {
    VARNUM = 2
    }
  ZDETAIL
    {
    VARNUM = 6
    }
$!VIEW FIT
$!THREEDAXIS 
  AXISMODE = XYDEPENDENT
  XYDEPXTOYRATIO = 1
  DEPXTOYRATIO = 1
  DEPXTOZRATIO = 1
$!THREEDAXIS 
  XDETAIL
    {
    RANGEMIN = -0.10499999839812516889
    RANGEMAX = 1.1049999499693512472
    GRSPACING = 0.5
    AXISLINE
      {
      EDGE = 2
      }
    }
$!THREEDAXIS 
  YDETAIL
    {
    RANGEMIN = -0.11749999839812517999
    RANGEMAX = 1.3674999499693512028
    GRSPACING = 0.2
    AXISLINE
      {
      EDGE = 1
      }
    }
$!THREEDAXIS 
  ZDETAIL
    {
    RANGEMIN = -0.17999999839812516611
    RANGEMAX = 2.6799999499693512028
    AUTOGRID = NO
    GRSPACING = 0.5
    AXISLINE
      {
      EDGE = 1
      }
    }
$!GLOBALISOSURFACE 
  MARCHINGCUBEALGORITHM = CLASSICPLUS
$!GLOBALSLICE 
  BOUNDARY
    {
    SHOW = NO
    }
$!GLOBALTHREED 
  AXISSCALEFACT
    {
    X = 1
    Y = 1
    Z = 0.38554328943
    }
  ROTATEORIGIN
    {
    X = 0.499999975786
    Y = 0.624999975786
    Z = 1.24999997579
    }
  LIGHTSOURCE
    {
    INTENSITY = 75
    BACKGROUNDLIGHT = 30
    }
  LINELIFTFRACTION = 0.2
  SYMBOLLIFTFRACTION = 0.6
  VECTORLIFTFRACTION = 0.7
$!THREEDVIEW 
  PSIANGLE = 75.2033
  THETAANGLE = 126.138
  VIEWERPOSITION
    {
    X = -10.1834973246
    Y = 8.42651755275
    Z = 10.3135475894
    }
  VIEWWIDTH = 2.47124
$!FIELDLAYERS 
  SHOWMESH = NO
  SHOWSHADE = YES
  SHOWBOUNDARY = YES
### Frame Number 5 ###
$!CREATENEWFRAME 
$!READDATASET  '|LFDSFN1|' 
  INITIALPLOTTYPE = CARTESIAN2D
  INCLUDETEXT = YES
  INCLUDEGEOM = YES
  VARLOADMODE = BYNAME
  VARNAMELIST = '|LFDSVL1|' 
$!REMOVEVAR |LFDSVL1|
$!REMOVEVAR |LFDSFN1|
$!FRAMELAYOUT 
  SHOWBORDER = NO
  SHOWHEADER = NO
  ISTRANSPARENT = YES
  HEADERCOLOR = GREEN
  XYPOS
    {
    X = 0.12025
    Y = 0.11009
    }
  WIDTH = 11.449
  HEIGHT = 8.0338
$!PLOTTYPE  = CARTESIAN2D
$!FRAMENAME  = 'Frame 002' 
$!ACTIVEFIELDZONES  =  [1]
$!GLOBALRGB 
  RANGEMIN = 0
  RANGEMAX = 1
$!GLOBALCONTOUR  1
  LEGEND
    {
    XYPOS
      {
      X = 95
      }
    }
  COLORMAPFILTER
    {
    CONTINUOUSCOLOR
      {
      CMIN = 0
      CMAX = 1
      }
    }
$!GLOBALCONTOUR  2
  LEGEND
    {
    XYPOS
      {
      X = 95
      }
    }
  COLORMAPFILTER
    {
    CONTINUOUSCOLOR
      {
      CMIN = 0
      CMAX = 1
      }
    }
$!GLOBALCONTOUR  3
  LEGEND
    {
    XYPOS
      {
      X = 95
      }
    }
  COLORMAPFILTER
    {
    CONTINUOUSCOLOR
      {
      CMIN = 0
      CMAX = 1
      }
    }
$!GLOBALCONTOUR  4
  LEGEND
    {
    XYPOS
      {
      X = 95
      }
    }
  COLORMAPFILTER
    {
    CONTINUOUSCOLOR
      {
      CMIN = 0
      CMAX = 1
      }
    }
$!GLOBALSCATTER 
  LEGEND
    {
    XYPOS
      {
      X = 95
      }
    }
  REFSCATSYMBOL
    {
    COLOR = RED
    FILLCOLOR = RED
    }
$!FIELD  [1-|NUMZONES|]
  MESH
    {
    COLOR = RED
    }
  CONTOUR
    {
    CONTOURTYPE = FLOOD
    COLOR = RED
    USELIGHTINGEFFECT = YES
    }
  VECTOR
    {
    COLOR = RED
    }
  SCATTER
    {
    COLOR = RED
    }
  SHADE
    {
    COLOR = WHITE
    }
  BOUNDARY
    {
    SHOW = NO
    COLOR = RED
    }
  POINTS
    {
    POINTSTOPLOT = SURFACENODES
    }
  SURFACES
    {
    SURFACESTOPLOT = KPLANES
    }
  VOLUMEMODE
    {
    VOLUMEOBJECTSTOPLOT
      {
      SHOWISOSURFACES = NO
      SHOWSLICES = NO
      SHOWSTREAMTRACES = NO
      }
    }



$!TWODAXIS 
  XDETAIL
    {
    VARNUM = 1
    }
  YDETAIL
    {
    VARNUM = 2
    }
$!VIEW FIT
$!TWODAXIS 
  DEPXTOYRATIO = 1
$!TWODAXIS 
  XDETAIL
    {
    SHOWAXIS = NO
    RANGEMIN = 0
    RANGEMAX = 0.22558688865862566719
    GRSPACING = 0.05
    }
$!TWODAXIS 
  YDETAIL
    {
    SHOWAXIS = NO
    RANGEMIN = 0
    RANGEMAX = 0.16251624403893946691
    GRSPACING = 0.05
    }
$!GLOBALISOSURFACE 
  MARCHINGCUBEALGORITHM = CLASSICPLUS
$!GLOBALSLICE 
  BOUNDARY
    {
    SHOW = NO
    }
$!FIELDLAYERS 
  SHOWMESH = NO
  SHOWBOUNDARY = NO
$!ATTACHTEXT 
  ANCHORPOS
    {
    X = 52.4186448582
    Y = 89.1797576003
    }
  COLOR = PURPLE
  TEXTSHAPE
    {
    FONT = HELV
    HEIGHT = 18
    }
  BOX
    {
    MARGIN = 10
    LINETHICKNESS = 0.4
    }
  SCOPE = GLOBAL
  MACROFUNCTIONCOMMAND = '' 
  TEXT = 'w(r,z,t)' 
$!ATTACHTEXT 
  ANCHORPOS
    {
    X = 2.7262876363
    Y = 90.4013980341
    }
  COLOR = PURPLE
  TEXTSHAPE
    {
    FONT = HELV
    HEIGHT = 18
    }
  BOX
    {
    MARGIN = 10
    LINETHICKNESS = 0.4
    }
  SCOPE = GLOBAL
  MACROFUNCTIONCOMMAND = '' 
  TEXT = 'u(r,z,t)' 
$!ATTACHTEXT 
  ANCHORPOS
    {
    X = 52.2947237679
    Y = 44.3281016752
    }
  COLOR = PURPLE
  TEXTSHAPE
    {
    FONT = HELV
    HEIGHT = 18
    }
  BOX
    {
    MARGIN = 10
    LINETHICKNESS = 0.4
    }
  SCOPE = GLOBAL
  MACROFUNCTIONCOMMAND = '' 
  TEXT = 'p(r,z,t)' 
$!ATTACHTEXT 
  ANCHORPOS
    {
    X = 1.61099782334
    Y = 44.3281016752
    }
  COLOR = PURPLE
  TEXTSHAPE
    {
    FONT = HELV
    HEIGHT = 18
    }
  BOX
    {
    MARGIN = 10
    LINETHICKNESS = 0.4
    }
  SCOPE = GLOBAL
  MACROFUNCTIONCOMMAND = '' 
  TEXT = 'v(r,z,t)' 

$!SETSTYLEBASE CONFIG


############################







$!IF |PNG|==1


        $!EXPORTSETUP EXPORTFORMAT = PNG
        $!EXPORTSETUP SUPERSAMPLEFACTOR = 3
        $!EXPORTSETUP USESUPERSAMPLEANTIALIASING = YES
        $!EXPORTSETUP IMAGEWIDTH = 600
        $!EXPORTSETUP EXPORTFNAME = 'spin_up3D_TH|istep|.png'
        $!EXPORT
          EXPORTREGION = ALLFRAMES

        $!EXPORTSETUP EXPORTFORMAT = EPS
        $!EXPORTSETUP USESUPERSAMPLEANTIALIASING = NO
        $!EXPORTSETUP IMAGEWIDTH = 1423
        $!EXPORTSETUP EXPORTFNAME = 'spin_up3D_TH|istep|.eps'

        $!EXPORTSETUP PRINTRENDERTYPE = IMAGE
        $!EXPORTSETUP EXPORTFNAME = 'spin_up3D_TH|istep|.img.eps'
        $!EXPORT
          EXPORTREGION = ALLFRAMES

$!ELSE

        $!IF |LOOP|>1
                $!EXPORTNEXTFRAME
        $!ELSE

                $!EXPORTSETUP
                 EXPORTFORMAT = AVI
                 EXPORTFNAME = "spin_up3D_TH.avi"
                $!EXPORTSETUP EXPORTREGION = ALLFRAMES
                $!EXPORTSETUP IMAGEWIDTH = 817
                $!EXPORTSETUP USESUPERSAMPLEANTIALIASING = NO
                $!EXPORTSTART
        $!ENDIF

$!ENDIF


$!VARSET |LAST_STEP|=|istep|

$!EndLoop


$!IF |PNG|==0
        $!EXPORTFINISH
$!ENDIF



