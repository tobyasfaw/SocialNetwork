#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
#include "network.h"
#include "ui_window.h"

QT_BEGIN_NAMESPACE
namespace Ui { class SocialNetworkWindow; }
QT_END_NAMESPACE

class SocialNetworkWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SocialNetworkWindow(QWidget* parent = nullptr);
    ~SocialNetworkWindow();

    void handleLogin();
    void display();
    void loadFriendsTable(User* user);
    void loadSuggestedFriendsTable(User* u);
    void goBack();
    void displayUserProfile(User* user);
    void viewFriendProfile(QTableWidgetItem *item);
    void addFriendFromSuggestions(QTableWidgetItem *item);

    void loadPostsTable(User* user);
    void updatePrivacySetting(int index);
    void viewSuggestedFriendProfile(int friendId);
    void searchUser();




private:
    Ui::SocialNetworkWindow *ui;
    Network network;
    User* loggedInUser;
    User* currentDisplayedUser;

    QLabel *profileHeader;          // For the profile header label
    QPushButton *backToMyProfileButton; // For the "Back" button
    QTableWidget *friendsTable;      // For the friends list table
    QLabel *recentPostsLabel;        // For displaying recent posts
};

#endif
