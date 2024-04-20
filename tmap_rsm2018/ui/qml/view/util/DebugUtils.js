function val() {
    var ret = ""
    for ( var i = 0; i < arguments.length; ) {
        var name = arguments[i++]
        var value = arguments[i++]
        if ( i > 0 ) {
            ret += ' '
        }
        ret += "[" + name + "]" + value
    }
    return ret
}

function obj( obj ) {
    console.log( "type: " + typeof obj )
    for ( var i in obj ) {
        var prop = obj[i]
        console.log( "{name: " + i + ", value: " + prop + "}" )
    }
}
