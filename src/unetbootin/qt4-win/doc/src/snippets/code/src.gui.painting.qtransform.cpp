//! [0]
        x' = m11*x + m21*y + dx
        y' = m22*y + m12*x + dy
//! [0]


//! [1]
        x' = m11*x + m21*y + dx
        y' = m22*y + m12*x + dy
        if (is not affine) {
            w' = m13*x + m23*y + m33
            x' /= w'
            y' /= w'
        }
//! [1]


//! [2]
        x' = m11*x + m21*y + dx
        y' = m22*y + m12*x + dy
        if (is not affine) {
            w' = m13*x + m23*y + m33
            x' /= w'
            y' /= w'
        }
//! [2]


//! [3]
        x' = m11*x + m21*y + dx
        y' = m22*y + m12*x + dy
//! [3]


