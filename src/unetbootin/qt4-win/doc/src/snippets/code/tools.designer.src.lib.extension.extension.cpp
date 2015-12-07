//! [0]
       QDesignerPropertySheetExtension *propertySheet;
       QExtensionManager manager = formEditor->extensionManager();

       propertySheet = qt_extension<QDesignerPropertySheetExtension*>(manager, widget);

       if(propertySheet) {...}
//! [0]


//! [1]
   Q_DECLARE_EXTENSION_INTERFACE(MyExtension, "com.mycompany.myproduct.myextension")
//! [1]


