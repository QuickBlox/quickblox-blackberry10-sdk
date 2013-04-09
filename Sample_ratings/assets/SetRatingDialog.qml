/*
 * SetRatingDialog.qml
 * 
 * Created on: 21.02.2013
 * Author: Sergey Fedunets
 * Company: Injoit Ltd
 */
 
import bb.cascades 1.0

Dialog {
    property alias indexRat: indexRat.text
    property alias urlImage: urlImage.text
    property alias idRat: idRat.text
    
    Container {
        preferredWidth: 768
        preferredHeight: 1280
        background: Color.create(0.0, 0.0, 0.0, 0.5)
        layout: DockLayout {
        }
        Container {
            maxHeight: 330
            horizontalAlignment: HorizontalAlignment.Center
            verticalAlignment: VerticalAlignment.Center
            layout: DockLayout {
            }
            ImageView {
                imageSource: "asset:///images/dialog_alarm.png"
            }
            Container {
                topPadding: 5
                bottomPadding: 23
                horizontalAlignment: HorizontalAlignment.Fill
                verticalAlignment: VerticalAlignment.Fill
                Label {
                    text: "Set rating"
                    textStyle.base: SystemDefaults.TextStyles.TitleText
                    textStyle.color: Color.create("#fafafa")
                    horizontalAlignment: HorizontalAlignment.Center
                }
                Label {
                    id: idRat
                    visible: false
                }
                Label {
                    id: indexRat
                    visible: false
                }
                Label {
                    id: valueRat
                    visible: false
                }
                Label {
                    id: urlImage
                    visible: false
                }
                Container {
                    visible: true
                    layout: StackLayout {
                        orientation: LayoutOrientation.LeftToRight
                    }
                    horizontalAlignment: HorizontalAlignment.Center
                    layoutProperties: StackLayoutProperties {
                        spaceQuota: 2.5
                    }
                    ImageButton {
                        id: buttonOne
                        defaultImageSource: "asset:///images/black_star.png"
                        pressedImageSource: "asset:///images/gold_star.png"
                        onClicked: {
                            buttonOne.defaultImageSource = "asset:///images/gold_star.png"
                            buttonTwo.defaultImageSource = "asset:///images/black_star.png"
                            buttonTree.defaultImageSource = "asset:///images/black_star.png"
                            buttonFour.defaultImageSource = "asset:///images/black_star.png"
                            buttonFive.defaultImageSource = "asset:///images/black_star.png"
                            valueRat.text = "1"
                        }
                    }
                    ImageButton {
                        id: buttonTwo
                        defaultImageSource: "asset:///images/black_star.png"
                        pressedImageSource: "asset:///images/gold_star.png"
                        onClicked: {
                            buttonOne.defaultImageSource = "asset:///images/gold_star.png"
                            buttonTwo.defaultImageSource = "asset:///images/gold_star.png"
                            buttonTree.defaultImageSource = "asset:///images/black_star.png"
                            buttonFour.defaultImageSource = "asset:///images/black_star.png"
                            buttonFive.defaultImageSource = "asset:///images/black_star.png"
                            valueRat.text = "2"
                        }
                    }
                    ImageButton {
                        id: buttonTree
                        defaultImageSource: "asset:///images/black_star.png"
                        pressedImageSource: "asset:///images/gold_star.png"
                        onClicked: {
                            buttonOne.defaultImageSource = "asset:///images/gold_star.png"
                            buttonTwo.defaultImageSource = "asset:///images/gold_star.png"
                            buttonTree.defaultImageSource = "asset:///images/gold_star.png"
                            buttonFour.defaultImageSource = "asset:///images/black_star.png"
                            buttonFive.defaultImageSource = "asset:///images/black_star.png"
                            valueRat.text = "3"
                        }
                    }
                    ImageButton {
                        id: buttonFour
                        defaultImageSource: "asset:///images/black_star.png"
                        pressedImageSource: "asset:///images/gold_star.png"
                        onClicked: {
                            buttonOne.defaultImageSource = "asset:///images/gold_star.png"
                            buttonTwo.defaultImageSource = "asset:///images/gold_star.png"
                            buttonTree.defaultImageSource = "asset:///images/gold_star.png"
                            buttonFour.defaultImageSource = "asset:///images/gold_star.png"
                            buttonFive.defaultImageSource = "asset:///images/black_star.png"
                            valueRat.text = "4"
                        }
                    }
                    ImageButton {
                        id: buttonFive
                        defaultImageSource: "asset:///images/black_star.png"
                        pressedImageSource: "asset:///images/gold_star.png"
                        onClicked: {
                            buttonOne.defaultImageSource = "asset:///images/gold_star.png"
                            buttonTwo.defaultImageSource = "asset:///images/gold_star.png"
                            buttonTree.defaultImageSource = "asset:///images/gold_star.png"
                            buttonFour.defaultImageSource = "asset:///images/gold_star.png"
                            buttonFive.defaultImageSource = "asset:///images/gold_star.png"
                            valueRat.text = "5"
                        }
                    }
                }
                Container {
                    horizontalAlignment: HorizontalAlignment.Center
                    layout: StackLayout {
                        orientation: LayoutOrientation.LeftToRight
                    }
                    Button {
                        horizontalAlignment: HorizontalAlignment.Center
                        text: "Set rating"
                        preferredWidth: 280
                        onClicked: {
                            dialogSetRating.close();
                            _app.setRating(idRat.text, valueRat.text, indexRat.text, urlImage.text);
                        }
                    }
                    Button {
                        horizontalAlignment: HorizontalAlignment.Center
                        text: "Cancel"
                        preferredWidth: 280
                        onClicked: {
                            dialogSetRating.close();
                        }
                    }
                }
            } // Container
        } // Container
    } // Container
    onClosed: {
        buttonOne.defaultImageSource = "asset:///images/black_star.png"
        buttonTwo.defaultImageSource = "asset:///images/black_star.png"
        buttonTree.defaultImageSource = "asset:///images/black_star.png"
        buttonFour.defaultImageSource = "asset:///images/black_star.png"
        buttonFive.defaultImageSource = "asset:///images/black_star.png"
    }
}
