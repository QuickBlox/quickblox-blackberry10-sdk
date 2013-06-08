/*
 * ImagePage.qml
 * 
 * Created on: 21.02.2013
 * Author: Sergey Fedunets
 * Company: Injoit Ltd
 */

import bb.cascades 1.0
import bb.system 1.0
import bb.cascades.pickers 1.0

Page {
    id: imagePage
    attachedObjects: [
        // Cascades FilePicker
        FilePicker {
            id: picker
            property string selectedFile
            mode: FilePickerMode.Picker
            type: FileType.Other
            sortBy: FilePickerSortFlag.Default
            sortOrder: FilePickerSortOrder.Default
            onFileSelected: {
                selectedFile = selectedFiles[0]
                fileActions()
            }
        },
        IndicatorDialog {
            id: indicatorDialog
        }
    ]
    actions: [
        ActionItem {
            id: cloudActionPreviewer
            enabled: false
            title: qsTr("Upload") + Retranslate.onLanguageChanged
            imageSource: "asset:///images/upload_logo.png"
            ActionBar.placement: ActionBarPlacement.OnBar
            onTriggered: {
                _app.uploadImage(picker.selectedFile)
                indicatorDialog.open();
            }
        },
        ActionItem {
            title: qsTr("Select | Capture Image") + Retranslate.onLanguageChanged
            imageSource: "asset:///images/select_image_logo.png"
            ActionBar.placement: ActionBarPlacement.InOverflow
            onTriggered: {
                picker.title = qsTr("Select Image")
                picker.type = FileType.Picture
                picker.directories = [
                ]
                picker.open();
            }
        }
    ]
    Container {
        id: mainContainer
        layout: DockLayout {
        }
        ImageView {
            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Fill
            imageSource: "asset:///images/bg.jpg"
        }
        Container {
            layoutProperties: StackLayoutProperties {
            }
            topPadding: 30
            horizontalAlignment: HorizontalAlignment.Center
            Label {
                id: selectedLabel
                visible: false
                verticalAlignment: VerticalAlignment.Top
                horizontalAlignment: HorizontalAlignment.Center
                topMargin: 30
                textStyle.color: Color.Black
                textStyle.fontSize: FontSize.Large
                text: "The expected load"
            }
            Container {
                verticalAlignment: VerticalAlignment.Top
                horizontalAlignment: HorizontalAlignment.Center
                layout: StackLayout {
                    orientation: LayoutOrientation.LeftToRight
                }
                ImageView {
                    id: selectedImage
                    verticalAlignment: VerticalAlignment.Top
                    horizontalAlignment: HorizontalAlignment.Center
                    visible: false
                    //imageSource: "asset:///images/DSCF1055.jpg"
                    topMargin: 30
                    preferredWidth: 300
                    preferredHeight: 600
                    scalingMethod: ScalingMethod.AspectFit
                }
                ImageButton {
                    id: imageCancel
                    visible: false
                    verticalAlignment: VerticalAlignment.Top
                    horizontalAlignment: HorizontalAlignment.Center
                    defaultImageSource: "asset:///images/cancel_red.png"
                    pressedImageSource: "asset:///images/cancel_red.png"
                    preferredWidth: 50
                    preferredHeight: 50
                    minHeight: 50
                    onTouch: {
                        imageCancel.visible = false;
                        selectedImage.visible = false;
                        selectedLabel.visible = false;
                        imageCancel.visible = false;
                        divider.visible = false;
                        cloudActionPreviewer.enabled = false;
                    }
                }
            }
            Divider {
                id: divider
                visible: false
                preferredWidth: 700
            }
            ListView {
                id: listView
                preferredWidth: 700
                preferredHeight: 1100
                horizontalAlignment: HorizontalAlignment.Center
                verticalAlignment: VerticalAlignment.Center
                objectName: "listView"
                topMargin: 30
                visible: false
                dataModel: _app.model
                listItemComponents: ListItemComponent {
                    type: ""
                    Container {
                        horizontalAlignment: HorizontalAlignment.Center
                        preferredWidth: 700
                        topPadding: 40.0
                        layout: DockLayout {
                        }

                        // The ActivityIndicator that is only active and visible while the image is loading
                        ActivityIndicator {
                            horizontalAlignment: HorizontalAlignment.Center
                            preferredHeight: 550
                            preferredWidth: 550
                            visible: ListItemData.loading
                            running: ListItemData.loading
                            onStarted: {
                                image.opacity = 0.5;
                            }
                            onStopped: {
                                image.opacity = 1;
                            }
                        }

                        // The ImageView that shows the loaded image after loading has finished without error
                        ImageView {
                            id: image
                            maxWidth: 650
                            horizontalAlignment: HorizontalAlignment.Center
                            image: ListItemData.image
                            visible: ! ListItemData.loading && ListItemData.label == ""
                        }

                        // The Label that shows a possible error message after loading has finished
                        Label {
                            horizontalAlignment: HorizontalAlignment.Center
                            preferredWidth: 550
                            visible: ! ListItemData.loading && ! ListItemData.label == ""
                            text: ListItemData.label
                            multiline: true
                        }
                    }
                }
            }
        }
    }
    function fileActions() {
        console.log("----------------------- picker.selectedFile = " + "file://" + picker.selectedFile)
        selectedImage.imageSource = "file://" + picker.selectedFile
        selectedLabel.visible = true;
        selectedImage.visible = true;
        imageCancel.visible = true;
        divider.visible = true;
        cloudActionPreviewer.enabled = true;
    }
    onCreationCompleted: {
        _app.loadImages()
        listView.visible = true
    }
}
