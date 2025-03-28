#include <Qdebug>
#include "network.h"
#include "user.h"
#include "post.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <queue>
#include <vector>
#include <set>
#include <string>
#include <stack>
#include <iostream>


Network::Network() {
    users_.clear();
}

bool Network::userExists(const std::string &username) {
    std::cout << users_.size() << std::endl;
    for (User* user : users_) {
        std::cout << user->getName() << std::endl;
        if (user && user->getName() == username) {
            return true;
        }
    }
    return false;
}

int Network::getUserIdByName(const std::string& username) const {
    for (const auto& user : users_) {
        if (user->getName() == username) {
            return user->getId();
        }
    }
    return -1;
}

User* Network::getUserById(int id) const {
    for (User* user : users_) {
        if (user->getId() == id) {
            return user;
        }
    }
    return nullptr;
}



std::vector<User*> Network::getFriends(int userId) {
    std::vector<User*> friendsList;
    return friendsList;
}

/*
void Network::updateUsersFile(const std::string& filename) {
    std::ofstream file(filename);
    if (file.is_open()) {
        for (const auto& user : users_) {
            file << user->toFileString() << "\n";
        }
        file.close();
    }
}
*/



void Network::addUser(User* user) {
    users_.push_back(user);
}

User* Network::getUser(int id) {
    return users_[id];
}

int Network::numUsers() const {
    return users_.size();
}
int Network::addConnection(const std::string& name1, const std::string& name2) {
    int id1 = getId(name1);
    int id2 = getId(name2);
    if (id1 == -1 || id2 == -1) {
        return -1;
    }
    users_[id1]->addFriend(id2);
    users_[id2]->addFriend(id1);
    return 0;
}

int Network::deleteConnection(const std::string& name1, const std::string& name2) {
    int id1 = getId(name1);
    int id2 = getId(name2);
    if (id1 == -1 || id2 == -1) {
        return -1;
    }
    users_[id1]->deleteFriend(id2);
    users_[id2]->deleteFriend(id1);
    return 0;
}

int Network::getId(const std::string& name) {
    for (size_t i = 0; i < users_.size(); ++i) {
        if (users_[i]->getName() == name) {
            return users_[i]->getId();
        }
    }
    return -1;
}

std::vector<int> Network::shortestPath(int from, int to) {
    std::queue<int> q;
    std::vector<bool> visited(users_.size(), false);
    std::vector<int> dist(users_.size(), -1);
    std::vector<std::vector<int> > adjLists_;
    
    dist[from] = 0;
    visited[from] = true;
    q.push(from);

    while (!q.empty()) {
        int cur = q.front();
        q.pop();

        for (int neighbor : adjLists_[cur]) {
            if (!visited[neighbor]) {
                dist[neighbor] = dist[cur] + 1;
                visited[neighbor] = true;
                q.push(neighbor);
                
                if (neighbor == to) {
                    return std::vector<int>(1, dist[to]);
                }
            }
        }
    }
    return std::vector<int>(1, dist[to]);
}

std::vector<int> Network::distanceUser(int from, int& to, int distance) {
    std::queue<int> q;
    std::vector<bool> visited(users_.size(), false);
    std::vector<int> prev(users_.size(), -1);
    std::vector<int> path;

    visited[from] = true;
    q.push(from);

    while (!q.empty() && distance > 0) {
        int cur = q.front();
        q.pop();

        for (int neighbor : users_[cur]->getFriends()) {
            if (!visited[neighbor]) {
                prev[neighbor] = cur;
                visited[neighbor] = true;
                q.push(neighbor);
            }
        }
        distance--;
    }

    if (distance == 0 && !q.empty()) {
        to = q.front();
        int current = to;
        while (current != -1) {
            path.push_back(current);
            current = prev[current];
        }
        std::reverse(path.begin(), path.end());
        return path;
    }
    return std::vector<int>();
}

std::vector<int> Network::suggestFriends(int who, int& score) {
    std::vector<int> suggestions;
    score = 0;

    if (who < 0 || who >= users_.size()) {
        qDebug() << "Invalid user ID for suggestFriends:" << who;
        return suggestions;
    }

    std::vector<int> myFriends(users_[who]->getFriends().begin(), users_[who]->getFriends().end());
    qDebug() << "Friends of user ID" << who << ":" << myFriends.size();

    for (int i = 0; i < users_.size(); ++i) {
        if (i == who) continue;

        bool isFriend = std::find(myFriends.begin(), myFriends.end(), i) != myFriends.end();
        if (isFriend) continue;

        int common = 0;
        for (int friendID : users_[i]->getFriends()) {
            if (std::find(myFriends.begin(), myFriends.end(), friendID) != myFriends.end()) {
                ++common;
            }
        }

        if (common > score) {
            score = common;
            suggestions.push_back(i);
        } else if (common == score) {
            suggestions.push_back(i);
        }
    }

    qDebug() << "Suggested friends for user ID" << who << ":" << suggestions.size();
    return suggestions;
}


std::vector<std::vector<int> > Network::groups() {
    int numGroups = 0;
    std::vector<int> group(users_.size(), -1);
    std::vector<std::vector<int> > groups;
    std::vector<std::vector<int> > adjLists_;

    for (int i = 0; i < users_.size(); ++i) {
        if (group[i] == -1) {
            std::vector<int> currentGroup;
            std::stack<int> s;
            s.push(i);
            group[i] = numGroups;
            currentGroup.push_back(i);

            while (!s.empty()) {
                int cur = s.top();
                s.pop();

                for (int neighbor : adjLists_[cur]) {
                    if (group[neighbor] == -1) {
                        group[neighbor] = numGroups;
                        currentGroup.push_back(neighbor);
                        s.push(neighbor);
                    }
                }
            }
            groups.push_back(currentGroup);
            ++numGroups;
        }
    }
    return groups;
}


void Network::readUsers(const char* fname) {
    std::ifstream infile(fname);
    if (!infile.is_open()) {
        std::cerr << "Error: Cannot open file " << fname << std::endl;
        return;
    }
    std::cout << "File " << fname << " opened successfully" << std::endl;

    int numUsers;
    infile >> numUsers;
    infile.ignore();

    for (int i = 0; i < numUsers; ++i) {
        int id, year, zip;
        std::string name, line;
        std::set<int> friends;

        infile >> id;
        infile.ignore();
        std::getline(infile, line);
        name = line.substr(1);

        infile >> year;
        infile.ignore();
        infile >> zip;
        infile.ignore();

        std::getline(infile, line);
        std::istringstream iss(line.substr(1));
        int friendId;
        while (iss >> friendId) {
            friends.insert(friendId);
        }

        User* newUser = new User(id, name, year, zip, friends);
        addUser(newUser);

        std::cout << "Loaded user: " << name << ", ID: " << id << std::endl;
    }

    infile.close();
    std::cout << "All users loaded successfully" << std::endl;
}




void Network::writeUsers(const char* fname) {
    std::ofstream outfile(fname);
    if (!outfile.is_open()) {
        std::cerr << "Error: Cannot open file " << fname << std::endl;
        return;
    }

    outfile << users_.size() << std::endl;

    for (const User* user : users_) {
        outfile << user->getId() << std::endl;
        outfile << "\t" << user->getName() << std::endl;
        outfile << "\t" << user->getYear() << std::endl;
        outfile << "\t" << user->getZip() << std::endl;

        const std::set<int>& friends = user->getFriends();
        outfile << "\t";
        for (int friendId : friends) {
            outfile << friendId << " ";
        }
        outfile << std::endl;
    }

    outfile.close();
}

bool comparePostsById(Post* a, Post* b) {
    return a->getMessageId() < b->getMessageId();
}

int Network::readPosts(const char* fname) {
    std::ifstream file(fname);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file " << fname << std::endl;
        return -1;
    }

    std::cout << "File " << fname << " opened successfully." << std::endl;

    int numPosts;
    file >> numPosts;
    file.ignore();
    std::cout << "Number of posts to load: " << numPosts << std::endl;

    for (int i = 0; i < numPosts; ++i) {
        int messageId, ownerId, likes;
        std::string message, author, visibility;

        file >> messageId;
        file.ignore();
        std::getline(file, message);
        file >> ownerId >> likes;
        file.ignore();

        std::getline(file, visibility);
        std::getline(file, author);

        std::cout << "Read post details - Message ID: " << messageId
                  << ", Owner ID: " << ownerId
                  << ", Likes: " << likes
                  << ", Visibility: " << visibility
                  << ", Author: " << author << std::endl;

        Post* post = nullptr;

        if (visibility == "public" || visibility == "private") {
            Post::PrivacyLevel level = (visibility == "public") ? Post::PrivacyLevel::PUBLIC : Post::PrivacyLevel::PRIVATE;
            post = new IncomingPost(messageId, ownerId, message, likes, level, author);
        } else {
            post = new Post(messageId, ownerId, message, likes);
        }

        User* user = getUser(ownerId);
        if (user) {
            user->addPost(post);
            std::cout << "Post added to user ID " << ownerId << std::endl;
        } else {
            std::cout << "User ID " << ownerId << " not found, deleting post" << std::endl;
            delete post;
        }
    }

    file.close();
    std::cout << "Finished loading posts from " << fname << std::endl;
    return 0;
}



int Network::writePosts(const char* fname) {
    std::ofstream file(fname);
    if (!file.is_open()) {
        return -1;
    }

    std::vector<Post*> allPosts;
    for (User* user : users_) {
        std::vector<Post*> userPosts = user->getPosts();
        allPosts.insert(allPosts.end(), userPosts.begin(), userPosts.end());
    }

    std::sort(allPosts.begin(), allPosts.end(), comparePostsById);

    file << allPosts.size() << "\n";

    for (const auto& post : allPosts) {
        file << post->getMessageId() << "\n";
        file << "\t" << post->getMessage() << "\n";
        file << "\t" << post->getOwnerId() << "\n";
        file << "\t" << post->getLikes() << "\n";


        std::string author = post->getAuthor();
        bool isPublic = post->getIsPublic();

        if (author.empty()) {
            file << "\t" << (isPublic ? "public" : "private") << "\n";
            file << "\t\n";
        } else {
            file << "\t" << (isPublic ? "public" : "private") << "\n";
            file << "\t" << author << "\n";
        }
    }

    file.close();
    return 0;
}





void Network::addPost(int ownerId, std::string message, int likes, bool isIncoming, std::string authorName, bool isPublic) {
    static int nextMessageId = 0; 

    User* user = getUser(ownerId);
    if (user == nullptr) {
        return;
    }

    int messageId = nextMessageId++;

    Post* newPost = nullptr;
    if (isIncoming) {
        Post::PrivacyLevel level = isPublic ? Post::PrivacyLevel::PUBLIC : Post::PrivacyLevel::PRIVATE;
        newPost = new IncomingPost(messageId, ownerId, message, likes, level, authorName);
    } else {
        newPost = new Post(messageId, ownerId, message, likes);
    }

    user->addPost(newPost);
}



std::string Network::getPostsString(int ownerId, int howMany, bool showOnlyPublic){
    User* user = getUser(ownerId);
      if (user == nullptr) {
        return "";
    }
    return user->getPostsString(howMany, showOnlyPublic);
}

//Pre: The userId should match an existing user in the network, and updatedUser should be a valid User object.
// Post: Updates the user object in the network collection if the user ID matches; otherwise, logs that the user was not found.
void Network::updateUser(int userId, User* updatedUser) {
    for (auto& user : users) { // Assuming `users` is a collection of User pointers
        if (user->getId() == userId) { // Match by userId
            user = updatedUser; // Replace with the updated user
            qDebug() << "User successfully updated in the network: ID =" << userId;
            return;
        }
    }
    qDebug() << "User not found in the network to update.";
}




