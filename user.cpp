#include "user.h"
#include "post.h"
#include "network.h"
#include <Qdebug>
#include <iostream>

//User::User() : id_(0), name_("Unknown"), year_(1934), zip_(89591) {}

User::User(int id, const std::string &name, int year, int zip, const std::set<int> &friends)
    : id_(id), name_(name), year_(year), zip_(zip), friends_(friends), privacyLevel_(ProfilePrivacyLevel::PUBLIC) {}




std::string User::toFileString() const {
    std::string result;

    // Line 1: ID
    result += std::to_string(getId()) + "\n";

    // Line 2: Name
    result += getName() + "\n";

    // Line 3: Year
    result += std::to_string(getYear()) + "\n";

    // Line 4: Zip
    result += std::to_string(getZip()) + "\n";

    // Line 5: Friends (space-separated IDs)
    for (int friendId : friends_) {
        result += std::to_string(friendId) + " ";
    }

    // Trim the trailing space and add a newline at the end
    if (!friends_.empty()) {
        result.pop_back();
    }
    result += "\n";

    return result;
}




int User::getId() const {
    return id_;
}

std::string User::getName() const {
    return name_;
}

int User::getYear() const {
    return year_;
}

int User::getZip() const {
    return zip_;
}

const std::set<int>& User::getFriends() const {
    return friends_;
}

void User::addFriend(int id) {
    friends_.insert(id);
    std::cout << "Friends for user: ";
    for (int friendId : friends_) {
        std::cout << friendId << " ";
    }
    std::cout << std::endl;
}


void User::deleteFriend(int id) {
    friends_.erase(id);
}

void User::addPost(Post * p)
{
    messages_.push_back(p);
}

std::vector<Post*> User::getPosts() {
    return messages_;
}

bool User::isFriend(int friendId) const {
    bool result = friends_.find(friendId) != friends_.end();
    std::cout << "isFriend called for user " << id_ << " checking friend " << friendId << ": " << result << std::endl;
    return result;
}


std::string User::getPostsString(int howMany, bool showOnlyPublic)
{
    int x = messages_.size()-1;
    int count = 0;
    std::string result;

    while(x >= 0 && count < howMany){
        Post* post = messages_[x];

        if(post){
            if(!showOnlyPublic || (showOnlyPublic && post->getIsPublic())){
                result += post->toString() + "\n\n";
                count++;
            }
        }
        x--;
    }
    if(!result.empty()){
        result.erase(result.size() - 2);
    }

    return result;

}
std::set<int>& User::getFriends(){
    return friends_;
}

User::ProfilePrivacyLevel User::getProfilePrivacyLevel() const {
    return privacyLevel_;
}

// Pre-condition: A valid ProfilePrivacyLevel enum value is passed.
// Post-condition: The user's profile privacy level is updated
void User::setProfilePrivacyLevel(ProfilePrivacyLevel level) {
    privacyLevel_ = level;
    qDebug() << "Privacy level updated for user ID " << id_
             << ": " << static_cast<int>(privacyLevel_);
}



// Pre-condition: The method is called with the ID of the viewer attempting to access the user's friends list.
// Post-condition: Returns true if the viewer has permission to view the user's friends list; otherwise, false.
bool User::canViewFriends(int viewerId) const {
    if (viewerId == id_) return true; // Self-access allowed
    if (privacyLevel_ == ProfilePrivacyLevel::PUBLIC) return true; // Public profiles are visible
    if (privacyLevel_ == ProfilePrivacyLevel::PRIVATE) {
        return friends_.find(viewerId) != friends_.end(); // Only friends can view
    }
    return false;
}







/*
bool User::isFriendOfFriend(int userId, Network& network) const {
    for (int friendId : friends_) {
        User* friendUser = network.getUserById(friendId);
        if (friendUser && friendUser->isFriend(userId)) {
            std::cout << "isFriendOfFriend: User " << userId << " is a friend of friend " << friendId << std::endl;
            return true;
        }
    }
    std::cout << "isFriendOfFriend: User " << userId << " is NOT a friend of any friends of user " << id_ << std::endl;
    return false;
}
*/




