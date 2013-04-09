/*
 * IndicatorDialog.qml
 * 
 * Created on: 21.02.2013
 * Author: Sergey Fedunets
 * Company: Injoit Ltd
 */

import bb.cascades 1.0

Dialog {
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
            ActivityIndicator {
                id: loadIndicator
                visible: _app.getObjectQBNetwork().loading
                horizontalAlignment: HorizontalAlignment.Center
                verticalAlignment: VerticalAlignment.Center
                preferredHeight: 300
                running: _app.getObjectQBNetwork().loading
                onStopped: {
                    indicatorDialog.close();
                    _app.updateList(picker.selectedFile);
                    selectedImage.visible = false;
                    selectedLabel.visible = false;
                    divider.visible = false;
                    imageCancel.visible = false;
                    cloudActionPreviewer.enabled = false;
                }
            }
        }
    }
}
