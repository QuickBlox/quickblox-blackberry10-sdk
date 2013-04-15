import bb.cascades 1.0

Container {
    property variant listItemData
    id: itemContainer
    background: {
        getBackground()
    }
    layout: DockLayout {
    }
    Container {
        layout: StackLayout {
            orientation: LayoutOrientation.LeftToRight
        }
        ImageView {
            imageSource: "asset:///images/user.png"
        }
        Label {
            text: listItemData.login;
            verticalAlignment: VerticalAlignment.Center
        }
    }
    Container {
        horizontalAlignment: HorizontalAlignment.Right
        verticalAlignment: VerticalAlignment.Center
        rightPadding: 150.0
        ImageView {
            imageSource: listItemData.online ? "asset:///images/onLine.png" : "asset:///images/offLine.png"
        }
    }
    function getBackground() {
        console.log("getBackground " + ((listItemData.unreadedCount == 0) ? "#00000000" : "#80FF5C00"));
        return (listItemData.unreadedCount == 0) ? Color.create("#00000000") : Color.create("#80FF5C00")
    }
    function setHighlight(highlighted, blank) {
        console.log("highlighted " + highlighted)
        if (highlighted) {
            background = Color.Blue;
        } else {
            if (blank) background = Color.create("00000000");
            else background = getBackground();
        }
    }
    onTouch: {
        if (event.isDown()) {
            setHighlight(true, false);
        } else if (event.isUp()) {
            setHighlight(false, true);
        } else if (event.isCancel()) {
            setHighlight(false, false);
        }
    }
}
