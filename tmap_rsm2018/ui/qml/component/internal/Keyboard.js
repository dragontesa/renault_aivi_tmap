var CHOSEONG = "choseong";
var JUNGSEONG = "jungseong";
var JONGSEONG = "jongseong";
var DOUBLEJONGSEONG = "doublejongseong";
var ETC = "ETC";
var FLAG_NOT_HANDLE_GNBVIEW = 0x00000001;

var CodeDelete = -5; // Keyboard.KEYCODE_DELETE
var CodeCancel = -3; // Keyboard.KEYCODE_CANCEL
var CodeSymbol = -2; // Keyboard.KEYCODE_SYMBOL
var CodeKorean = -4; // Keyboard.KEYCODE_KOREAN
var CodeEnglish = -6; // Keyboard.KEYCODE_ENGLISH
var CodeShift = -1; // Keyboard.KEYCODE_SHIFT
var CodeClear = -200;
var CodeSetting = -100; // keyboard.KEYCODE_SETTING
var CodeSearch = 84; // keyboard.KEYCODE_SEARCH
var CodeConfirm = -101; // keyboard.KEYCODE_SEARCH
var CodeSpace  = 32;

//키보드의 종류
var QwertyKeyboard        = 0;
var QwertyShiftedKeyboard = 1;
var HangulKeyboard        = 2;
var HangulShiftedKeyboard = 3;
var SymbolKeyboard        = 4;
var SymbolShiftedKeyboard = 5;

//선택된 키보드 종류
var Keyboard = HangulKeyboard;

//포커스를 가진 객체 ID
var focusObjectId = null;

var symbolKeybd  =  [3,1]
var hangleKeybd  =  [3,2]
var englishKeybd =  [3,3]
var searchKeybd  =  [3,5]


//에디티 박스의 ID를 넣어준다.
function setFocusObjectId(editId) {
    focusObjectId = editId;
}


function getKeyboard() {
    return Keyboard;
}

function setKeyboard(value) {
    Keyboard = value;
}

function onKey( primaryCode){
   if(focusObjectId == null || !focusObjectId.enabled)
       return;

   var  start = focusObjectId.cursorPosition;

   if (primaryCode == CodeCancel) {
       hideCustomKeyboard();
   } else if (primaryCode == CodeDelete) {
       if (start != focusObjectId.length) {
           handleCursorPosition();
       } else {
           if (start > 0){
                focusObjectId.select(start - 1, start);   focusObjectId.cut();
            }
       }

   } else if (primaryCode == CodeClear) {
               focusObjectId.text="";
   } else if (primaryCode == CodeShift) {
       switchToggleBtn(primaryCode);
   } else if (primaryCode == CodeSetting) {

   } else if (primaryCode == CodeConfirm) {
       hideCustomKeyboard();
   } else if (primaryCode == CodeSymbol) {
       switchToggleBtn(primaryCode);
   } else if (primaryCode == CodeKorean) {
       switchToggleBtn(primaryCode);
   } else if (primaryCode == CodeEnglish) {
       switchToggleBtn(primaryCode);
   } else if (primaryCode == CodeSearch) {
   } else {
       if (isEnglish()) {
           handleEnglish(primaryCode);
       } else if (isHangul()) {
           handleHangul(primaryCode);
       } else if (isSymbol()) {
           focusObjectId.insert(start, String.fromCharCode(primaryCode));           
       }
   }
}

function handleCursorPosition() {
   var last = focusObjectId.text;
   var start = focusObjectId.cursorPosition;

   if (postCharStatus() == JUNGSEONG && (post2CharStatus() == JONGSEONG || post2CharStatus() == DOUBLEJONGSEONG)) {
       focusObjectId.cursorPosition = (start + 2);
       if (start > 0) {
           focusObjectId.select(start - 1, start + 2);   focusObjectId.cut();
       }
   } else if ((preCharStatus() == JONGSEONG || preCharStatus() == DOUBLEJONGSEONG) && postCharStatus() == CHOSEONG) {
       focusObjectId.cursorPosition = start;
       if (start > 0) {
           focusObjectId.select(start - 3, start); focusObjectId.cut();
       }
   } else if (preCharStatus() == JUNGSEONG && (postCharStatus() == JONGSEONG || postCharStatus() == DOUBLEJONGSEONG)) {
       focusObjectId.cursorPosition = (start + 2);
       if (start > 0) {
           focusObjectId.select(start - 1, start + 2); focusObjectId.cut();
       }
   } else if (preCharStatus() == CHOSEONG && postCharStatus() == JUNGSEONG && (post2CharStatus() == CHOSEONG || post2CharStatus() == JUNGSEONG)) {
       focusObjectId.cursorPosition =(start + 1);
       if (start > 0) {
           focusObjectId.select(start - 1, start + 1); focusObjectId.cut();
       }
   } else if (preCharStatus() == CHOSEONG && postCharStatus() == JUNGSEONG) {
       if ((post2CharStatus() == JONGSEONG || post2CharStatus() == DOUBLEJONGSEONG)) {
           focusObjectId.cursorPosition = (start + 2);
           if (start > 0) {
               focusObjectId.select(start - 1, start + 2); focusObjectId.cut();
           }
       } else if (post2CharStatus() == ETC) {
           focusObjectId.cursorPosition = (start + 1);
           if (start > 0) {
               focusObjectId.select(start, start + 1);focusObjectId.cut();
           }
       }
   } else if (pre2CharStatus() == CHOSEONG && preCharStatus() == JUNGSEONG && (postCharStatus() == CHOSEONG || postCharStatus() == JUNGSEONG)) {
       if ( start > 0) {
           focusObjectId.select(start - 2, start);focusObjectId.cut();
       }
   } else {
       if (start > 0) {
           focusObjectId.select(start - 1, start);focusObjectId.cut();
       }
   }
}


function handleHangul(primaryCode) {
   var  start = focusObjectId.cursorPosition;
   if (preCharStatus() == JUNGSEONG && isChoseong(primaryCode) && possibleJongseong(primaryCode)) {
       var  jongseongPrimaryCode = choseongTojongseong(primaryCode);
       focusObjectId.insert(start, String.fromCharCode( jongseongPrimaryCode));
   } else if (preCharStatus() == CHOSEONG && postCharStatus() == JUNGSEONG && (post2CharStatus() == CHOSEONG || post2CharStatus() == ETC)) {
       focusObjectId.cursorPosition = (start + 1);
       focusObjectId.insert(start + 1, String.fromCharCode( primaryCode));
   } else if (preCharStatus() == JONGSEONG && isJungseong(primaryCode)) {
       deletePreChar(primaryCode);
       focusObjectId.insert(start, String.fromCharCode( primaryCode));
   } else if (preCharStatus() == JONGSEONG && isChoseong(primaryCode)) {
       var  doubleJongseong = combinationDoubleJongseong(primaryCode);
       if (doubleJongseong == 0 || doubleJongseong == primaryCode) {
           focusObjectId.insert(start, String.fromCharCode( primaryCode));
       } else {                      
           focusObjectId.select(start - 1, start);  focusObjectId.cut();
           focusObjectId.insert(start - 1, String.fromCharCode( doubleJongseong));
       }
   } else if (preCharStatus() == DOUBLEJONGSEONG && isChoseong(primaryCode)) {
       focusObjectId.insert(start, String.fromCharCode( primaryCode));
   } else if (preCharStatus() == DOUBLEJONGSEONG && isJungseong(primaryCode)) {
       var  predoublechar = preDoubleCharSeparate();       
       focusObjectId.select(start - 1, start);  focusObjectId.cut();
       focusObjectId.insert(start - 1, String.fromCharCode( predoublechar[0]));
       focusObjectId.insert(start, String.fromCharCode( predoublechar[1]));
       focusObjectId.insert(start + 1, String.fromCharCode( primaryCode));
   } else if (preCharStatus() == JUNGSEONG && isJungseong(primaryCode)) {
       var  doubleJaeum = combinationDoubleMoeum(primaryCode);
       if (doubleJaeum == 0 || doubleJaeum == primaryCode) {
           focusObjectId.insert(start, String.fromCharCode( primaryCode));
       } else {           
           focusObjectId.select(start - 1, start); focusObjectId.cut();
           focusObjectId.insert(start - 1, String.fromCharCode( doubleJaeum));
       }
   } else {
       focusObjectId.insert(start, String.fromCharCode( primaryCode));
   }   
}

function handleEnglish( primaryCode) {
   var  start = focusObjectId.cursorPosition;

   var current = getKeyboard();
   if (current == QwertyShiftedKeyboard) {
       var temp = String.fromCharCode(primaryCode);
       focusObjectId.insert(start, temp.toUpperCase());       
   } else {
       focusObjectId.insert(start, String.fromCharCode( primaryCode));
   }
}

function switchToggleBtn( primaryCode) {
       var current = getKeyboard();
       if (primaryCode == CodeSymbol) {
           setKeyboard(SymbolKeyboard);
       } else if (primaryCode == CodeKorean) {
           setKeyboard(HangulKeyboard);
       } else if (primaryCode == CodeEnglish) {
           setKeyboard(QwertyKeyboard);
       } else if (primaryCode == CodeShift) {
           if (current == SymbolKeyboard) {
               setKeyboard(SymbolShiftedKeyboard);
           } else if (current == HangulKeyboard) {
               setKeyboard(HangulShiftedKeyboard);
           } else if (current == QwertyKeyboard) {
               setKeyboard(QwertyShiftedKeyboard);
           } else if (current == SymbolShiftedKeyboard) {
               setKeyboard(SymbolKeyboard);
           } else if (current == HangulShiftedKeyboard) {
               setKeyboard(HangulKeyboard);
           } else if (current == QwertyShiftedKeyboard) {
               setKeyboard(QwertyKeyboard);
           }
       }
}

function isShift() {
    var current = getKeyboard();
    if(current == HangulShiftedKeyboard ||
          current == QwertyShiftedKeyboard ||
            current == SymbolShiftedKeyboard)
        return true;
    return false;
}

function isHangul() {
       var current = getKeyboard();
       if (current == HangulKeyboard || current == HangulShiftedKeyboard) {
           return true;
       }
       return false;
}

function isEnglish() {
       var current = getKeyboard();
       if (current == QwertyKeyboard || current == QwertyShiftedKeyboard) {
           return true;
       }
       return false;
}

function isSymbol() {
       var current = getKeyboard();
       if (current == SymbolKeyboard || current == SymbolShiftedKeyboard) {
           return true;
       }
       return false;
}

function pre2CharStatus() {
   var last = focusObjectId.text;
   var  start = focusObjectId.cursorPosition;
   if (start > 0) {
           last = last.substring(start - 2);
           var ch = last.charCodeAt(0);
           var  ich = ch;
           if (isChoseong(ich)) {
               return CHOSEONG;
           } else if (isJungseong(ich)) {
               return JUNGSEONG;
           } else if (isJongseong(ich)) {
               return JONGSEONG;
           } else if (isDoubleJongseong(ich)) {
               return DOUBLEJONGSEONG;
           }
   }
   return ETC;
}

function preCharStatus() {
   var last = focusObjectId.text;
   var  start = focusObjectId.cursorPosition;   
   if (start > 0) {
       last = last.substring(start - 1);
       var ch = last.charCodeAt(0);
       var  ich = ch;
       if (isChoseong(ich)) {
           return CHOSEONG;
       } else if (isJungseong(ich)) {
           return JUNGSEONG;
       } else if (isJongseong(ich)) {
           return JONGSEONG;
       } else if (isDoubleJongseong(ich)) {
           return DOUBLEJONGSEONG;
       }
   }
   return ETC;
}

function postCharStatus() {
   var last = focusObjectId.text;
   var  start = focusObjectId.cursorPosition;
   if (start > 0) {
           last = last.substring(start);
           var ch = last.charCodeAt(0);
           var  ich =  ch;
           if (isChoseong(ich)) {
               return CHOSEONG;
           } else if (isJungseong(ich)) {
               return JUNGSEONG;
           } else if (isJongseong(ich)) {
               return JONGSEONG;
           } else if (isDoubleJongseong(ich)) {
               return DOUBLEJONGSEONG;
           }
   }
   return ETC;
}

function post2CharStatus() {
   var last = focusObjectId.text;
   var  start = focusObjectId.cursorPosition;
   if (start > 0) {
           last = last.substring(start + 1);
           var ch = last.charCodeAt(0);
           var  ich =  ch;
           if (isChoseong(ich)) {
               return CHOSEONG;
           } else if (isJungseong(ich)) {
               return JUNGSEONG;
           } else if (isJongseong(ich)) {
               return JONGSEONG;
           } else if (isDoubleJongseong(ich)) {
               return DOUBLEJONGSEONG;
           }
   }
   return ETC;
}

function preCharCode() {
   var last = focusObjectId.text;
   var  start = focusObjectId.cursorPosition;
   var  ich = 0;
   if (start > 0) {
       last = last.substring(start - 1);
       var ch = last.charCodeAt(0);
       ich =  ch;
   }
   return ich;
}


function preDoubleCharSeparate() {
   var preCharValues = [null, null]
   var  preChar = preCharCode();
   switch (preChar) {
        //          ㄳ
       case 0x11aa:
           preCharValues[0] = 0x11a8;
           preCharValues[1] = 0x1109;
           break;
        //          ㄵ
       case 0x11ac:
           preCharValues[0] = 0x11ab;
           preCharValues[1] = 0x110c;
           break;
        //          ㄶ
       case 0x11ad:
           preCharValues[0] = 0x11ab;
           preCharValues[1] = 0x1112;
           break;
        //          ㄺ
       case 0x11b0:
           preCharValues[0] = 0x11af;
           preCharValues[1] = 0x1100;
           break;
        //          ㄻ
       case 0x11b1:
           preCharValues[0] = 0x11af;
           preCharValues[1] = 0x1106;
           break;
        //          ㄼ
       case 0x11b2:
           preCharValues[0] = 0x11af;
           preCharValues[1] = 0x1107;
           break;
        //          ㄽ
       case 0x11b3:
           preCharValues[0] = 0x11af;
           preCharValues[1] = 0x1109;
           break;
        //          ㄾ
       case 0x11b4:
           preCharValues[0] = 0x11af;
           preCharValues[1] = 0x1110;
           break;
        //          ㄿ
       case 0x11b5:
           preCharValues[0] = 0x11af;
           preCharValues[1] = 0x1111;
           break;
        //          ㅀ
       case 0x11b6:
           preCharValues[0] = 0x11af;
           preCharValues[1] = 0x1112;
           break;
        //          ㅄ
       case 0x11b9:
           preCharValues[0] = 0x11b8;
           preCharValues[1] = 0x1109;
           break;
   }
   return preCharValues;
}

function deletePreChar( primaryCode) {
   var  start = focusObjectId.cursorPosition;
   var last = focusObjectId.text;
   if (start > 0) {
       last = last.substring(start - 1);
       var ch = last.charCodeAt(0);
       var  ich =  ch;
       focusObjectId.select(start - 1, start);    focusObjectId.cut();
       focusObjectId.insert(start - 1, String.fromCharCode( jongseongTochoseong(ich)));
   }
}

function combinationDoubleMoeum( primaryCode) {
   if (preCharCode() == 0x1169) {
       var  value = 0;
       switch (primaryCode) {
            //            ㅘ
           case 0x1161:
               value = 0x116a;
               break;
            //            ㅙ
           case 0x1162:
               value = 0x116b;
               break;
            //            ㅚ
           case 0x1175:
               value = 0x116c;
               break;
       }
       return value;
   } else if (preCharCode() == 0x116e) {
       var  value = 0;
       switch (primaryCode) {
            //            ㅝ
           case 0x1165:
               value = 0x116f;
               break;
            //            ㅞ
           case 0x1166:
               value = 0x1170;
               break;
            //            ㅟ
           case 0x1175:
               value = 0x1171;
               break;
       }
       return value;
   } else if (preCharCode() == 0x1173) {
       var  value = 0;
       switch (primaryCode) {
            //            ㅢ
           case 0x1175:
               value = 0x1174;
               break;
       }
       return value;
   }
   return primaryCode;
}

function combinationDoubleJongseong( primaryCode) {
   if (preCharCode() == 0x11a8) {
       var  value = 0;
       switch (primaryCode) {
            //            ㄳ
           case 0x1109:
               value = 0x11aa;
               break;
       }
       return value;
   } else if (preCharCode() == 0x11ab) {
       var  value = 0;
       switch (primaryCode) {
            //            ㄵ
           case 0x110c:
               value = 0x11ac;
               break;
            //            ㄶ
           case 0x1112:
               value = 0x11ad;
               break;
       }
       return value;
   } else if (preCharCode() == 0x11af) {
       var  value = 0;
       switch (primaryCode) {
            //            ㄺ
           case 0x1100:
               value = 0x11b0;
               break;
            //           ㄻ
           case 0x1106:
               value = 0x11b1;
               break;
            //           ㄼ
           case 0x1107:
               value = 0x11b2;
               break;
            //           ㄽ
           case 0x1109:
               value = 0x11b3;
               break;
            //          ㄾ
           case 0x1110:
               value = 0x11b4;
               break;
            //          ㄿ
           case 0x1111:
               value = 0x11b5;
               break;
            //           ㅀ
           case 0x1112:
               value = 0x11b6;
               break;
       }
       return value;
   } else if (preCharCode() == 0x11b8) {
       var  value = 0;
       switch (primaryCode) {
            //            ㅄ
           case 0x1109:
               value = 0x11b9;
               break;
       }
       return value;
   }
   return primaryCode;
}

function isChoseong( primaryCode) {
   if (primaryCode >= 0x1100 && primaryCode <= 0x1112) {
       return true;
   }
   return false;
}


function isJungseong( primaryCode) {
   if (primaryCode >= 0x1161 && primaryCode <= 0x1175) {
       return true;
   }
   return false;
}

function isJongseong( primaryCode) {
   if (primaryCode == 0x11a8 || primaryCode == 0x11a9 || primaryCode == 0x11ab || primaryCode == 0x11ae ||
           primaryCode == 0x11af || primaryCode == 0x11b7 || primaryCode == 0x11b8 || primaryCode == 0x11ba ||
           primaryCode == 0x11bb || primaryCode == 0x11bc || primaryCode == 0x11bd || primaryCode == 0x11be ||
           primaryCode == 0x11bf || primaryCode == 0x11c0 || primaryCode == 0x11c1 || primaryCode == 0x11c2) {
       return true;
   }
   return false;
}

function isDoubleJongseong( primaryCode) {
   if (primaryCode == 0x11aa || primaryCode == 0x11ac || primaryCode == 0x11ad || primaryCode == 0x11b0 ||
           primaryCode == 0x11b1 || primaryCode == 0x11b2 || primaryCode == 0x11b3 || primaryCode == 0x11b4 || primaryCode == 0x11b5 ||
           primaryCode == 0x11b6 || primaryCode == 0x11b9) {
       return true;
   }
   return false;
}

function choseongTojongseong( primaryCode) {
   var  value = 0;
   switch (primaryCode) {
        //            ㄱ1
       case 0x1100:
           value = 0x11a8;
           break;
        //            ㄲ1
       case 0x1101:
           value = 0x11a9;
           break;
        //            ㄴ1
       case 0x1102:
           value = 0x11ab;
           break;
        //            ㄷ1
       case 0x1103:
           value = 0x11ae;
           break;
        //            ㄹ1
       case 0x1105:
           value = 0x11af;
           break;
        //            ㅁ1
       case 0x1106:
           value = 0x11b7;
           break;
        //            ㅂ1
       case 0x1107:
           value = 0x11b8;
           break;
        //            ㅅ1
       case 0x1109:
           value = 0x11ba;
           break;
        //            ㅆ1
       case 0x110a:
           value = 0x11bb;
           break;
        //            ㅇ1
       case 0x110b:
           value = 0x11bc;
           break;
        //            ㅈ1
       case 0x110c:
           value = 0x11bd;
           break;
        //            ㅊ1
       case 0x110e:
           value = 0x11be;
           break;
        //            ㅋ1
       case 0x110f:
           value = 0x11bf;
           break;
        //            ㅌ1
       case 0x1110:
           value = 0x11c0;
           break;
        //            ㅍ1
       case 0x1111:
           value = 0x11c1;
           break;
        //            ㅎ1
       case 0x1112:
           value = 0x11c2;
           break;
   }
   return value;
}


function possibleJongseong( primaryCode) {
   //ㄸ                          //ㅃ                     //ㅉ
   if (primaryCode == 0x1104 || primaryCode == 0x1108 || primaryCode == 0x110d) {
       return false;
   }
   return true;
}

function jongseongTochoseong( primaryCode) {
   var  value = 0;
   switch (primaryCode) {
        //            ㄱ1
       case 0x11a8:
           value = 0x1100;
           break;
        //            ㄲ1
       case 0x11a9:
           value = 0x1101;
           break;
        //            ㄴ1
       case 0x11ab:
           value = 0x1102;
           break;
        //            ㄷ1
       case 0x11ae:
           value = 0x1103;
           break;
        //            ㄹ1
       case 0x11af:
           value = 0x1105;
           break;
        //            ㅁ1
       case 0x11b7:
           value = 0x1106;
           break;
        //            ㅂ1
       case 0x11b8:
           value = 0x1107;
           break;
        //            ㅅ1
       case 0x11ba:
           value = 0x1109;
           break;
        //            ㅆ1
       case 0x11bb:
           value = 0x110a;
           break;
        //            ㅇ1
       case 0x11bc:
           value = 0x110b;
           break;
        //            ㅈ1
       case 0x11bd:
           value = 0x110c;
           break;
        //            ㅊ1
       case 0x11be:
           value = 0x110e;
           break;
        //            ㅋ1
       case 0x11bf:
           value = 0x110f;
           break;
        //            ㅌ1
       case 0x11c0:
           value = 0x1110;
           break;
        //            ㅍ1
       case 0x11c1:
           value = 0x1111;
           break;
        //            ㅎ1
       case 0x11c2:
           value = 0x1112;
           break;
   }
   return value;
}

function hideCustomKeyboard(){
   //close();
}
