//! [0]
        QDesignerFormWindowManagerInterface *manager = 0;
        QDesignerFormWindowInterface *formWindow = 0;

        manager = formEditor->formWindowManager();
        formWindow = manager->formWindow(0);

        manager->setActiveFormWindow(formWindow);
//! [0]


