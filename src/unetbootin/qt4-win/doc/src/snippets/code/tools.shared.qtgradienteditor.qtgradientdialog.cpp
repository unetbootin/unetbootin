//! [0]
    bool ok;
    QGradient gradient = QtGradientDialog::getGradient(&ok, QRadialGradient(), this);
    if (ok) {
        // the user clicked OK and gradient is set to the gradient the user selected
    } else {
        // the user canceled the dialog; gradient is set to the initial
        // value, in this case radial gradient
    }
//! [0]


//! [1]
    bool detailsVisible;
    QColor::Spec spec;
    QGradient gradient;

    QtGradientDialog dlg(this);
    dlg.setDetailsVisible(detailsVisible);
    dlg.setSpec(spec);
    dlg.setGradient(gradient);
    if (dlg.exec() != QDialog::Accepted)
        return;

    gradient = dlg.gradient();
    // detailsVisible and spec properties can be changed interactively by the user too,
    // we store the values of these properties for the next time QtGradientDialog is executed.
    detailsVisible = dlg.detailsVisible();
    spec = dlg.spec();
//! [1]


//! [2]
        QtGradientDialog dlg;
        dlg.setBackgroundCheckered(true);
//! [2]


//! [3]
        QtGradientDialog dlg;
        dlg.setBackgroundCheckered(false);
//! [3]


