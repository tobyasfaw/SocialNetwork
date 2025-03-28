#ifndef NETWORK_H
#define NETWORK_H

#include "user.h"
#include <vector>
#include <string>
#include <iostream>

class User;

class Network {
public:
    Network();

    void addUser(User* user);
    User* getUser(int id);
    int addConnection(const std::string& name1, const std::string& name2);
    int deleteConnection(const std::string& name1, const std::string& name2);
    int getId(const std::string& name);
    int numUsers() const;

    std::vector<int> shortestPath(int from, int to);
    std::vector<int> distanceUser(int from, int& to, int distance);
    std::vector<int> suggestFriends(int who, int& score);
    std::vector<std::vector<int> > groups();

    void readUsers(const char* fname);
    void writeUsers(const char* fname);
    bool userExists(const std::string &username);

    std::vector<User*> getFriends(int userId);
    int getUserIdByName(const std::string& username) const;
    User* getUserById(int id) const;

    /*void updateUsersFile(const std::string& filename);*/


// Read posts from a file and associate them with users
    int readPosts(const char *fname);
// Write all posts to a file
    int writePosts(const char *fname);
// Add a post for a specific user in the network
    void addPost(int ownerId, std::string message, int likes, bool isIncoming, std::string authorName, bool isPublic);

// Get a formatted string of recent posts for a user, showing only public posts if specified
    std::string getPostsString(int ownerId, int howMany, bool showOnlyPublic);
    void updateUser(int userId, User* updatedUser);



private:
    std::vector<User*> users_;
    std::vector<Post> posts_;
    std::vector<User*> users;
};

#endif
