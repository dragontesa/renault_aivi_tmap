    .import CommonConstants 1.0 as ImportConstants

    var MAX_INT_VALUE = ImportConstants.CommonConstants.MAX_INT_VALUE

    function createEnum(enumObj) {
        var data = ({})
        data[ImportConstants.CommonConstants.Empty] = 0
        data[ImportConstants.CommonConstants.Properties] = ({})
        enumObj = data
        return enumObj
    }

    function getObjectLen(enumObj) {
        var length = Object.keys(enumObj).length
        if (enumObj.hasOwnProperty(ImportConstants.CommonConstants.Empty))
            length -= 1
        if (enumObj.hasOwnProperty(ImportConstants.CommonConstants.Properties))
           length -= 1
        return length
    }

    function insertEnum(enumObj, enumKey, enumValue, propKeys, propValues ) {
        if (enumObj === undefined ||  enumValue === undefined )
            return
        if (enumKey !== undefined)
            enumObj[enumKey] = enumValue
        if (propKeys === undefined || propValues === undefined)
            return
        if (enumObj.properties[enumValue] === undefined) {
            enumObj.properties[enumValue] = {}
        }
        var len = (Array.isArray(propKeys)) ? propKeys.length : 1
        for (var i=0; i<len; i++) {
            var propValue = (Array.isArray(propValues)) ? propValues[i] : propValues
            var propKey = (Array.isArray(propKeys)) ? propKeys[i] : propKeys
            enumObj.properties[enumValue][propKey] =  propValue
        }
    }

    function appendEnum(enumObj, enumKey, propKeys, propValues ) {
        if (enumObj === undefined || enumKey === undefined)
            return

        var index = getObjectLen(enumObj)
        enumObj[enumKey] = index

        if (propKeys === undefined || propValues === undefined)
            return

        if (enumObj.properties[index] === undefined) {
            enumObj.properties[index] = {}
        }

        var len = (Array.isArray(propKeys)) ? propKeys.length : 1
        for (var i=0; i<len; i++) {
            var propValue = (Array.isArray(propValues)) ? propValues[i] : propValues
            var propKey = (Array.isArray(propKeys)) ? propKeys[i] : propKeys
            enumObj.properties[index][propKey] =  propValue
        }
   }

   function fetchArr(enumObj, propName, arrObj) {
       if (enumObj === undefined || propName === undefined || arrObj === undefined)
           return
       if (!enumObj.hasOwnProperty(ImportConstants.CommonConstants.Properties))
           return

       var tempArr = ([])
       for (var key in enumObj) {
           var i = enumObj[key]

           //undefined도 배열의 갯수에 포함 시키기 위하여
           tempArr[i] = null

           if (enumObj.properties[i] === undefined || enumObj.properties[i][propName] === undefined) {
               continue
           }
           arrObj[i] = enumObj.properties[i][propName]
       }

       var maxlength =  Math.max(arrObj.length, tempArr.length)
       if (maxlength > arrObj.length) {
        arrObj.length = maxlength
       }
   }

    function getArrivalTimeUsingDate(date, remain) {
        var hh = parseInt(Qt.formatDateTime(date, "hh"))
        var mm = parseInt(Qt.formatDateTime(date, "mm"))
        var ss = parseInt(Qt.formatDateTime(date, "ss"))
        var unit = 24 * 60 * 60;
        var t = (hh*3600 + mm*60 + ss + remain) % unit

        return t;
    }
    function getArrivalTimeUsingTime(h, m , s, remain) {
        var unit = 24 * 60 * 60;
        var t = (h*3600 + m*60 + s + remain) % unit

        return t;
    }

    function getHour(t) {
        return Math.floor((t/3600)).toFixed(0)
    }

    function getMin(t) {
        return Math.floor(((t%3600)/60)).toFixed(0)
    }

    function addComma(dataValue) {
        var txtNumber = '' + dataValue
        var rxSplit = new RegExp('([0-9])([0-9][0-9][0-9][,.])')
        var arrNumber = txtNumber.split('.')
        arrNumber[0] += '.'
        do {
            arrNumber[0] = arrNumber[0].replace(rxSplit, '$1,$2')
        } while (rxSplit.test(arrNumber[0]))
        if (arrNumber.length > 1) {
            return arrNumber.join('')
        }
        else {
            return arrNumber[0].split('.')[0]
        }
    }

    function phoneFomatter(num){
        if (!isString(num))
            return num
        if (isIndexOf(num, "-"))
            return num
        num = trim(num)
        num = replaceAll(num, String.fromCharCode(0x20), ImportConstants.CommonConstants.Empty)
        return num.replace(/(^02.{0}|^01.{1}|[0-9]{3})([0-9]+)([0-9]{4})/,'$1-$2-$3')
    }


    function Timer(id) {
        return Qt.createQmlObject("import QtQuick 2.0; Timer {property var cb:null; property var object: null}", id)
    }

    function getOject(obj) {
        if (!isUndefined(typeof obj.object)) {
            return obj.object
        }
        return obj
    }

    function destroy(obj, delayTime) {
        var ret = false
        if (isNull(obj))
            return
        if (!isUndefined(typeof obj.triggered) &&
            !isUndefined(typeof obj.triggered.disconnect) &&
            !isUndefined(typeof obj.cb)) {
            if (!isNull(obj.cb)) {
                obj.triggered.disconnect(obj.cb)
            }
        }
        if (!isUndefined(typeof obj.stop))
            obj.stop()
        if (!isUndefined(typeof obj.destroy)) {
            if (isNull(delayTime)) {
                if (!isUndefined(typeof obj.object)) {
                    obj.object = null
                }
                obj.destroy()
            } else {
                if (!isUndefined(typeof obj.object)) {
                    obj.object = null
                }
                obj.destroy(delayTime)
            }
        }
        return initNull(obj)
    }

    function initNull(obj) {
        if (isNull(obj))
            return obj
        obj = null
        return obj
    }

    function delay(id, delayTime,  cb , autoDelete) {
        var timer = new Timer(id)
        timer.object = timer
        timer.interval = delayTime
        timer.repeat = false
        timer.cb = cb
        timer.triggered.connect(timer.cb)
        timer.triggered.connect(function() {
            if (isNull(autoDelete) || autoDelete) {
                timer.triggered.disconnect(timer.cb)
                destroy(timer)
            }
        })
        timer.start()
        return timer
    }

    function sleep(id, delayTime,  cb, autoDelete) {
        var timer = new Timer(id)
        timer.interval = delayTime
        timer.repeat = false
        timer.triggered.connect(function(){id.enabled=true})
        timer.triggered.connect(cb)
        timer.triggered.connect(function() {
            if (isNull(autoDelete) || autoDelete) {
                timer.triggered.disconnect(cb)
                destroy(timer)
            }
        })
        timer.start()
        id.enabled = false
        return timer
    }

    function getTime() {
        return new Date().getTime()
    }


    function getItemFromObj(obj,offset) {
        var retItem = null
        var item = obj.children
        for(var i=0; i<item.length; i++) {
            if (i === offset) {
                retItem = item[i]
                break
            }
        }
        return retItem
    }

    function getItemFromParent(obj,offset) {
        var retItem = null
        var item = obj.parent.children
        for(var i=0; i<item.length; i++) {
            if (item[i] === obj){
                retItem = item[i+offset]
                break
            }
        }
        return retItem
    }

    function getCalcWidth(obj) {
        var width = 0
        var item = obj.children
        for(var i=0; i<item.length; i++) {
                width+=item[i].width
        }
        return width
    }

    function getCalcHeight(obj) {
        var height = 0
        var item = obj.children
        for(var i=0; i<item.length; i++) {
            height+=item[i].height
        }
        return height
    }

    function applyWidth(obj) {
        return obj.width =  Math.max(0, getCalcWidth(obj))
    }

    function applyHeight(obj) {
        return obj.height = Math.max(0, getCalcHeight(obj))
    }

    function applyRemainWidth(obj,owner) {
        var parent = isNull(owner) ? obj.parent : owner
        return obj.width = Math.max(0, (parent.width - (getCalcWidth(parent) - obj.width)))
    }

    function applyRemainHeight(obj,owner) {
        var parent = isNull(owner) ? obj.parent : owner
        return obj.height = Math.max(0, (parent.height - (getCalcHeight(parent) - obj.height)))
    }

    //변수의 타입별 처리를 위함
    var eType = {NUMBER: 0,STRING: 1,BOOLEAN: 2,OBJECT: 3,FUNCTION: 4,UNDEFINED: 5,UNKNOWN: 6,}
    function typeOf(v) {
        var ret = eType.UNKNOWN
        var c = (typeof v).toLowerCase()
        switch(c) {
            case "number":      ret = eType.NUMBER;    break;
            case "string":      ret = eType.STRING;    break;
            case "boolean":     ret = eType.BOOLEAN;   break;
            case "object":      ret = eType.OBJECT;    break;
            case "function":    ret = eType.FUNCTION;  break;
            case "undefined":   ret = eType.UNDEFINED; break;
            default:            ret = eType.UNKNOWN;   break;
        }
        return ret
    }

    //연산자 "==" 를  "==="으로,  "!="을 "!=="으로 적용하기 함수 처리
    var eRet = {DATA:  0, UNDEFINED: 1, NULL: 2, EMPTY: 3, ZEROLENGTH: 4,}
    function isNull(v) {
        if (isEqual(v, ImportConstants.CommonConstants.Undefined)) {
            return eRet.UNDEFINED
        } else if (v === undefined) {
            return eRet.UNDEFINED
        } else if (v === null) {
            return eRet.NULL
        }
        return false
    }

    // type이 다른 경우를 추후 확인하기 위하여...
    function isEmpty(v) {
        var ret = false
        if (ret = isNull(v))
            return ret
        if (!isString(v)) {
          if (!isNumber(v))
              return false
        }
        if (v === ImportConstants.CommonConstants.Empty) {
            return eRet.EMPTY
        } else if ( v == ImportConstants.CommonConstants.Empty) {
            return eRet.EMPTY
        } else if (typeOf(v) === eType.OBJECT && Object.keys(v).length <= 0) {
            return eRet.ZEROLENGTH
        }
        return false
    }

    function isUndefined(s) {
        return isEqual(s, ImportConstants.CommonConstants.Undefined)
    }

    // type이 다른 경우를 추후 확인하기 위하여...
    function isEqual(l,r) {
        if (typeOf(l) === eType.OBJECT ) {
            if (typeOf(r) !== eType.OBJECT) {
                return false
            }
        } else if (typeOf(r) === eType.OBJECT ) {
            if (typeOf(l) !== eType.OBJECT) {
                return false
            }
        }

        if (l === r) {
            return true
        } else  if (l == r) {
            return true
        }
        return false
    }

    function isIndexOf(s,t) {
        if (isNull(s))
            return false
        if (Array.isArray(t)) {
            for(var i=0; i<t.length;i++){
                if (s.indexOf(t[i])>=0)
                    return true
            }
        }else {
            if (s.indexOf(t)>=0)
                return true
        }
        return false
    }

    function isNumber(v)  { return isEqual(typeOf(v),  eType.NUMBER)  }
    function isString(v)  { return isEqual(typeOf(v),  eType.STRING)  }    
    function isBoolean(v) { return isEqual(typeOf(v),  eType.BOOLEAN) }
    function isObject(v)  { return isEqual(typeOf(v),  eType.OBJECT)  }
    function isFunction(v){ return isEqual(typeOf(v),  eType.FUNCTION)}

    function trim(s) {
        var ret = s
        if (!isString(s))
            return ret
        ret = s.trim()
        return ret
    }

    function split(s, t) {
        var ret = s
        if (!isString(s))
            return ret
        if (!isString(t))
            return ret
        if (isEmpty(s))
            return ret
        ret = s.split(t)
        return ret
    }

    function indexOf(s, t) {
        var ret = s
        if (!isString(s))
            return ret
        if (!isString(t))
            return ret
        ret = s.indexOf(t)
        return ret
    }

    function lastIndexOf(s, t) {
        var ret = s
        if (!isString(s))
            return ret
        if (!isString(t))
            return ret
        ret = s.lastIndexOf(t)
        return ret
    }

    function substring(s, start, end) {
        var ret = s
        if (!isString(s))
            return ret
        if (!isNumber(start))
            return ret
        if (isNull(end)) {
            ret = s.substring(start)
        } else {
            if (!isNumber(end))
                return ret
            ret = s.substring(start, end)
        }
        return ret
    }

    function replace(s, exp, txt) {
        var ret = s
        if (!isString(s))
            return ret
        if (!isString(exp))
            return ret
        if (!isString(txt))
            return ret
        ret = s.replace(exp, txt)
        return ret
    }

    function replaceAll(s, exp, txt) {
        var ret = s
        if (!isString(s))
            return ret
        if (!isString(exp))
            return ret
        if (!isString(txt))
            return ret
        ret = s.replace(new RegExp(exp,"gi"), txt)
        return ret
    }

    function zeroPadding(n, size) {
      var s = String(n);
      while (s.length < size) {
          s = String(0) + s
      }
      return s
    }

    function isAlpaNum(s) {
      if (isNumber(s))
          return true
      s = trim(s)
      if (isEmpty(s) || isNaN(s))
          return false
      return true
    }

    function dataFormat(date, format) {
        var yyyy = date.getFullYear().toString()
        format = replaceAll(format,"yyyy", yyyy)
        var mm = (date.getMonth()+1).toString()
        format = replaceAll(format,"mm", zeroPadding(mm,2))
        var dd  = date.getDate().toString()
        format = replaceAll(format,"dd", zeroPadding(dd,2))
        var hh = date.getHours().toString()
        format = replaceAll(format,"hh", zeroPadding(hh,2))
        var ii = date.getMinutes().toString()
        format = replaceAll(format,"ii", zeroPadding(ii,2))
        var ss  = date.getSeconds().toString()
        format = replaceAll(format,"ss", zeroPadding(ss,2))
        return format
    }

    function number(s) {
       return Number(s)
    }

    function string(s) {
        return String(s)
    }

    function getGridRowCol(menuName, div, min) {
       var count = null
       if (isNull(div) ||
           menuName.length <= 0) {
           if (isNull(min))
              return null
           return min
       }
       count = parseInt(menuName.length / div)
       if (parseFloat(menuName.length % div)  > 0.0) {
           count += 1
       }
       if (!isNull(min)) {
           return Math.max(count, min)
       }
       return count
    }
