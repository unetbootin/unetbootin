//! [0]
listWidget.addItem("Red");
listWidget.addItem("Blue");
listWidget.addItem("Green");
listWidget.addItem("Cyan");
listWidget.addItem("Yellow");
listWidget.addItem("Purple");
listWidget.addItems(["Orange", "Gray"]);
//! [0]

//! [1]
listWidget.currentItemChanged.connect(
    function(item)
    {
        listWidget.setBackgroundColor(item.text);
    }
);
//! [1]

listWidget.show();
