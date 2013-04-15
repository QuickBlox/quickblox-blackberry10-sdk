import bb.cascades 1.0
import com.rim.example.custom 1.0
import my.library 1.0

PreDestroyablePage {
    id: self
    property alias coDataModel: coDataModel
    Container {
        ListView {
            property real itemsWidth: _app.displayWidth()
            id: coListView
            verticalAlignment: VerticalAlignment.Fill
            horizontalAlignment: HorizontalAlignment.Fill
            dataModel: CustomObjectsModel {
                id: coDataModel
                grouping: ItemGrouping.None
                sortedAscending: false
                sortingKeys: [
                    "updatedAt"
                ]
                onItemAdded: {
                    timer.stop()
                    timer.start()
                    console.log("item was added at " + indexPath);
                    var itemData = data(indexPath);
                    if (self.noteDetailsPage) {
                        if (itemData.id == getNoteDetailsPage().noteId) {
                            getNoteDetailsPage().noteData = itemData;
                        }
                    }
                }
                onItemUpdated: {
                    console.log("item at " + indexPath + " was updated");
                    var itemData = data(indexPath);
                    if (self.noteDetailsPage) {
                        if (itemData.id == getNoteDetailsPage().noteId) {
                            getNoteDetailsPage().noteData = itemData;
                        }
                    }
                }
            }
            function scrollToTop() {
                scrollToPosition(0, 0x2);
            }
            listItemComponents: [
                ListItemComponent {
                    type: "coItem"
                    Container {
                        preferredWidth: (ListItem.initialized ? ListItem.view.itemsWidth : 0)
                        layout: DockLayout {
                        }
                        ImageView {
                            imageSource: "asset:///images/imagesbackground.jpeg"
                            verticalAlignment: VerticalAlignment.Fill
                            horizontalAlignment: HorizontalAlignment.Fill
                        }
                        Container {
                            verticalAlignment: VerticalAlignment.Center
                            layout: StackLayout {
                                orientation: LayoutOrientation.LeftToRight
                            }
                            Container {
                                leftPadding: 20.0
                                minWidth: 150.0
                                Label {
                                    text: "Note:"
                                    textStyle.fontWeight: FontWeight.Bold
                                }
                                Label {
                                    text: "Status:"
                                    textStyle.fontWeight: FontWeight.Bold
                                }
                                Label {
                                    text: "Date:"
                                    textStyle.fontWeight: FontWeight.Bold
                                }
                            }
                            Container {
                                leftMargin: 20.0
                                Label {
                                    text: ListItemData.fields["note"]
                                }
                                Label {
                                    text: ListItemData.fields["status"]
                                }
                                Label {
                                    text: ListItemData.createdAtString
                                }
                            }
                        }
                    }
                }
            ]
            // Item type-mapping
            function itemType(data, indexPath) {
                return 'coItem'
            }
            onTriggered: {
                console.log("try to open note details on index " + indexPath);
                var chosenItem = dataModel.data(indexPath);
                var page = self.getNoteDetailsPage();
                page.noteData = chosenItem;
                navigationPane.push(page);
            }
            attachedObjects: [
                QTimer {
                    id: timer
                    singleShot: true
                    interval: 100
                    onTimeout: {
                        coListView.scrollToTop();
                    }
                }
            ]
        }
    }
    actions: [
        ActionItem {
            id: newNoteActionItem
            title: "Add New Note"
            ActionBar.placement: ActionBarPlacement.OnBar
            onTriggered: {
                var page = self.getNewNotePage();
                navigationPane.push(page);
            }
        }
    ]
    property Page newNotePage
    function getNewNotePage() {
        if (! newNotePage) {
            console.log("creating new newNotePage");
            newNotePage = newNotePageDefinition.createObject();
        }
        return newNotePage;
    }
    property Page noteDetailsPage
    function getNoteDetailsPage() {
        if (! noteDetailsPage) {
            console.log("creating new noteDetailsPage");
            noteDetailsPage = noteDetailsPageDefinition.createObject();
        }
        return noteDetailsPage;
    }
    function showLastAdded() {
        var chosenItem = coDataModel.lastAdded();
        var page = self.getNoteDetailsPage();
        page.noteData = chosenItem;
        navigationPane.push(page);
    }
    attachedObjects: [
        ComponentDefinition {
            id: newNotePageDefinition
            source: "NewNote.qml"
        },
        ComponentDefinition {
            id: noteDetailsPageDefinition
            source: "NoteDetails.qml"
        }
    ]
}
