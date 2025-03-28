
#ifndef USER_H
#define USER_H

#include <string>
#include <set>
#include "post.h"



class Network;

class User {
public:
    // Define ProfilePrivacyLevel specifically for User
    enum class ProfilePrivacyLevel { PUBLIC, PRIVATE };

private:
    int id_;
    std::string name_;
    int year_;
    int zip_;
    std::set<int> friends_;
    std::vector<Post*> messages_;
    ProfilePrivacyLevel privacyLevel_;

public:
    User();
    User(int id, const std::string &name, int year, int zip, const std::set<int> &friends);

    int getId() const;
    std::string getName() const;
    int getYear() const;
    int getZip() const;
    std::set<int>& getFriends();

    void addFriend(int id); // Adds a friend by ID
    void deleteFriend(int id); // Removes a friend by ID
    const std::set<int>& getFriends() const; 

    void addPost(Post*); // Adds a new post to the user's messages
    std::vector<Post*>getPosts(); // Returns the user's posts

    // Returns a formatted string of recent posts
    std::string getPostsString(int howMany, bool showOnlyPublic);
    bool isFriend(int friendID) const; // Checks if a given ID belongs to a friend

    std::string toFileString() const;
    ProfilePrivacyLevel getProfilePrivacyLevel() const;
    void setProfilePrivacyLevel(ProfilePrivacyLevel level);
    bool canViewFriends(int viewerId) const;
    bool isFriendOfFriend(int userId, Network& network) const;


};

#endif
