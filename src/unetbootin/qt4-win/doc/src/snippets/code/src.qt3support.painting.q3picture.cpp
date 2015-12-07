//! [0]
        Q3Picture picture;
        picture.load("drawing.svg", SVG);
//! [0]


//! [1]
         Q3Picture picture;
         QPainter painter;
         painter.begin(&picture);           // paint in picture
         ...
         painter.end();                     // painting done
         picture.save("drawing.svg", SVG);  // save picture
//! [1]


