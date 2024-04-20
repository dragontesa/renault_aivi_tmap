function getDistanceText(distance)
{
    if (distance < 1000) {
        return distance + 'm'
    } else if (distance < 10 * 1000) {
        var d1 = parseInt(distance / 1000)
        var d2 = parseInt((distance % 1000) / 100)
        return d1 + '.' + d2 + 'km'
    } else {
        return parseInt(distance / 1000) + 'km'
    }
}

function getTimeText(t)
{
    var str = ''
    if (3600 <= t) {
        str = parseInt(t / 3600) + qsTr('시간') + ' '
    }
    t = t % 3600
    str += parseInt(t / 60) + qsTr('분')
    return str
}

function getFeeText(n, postfix) {
    if (0 === n)
        return '0' + postfix

    var reg = /(^[+-]?\d+)(\d{3})/;   // 정규식
    n += '';                          // 숫자를 문자열로 변환

    while (reg.test(n))
        n = n.replace(reg, '$1' + ',' + '$2');

    return n + postfix;
}

function getRouteOptionText(option) {
    switch (option) {
    case 1:
        return qsTr('최적길')
    case 2:
        return qsTr('고속도로우선')
    case 4:
        return qsTr('무료도로')
    case 8:
        return qsTr('최단거리')
    case 16:
        return qsTr('초보자경로')
    case 32:
        return qsTr('최소시간')
    }
    return ''
}
