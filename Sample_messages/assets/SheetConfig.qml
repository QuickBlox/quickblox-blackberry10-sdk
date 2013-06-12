/*!
 * Copyright (c) 2012 Research In Motion Limited.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import bb.cascades 1.0

// The page to configure the settings of the notification service

Page {
    id: sheetConfig

    // load the configuration values into the QML components
    function setValues() {
        appId.text = _pushAPIHandler.appId;
        ppgUrl.text = _pushAPIHandler.ppgUrl;
        publicPpgOption.selected = _pushAPIHandler.usePublicPpg;
    }

    // Signals for canceling, saving and refreshing values
    signal cancel()
    signal save()
    signal refresh()

    // Refreshes the configuration values to their last saved values
    onRefresh: {
        _pushAPIHandler.loadConfiguration();
        setValues();
    }

    // When this has finished being created, load the configuration values into the components
    onCreationCompleted: {
        OrientationSupport.supportedDisplayOrientation = SupportedDisplayOrientation.DisplayPortrait;
        setValues();
    }
    titleBar: TitleBar {
        id: addBar
        title: qsTr("Configuration")
        visibility: ChromeVisibility.Visible
        dismissAction: ActionItem {
            title: qsTr("Cancel")
            onTriggered: {
                // Cancel has been pressed so we emit cancel (will hide the sheet).
                sheetConfig.cancel();
            }
        }
        acceptAction: ActionItem {
            title: qsTr("Save")
            onTriggered: {
                // Emit save signal
                sheetConfig.save();
            }
        }
    }
    ScrollView {
        scrollViewProperties {
            scrollMode: ScrollMode.Vertical
        }
        Container {
            Container {
                topPadding: 30
                leftPadding: 20
                rightPadding: 20
                bottomPadding: 10
                DropDown {
                    horizontalAlignment: HorizontalAlignment.Center
                    title: qsTr("Mode")
                    enabled: true
                    Option {
                        id: publicPpgOption
                        text: qsTr("Public/BIS")
                        value: text
                        selected: true
                    }

                    // When a new option is selected determine which fields should be enabled
                    onSelectedIndexChanged: {
                        _pushAPIHandler.usePublicPpg = publicPpgOption.selected;
                    }
                }
            }
            Divider {
            }
            CustomTextField {
                id: appId
                description: qsTr("Application ID")
                inputMode: TextFieldInputMode.Text
                text: "3878-s742877B3mr552yO5a48e3m9832Ma677488"
                onTextChanged: {
                    _pushAPIHandler.appId = text;
                }
                textEnabled: (publicPpgOption.selected ? true : useSDKasPushInitiator.checked)
            }
            Divider {
            }
            CustomTextField {
                id: ppgUrl
                description: qsTr("PPG Url")
                inputMode: TextFieldInputMode.Url
                textEnabled: publicPpgOption.selected
                text: "http://cp3878-s742877B3mr552yO5a48e3m9832Ma677488.pushapi.eval.blackberry.com"
                onTextChanged: {
                    _pushAPIHandler.ppgUrl = text;
                }
            }
            Divider {
            }
        }
    }
}
