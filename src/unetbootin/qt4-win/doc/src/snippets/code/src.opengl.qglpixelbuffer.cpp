//! [0]
        QGLPixelBuffer pbuffer(...);
        ...
        pbuffer.makeCurrent();
        GLuint dynamicTexture = pbuffer.generateDynamicTexture();
        pbuffer.bindToDynamicTexture(dynamicTexture);
        ...
        pbuffer.releaseFromDynamicTexture();
//! [0]


//! [1]
        QGLPixelBuffer pbuffer(...);
        ...
        pbuffer.makeCurrent();
        GLuint dynamicTexture = pbuffer.generateDynamicTexture();
        ...
        pbuffer.updateDynamicTexture(dynamicTexture);
//! [1]


