import bb.cascades 1.0

Container {
    id: chatItem
    horizontalAlignment: HorizontalAlignment.Fill
    preferredWidth: (ListItem.initialized ? ListItem.view.itemsWidth : 0)
    focusPolicy: FocusPolicy.Touch
    Container {
        background: {
            isMy() ? myBackground.imagePaint : theirBackground.imagePaint
        }
        horizontalAlignment: {
            isMy() ? HorizontalAlignment.Left : HorizontalAlignment.Right
        }
        focusPolicy: FocusPolicy.Touch
        Container {
            leftPadding: 35.0
            rightPadding: 35.0
            topPadding: 25.0
            bottomPadding: { (ListItemData.time || ListItemData.nick) ? 0 : 50 }
            focusPolicy: FocusPolicy.Touch
            Label {
                id: textLabel
                multiline: true
                text: ListItemData.text
                textStyle.fontSize: FontSize.PointValue
                textStyle.fontSizeValue: 10
                focusPolicy: FocusPolicy.Touch
            }
        }
        Container {
            leftPadding: 35.0
            rightPadding: 35.0
            topPadding: 20.0
            bottomPadding: {
                (ListItemData.time) ? 0 : 50
            }
            visible: {
                (ListItemData.nick) ? true : false
            }
            focusPolicy: FocusPolicy.Touch
            Label {
                id: nickLabel
                text: ListItemData.nick
                textStyle.color: Color.Gray
                textStyle.fontSize: FontSize.PointValue
                textStyle.fontSizeValue: 6
                focusPolicy: FocusPolicy.Touch
            }
        }
        Container {
            leftPadding: 35.0
            rightPadding: 35.0
            topPadding: 20.0
            bottomPadding: 50.0
            visible: {
                (ListItemData.time) ? true : false
            }
            focusPolicy: FocusPolicy.Touch
            Label {
                id: timeLabel
                text: ListItemData.time
                textStyle.color: Color.LightGray
                textStyle.fontSize: FontSize.PointValue
                textStyle.fontSizeValue: 6
                focusPolicy: FocusPolicy.Touch
            }
        }
    }
    attachedObjects: [
        ImagePaintDefinition {
            id: myBackground
            repeatPattern: RepeatPattern.Fill
            imageSource: "asset:///images/sentbubble.amd"
        },
        ImagePaintDefinition {
            id: theirBackground
            repeatPattern: RepeatPattern.Fill
            imageSource: "asset:///images/receivebubble.amd"
        }
    ]
    function isMy() {
        if (ListItem.initialized) {
            return (ListItemData.from == ListItem.view.currentUserId);
        }
        return true;
    }
}
