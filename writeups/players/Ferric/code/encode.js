let str_constructor = '($=[]+{}+``)[5]+$[1]+(([]+[])[1]+``)[1]+((!1)+``)[3]+($=!0+``)[0]+$[1]+$[2]+($=[]+{}+``)[5]+$[6]+$[1]+((!0)+``)[1]'
let str_toString = '(!0+``)[0]+([]+{}+``)[1]+($=((``)[' + str_constructor + ']+``))[9]+$[10]+$[11]+$[12]+$[13]+$[14]'
let mapping = {
    // true
    t: '(!0+``)[0]',
    r: '(!0+``)[1]',
    u: '(!0+``)[2]',
    e: '(!0+``)[3]',
    // false
    f: '(!1+``)[0]',
    a: '(!1+``)[1]',
    l: '(!1+``)[2]',
    s: '(!1+``)[3]',
    // [object Object]
    o: '([]+{}+``)[1]',
    b: '([]+{}+``)[2]',
    j: '([]+{}+``)[3]',
    c: '([]+{}+``)[5]',
    t: '([]+{}+``)[6]',
    // undefined
    n: '([][1]+``)[1]',
    d: '([][1]+``)[2]',
    i: '([][1]+``)[5]',
    // function String() { [native code] }
    S: '((``)[' + str_constructor + ']+``)[9]'
}

function lookup_escape(str) {
    // magic: 'pxhqywg' === (56444753968).toString(36)
    let innerMapping = {
        p: 0, x: 1, h: 2, q: 3, y: 4, w: 5, g: 6
    }
    return str.split('').map(x => {
        // though z is not represented
        if (/[a-zS]/.test(x))
            return mapping[x] || '56444753968[' + str_toString + '](36)[' + innerMapping[x] + ']'
        else if (x === '`')
            return '`\\``'
        else if (x === '\\')
            return '`\\\\`'
        else return '`' + x + '`'
    }).join('+')
}

function encode(code) {
    return '[][' + lookup_escape('filter') + '][' + str_constructor + '](' + lookup_escape(code) + ')()'
}

module.exports = encode