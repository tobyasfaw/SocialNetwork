#include "network.h"
#include "window.h"
#include "ui_window.h"
#include "post.h"

#include <QVBoxLayout>
#include <QMessageBox>
#include <QDebug>

// Constructor
SocialNetworkWindow::SocialNetworkWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::SocialNetworkWindow), loggedInUser(nullptr), currentDisplayedUser(nullptr)
{
    ui->setupUi(this);

    ui->searchInput->hide();
    ui->searchButton->hide();

    ui->profileHeader->hide();
    ui->profilePrivacyDropdown->hide();
    ui->friendsTable->hide();
    ui->suggestfriendsTable->hide();
    ui->postsTable->hide();
    ui->backToMyProfileButton->hide();

    //network data
    network.readUsers("users.txt");
    network.readPosts("posts.txt");

    //signals and slots
    connect(ui->loginButton, &QPushButton::clicked, this, &SocialNetworkWindow::handleLogin);
    connect(ui->backToMyProfileButton, &QPushButton::clicked, this, &SocialNetworkWindow::goBack);
    connect(ui->friendsTable, &QTableWidget::itemClicked, this, &SocialNetworkWindow::viewFriendProfile);
    connect(ui->suggestfriendsTable, &QTableWidget::itemClicked, this, &SocialNetworkWindow::addFriendFromSuggestions);
    connect(ui->searchButton, &QPushButton::clicked, this, &SocialNetworkWindow::searchUser);
    connect(ui->profilePrivacyDropdown, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &SocialNetworkWindow::updatePrivacySetting);




}





SocialNetworkWindow::~SocialNetworkWindow() {
    delete ui;
}

// Pre-condition: The user must click on a friend's name in the friends table.
// Post-condition: The selected friend's profile is displayed if found; otherwise, an error message is shown.
void SocialNetworkWindow::viewFriendProfile(QTableWidgetItem* item) {
    QString friendName = item->text();
    int friendUserId = network.getUserIdByName(friendName.toStdString());
    User* friendUser = network.getUserById(friendUserId);

    if (friendUser) {
        currentDisplayedUser = friendUser;
        // Navigate to the profile even if the friends list is private
        displayUserProfile(friendUser);
    } else {
        QMessageBox::warning(this, "Error", "User not found.");
    }
}



// Pre-condition: The method is called with a valid User object.
// Post-condition: The friends table is populated with the user's friends or displays a "Friend list is private" message.
void SocialNetworkWindow::loadFriendsTable(User* user) {
    ui->friendsTable->setRowCount(0);
    ui->friendsTable->setColumnCount(1);
    ui->friendsTable->setHorizontalHeaderLabels({"Friend Name"});

    // Check if the current logged-in user can view this user's friends
    if (!user->canViewFriends(loggedInUser->getId())) {
        qDebug() << "Access denied to friends list for user: " << QString::fromStdString(user->getName());
        ui->friendsTable->setRowCount(1);
        QTableWidgetItem* noAccessItem = new QTableWidgetItem("Friend list is private");
        noAccessItem->setFlags(Qt::ItemIsEnabled); // Make it non-editable
        ui->friendsTable->setItem(0, 0, noAccessItem);

        // Resize for better display
        ui->friendsTable->resizeColumnsToContents();
        ui->friendsTable->resizeRowsToContents();
        return;
    }

    // Populate the friends table if allowed
    const std::set<int>& friends = user->getFriends();
    int row = 0;
    for (int friendId : friends) {
        User* friendUser = network.getUserById(friendId);
        if (friendUser) {
            QTableWidgetItem* friendItem = new QTableWidgetItem(QString::fromStdString(friendUser->getName()));
            ui->friendsTable->insertRow(row);
            ui->friendsTable->setItem(row, 0, friendItem);
            ++row;
        }
    }

    ui->friendsTable->resizeColumnsToContents();
    ui->friendsTable->resizeRowsToContents();
}





void SocialNetworkWindow::loadSuggestedFriendsTable(User* u) {
    ui->suggestfriendsTable->setRowCount(0);
    ui->suggestfriendsTable->setColumnCount(1);

    int score = 0;
    const std::vector<int> suggestedFriends = network.suggestFriends(u->getId(), score);

    ui->suggestfriendsTable->setRowCount(suggestedFriends.size() + 1);
    QTableWidgetItem* title = new QTableWidgetItem("Suggested Friends");
    ui->suggestfriendsTable->setItem(0, 0, title);

    int row = 1;
    for (int friendId : suggestedFriends) {
        User* friendUser = network.getUser(friendId);
        if (!friendUser) continue;
        QTableWidgetItem* nameItem = new QTableWidgetItem(QString::fromStdString(friendUser->getName()));
        ui->suggestfriendsTable->setItem(row, 0, nameItem);
        ++row;
    }

    ui->suggestfriendsTable->resizeColumnsToContents();
}

void SocialNetworkWindow::goBack() {
    if (!loggedInUser) {
        QMessageBox::warning(this, "Error", "No user is currently logged in.");
        return;
    }

    if (currentDisplayedUser != loggedInUser) {
        // Navigate back to the logged-in user's profile
        currentDisplayedUser = loggedInUser;
        displayUserProfile(loggedInUser);
    } else {
        // Reset to login state
        ui->profileHeader->hide();
        ui->profilePrivacyDropdown->hide();
        ui->friendsTable->hide();
        ui->suggestfriendsTable->hide();
        ui->postsTable->hide();
        ui->backToMyProfileButton->hide();

        // Hide search bar and show login components
        ui->searchInput->hide();
        ui->searchButton->hide();
        ui->usernameInput->clear();
        ui->usernameInput->show();
        ui->loginButton->show();
        ui->statusLabel->show();

        loggedInUser = nullptr;
        currentDisplayedUser = nullptr;
    }
}






void SocialNetworkWindow::handleLogin() {
    QString username = ui->usernameInput->toPlainText();
    int userId = network.getUserIdByName(username.toStdString());

    if (userId != -1) {
        User* user = network.getUserById(userId);
        if (user) {
            loggedInUser = user;
            currentDisplayedUser = user;
            displayUserProfile(user);

            ui->searchInput->show();
            ui->searchButton->show();
        } else {
            QMessageBox::warning(this, "Login Failed", "User not found.");
        }
    } else {
        QMessageBox::warning(this, "Login Failed", "User not found.");
    }
}


// Pre-condition: The method is called with a valid User object.
// Post-condition: The posts table is dynamically populated with the user's posts, and each post includes a privacy dropdown.
void SocialNetworkWindow::loadPostsTable(User* user) {
    ui->postsTable->setRowCount(0);
    ui->postsTable->setColumnCount(3);
    ui->postsTable->setHorizontalHeaderLabels({"Content", "Likes", "Privacy"});

    // Resize columns dynamically
    ui->postsTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch); // Content column
    ui->postsTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents); // Likes column
    ui->postsTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents); // Privacy column

    for (Post* post : user->getPosts()) {
        int row = ui->postsTable->rowCount();
        ui->postsTable->insertRow(row);

        // Post Content
        QTableWidgetItem* contentItem = nullptr;

        // Check if the logged-in user can view the post
        if (post->getPrivacyLevel() == Post::PrivacyLevel::PRIVATE &&
            loggedInUser->getId() != post->getOwnerId()) {
            contentItem = new QTableWidgetItem("This post is hidden");
        } else {
            // Show the actual post content
            contentItem = new QTableWidgetItem(QString::fromStdString(post->getMessage()));
        }

        contentItem->setTextAlignment(Qt::AlignLeft | Qt::AlignTop);
        contentItem->setFlags(contentItem->flags() & ~Qt::ItemIsEditable);
        ui->postsTable->setItem(row, 0, contentItem);

        // Post Likes
        QTableWidgetItem* likesItem = new QTableWidgetItem(QString::number(post->getLikes()));
        likesItem->setFlags(likesItem->flags() & ~Qt::ItemIsEditable);
        ui->postsTable->setItem(row, 1, likesItem);

        // Privacy Dropdown
        QComboBox* privacyDropdown = new QComboBox();
        privacyDropdown->addItems({"Public", "Private"});

        privacyDropdown->setCurrentIndex(static_cast<int>(post->getPrivacyLevel()));

        // Allow only the owner of the post to change the privacy level
        if (loggedInUser->getId() == post->getOwnerId()) {
            //Connect the QComboBox's currentIndexChanged signal to a lambda function
            connect(privacyDropdown, QOverload<int>::of(&QComboBox::currentIndexChanged), [post, this](int index) {
                post->setPrivacyLevel(static_cast<Post::PrivacyLevel>(index));
                QMessageBox::information(this, "Privacy Updated", "Post privacy updated to: " +
                                                                      QString::fromStdString(index == 0 ? "Public" : "Private"));
            });
        } else {
            privacyDropdown->setEnabled(false);
        }

        ui->postsTable->setCellWidget(row, 2, privacyDropdown);
    }

    // Adjust row and column sizes for better readability
    ui->postsTable->resizeRowsToContents(); // Adjust row heights
    ui->postsTable->resizeColumnsToContents(); // Ensure columns fit the content
    ui->postsTable->show();
}




void SocialNetworkWindow::displayUserProfile(User* user) {
    // Hide login components
    ui->usernameInput->hide();
    ui->loginButton->hide();
    ui->statusLabel->hide();

    // Keep the search bar visible when navigating profiles
    ui->searchInput->show();
    ui->searchButton->show();

    // Update profile header
    ui->profileHeader->setText(QString::fromStdString(user->getName()) + "'s Profile");
    ui->profileHeader->show();

    // Show privacy dropdown only for the logged-in user's profile
    if (loggedInUser && user->getId() == loggedInUser->getId()) {
        ui->profilePrivacyDropdown->setCurrentIndex(static_cast<int>(user->getProfilePrivacyLevel()));
        ui->profilePrivacyDropdown->show();
    } else {
        ui->profilePrivacyDropdown->hide();
    }

    // Load and display tables
    loadFriendsTable(user);
    ui->friendsTable->show();

    loadSuggestedFriendsTable(user);
    ui->suggestfriendsTable->show();

    loadPostsTable(user);
    ui->postsTable->show();

    // Show back button
    ui->backToMyProfileButton->show();
}






// Pre: A valid privacy level index is passed when the user modifies their profile privacy settings.
// Post: The logged-in user's privacy level is updated, and the change is reflected in the UI.
void SocialNetworkWindow::updatePrivacySetting(int index) {
    if (!loggedInUser) return;

    // Check if the privacy level is actually changing
    if (loggedInUser->getProfilePrivacyLevel() == static_cast<User::ProfilePrivacyLevel>(index)) {
        // If the privacy level is the same as before, don't do anything
        return;
    }

    loggedInUser->setProfilePrivacyLevel(static_cast<User::ProfilePrivacyLevel>(index));
    qDebug() << "Privacy updated for:" << QString::fromStdString(loggedInUser->getName())
             << ", Level:" << static_cast<int>(loggedInUser->getProfilePrivacyLevel());

    QMessageBox::information(this, "Privacy Updated", "Your privacy setting has been updated.");
}







void SocialNetworkWindow::addFriendFromSuggestions(QTableWidgetItem* item) {
    if (!item) return;

    QString suggestedFriendName = item->text();
    int suggestedFriendId = network.getUserIdByName(suggestedFriendName.toStdString());
    User* suggestedUser = network.getUserById(suggestedFriendId);

    if (suggestedUser) {
        loggedInUser->addFriend(suggestedUser->getId());
        QMessageBox::information(this, "Friend Added", suggestedFriendName + " has been added as a friend.");

        int row = item->row();
        ui->suggestfriendsTable->removeRow(row);

        loadFriendsTable(loggedInUser);
    }
}


// Pre: The search bar must contain a non-empty, valid user name as input.
// Post: Displays the profile of the searched user if they exist in the network; otherwise, shows an appropriate error message.
void SocialNetworkWindow::searchUser() {
    QString searchName = ui->searchInput->text().trimmed();
    if (searchName.isEmpty()) {
        QMessageBox::warning(this, "Error", "Search field is empty.");
        return;
    }

    int userId = network.getUserIdByName(searchName.toStdString());
    if (userId == -1) {
        QMessageBox::warning(this, "User Not Found", "No user with that name exists.");
        return;
    }

    User* searchedUser = network.getUserById(userId);
    if (!searchedUser) {
        QMessageBox::warning(this, "Error", "Failed to retrieve user data.");
        return;
    }

    // Display the searched user's profile
    qDebug() << "Navigating to profile of:" << QString::fromStdString(searchedUser->getName());
    displayUserProfile(searchedUser);
}






