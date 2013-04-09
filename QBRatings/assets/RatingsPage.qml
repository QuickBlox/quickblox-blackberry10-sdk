/*
 * RatingsPage.qml
 * 
 * Created on: 21.02.2013
 * Author: Sergey Fedunets
 * Company: Injoit Ltd
 */

import bb.cascades 1.0
import bb.system 1.0

Page {
    id: theRatingsPage
    actions: [
        ActionItem {
            title: "Logout"
            ActionBar.placement: ActionBarPlacement.OnBar
            onTriggered: {
                _app.singout();
                navigationPane.pop();
            }
        }
    ]
    attachedObjects: [
        SetRatingDialog {
            objectName: "dialogSetRating"
            id: dialogSetRating
        }
    ]
    Container {
        id: mainContainer
        layout: DockLayout {
        }

        // The background image
        ImageView {
            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Fill
            imageSource: "asset:///images/bg.jpg"
        }
        // The ListView that shows the progress of loading and result images
        ListView {
            id: listView
            preferredWidth: 600
            horizontalAlignment: HorizontalAlignment.Center
            verticalAlignment: VerticalAlignment.Center
            objectName: "listView"
            topPadding: 20.0
            visible: false
            dataModel: _app.model
            listItemComponents: ListItemComponent {
                type: ""
                Container {
                    horizontalAlignment: HorizontalAlignment.Center
                    verticalAlignment: VerticalAlignment.Center
                    layout: StackLayout {
                    }
                    // The ActivityIndicator that is only active and visible while the image is loading
                    ActivityIndicator {
                        horizontalAlignment: HorizontalAlignment.Center
                        verticalAlignment: VerticalAlignment.Center
                        preferredWidth: 600
                        visible: ListItemData.loading
                        running: ListItemData.loading
                        onStarted: {
                            image.opacity = 0.5;
                        }
                        onStopped: {
                            image.opacity = 1;
                            imageRatings.imageSource = imageShow();
                        }
                    }
                    // The ImageView that shows the loaded image after loading has finished without error
                    ImageView {
                        id: image
                        visible: ! ListItemData.loading && ListItemData.label == ""
                        horizontalAlignment: HorizontalAlignment.Center
                        verticalAlignment: VerticalAlignment.Center
                        maxWidth: 500
                        maxHeight: 500
                        image: ListItemData.image
/*                        onImageChanged: {
                            imageRatings.imageSource = imageShow();
                        }*/
                    }
                    ImageView {
                        id: imageRatings
                        horizontalAlignment: HorizontalAlignment.Center
                        visible: ! ListItemData.loading && ListItemData.label == ""
                    }
                    // The Label that shows a possible error message after loading has finished
                    Label {
                        horizontalAlignment: HorizontalAlignment.Center
                        verticalAlignment: VerticalAlignment.Center
                        preferredWidth: 550
                        visible: true
                        text: ListItemData.label
                        multiline: true
                    }
                    function imageShow() {
                        if (ListItemData.ratValue < 1) {
                            return "asset:///images/rating_0.png";
                        } else if (ListItemData.ratValue < 2) {
                            return "asset:///images/rating_1.png";
                        } else if (ListItemData.ratValue < 3) {
                            return "asset:///images/rating_2.png";
                        } else if (ListItemData.ratValue < 4) {
                            return "asset:///images/rating_3.png";
                        } else if (ListItemData.ratValue < 5) {
                            return "asset:///images/rating_4.png";
                        } else if (ListItemData.ratValue < 6) {
                            return "asset:///images/rating_5.png";
                        }
                    }
                }
            }
            onTriggered: {
                if (dataModel.data(indexPath).loading) {
                    return;
                }
                dialogSetRating.open();
                tempLabel.text = _app.convertVariant(indexPath);
                dialogSetRating.indexRat = tempLabel.text;
                dialogSetRating.urlImage = dataModel.data(indexPath).urlImage;
                dialogSetRating.idRat = dataModel.data(indexPath).idRat;
            }
        }
        Label {
            id: tempLabel
            visible: false
        }
    }
    onCreationCompleted: {
        _app.loadImages()
        listView.visible = true
    }
}