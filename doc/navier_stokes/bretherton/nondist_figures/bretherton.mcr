#!MC 1000


$!VARSET |PNG|=0


$!VARSET |MFBD|="RESLT"


#$!GETUSERINPUT |lostep| INSTRUCTIONS = "Loop. First Step??"
$!VARSET  |lostep|=0
#$!GETUSERINPUT |dlstep| INSTRUCTIONS = "Loop. Step Increment?"
$!VARSET  |dlstep|=1
$!GETUSERINPUT |nstep| INSTRUCTIONS = "Loop. Number of Steps??"

$!LOOP |nstep|
$!VarSet |nnstep| = |LOOP|
#$!VarSet |nnstep| -= 1
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


$!READDATASET "|MFBD|/soln|istep|.dat " 
  READDATAOPTION = NEW
  RESETSTYLE = YES

$!RENAMEDATASETVAR
  VAR = 5
  NAME = 'pressure'


$!FRAMEMODE = TWOD
$!FIELDLAYERS SHOWMESH = NO
$!FIELDLAYERS SHOWSHADE= NO
$!GLOBALTWODVECTOR UVAR = 3
$!GLOBALTWODVECTOR VVAR = 4
$!RESETVECTORLENGTH 
$!GLOBALCONTOUR VAR = 5
$!FIELDLAYERS SHOWCONTOUR = YES
$!FIELD [1-|NUMZONES|]  BOUNDARY{COLOR = BLACK}
$!FIELD [1-|NUMZONES|]  BOUNDARY{LINETHICKNESS = 0.02}
$!FIELD [1-|NUMZONES|]  VECTOR{COLOR = BLACK}
$!GLOBALTWODVECTOR RELATIVELENGTH = 0.30003
$!FIELDLAYERS SHOWVECTOR= YES




#===========================================
#VELOC FRAME
#===========================================
$!FRAMELAYOUT XYPOS{X = 0.12541}
$!FRAMELAYOUT XYPOS{Y = 0.12551}
$!FRAMELAYOUT WIDTH = 10.6951182455
$!FRAMELAYOUT HEIGHT = 4.0040214659
$!TWODAXIS YDETAIL{SHOWAXIS = NO}
$!TWODAXIS XDETAIL{SHOWAXIS = NO}

$!TWODAXIS VIEWPORTPOSITION{X1 = 0.79321365381}
$!TWODAXIS VIEWPORTPOSITION{X2 = 99.3207296}
$!TWODAXIS VIEWPORTPOSITION{Y2 = 97.662077258}
$!TWODAXIS VIEWPORTPOSITION{Y1 = 6.8505383179}
$!VIEW FIT

$!GLOBALCONTOUR 1  LEGEND{SHOW = YES}
$!GLOBALCONTOUR 1  LEGEND{ISVERTICAL = NO}
$!GLOBALCONTOUR 1  LABELS{NUMFORMAT{FORMATTING = SUPERSCRIPT}}
$!GLOBALCONTOUR 1  LABELS{NUMFORMAT{PRECISION = 1}}
$!GLOBALCONTOUR 1  LABELS{NUMFORMAT{NEGATIVEPREFIX = ''}}
$!GLOBALCONTOUR 1  LABELS{NUMFORMAT{NEGATIVESUFFIX = ''}}
$!GLOBALCONTOUR 1  LABELS{NUMFORMAT{ZEROPREFIX = ''}}
$!GLOBALCONTOUR 1  LABELS{NUMFORMAT{ZEROSUFFIX = ''}}
$!GLOBALCONTOUR 1  LABELS{AUTOLEVELSKIP = 3}
$!GLOBALCONTOUR 1  LEGEND{NUMBERTEXTSHAPE{HEIGHT = 4.5}}
$!GLOBALCONTOUR 1  LEGEND{HEADERTEXTSHAPE{HEIGHT = 4.5}}
$!GLOBALCONTOUR 1  LEGEND{XYPOS{X = 80.745083947}}
$!GLOBALCONTOUR 1  LEGEND{XYPOS{Y = 85.87590405}}




$!PICK ADDALL
  SELECTFRAMES = YES
$!PICK COPY
$!PICK PASTE
$!PICK PASTE
$!PICK PASTE


#===========================================
# PRESSURE FRAME
#===========================================
$!FRAMELAYOUT XYPOS{X = 0.14254454545}
$!FRAMELAYOUT XYPOS{Y = 4.1895947629}
$!FRAMELAYOUT WIDTH = 3.5247227273
$!FRAMELAYOUT HEIGHT = 4.1891254113
$!FIELDLAYERS SHOWVECTOR = NO


$!PLOTTYPE = CARTESIAN3D
$!FIELDLAYERS SHOWCONTOUR = NO
$!FIELDLAYERS SHOWSHADE = YES
$!THREEDAXIS ZDETAIL{VARNUM = 5}
$!THREEDAXIS XDETAIL{TITLE{TITLEMODE = USETEXT}}
$!THREEDAXIS YDETAIL{TITLE{TITLEMODE = USETEXT}}
$!THREEDAXIS FRAMEAXIS{SHOW = NO}
$!VIEW FIT
$!FRAMELAYOUT ISTRANSPARENT = YES

$!ATTACHTEXT
  ANCHORPOS
    {
    X = 11.5212121212
    Y = 86.2334642102
    }
  TEXTSHAPE
    {
    FONT = HELV
    HEIGHT = 19
    }
  TEXT = 'pressure'





#===========================================
# U FRAME
#===========================================
$!FRAMECONTROL PUSHTOP
$!FRAMELAYOUT XYPOS{X = 3.8290990909}
$!FRAMELAYOUT XYPOS{Y = 4.189594763}
$!FRAMELAYOUT WIDTH = 3.4843663636
$!FRAMELAYOUT HEIGHT = 4.1061629407

$!FIELDLAYERS SHOWVECTOR = NO


$!PLOTTYPE = CARTESIAN3D
$!FIELDLAYERS SHOWCONTOUR = NO
$!FIELDLAYERS SHOWSHADE = YES
$!THREEDAXIS ZDETAIL{VARNUM = 3}
$!THREEDAXIS XDETAIL{TITLE{TITLEMODE = USETEXT}}
$!THREEDAXIS YDETAIL{TITLE{TITLEMODE = USETEXT}}
$!THREEDAXIS FRAMEAXIS{SHOW = NO}
$!VIEW FIT
$!FRAMELAYOUT ISTRANSPARENT = YES

$!ATTACHTEXT
  ANCHORPOS
    {
    X = 11.5212121212
    Y = 86.2334642102
    }
  TEXTSHAPE
    {
    FONT = HELV
    HEIGHT = 19
    }
  TEXT = 'horizontal velocity'

$!REDRAWALL 




#===========================================
# V FRAME
#===========================================
$!FRAMECONTROL PUSHTOP
$!FRAMELAYOUT XYPOS{X = 7.3541181818}
$!FRAMELAYOUT XYPOS{Y = 4.2032051747}
$!FRAMELAYOUT WIDTH = 3.4978118182
$!FRAMELAYOUT HEIGHT = 4.202955823

$!FIELDLAYERS SHOWVECTOR = NO


$!PLOTTYPE = CARTESIAN3D
$!FIELDLAYERS SHOWCONTOUR = NO
$!FIELDLAYERS SHOWSHADE = YES
$!THREEDAXIS ZDETAIL{VARNUM = 4}
$!THREEDAXIS XDETAIL{TITLE{TITLEMODE = USETEXT}}
$!THREEDAXIS YDETAIL{TITLE{TITLEMODE = USETEXT}}
$!THREEDAXIS FRAMEAXIS{SHOW = NO}
$!VIEW FIT
$!FRAMELAYOUT ISTRANSPARENT = YES

$!ATTACHTEXT
  ANCHORPOS
    {
    X = 11.5212121212
    Y = 86.2334642102
    }
  TEXTSHAPE
    {
    FONT = HELV
    HEIGHT = 19
    }
  TEXT = 'vertical velocity'




#===========================================
# VELOC FRAME AGAIN
#===========================================
$!FRAMECONTROL PUSHTOP
$!ATTACHTEXT
  ANCHORPOS
    {
    X = 41.635384003
    Y = 91.093423852
    }
  TEXTSHAPE
    {
    FONT = HELV
    HEIGHT = 24
    }
  TEXT = 'Step |istep|'

$!REDRAWALL 


############################


$!DRAWGRAPHICS TRUE
$!REDRAWALL





$!IF |PNG|==1


        $!EXPORTSETUP EXPORTFORMAT = PNG
        $!EXPORTSETUP IMAGEWIDTH = 829
        $!EXPORTSETUP EXPORTFNAME = 'bretherton_flow|LOOP|.png'
        $!EXPORT
          EXPORTREGION = ALLFRAMES

        $!EXPORTSETUP EXPORTFORMAT = EPS
        $!EXPORTSETUP IMAGEWIDTH = 1423
        $!EXPORTSETUP EXPORTFNAME = 'bretherton_flow|LOOP|.eps'

        $!EXPORTSETUP PRINTRENDERTYPE = IMAGE
        $!EXPORTSETUP EXPORTFNAME = 'bretherton_flow|LOOP|.img.eps'
        $!EXPORT
          EXPORTREGION = ALLFRAMES

$!ELSE

        $!IF |LOOP|>1
                $!EXPORTNEXTFRAME
        $!ELSE

                $!EXPORTSETUP
                  EXPORTREGION = ALLFRAMES
                  EXPORTFORMAT = AVI
                  EXPORTFNAME = "bretherton_flow.avi"
                $!EXPORTSETUP IMAGEWIDTH = 829
                $!EXPORTSTART
        $!ENDIF

$!ENDIF


$!ENDLOOP



$!IF |PNG|==0
        $!EXPORTFINISH
$!ENDIF



