//! [0]
    #include <QApplication>
    #include <QGLColormap>

    int main()
    {
        QApplication app(argc, argv);

        MySuperGLWidget widget;     // a QGLWidget in color-index mode
        QGLColormap colormap;

        // This will fill the colormap with colors ranging from
        // black to white.
        for (int i = 0; i < colormap.size(); i++)
            colormap.setEntry(i, qRgb(i, i, i));

        widget.setColormap(colormap);
        widget.show();
        return app.exec();
    }
//! [0]


