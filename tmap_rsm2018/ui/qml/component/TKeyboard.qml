import QtQuick 2.1
import CommonConstants 1.0
import "internal"
import "./internal/Keyboard.js" as CustomKeyboard

Rectangle {
    id: idRoot
    width: parent.width
    height: parent.height
    visible: true
    color: CommonConstants.Black

    ///////////////////////////////////////////////////////////
    //csw관련...
    property int count: getRowArrLen() * getColArrLen(0)
    property int currentIndex: -1
    property int currentRow: currentIndex >= 0 ?  currentIndex / getColArrLen(0) : 0
    property int currentCol: currentIndex >= 0 ?  currentIndex % getColArrLen(0) : 0
    property var currentItem: currentIndex >= 0 ? boardObjArr[currentRow][currentCol] : undefined
    property bool focused: false
    property bool isFocused: false
    property color highlightColor: CommonConstants.Transparent

    signal buttonCompleted(int row , int col)
    signal buttonPressed(int row, int col)
    signal buttonReleased(int row, int col)
    signal buttonFocused(int row, int col)
    signal buttonLostFocused(int row, int col)
    ///////////////////////////////////////////////////////////

    property bool searchBtnAlwaysEnable: false
    property var pressed:([-1,-1])


    property var  symbolKeybd:  CustomKeyboard.symbolKeybd
    property var  hangleKeybd:  CustomKeyboard.hangleKeybd
    property var  englishKeybd: CustomKeyboard.englishKeybd
    property var  searchKeybd: CustomKeyboard.searchKeybd


    function isSymbol()  {return CustomKeyboard.isSymbol()}
    function isHangle()  {return CustomKeyboard.isHangul()}
    function isEnglish() {return CustomKeyboard.isEnglish()}



    onPressedChanged: {
        var row = pressed[0]
        var col = pressed[1]
        if (row<0 || col<0)
            return
        clickButton(row,col)
    }

    signal textInput(int key)

    onSearchBtnAlwaysEnableChanged: {
        initSearchBtn()
    }

    onWidthChanged: {
        refreshBtn()
    }

    onHeightChanged: {
        refreshBtn()
    }

    Component.onCompleted: {        
        refreshBtn()
        //버튼 초기화
        initRadioBtn()
        initSearchBtn()
    }

    function onTextInputChanged() {
        initSearchBtn()
    }

    //키보드 펑션키
    property int codeDelete :  CustomKeyboard.CodeDelete
    property int codeCancel :  CustomKeyboard.CodeCancel
    property int codeSymbol :  CustomKeyboard.CodeSymbol
    property int codeKorean :  CustomKeyboard.CodeKorean
    property int codeEnglish:  CustomKeyboard.CodeEnglish
    property int codeShift  :  CustomKeyboard.CodeShift
    property int codeClear  :  CustomKeyboard.CodeClear
    property int codeSetting:  CustomKeyboard.CodeSetting
    property int codeSearch :  CustomKeyboard.CodeSearch
    property int codeConfirm:  CustomKeyboard.CodeConfirm
    property int codeSpace  :  CustomKeyboard.CodeSpace

    //키보드 행열의 spacing
    property int rowColSpacing : 2

    //버튼 상태
    property string stateNormal  : CommonConstants.Normal
    property string statePressed : CommonConstants.Pressed
    //버튼 박스 컬러
    property string boxNormalColor : "#222226"
    property string boxPressedColor : CommonConstants.Transparent
    //버튼(검색) 컬러
    property string searchXtxtColor  : CommonConstants.Dark + CommonConstants.Gray
    property string searchOtxtColor  : CommonConstants.Red
    //문자 테이블
    property var boardArr : boardKorArr
    //문자 이미지 테이블
    property var boardImgArr : boardImgKorArr

    //에디터 객체
    property var focusObjectId: null

    //버튼 객체
    property var boardObjArr : [
        //ㅂ   ,ㅈ    ,ㄷ    ,ㄱ    ,ㅅ    ,ㅛ    ,ㅕ     ,ㅑ   ,ㅐ    ,ㅔ
        [null ,null  ,null  ,null ,null  ,null ,null   ,null ,null ,null],
        //ㅁ   ,ㄴ    ,ㅇ    ,ㄹ    ,ㅎ    ,ㅗ    ,ㅓ     ,ㅏ   ,ㅣ
        [null  ,null ,null  ,null ,null  ,null  ,null  ,null ,null ,null],
        //Shift,ㅋ   ,ㅌ     ,ㅊ   ,ㅍ     ,ㅠ    ,ㅜ     ,ㅡ   ,DEL
        [null  ,null ,null  ,null ,null  ,null  ,null  ,null ,null ,null],
        //Down ,NUM  ,KOR   ,ENG  ,SPACE ,SEARCH
        [null  ,null ,null  ,null ,null  ,null  ,null  ,null ,null ,null]
    ]

    //한글
    property var boardKorArr : [
        //ㅂ   ,ㅈ    ,ㄷ    ,ㄱ    ,ㅅ    ,ㅛ    ,ㅕ     ,ㅑ   ,ㅐ    ,ㅔ
        [0x1107,0x110c,0x1103,0x1100,0x1109,0x116d,0x1167,0x1163,0x1162,0x1166],
        //ㅁ   ,ㄴ    ,ㅇ    ,ㄹ    ,ㅎ    ,ㅗ    ,ㅓ     ,ㅏ   ,ㅣ
        [0x1106,0x1102,0x110b,0x1105,0x1112,0x1169,0x1165,0x1161,0x1175,0     ],
        //Shift ,ㅋ    ,ㅌ    ,ㅊ    ,ㅍ     ,ㅠ    ,ㅜ    ,ㅡ   ,DEL
        [-1     ,0x110f,0x1110,0x110e,0x1111,0x1172,0x116e,0x1173,-5   ,0     ],
        //Down  ,NUM   ,KOR   ,ENG   ,SPACE ,SEARCH
        [-3     ,-2    ,-4    ,-6    ,32    ,-101  ,0     ,0     ,0    ,0     ]
    ]

    //한글(쉬프트)
    property var boardKorShiftArr : [
        //ㅃ   ,ㅉ    ,ㄸ    ,ㄲ    ,ㅆ    ,ㅛ    ,ㅕ     ,ㅑ   ,ㅒ    ,ㅖ
        [0x1108,0x110d,0x1104,0x1101,0x110a,0x116d,0x1167,0x1163,0x1164,0x1168],
        //ㅁ   ,ㄴ    ,ㅇ    ,ㄹ    ,ㅎ    ,ㅗ    ,ㅓ     ,ㅏ   ,ㅣ
        [0x1106,0x1102,0x110b,0x1105,0x1112,0x1169,0x1165,0x1161,0x1175,0     ],
        //Shift ,ㅋ    ,ㅌ    ,ㅊ    ,ㅍ     ,ㅠ    ,ㅜ    ,ㅡ   ,DEL
        [-1     ,0x110f,0x1110,0x110e,0x1111,0x1172,0x116e,0x1173,-5   ,0     ],
        //Down  ,NUM   ,KOR   ,ENG   ,SPACE ,SEARCH
        [-3     ,-2    ,-4    ,-6    ,32    ,-101  ,0     ,0     ,0    ,0     ]
    ]

    //영문/영문(쉬프트)
    property var boardEngArr : [
        //q ,w  ,e  ,r  ,t  ,y  ,u  ,i  ,o  ,p
        [113,119,101,114,116,121,117,105,111,112],
        //a ,s  ,d  ,f  ,g  ,h  ,j  ,k  , l
        [97 ,115,100,102,103,104,106,107,108,0  ],
        //Sht,z ,x  ,c ,v  ,b ,n  ,m  ,DEL
        [-1 ,122,120,99,118,98,110,109,-5   ,0  ],
        //Down  ,NUM   ,KOR   ,ENG   ,SPACE ,SEARCH
        [-3     ,-2    ,-4    ,-6    ,32    ,-101  , 0    ,0     ,0     ,0    ]
    ]

    //심볼
    property var boardSymbolArr : [
        //1,2 ,3 ,4 ,5 ,6 ,7 ,8 ,9 ,0
        [49,50,51,52,53,54,55,56,57,48],
        //' ," ,: ,; ,(  ,) ,_ ,& ,$
        [180,34,58,59,40,41,95,38,36,0    ],
        //Sht,.,',',?,! ,-, / ,@ ,DEL
        [-1 ,46,44,63,33,45,47,64,-5,0     ],
        //Down  ,NUM   ,KOR   ,ENG   ,SPACE ,SEARCH
        [-3     ,-2    ,-4    ,-6    ,32    ,-101  , 0    ,0     ,0     ,0    ]
    ]

    //심볼(쉬프트)
    property var boardSymbolShiftArr : [
        //[,]  ,{ ,}  ,# ,%,^ ,* ,+ ,=
        [91,93,123,125,35,37,710,42,43,61],
        //`,\ ,|  ,~  ,< ,> ,₩   ,€   ,¥
        [96,92,124,126,60,62,8361,8364,165,0    ],
        //Sht,•   ,℃ ,℉  ,¢   ,£  ,¤  ,§ ,DEL
        [-1 ,8226,8451,8457,162,163,164,167,-5,0     ],
        //Down  ,NUM   ,KOR   ,ENG   ,SPACE ,SEARCH
        [-3     ,-2    ,-4    ,-6    ,32    ,-101  , 0    ,0     ,0     ,0    ]
    ]

    //한글
    property var boardImgKorArr :[
            /*ㅂ*/["ime_label_kor_06",
            /*ㅈ*/ "ime_label_kor_09",
            /*ㄷ*/ "ime_label_kor_03",
            /*ㄱ*/ "ime_label_kor_01",
            /*ㅅ*/ "ime_label_kor_07",
            /*ㅛ*/ "ime_label_kor_25",
            /*ㅕ*/ "ime_label_kor_23",
            /*ㅑ*/ "ime_label_kor_21",
            /*ㅐ*/ "ime_label_kor_30",
            /*ㅔ*/ "ime_label_kor_31"],
            /*-------------------------*/
            /*ㅁ*/["ime_label_kor_05",
            /*ㄴ*/ "ime_label_kor_02",
            /*ㅇ*/ "ime_label_kor_08",
            /*ㄹ*/ "ime_label_kor_04",
            /*ㅎ*/ "ime_label_kor_14",
            /*ㅗ*/ "ime_label_kor_24",
            /*ㅓ*/ "ime_label_kor_22",
            /*ㅏ*/ "ime_label_kor_20",
            /*ㅣ*/ "ime_label_kor_29",
                   ""],
            /*-------------------------*/
            /* */ ["ime_icon_1"      ,
            /*ㅋ*/ "ime_label_kor_11",
            /*ㅌ*/ "ime_label_kor_12",
            /*ㅊ*/ "ime_label_kor_10",
            /*ㅍ*/ "ime_label_kor_13",
            /*ㅠ*/ "ime_label_kor_27",
            /*ㅜ*/ "ime_label_kor_26",
            /*ㅡ*/ "ime_label_kor_28",
            /* */ "ime_icon_2"     ,
                   ""],
            /*-------------------------*/
            /*▦*/  ["ime_icon_3"    ,
            /*1#*/   "ime_icon_4"    ,
            /*한*/   "ime_icon_5"    ,
            /*A */   "ime_icon_6"    ,
            /*└┘*/  "ime_icon_7"    ,
            /*검*/   "",
                    "",
                    "",
                    "",
                    ""
            ]
    ]

    //한글(쉬프트)
    property var boardImgKorShiftArr : [
            /*ㅃ*/["ime_label_kor_17",
            /*ㅉ*/ "ime_label_kor_19",
            /*ㄸ*/ "ime_label_kor_16",
            /*ㄲ*/ "ime_label_kor_15",
            /*ㅆ*/ "ime_label_kor_18",
            /*ㅛ*/ "ime_label_kor_25",
            /*ㅕ*/ "ime_label_kor_23",
            /*ㅑ*/ "ime_label_kor_21",
            /*ㅒ*/ "ime_label_kor_32",
            /*ㅖ*/ "ime_label_kor_33"],
            /*-------------------------*/
            /*ㅁ*/["ime_label_kor_05",
            /*ㄴ*/ "ime_label_kor_02",
            /*ㅇ*/ "ime_label_kor_08",
            /*ㄹ*/ "ime_label_kor_04",
            /*ㅎ*/ "ime_label_kor_14",
            /*ㅗ*/ "ime_label_kor_24",
            /*ㅓ*/ "ime_label_kor_22",
            /*ㅏ*/ "ime_label_kor_20",
            /*ㅣ*/ "ime_label_kor_29",
                  ""],
            /*-------------------------*/
            /* */ ["ime_icon_1_s"    ,
            /*ㅋ*/ "ime_label_kor_11",
            /*ㅌ*/ "ime_label_kor_12",
            /*ㅊ*/ "ime_label_kor_10",
            /*ㅍ*/ "ime_label_kor_13",
            /*ㅠ*/ "ime_label_kor_27",
            /*ㅜ*/ "ime_label_kor_26",
            /*ㅡ*/ "ime_label_kor_28",
            /* */  "ime_icon_2"     ,
                    ""],
            /*-------------------------*/
            /*▦*/  ["ime_icon_3"    ,
            /*1#*/   "ime_icon_4"    ,
            /*한*/   "ime_icon_5"    ,
            /*A */   "ime_icon_6"    ,
            /*└┘*/ "ime_icon_7"    ,
            /*검*/   "",
                    "",
                    "",
                    "",
                    ""       ]
    ]

    //영어
    property var boardImgEngArr : [
            /*q*/["ime_label_eng_43",
            /*w*/ "ime_label_eng_49",
            /*e*/ "ime_label_eng_31",
            /*r*/ "ime_label_eng_44",
            /*t*/ "ime_label_eng_46",
            /*y*/ "ime_label_eng_51",
            /*u*/ "ime_label_eng_47",
            /*i*/ "ime_label_eng_35",
            /*o*/ "ime_label_eng_41",
            /*p*/ "ime_label_eng_42"],
            /*-------------------------*/
            /*a*/["ime_label_eng_27",
            /*s*/ "ime_label_eng_45",
            /*d*/ "ime_label_eng_30",
            /*f*/ "ime_label_eng_32",
            /*g*/ "ime_label_eng_33",
            /*h*/ "ime_label_eng_34",
            /*j*/ "ime_label_eng_36",
            /*k*/ "ime_label_eng_37",
            /*l*/ "ime_label_eng_38",
                  ""],
            /*-------------------------*/
            /* */["ime_icon_1"      ,
            /*z*/ "ime_label_eng_52",
            /*x*/ "ime_label_eng_50",
            /*c*/ "ime_label_eng_29",
            /*v*/ "ime_label_eng_48",
            /*b*/ "ime_label_eng_28",
            /*n*/ "ime_label_eng_40",
            /*m*/ "ime_label_eng_39",
            /* */ "ime_icon_2"     ,
                   ""],
            /*-------------------------*/
            /*▦*/  ["ime_icon_3"    ,
            /*1#*/   "ime_icon_4"    ,
            /*한*/   "ime_icon_5"    ,
            /*A */   "ime_icon_6"    ,
            /*└┘*/ "ime_icon_7"    ,
            /*검*/   "",
                    "",
                    "",
                    "",
                    ""]
    ]

    //영어(쉬프트)
    property var boardImgEngShiftArr : [
            /*Q*/["ime_label_eng_17",
            /*W*/ "ime_label_eng_23",
            /*E*/ "ime_label_eng_05",
            /*R*/ "ime_label_eng_18",
            /*T*/ "ime_label_eng_20",
            /*Y*/ "ime_label_eng_25",
            /*U*/ "ime_label_eng_21",
            /*I*/ "ime_label_eng_09",
            /*O*/ "ime_label_eng_15",
            /*P*/ "ime_label_eng_16"],
            /*-------------------------*/
            /*A*/["ime_label_eng_01",
            /*S*/ "ime_label_eng_19",
            /*D*/ "ime_label_eng_04",
            /*F*/ "ime_label_eng_06",
            /*G*/ "ime_label_eng_07",
            /*H*/ "ime_label_eng_08",
            /*J*/ "ime_label_eng_10",
            /*K*/ "ime_label_eng_11",
            /*L*/ "ime_label_eng_12",
                  ""],
            /*-------------------------*/
            /* */["ime_icon_1"      ,
            /*Z*/ "ime_label_eng_26",
            /*X*/ "ime_label_eng_24",
            /*C*/ "ime_label_eng_03",
            /*V*/ "ime_label_eng_22",
            /*B*/ "ime_label_eng_02",
            /*N*/ "ime_label_eng_14",
            /*M*/ "ime_label_eng_13",
            /* */ "ime_icon_2"     ,
                   ""],
            /*-------------------------*/
            /*▦*/  ["ime_icon_3"    ,
            /*1#*/   "ime_icon_4"    ,
            /*한*/   "ime_icon_5"    ,
            /*A */   "ime_icon_6"    ,
            /*└┘*/   "ime_icon_7"    ,
            /*검*/   "",
                    "",
                    "",
                    "",
                    ""]
    ]

    //심볼
    property var boardImgSymbolArr : [
            /*1*/["ime_label_spe_01",
            /*2*/ "ime_label_spe_02",
            /*3*/ "ime_label_spe_03",
            /*4*/ "ime_label_spe_04",
            /*5*/ "ime_label_spe_05",
            /*6*/ "ime_label_spe_06",
            /*7*/ "ime_label_spe_07",
            /*8*/ "ime_label_spe_08",
            /*9*/ "ime_label_spe_09",
            /*0*/ "ime_label_spe_10"],
            /*-------------------------*/
            /*'*/["ime_label_spe_11",
            /*"*/ "ime_label_spe_12",
            /*:*/ "ime_label_spe_13",
            /*;*/ "ime_label_spe_14",
            /*(*/ "ime_label_spe_15",
            /*)*/ "ime_label_spe_16",
            /*_*/ "ime_label_spe_17",
            /*&*/ "ime_label_spe_18",
            /*$*/ "ime_label_spe_19",
                  ""],
            /*-------------------------*/
            /* */["ime_icon_1"      ,
            /*.*/ "ime_label_spe_20",
            /*,*/ "ime_label_spe_21",
            /*?*/ "ime_label_spe_22",
            /*!*/ "ime_label_spe_23",
            /*-*/ "ime_label_spe_24",
            /*/*/ "ime_label_spe_25",
            /*@*/ "ime_label_spe_26",
            /* */ "ime_icon_2"     ,
                   ""],
            /*-------------------------*/
            /*▦*/  ["ime_icon_3"    ,
            /*1#*/   "ime_icon_4"    ,
            /*한*/   "ime_icon_5"    ,
            /*A */   "ime_icon_6"    ,
            /*└┘*/   "ime_icon_7"    ,
            /*검*/   "",
                    "",
                    "",
                    "",
                    ""]
    ]

    //심볼(쉬프트)
    property var boardImgSymbolShiftArr : [
            /*[*/["ime_label_spe_27",
            /*]*/ "ime_label_spe_28",
            /*[*/ "ime_label_spe_29",
            /*]*/ "ime_label_spe_30",
            /*#*/ "ime_label_spe_31",
            /*%*/ "ime_label_spe_32",
            /*^*/ "ime_label_spe_33",
            /***/ "ime_label_spe_34",
            /*+*/ "ime_label_spe_35",
            /*=*/ "ime_label_spe_36"],
           /*-------------------------*/
            /*`*/[ "ime_label_spe_37",
            /*\*/  "ime_label_spe_38",
            /*|*/  "ime_label_spe_39",
            /*~*/  "ime_label_spe_40",
            /*<*/  "ime_label_spe_41",
            /*>*/  "ime_label_spe_42",
            /*₩ */ "ime_label_spe_43",
            /*€ */ "ime_label_spe_44",
            /*¥ */ "ime_label_spe_45",
                   ""],
            /*-------------------------*/
            /**/  ["ime_icon_1"     ,
            /*•*/ "ime_label_spe_46",
            /*℃*/"ime_label_spe_47",
            /*℉*/"ime_label_spe_48",
            /*¢*/ "ime_label_spe_49",
            /*£*/ "ime_label_spe_50",
            /*¤*/"ime_label_spe_51",
            /*§*/"ime_label_spe_52",
            /**/  "ime_icon_2"    ,
                    ""],
            /*-------------------------*/
            /*▦*/  ["ime_icon_3"    ,
            /*1#*/   "ime_icon_4"    ,
            /*한*/   "ime_icon_5"    ,
            /*A */   "ime_icon_6"    ,
            /*└┘*/ "ime_icon_7"    ,
            /*검*/   "",
                    "",
                    "",
                    "",
                    ""]
    ]

    property int keyboardC : getColArrLen(0)
    property int keyboardR : getRowArrLen()
    //키보드 버튼 넓이 = (width -(spacing*11개) /10열)
    function getKeyBtnWidth() {return ((idRoot.width - (rowColSpacing*(keyboardC+1))) / keyboardC)}
    function getKeyBtnHeight() {return ((idRoot.height - (rowColSpacing*(keyboardR+1))) / keyboardR)}

    function refreshBtn(){
        for(var i= 0; i< getRowArrLen(); i++){
            for(var j=0; j<getColArrLen(i); j++){
                setBtnProperty(i,j)
            }
        }        
    }


    function setBtnProperty(row, col) {
        var obj = boardObjArr[row][col]
        if (!obj)    return
        //속성
        obj.visible                 = getVisibleBtn(row, col)
        obj.width                   = getWidthBtn(row, col)
        obj.height                  = getHeightBtn(row, col)
        //BOX
        obj.box.colorN              = boxNormalColor
        obj.box.colorP              = boxPressedColor
        //텍스트
        obj.txt.horizontalAlignment = Qt.AlignHCenter
        obj.txt.verticalAlignment   = Qt.AlignVCenter
        obj.txt.visible             = false
        obj.txt.text                = getTextBtn(row,col)
        //이미지
        obj.imgP.width              = obj.width
        obj.imgP.height             = obj.height
        obj.imgP.sourceP            = getImgPressed(row, col)
        obj.imgN.sourceN            = getImgNormal(row, col)

        //검색 버튼
        if (row === searchKeybd[0] && col === searchKeybd[1])
            initSearchBtn()
    }

    //행 갯수
    function getRowArrLen() {
        return boardArr.length
    }

    //열 갯수
    function getColArrLen(row) {
        return boardArr[row].length
    }

    //객체 저장
    function setOwnerObjBtn(obj, row,col)  {
        boardObjArr[row][col] = obj
        setBtnProperty(row, col)
    }

    //키보드 버튼 SHOW/HIDE 여부
    function getVisibleBtn(row, col) {
        return  (boardArr[row][col] === 0) ? false : true
    }

    //행별 첫번째 컬럼의 x좌표
    function getRowX(row) {
        return (row === 1) ? (getKeyBtnWidth()/2) : 0
    }

    //키보드 버튼의 크기
    function getWidthBtn(row, col) {
        var ret = getKeyBtnWidth()
        if (row === 2) {
            if (col === 0 || col === 8)
             ret = getKeyBtnWidth()+(getKeyBtnWidth()/2)
        }
        else if (row === 3) {
            if (col === 0 ||col === 1 || col === 2 || col === 3)
                ret = getKeyBtnWidth()+(getKeyBtnWidth()/2)
            else if (col === 4 || col === 5)
                ret = (getKeyBtnWidth()*2)+3
        }
        return ret
    }

    //키보드 버튼의 높이
    function getHeightBtn(row, col) {
        return getKeyBtnHeight()
    }

    //키보드 버튼이미지(Normal)
    function getImgNormal(row, col) {
        var ret =boardImgArr[row][col]
        if (ret !== ""){
            var src = boardImgArr[row][col] +".png"
            ret = Resource.imageUrl( 'keyboard', src)
        }
        return ret
    }

    //키보드 버튼이미지(Pressed)
    function getImgPressed(row, col) {
          var ret = Resource.imageUrl( 'keyboard', 'ime_bg_p.png')
          return ret
    }

    property var arryBtnText: [String.fromCharCode(0x20), qsTr("123#"), "▦", qsTr("한글"), String.fromCharCode(0x20), qsTr("ABC"), qsTr("검색"), "└┘"]

    function setTextBtn(row, col, text) {
        var v = boardArr[row][col]
        switch(v)
        {
        case CustomKeyboard.CodeShift:
            arryBtnText[0] = text
            break
        case CustomKeyboard.CodeSymbol:
            arryBtnText[1] = text
            break
        case CustomKeyboard.CodeCancel:
            arryBtnText[2] = text
            break
        case CustomKeyboard.CodeKorean:
            arryBtnText[3] = text
            break
        case CustomKeyboard.CodeDelete:
            arryBtnText[4] = text
            break
        case CustomKeyboard.CodeEnglish:
            arryBtnText[5] = text
            break
        case CustomKeyboard.CodeConfirm:
            arryBtnText[6] = text
            break
        case CustomKeyboard.CodeSpace:
            arryBtnText[7] = text
            break
        }
        setBtnProperty(row,col)
    }

    //키보드 버튼(텍스트)
    function getTextBtn(row, col) {
        var ret = ""
        var v = boardArr[row][col]
        switch(v)
        {
        case CustomKeyboard.CodeShift:
            ret = arryBtnText[0]
            break
        case CustomKeyboard.CodeSymbol:
            ret = arryBtnText[1]
            break
        case CustomKeyboard.CodeCancel:
            ret = arryBtnText[2]
            break
        case CustomKeyboard.CodeKorean:
            ret = arryBtnText[3]
            break
        case CustomKeyboard.CodeDelete:
            ret = arryBtnText[4]
            break
        case CustomKeyboard.CodeEnglish:
            ret = arryBtnText[5]
            break
        case CustomKeyboard.CodeConfirm:
            ret = arryBtnText[6]
            break
        case CustomKeyboard.CodeSpace:
            ret = arryBtnText[7]
            break
        default:
            if (CustomKeyboard.isEnglish()) {
                if (CustomKeyboard.getKeyboard() === CustomKeyboard.QwertyShiftedKeyboard){
                    ret = String.fromCharCode(v).toUpperCase()
                }
                else
                    ret = String.fromCharCode(v)
            } else {
                if (v === 38)
                    ret = "&&"
                else
                    ret = String.fromCharCode(v)
            }
            break
        }
        return ret
    }

    //키보드 버튼 클릭 > 입력값
    function inputText(row, col) {
        var ret = boardArr[row][col]        
        CustomKeyboard.onKey(ret)
        textInput(ret)
        switch(ret) {
            case CustomKeyboard.CodeShift:
                if (CustomKeyboard.isHangul()) {
                    if (CustomKeyboard.getKeyboard() === CustomKeyboard.HangulShiftedKeyboard){
                        boardArr = boardKorShiftArr
                        boardImgArr = boardImgKorShiftArr
                    } else{
                        boardArr = boardKorArr
                        boardImgArr = boardImgKorArr
                    }
                } else if (CustomKeyboard.isEnglish()) {
                    if (CustomKeyboard.getKeyboard() === CustomKeyboard.QwertyShiftedKeyboard){
                        boardArr = boardEngArr
                        boardImgArr = boardImgEngShiftArr
                    } else{
                        boardArr = boardEngArr
                        boardImgArr = boardImgEngArr
                    }
                }
                else if (CustomKeyboard.isSymbol()) {
                    if (CustomKeyboard.getKeyboard() === CustomKeyboard.SymbolShiftedKeyboard){
                        boardArr = boardSymbolShiftArr
                        boardImgArr = boardImgSymbolShiftArr
                    } else{
                        boardArr = boardSymbolArr
                        boardImgArr = boardImgSymbolArr
                    }
                }
                break
            case CustomKeyboard.CodeKorean:
                boardArr = boardKorArr
                boardImgArr = boardImgKorArr
                break
            case CustomKeyboard.CodeEnglish:
                boardArr = boardEngArr
                boardImgArr = boardImgEngArr
                break
            case CustomKeyboard.CodeSymbol:
                boardArr = boardSymbolArr
                boardImgArr = boardImgSymbolArr
                break
            case CustomKeyboard.CodeCancel:
                CustomKeyboard.focusObjectId.focus = false
                break
            case CustomKeyboard.CodeDelete:
                break
            case CustomKeyboard.CodeConfirm:
                break
            case CustomKeyboard.CodeSpace:
                break
            default:
                break
        }
        //버튼(검색) 텍스트 처리
        initSearchBtn()
    }

    //키보드 버튼(토글) 처리
    function clickToggleBtn(row,col) {
        if (row === 2) {
            //쉬프트
            if (col === 0) {
                if (CustomKeyboard.isShift())
                    boardObjArr[row][col].change = statePressed
                else
                    boardObjArr[row][col].change = stateNormal
                reloadBtn()
            }
        }
    }

    //키보드 버튼(라디오) 처리
    function clickRadioBtn(row,col) {
        if (row === 3) {
            //숫자키          ,한글키      ,영문키
            if (col === 1 || col === 2 || col === 3) {
                for(var i=1; i<4; i++) {
                    if (boardObjArr[row][i])
                        boardObjArr[row][i].change = (col === i) ? statePressed : stateNormal
                }
                if (boardObjArr[2][0])
                 boardObjArr[2][0].change = stateNormal
                reloadBtn()
            }
        }
    }

    //키보드 버튼 이미지를 변경한다.
    function reloadBtn(){
        for(var row = 0; row < getRowArrLen() ; row++) {
            for(var col = 0; col<getColArrLen(row); col++){
                if (boardObjArr[row][col] && boardObjArr[row][col].imgP)
                    boardObjArr[row][col].imgP.sourceP  = getImgPressed(row, col)
                if (boardObjArr[row][col] && boardObjArr[row][col].imgN)
                    boardObjArr[row][col].imgN.sourceN  = getImgNormal(row, col)
            }
        }
    }

    //키보드 버튼(서치) Enable/Disable
    function clickSearchBtn(row, col) {
        if (row !== 3 || col !== 5)
            return
        initSearchBtn()
    }

    //1.키보드 입력 객체 설정
    onFocusObjectIdChanged:{
        if(focusObjectId)
            setFocusObject(focusObjectId)
    }

    function setFocusObject(id) {
        CustomKeyboard.setFocusObjectId(id)        
        id.textChanged.connect(onTextInputChanged)
    }

    //2.키보드 버튼(라디오) 초기화
    function initRadioBtn() {
        if (CustomKeyboard.isSymbol()){
            clickRadioBtn(3,1)
        } else if (CustomKeyboard.isHangul()) {
            clickRadioBtn(3,2)
        } else if (CustomKeyboard.isEnglish()) {
            clickRadioBtn(3,3)
        }
    }

    //3.키보드 버튼(서치) 초기화
    function initSearchBtn() {
        //검색 버튼 처리                        
        if (CustomKeyboard.focusObjectId &&
            typeof CustomKeyboard.focusObjectId.text != "undefined" &&
            CustomKeyboard.focusObjectId.text.length>0){
            enableSearchBtn(true)
        } else {
            enableSearchBtn(false)
        }
    }

    function enableSearchBtn(enable) {
        if (!boardObjArr[3][5])
            return
        if (searchBtnAlwaysEnable)
            enable = true
        if (enable){
            boardObjArr[3][5].enabled  = true
            boardObjArr[3][5].txt.visible = true
            boardObjArr[3][5].txt.colorN = searchOtxtColor
            boardObjArr[3][5].txt.colorP  = searchOtxtColor
            boardObjArr[3][5].change = stateNormal
        } else {
            if (CustomKeyboard.focusObjectId)
                CustomKeyboard.focusObjectId.text = ""
            boardObjArr[3][5].enabled  = false
            boardObjArr[3][5].txt.visible = true
            boardObjArr[3][5].txt.colorN = searchXtxtColor
            boardObjArr[3][5].txt.colorP  = searchXtxtColor
            boardObjArr[3][5].change = stateNormal
        }
    }

    function clickButton(row,col) {
        inputText(row, col)
        clickRadioBtn(row, col)
        clickToggleBtn(row, col)
        clickSearchBtn(row, col)
    }


    Column{        
        anchors.centerIn: parent
        spacing: rowColSpacing
        Repeater {            
            model: getRowArrLen()
            delegate:
                 Row{
                    id: nodeRow
                    property int  row : index
                    x: getRowX(row)
                    spacing: rowColSpacing
                    Repeater {
                        model: getColArrLen(nodeRow.row)
                        delegate:
                            KeyboardButton{
                                property int  row : nodeRow.row
                                property int  col : index
                                property int  idx : (row * getColArrLen(row)) + col
                                property bool isFocused: idRoot.currentIndex === idx && idRoot.focused
                                function selectItemTwice() {
                                    buttonClicked()
                                }

                                onIsFocusedChanged: {
                                    if (isFocused) {
                                        idRoot.buttonFocused(row, col)
                                    } else {
                                        idRoot.buttonLostFocused(row,col)
                                    }
                                }

                                Component.onCompleted: {
                                    setOwnerObjBtn(this, row, col)                                    
                                    idRoot.buttonCompleted(row,col)
                                }
                                onButtonPressed: {
                                    idRoot.buttonPressed(row, col)
                                }
                                onButtonReleased: {
                                    idRoot.buttonReleased(row, col)
                                }
                                onButtonClicked:  {
                                    clickButton(row,col)
                                }
                            }
                    }
                  }
        }
      }
}
